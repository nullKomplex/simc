// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.hpp"

namespace { // UNNAMED NAMESPACE

struct proc_parse_opt_t
{
  const char* opt;
  unsigned    flags;
};
  
const proc_parse_opt_t __proc_opts[] =
{
  { "aoespell",    PF_AOE_SPELL                                                },
  { "spell",       PF_SPELL | PF_PERIODIC                                      },
  { "directspell", PF_SPELL                                                    },
  { "periodic",    PF_PERIODIC                                                 },
  { "aoeheal",     PF_AOE_HEAL                                                 },
  { "heal",        PF_HEAL | PF_PERIODIC                                       },
  { "directheal",  PF_HEAL                                                     },
  { "attack",      PF_MELEE | PF_MELEE_ABILITY | PF_RANGED | PF_RANGED_ABILITY },
  { "wattack",     PF_MELEE | PF_RANGED                                        },
  { "sattack",     PF_MELEE_ABILITY | PF_RANGED_ABILITY                        },
  { "melee",       PF_MELEE | PF_MELEE_ABILITY                                 },
  { "wmelee",      PF_MELEE                                                    },
  { "smelee",      PF_MELEE_ABILITY                                            },
  { "wranged",     PF_RANGED                                                   },
  { "sranged",     PF_RANGED_ABILITY                                           },
  { 0,             0                                                           },
};

const proc_parse_opt_t __proc2_opts[] =
{
  { "hit",        PF2_ALL_HIT         },
  { "crit",       PF2_CRIT            },
  { "glance",     PF2_GLANCE          },
  { "dodge",      PF2_DODGE           },
  { "parry",      PF2_PARRY           },
  { "miss",       PF2_MISS            },
  { "cast",       PF2_CAST            },
  { "impact",     PF2_LANDED          },
  { "tickheal",   PF2_PERIODIC_HEAL   },
  { "tickdamage", PF2_PERIODIC_DAMAGE },
  { 0,            0                   },
};

bool has_proc( const std::vector<std::string>& opts, const std::string& proc )
{ return std::find( opts.begin(), opts.end(), proc ) != opts.end(); }

void parse_proc_flags( const std::string&      flags,
                              const proc_parse_opt_t* opts,
                              unsigned&               proc_flags )
{
  std::vector<std::string> splits = util::string_split( flags, "/" );

  const proc_parse_opt_t* proc_opt = opts;
  do
  {
    if ( has_proc( splits, proc_opt -> opt ) )
      proc_flags |= proc_opt -> flags;

    proc_opt++;
  } while ( proc_opt -> opt );
}

// Helper function to detect stat buff type from spell effect data, more
// specifically by mapping Blizzard's effect sub types to actual stats in simc.
stat_e stat_buff_type( const spelleffect_data_t& effect )
{
  stat_e stat = STAT_NONE;

  // All stat buffs in game client data are "auras"
  if ( effect.type() != E_APPLY_AURA )
    return stat;

  switch ( effect.subtype() )
  {
    case A_MOD_STAT:
      if ( effect.misc_value1() == -1 )
        stat = STAT_ALL;
      // Primary stats only, ratings are A_MOD_RATING
      else
        stat = static_cast< stat_e >( effect.misc_value1() + 1 );
      break;
    case A_MOD_RATING:
      stat = util::translate_rating_mod( effect.misc_value1() );
      break;
    case A_MOD_INCREASE_HEALTH_2:
    case A_MOD_INCREASE_HEALTH:
      stat = STAT_MAX_HEALTH;
      break;
    case A_MOD_RESISTANCE:
      stat = STAT_BONUS_ARMOR;
      break;
    case A_MOD_ATTACK_POWER:
    case A_MOD_RANGED_ATTACK_POWER:
      stat = STAT_ATTACK_POWER;
      break;
    case A_MOD_DAMAGE_DONE:
      if ( effect.misc_value1() == 0xFE )
        stat = STAT_SPELL_POWER;
      break;
    default:
      break;
  }

  return stat;
}

} // UNNAMED NAMESPACE

special_effect_t::special_effect_t( const item_t* item ) :
    item( item ),
    player( item -> player )
{
  reset();
}
// special_effect_t::reset ======================================

void special_effect_t::reset()
{
  type = SPECIAL_EFFECT_NONE;
  source = SPECIAL_EFFECT_SOURCE_NONE;

  name_str.clear();
  trigger_str.clear();
  encoding_str.clear();
  
  trigger_type = PROC_NONE;
  trigger_mask = 0;

  proc_flags_ = 0;
  proc_flags2_ = 0;
  
  stat = STAT_NONE;
  stat_amount = 0;

  school = SCHOOL_NONE;
  discharge_amount = 0;
  discharge_scaling = 0;
  aoe = 0;
  
  // Must match buff creator defaults for now
  max_stacks = -1;
  proc_chance_ = -1;

  // ppm < 0 = real ppm, ppm > 0 = normal "ppm", min_double off
  ppm_ = std::numeric_limits<double>::min();
  rppm_scale = RPPM_NONE;

  // Must match buff creator defaults for now
  duration_ = timespan_t::min();

  // Currently covers both cooldown and internal cooldown, as typically
  // cooldown is only used for on-use effects. Must match buff creator default
  // for now.
  cooldown_ = timespan_t::min();

  tick = timespan_t::zero();
  
  cost_reduction = false;
  refresh = -1;
  chance_to_discharge = false;
  reverse = false;
  proc_delay = false;
  unique = false;
  weapon_proc = false;
  
  override_result_es_mask = 0;
  result_es_mask = 0;
  
  spell_id = 0;
  trigger_spell_id = 0;

  execute_action = 0;
  custom_buff = 0;
  custom_init = 0;
}

// special_effect_t::driver =================================================

const spell_data_t* special_effect_t::driver() const
{
  if ( !player )
    return spell_data_t::nil();

  return player -> find_spell( spell_id );
}

// special_effect_t::trigger ================================================

const spell_data_t* special_effect_t::trigger() const
{
  if ( ! player )
    return spell_data_t::nil();

  if ( trigger_spell_id > 0 )
    return player -> find_spell( trigger_spell_id );

  for ( size_t i = 1, end = driver() -> effect_count(); i <= end; i++ )
  {
    if ( driver() -> effectN( i ).trigger() -> ok() )
      return driver() -> effectN( i ).trigger();
  }

  // As a fallback, return the driver spell as the "trigger" as well. This
  // allows on-use side of special effects use the same code paths, and
  // shouldnt break procs.
  return driver();
}

// special_effect_t::is_stat_buff ===========================================

bool special_effect_t::is_stat_buff() const
{
  if ( stat != STAT_NONE )
    return true;

  // Prioritize trigger scanning before driver scanning; in reality if both 
  // the trigger and driver spells contain "stat buffs" (as per simc
  // definition), the system will not support it. In this case, custom buffs
  // and/or callbacks should be used.
  for ( size_t i = 1, end = trigger() -> effect_count(); i <= end; i++ )
  {
    const spelleffect_data_t& effect = trigger() -> effectN( i );
    if ( effect.id() == 0 )
      continue;

    if ( stat_buff_type( effect ) != STAT_NONE )
      return true;
  }

  return false;
}

// special_effect_t::stat ===================================================

stat_e special_effect_t::stat_type() const
{
  if ( stat != STAT_NONE )
    return stat;

  for ( size_t i = 1, end = trigger() -> effect_count(); i <= end; i++ )
  {
    const spelleffect_data_t& effect = trigger() -> effectN( i );
    if ( effect.id() == 0 )
      continue;

    stat_e type = stat_buff_type( effect );
    if ( type != STAT_NONE )
      return type;
  }

  return STAT_NONE;
}

// special_effect_t::max_stack ==============================================

int special_effect_t::max_stack() const
{
  if ( max_stacks != -1 )
    return max_stacks;

  if ( trigger() -> max_stacks() > 0 )
    return trigger() -> max_stacks();

  if ( driver() -> max_stacks() > 0 )
    return driver() -> max_stacks();

  return -1;
}


// special_effect_t::initialize_stat_buff ===================================

stat_buff_t* special_effect_t::initialize_stat_buff() const
{
  stat_buff_creator_t creator( player, name(), spell_data_t::nil(),
                               source == SPECIAL_EFFECT_SOURCE_ITEM ? item : 0 );

  // Setup the spell for the stat buff
  if ( trigger() -> id() > 0 )
    creator.spell( trigger() );
  else if ( driver() -> id() > 0 )
    creator.spell( driver() );

  // Setup user option overrides. Note that if there are no user set overrides,
  // the buff will automagically deduce correct options from the spell data,
  // the special_effect_t object should not issue overrides to the creator
  // object.
  if ( max_stacks > 0 )
    creator.max_stack( max_stacks );

  if ( duration_ > timespan_t::zero() )
    creator.duration( duration_ );

  if ( reverse )
    creator.reverse( true );

  if ( tick > timespan_t::zero() )
    creator.period( tick );

  // If user given stat is defined, override whatever the spell would contain
  if ( stat != STAT_NONE )
    creator.add_stat( stat, stat_amount );

  return creator;
}

bool special_effect_t::is_absorb_buff() const
{
  for ( size_t i = 1, end = trigger() -> effect_count(); i <= end; i++ )
  {
    const spelleffect_data_t& effect = trigger() -> effectN( i );
    if ( effect.id() == 0 )
      continue;

    if ( effect.subtype() == A_SCHOOL_ABSORB )
      return true;
  }

  return false;
}

absorb_buff_t* special_effect_t::initialize_absorb_buff() const
{
  absorb_buff_creator_t creator( player, name(), spell_data_t::nil(),
                               source == SPECIAL_EFFECT_SOURCE_ITEM ? item : 0 );

  // Setup the spell for the stat buff
  if ( trigger() -> id() > 0 )
    creator.spell( trigger() );
  else if ( driver() -> id() > 0 )
    creator.spell( driver() );

  // Setup user option overrides. Note that if there are no user set overrides,
  // the buff will automagically deduce correct options from the spell data,
  // the special_effect_t object should not issue overrides to the creator
  // object.
  if ( max_stacks > 0 )
    creator.max_stack( max_stacks );

  if ( duration_ > timespan_t::zero() )
    creator.duration( duration_ );

  if ( reverse )
    creator.reverse( true );

  if ( tick > timespan_t::zero() )
    creator.period( tick );

  return creator;
}

// special_effect_t::buff_type ==============================================

special_effect_buff_e special_effect_t::buff_type() const
{
  if ( type == SPECIAL_EFFECT_CUSTOM )
    return SPECIAL_EFFECT_BUFF_CUSTOM;
  else if ( custom_buff != 0 )
    return SPECIAL_EFFECT_BUFF_CUSTOM;
  else if ( is_stat_buff() )
    return SPECIAL_EFFECT_BUFF_STAT;
  else if ( is_absorb_buff() )
    return SPECIAL_EFFECT_BUFF_ABSORB;

  return SPECIAL_EFFECT_BUFF_NONE;
}

// special_effect_t::create_buff ============================================

buff_t* special_effect_t::create_buff() const
{
  switch ( buff_type() )
  {
    case SPECIAL_EFFECT_BUFF_CUSTOM:
      return custom_buff;
    case SPECIAL_EFFECT_BUFF_STAT:
      return initialize_stat_buff();
    case SPECIAL_EFFECT_BUFF_ABSORB:
      return initialize_absorb_buff();
    default:
      return 0;
  }
}

namespace {

struct proc_spell_t : public spell_t
{

  proc_spell_t( const std::string& token, player_t* p, const spell_data_t* s ) :
    spell_t( token, p, s )
  {
    background = true;
    // Periodic procs shouldnt ever haste ticks, probably
    hasted_ticks = false;
  }
};

struct proc_heal_t : public heal_t
{

  proc_heal_t( const std::string& token, player_t* p, const spell_data_t* s ) :
    heal_t( token, p, s )
  {
    background = true;
    // Periodic procs shouldnt ever haste ticks, probably
    hasted_ticks = false;
  }
};

struct proc_attack_t : public attack_t
{

  proc_attack_t( const std::string& token, player_t* p, const spell_data_t* s ) :
    attack_t( token, p, s )
  {
    background = true;
  }
};

}
action_t* special_effect_t::create_action() const
{
  switch ( action_type() )
  {
  case SPECIAL_EFFECT_ACTION_CUSTOM:
    return execute_action;
  case SPECIAL_EFFECT_ACTION_SPELL:
    return initialize_offensive_spell_action();
  case SPECIAL_EFFECT_ACTION_HEAL:
    return initialize_heal_action();
  case SPECIAL_EFFECT_ACTION_ATTACK:
    return initialize_attack_action();
  default:
    return nullptr;
  }
}

special_effect_action_e special_effect_t::action_type() const
{
  if ( type == SPECIAL_EFFECT_CUSTOM )
    return SPECIAL_EFFECT_ACTION_CUSTOM;
  else if ( execute_action != 0 )
    return SPECIAL_EFFECT_ACTION_CUSTOM;
  else if ( is_offensive_spell_action() )
    return SPECIAL_EFFECT_ACTION_SPELL;
  else if ( is_heal_action() )
    return SPECIAL_EFFECT_ACTION_HEAL;
  else if ( is_attack_action() )
    return SPECIAL_EFFECT_ACTION_ATTACK;

  return SPECIAL_EFFECT_ACTION_NONE;
}

bool special_effect_t::is_offensive_spell_action() const
{
  for ( size_t i = 1, end = trigger() -> effect_count(); i <= end; i++ )
  {
    const spelleffect_data_t& effect = trigger() -> effectN( i );
    if ( effect.id() == 0 )
      continue;

    if ( effect.type() == E_SCHOOL_DAMAGE || effect.type() == E_HEALTH_LEECH )
      return true;

    if ( effect.type() == E_APPLY_AURA )
    {
      if ( effect.subtype() == A_PERIODIC_DAMAGE || effect.subtype() == A_PERIODIC_LEECH )
        return true;
    }
  }

  return false;
}

spell_t* special_effect_t::initialize_offensive_spell_action() const
{
  const spell_data_t* s = spell_data_t::nil();

  // Setup the spell data
  if ( trigger() -> id() > 0 )
    s = trigger();
  else if ( driver() -> id() > 0 )
    s = driver();

  proc_spell_t* spell = new proc_spell_t( name(), player, s );
  spell -> init();
  return spell;
}

bool special_effect_t::is_heal_action() const
{
  for ( size_t i = 1, end = trigger() -> effect_count(); i <= end; i++ )
  {
    const spelleffect_data_t& effect = trigger() -> effectN( i );
    if ( effect.id() == 0 )
      continue;

    if ( effect.type() == E_HEAL )
      return true;

    if ( effect.type() == E_APPLY_AURA )
    {
      if ( effect.subtype() == A_PERIODIC_HEAL )
        return true;
    }
  }

  return false;
}

heal_t* special_effect_t::initialize_heal_action() const
{
  const spell_data_t* s = spell_data_t::nil();

  // Setup the spell data
  if ( trigger() -> id() > 0 )
    s = trigger();
  else if ( driver() -> id() > 0 )
    s = driver();

  proc_heal_t* heal = new proc_heal_t( name(), player, s );
  heal -> init();
  return heal;
}

bool special_effect_t::is_attack_action() const
{
  for ( size_t i = 1, end = trigger() -> effect_count(); i <= end; i++ )
  {
    const spelleffect_data_t& effect = trigger() -> effectN( i );
    if ( effect.id() == 0 )
      continue;

    if ( effect.type() == E_WEAPON_DAMAGE || effect.type() == E_WEAPON_PERCENT_DAMAGE )
      return true;
  }

  return false;
}

attack_t* special_effect_t::initialize_attack_action() const
{
  const spell_data_t* s = spell_data_t::nil();

  // Setup the spell data
  if ( trigger() -> id() > 0 )
    s = trigger();
  else if ( driver() -> id() > 0 )
    s = driver();

  proc_attack_t* attack = new proc_attack_t( name(), player, s );
  attack -> init();
  return attack;
}

// special_effect_t::proc_flags =============================================

unsigned special_effect_t::proc_flags() const
{
  if ( proc_flags_ != 0 )
    return proc_flags_;

  if ( driver() -> proc_flags() != 0 )
    return driver() -> proc_flags();

  return 0;
}

// special_effect_t::proc_flags2 ============================================

unsigned special_effect_t::proc_flags2() const
{
  // These do not have a spell-data equivalent, so they are going to always be
  // set manually, or (most commonly) default to 0. The new proc callback
  // system will automatically use "default values" appropriate for the proc,
  // if it is passed a zero proc_flags2. The default value is "spell landing",
  // as in a positive result of the spell type. For periodic effects and heals,
  // the system will require "an amount value", i.e., a healing or damaging
  // result.
  return proc_flags2_;
}

// special_effect_t::ppm ====================================================

// DBC data has no concept of "old style" procs per minute, they will be
// manually set by simc.
double special_effect_t::ppm() const
{
  if ( ppm_ != std::numeric_limits<double>::min() )
    return ppm_;

  return 0;
}

// special_effect_t::rppm ===================================================

double special_effect_t::rppm() const
{
  if ( ppm_ <= 0 && ppm_ != std::numeric_limits<double>::min() )
    return std::fabs( ppm_ );

  return driver() -> real_ppm();
}

// special_effect_t::cooldown ===============================================

// Cooldowns are automatically extracted from the driver spell. However, the
// "Cooldown" value is prioritized over "Internal Cooldown" in DBC data. In
// reality, there should not be any (single) driver, that includes both.
timespan_t special_effect_t::cooldown() const
{
  if ( cooldown_ > timespan_t::zero() )
    return cooldown_;

  // First, check item cooldowns, since they override (in simc) whatever the
  // spell cooldown may be
  if ( item )
  {
    for ( size_t i = 0, end = sizeof_array( item -> parsed.data.cooldown_category_duration ); i < end; i++ )
    {
      if ( item -> parsed.data.cooldown_category_duration[ i ] > 0 )
        return timespan_t::from_millis( item -> parsed.data.cooldown_category_duration[ i ] );
    }
  }

  if ( driver() -> cooldown() > timespan_t::zero() )
    return driver() -> cooldown();
  else if ( driver() -> internal_cooldown() > timespan_t::zero() )
    return driver() -> internal_cooldown();

  return timespan_t::zero();
}

// special_effect_t::duration ===============================================

timespan_t special_effect_t::duration() const
{
  if ( duration_ > timespan_t::zero() )
    return duration_;
  else if ( trigger() -> duration() > timespan_t::zero() )
    return trigger() -> duration();
  else if ( driver() -> duration() > timespan_t::zero() )
    return driver() -> duration();

  return timespan_t::zero();
}


// special_effect_t::tick_time ==============================================

// The tick time is selected from the first periodically executed effect of the
// _triggered_ spell. Periodically executing drivers are currently not
// supported. Note that ticking behavior has now moved to buff_t, instead of
// being kludged in the callback itself (as it was done in the
// "proc_callback_t" days).
timespan_t special_effect_t::tick_time() const
{
  if ( tick > timespan_t::zero() )
    return tick;

  // Search trigger for now, it's not likely that the driver is ticking
  for ( size_t i = 1, end = trigger() -> effect_count(); i <= end; i++ )
  {
    if ( trigger() -> effectN( i ).period() > timespan_t::zero() )
      return trigger() -> effectN( i ).period();
  }

  return timespan_t::zero();
}

// special_effect_t::proc_chance ============================================

double special_effect_t::proc_chance() const
{
  if ( proc_chance_ > 0 )
    return proc_chance_;

  return driver() -> proc_chance();
}

// special_effect_t::name ===================================================

std::string special_effect_t::name() const
{
  if ( ! name_str.empty() )
    return name_str;

  // Guess proc name based on spells.
  std::string n;
  // Use driver name, if it's not hidden or passive, or there's no trigger spell to use
  if ( ( ! driver() -> flags( SPELL_ATTR_HIDDEN ) && ! driver() -> flags( SPELL_ATTR_PASSIVE ) ) || 
       ! trigger() -> ok() )
    n = driver() -> name_cstr();
  // Driver is hidden, try to use the spell that the driver procs
  else if ( trigger() -> ok() )
    n = trigger() -> name_cstr();

  util::tokenize( n );

  // Assert on empty names; we do need a reasonable name for the proc.
  assert( ! n.empty() );

  // Append weapon suffix automatically to the name.
  // TODO: We need a "shared" mechanism here
  if ( item && item -> slot == SLOT_OFF_HAND )
    n += "_oh";

  return n;
}

// special_effect_t::to_string ==============================================

std::string special_effect_t::to_string() const
{
  std::ostringstream s;

  s << name();
  s << " type=" << util::special_effect_string( type );
  s << " source=" << util::special_effect_source_string( source );

  if ( ! trigger_str.empty() )
    s << " proc=" << trigger_str;

  if ( spell_id > 0 )
    s << " driver=" << spell_id;

  if ( trigger() -> ok() )
    s << " trigger=" << trigger() -> id();

  if ( is_stat_buff() )
  {
    s << " stat=" << util::stat_type_abbrev( stat );
    s << " amount=" << stat_amount;
    s << " duration=" << duration().total_seconds();
    if ( tick_time() != timespan_t::zero() )
      s << " tick=" << tick_time().total_seconds();
    if ( reverse )
      s << " Reverse";
  }

  if ( school != SCHOOL_NONE )
  {
    s << " school=" << util::school_type_string( school );
    s << " amount=" << discharge_amount;
    if ( discharge_scaling > 0 )
      s << " coeff=" << discharge_scaling;
  }

  if ( max_stacks > 0 )
    s << " max_stack=" << max_stacks;

  if ( proc_chance() > 0 )
    s << " proc_chance=" << proc_chance() * 100.0 << "%";

  if ( ppm() > 0 )
    s << " ppm=" << ppm();

  if ( rppm() > 0 )
  {
    s << " rppm=" << rppm();
    switch ( rppm_scale )
    {
      case RPPM_HASTE:
        s << " (Haste)";
        break;
      case RPPM_ATTACK_CRIT:
        s << " (AttackCrit)";
        break;
      case RPPM_SPELL_CRIT:
        s << " (SpellCrit)";
        break;
      default:
        break;
    }
  }

  if ( cooldown() > timespan_t::zero() )
  {
    if ( type == SPECIAL_EFFECT_EQUIP )
      s << " icd=";
    else if ( type == SPECIAL_EFFECT_USE )
      s << " cd=";
    else
      s << " (i)cd=";
    s << cooldown().total_seconds();
  }

  return s.str();
}

// proc::parse_special_effect ===============================================

bool proc::parse_special_effect_encoding( special_effect_t& effect,
                                          const item_t& item,
                                          const std::string& encoding )
{
  if ( encoding.empty() || encoding == "custom" || encoding == "none" ) return true;

  std::vector<item_database::token_t> tokens;

  size_t num_tokens = item_database::parse_tokens( tokens, encoding );

  effect.encoding_str = encoding;

  for ( size_t i = 0; i < num_tokens; i++ )
  {
    item_database::token_t& t = tokens[ i ];
    stat_e s;
    school_e sc;

    if ( ( s = util::parse_stat_type( t.name ) ) != STAT_NONE )
    {
      effect.stat = s;
      effect.stat_amount = t.value;
    }
    else if ( ( sc = util::parse_school_type( t.name ) ) != SCHOOL_NONE )
    {
      effect.school = sc;
      effect.discharge_amount = t.value;

      std::vector<std::string> splits = util::string_split( t.value_str, "+" );
      if ( splits.size() == 2 )
      {
        effect.discharge_amount  = atof( splits[ 0 ].c_str() );
        effect.discharge_scaling = atof( splits[ 1 ].c_str() ) / 100.0;
      }
    }
    else if ( t.name == "stacks" || t.name == "stack" )
    {
      effect.max_stacks = ( int ) t.value;
    }
    else if ( t.name == "%" )
    {
      effect.proc_chance_ = t.value / 100.0;
    }
    else if ( t.name == "ppm" )
    {
      effect.ppm_ = t.value;
    }
    else if ( util::str_prefix_ci( t.name, "rppm" ) )
    {
      if ( t.value != 0 )
        effect.ppm_ = -t.value;

      if ( util::str_in_str_ci( t.name, "spellcrit" ) )
        effect.rppm_scale = RPPM_SPELL_CRIT;
      else if ( util::str_in_str_ci( t.name, "attackcrit" ) )
        effect.rppm_scale = RPPM_ATTACK_CRIT;
      else if ( util::str_in_str_ci( t.name, "haste" ) )
        effect.rppm_scale = RPPM_HASTE;
      else
        effect.rppm_scale = RPPM_NONE;
    }
    else if ( t.name == "duration" || t.name == "dur" )
    {
      effect.duration_ = timespan_t::from_seconds( t.value );
    }
    else if ( t.name == "cooldown" || t.name == "cd" )
    {
      effect.cooldown_ = timespan_t::from_seconds( t.value );
    }
    else if ( t.name == "tick" )
    {
      effect.tick = timespan_t::from_seconds( t.value );
    }
    else if ( t.full == "reverse" )
    {
      effect.reverse = true;
    }
    else if ( t.full == "chance" )
    {
      effect.chance_to_discharge = true;
    }
    else if ( t.name == "costrd" )
    {
      effect.cost_reduction = true;
      effect.refresh = 0;
    }
    else if ( t.name == "nocrit" )
    {
      effect.override_result_es_mask |= RESULT_CRIT_MASK;
      effect.result_es_mask &= ~RESULT_CRIT_MASK;
    }
    else if ( t.name == "maycrit" )
    {
      effect.override_result_es_mask |= RESULT_CRIT_MASK;
      effect.result_es_mask |= RESULT_CRIT_MASK;
    }
    else if ( t.name == "nomiss" )
    {
      effect.override_result_es_mask |= RESULT_MISS_MASK;
      effect.result_es_mask &= ~RESULT_MISS_MASK;
    }
    else if ( t.name == "maymiss" )
    {
      effect.override_result_es_mask |= RESULT_MISS_MASK;
      effect.result_es_mask |= RESULT_MISS_MASK;
    }
    else if ( t.name == "nododge" )
    {
      effect.override_result_es_mask |= RESULT_DODGE_MASK;
      effect.result_es_mask &= ~RESULT_DODGE_MASK;
    }
    else if ( t.name == "maydodge" )
    {
      effect.override_result_es_mask |= RESULT_DODGE_MASK;
      effect.result_es_mask |= RESULT_DODGE_MASK;
    }
    else if ( t.name == "noparry" )
    {
      effect.override_result_es_mask |= RESULT_PARRY_MASK;
      effect.result_es_mask &= ~RESULT_PARRY_MASK;
    }
    else if ( t.name == "mayparry" )
    {
      effect.override_result_es_mask |= RESULT_PARRY_MASK;
      effect.result_es_mask |= RESULT_PARRY_MASK;
    }
    else if ( t.name == "norefresh" )
    {
      effect.refresh = 0;
    }
    else if ( t.name == "refresh" )
    {
      effect.refresh = 1;
    }
    else if ( t.full == "aoe" )
    {
      effect.aoe = -1;
    }
    else if ( t.name == "aoe" )
    {
      effect.aoe = ( int ) t.value;
      if ( effect.aoe < -1 )
        effect.aoe = -1;
    }
    else if ( t.name == "driver" )
    {
      effect.spell_id = static_cast<int>( t.value );
    }
    else if ( t.name == "trigger" )
    {
      effect.trigger_spell_id = static_cast<int>( t.value );
    }
    else if ( t.full == "ondamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = SCHOOL_ALL_MASK;
    }
    else if ( t.full == "onheal" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_HEAL;
      effect.trigger_mask = SCHOOL_ALL_MASK;
    }
    else if ( t.full == "ontickdamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_TICK_DAMAGE;
      effect.trigger_mask = SCHOOL_ALL_MASK;
    }
    else if ( t.full == "onspelltickdamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_SPELL_TICK_DAMAGE;
      effect.trigger_mask = SCHOOL_ALL_MASK;
    }
    else if ( t.full == "ondirectdamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DIRECT_DAMAGE;
      effect.trigger_mask = SCHOOL_ALL_MASK;
    }
    else if ( t.full == "ondirectcrit" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DIRECT_CRIT;
      effect.trigger_mask = SCHOOL_ALL_MASK;
    }
    else if ( t.full == "onspelldirectdamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_SPELL_DIRECT_DAMAGE;
      effect.trigger_mask = SCHOOL_ALL_MASK;
    }
    else if ( t.full == "ondirectharmfulspellhit" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DIRECT_HARMFUL_SPELL;
      effect.trigger_mask = RESULT_HIT_MASK;
    }
    else if ( t.full == "onspelldamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = SCHOOL_SPELL_MASK;
    }
    else if ( t.full == "onattackdamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = SCHOOL_ATTACK_MASK;
    }
    else if ( t.full == "onattacktickdamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_TICK_DAMAGE;
      effect.trigger_mask = SCHOOL_ATTACK_MASK;
    }
    else if ( t.full == "onattackdirectdamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DIRECT_DAMAGE;
      effect.trigger_mask = SCHOOL_ATTACK_MASK;
    }
    else if ( t.full == "onarcanedamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = ( int64_t( 1 ) << SCHOOL_ARCANE );
    }
    else if ( t.full == "onbleeddamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_TICK_DAMAGE;
      effect.trigger_mask = ( int64_t( 1 ) << SCHOOL_PHYSICAL );
    }
    else if ( t.full == "onchaosdamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = ( int64_t( 1 ) << SCHOOL_CHAOS );
    }
    else if ( t.full == "onfiredamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = ( int64_t( 1 ) << SCHOOL_FIRE );
    }
    else if ( t.full == "onfrostdamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = ( int64_t( 1 ) << SCHOOL_FROST );
    }
    else if ( t.full == "onfrostfiredamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = ( int64_t( 1 ) << SCHOOL_FROSTFIRE );
    }
    else if ( t.full == "onholydamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = ( int64_t( 1 ) << SCHOOL_HOLY );
    }
    else if ( t.full == "onnaturedamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = ( int64_t( 1 ) << SCHOOL_NATURE );
    }
    else if ( t.full == "onphysicaldamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = ( int64_t( 1 ) << SCHOOL_PHYSICAL );
    }
    else if ( t.full == "onshadowdamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = ( int64_t( 1 ) << SCHOOL_SHADOW );
    }
    else if ( t.full == "ondraindamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = ( int64_t( 1 ) << SCHOOL_DRAIN );
    }
    else if ( t.full == "ontick" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_TICK;
      effect.trigger_mask = RESULT_ALL_MASK;
    }
    else if ( t.full == "ontickhit" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_TICK;
      effect.trigger_mask = RESULT_HIT_MASK;
    }
    else if ( t.full == "ontickcrit" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_TICK;
      effect.trigger_mask = RESULT_CRIT_MASK;
    }
    else if ( t.full == "onspellcast" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_SPELL;
      effect.trigger_mask = RESULT_NONE_MASK;
    }
    else if ( t.full == "onspellhit" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_SPELL;
      effect.trigger_mask = RESULT_HIT_MASK;
    }
    else if ( t.full == "onspellcrit" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_SPELL_AND_TICK;
      effect.trigger_mask = RESULT_CRIT_MASK;
    }
    else if ( t.full == "onspelltickcrit" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_TICK;
      effect.trigger_mask = RESULT_CRIT_MASK;
    }
    else if ( t.full == "onspelldirectcrit" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_SPELL;
      effect.trigger_mask = RESULT_CRIT_MASK;
    }
    else if ( t.full == "onspellmiss" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_SPELL;
      effect.trigger_mask = RESULT_MISS_MASK;
    }
    else if ( t.full == "onharmfulspellcast" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_HARMFUL_SPELL;
      effect.trigger_mask = RESULT_NONE_MASK;
    }
    else if ( t.full == "onharmfulspellhit" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_HARMFUL_SPELL_LANDING;
      effect.trigger_mask = RESULT_HIT_MASK;
    }
    else if ( t.full == "onharmfulspellcrit" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_HARMFUL_SPELL;
      effect.trigger_mask = RESULT_CRIT_MASK;
    }
    else if ( t.full == "onharmfulspellmiss" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_HARMFUL_SPELL_LANDING;
      effect.trigger_mask = RESULT_MISS_MASK;
    }
    else if ( t.full == "onhealcast" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_HEAL_SPELL;
      effect.trigger_mask = RESULT_NONE_MASK;
    }
    else if ( t.full == "onhealhit" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_HEAL_SPELL;
      effect.trigger_mask = RESULT_HIT_MASK;
    }
    else if ( t.full == "onhealdirectcrit" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_HEAL_SPELL;
      effect.trigger_mask = RESULT_CRIT_MASK;
    }
    else if ( t.full == "onhealmiss" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_HEAL_SPELL;
      effect.trigger_mask = RESULT_MISS_MASK;
    }
    else if ( t.full == "onattack" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_ATTACK;
      effect.trigger_mask = RESULT_ALL_MASK;
    }
    else if ( t.full == "onattackhit" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_ATTACK;
      effect.trigger_mask = RESULT_HIT_MASK;
    }
    else if ( t.full == "onattackcrit" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_ATTACK;
      effect.trigger_mask = RESULT_CRIT_MASK;
    }
    else if ( t.full == "onattackmiss" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_ATTACK;
      effect.trigger_mask = RESULT_MISS_MASK;
    }
    else if ( t.full == "onspelldamageheal" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE_HEAL;
      effect.trigger_mask = SCHOOL_SPELL_MASK;
    }
    else if ( t.full == "ondamagehealspellcast" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_DAMAGE_HEAL_SPELL;
      effect.trigger_mask = RESULT_NONE_MASK;
    }
    else if ( util::str_in_str_ci( t.full, "procby" ) )
      parse_proc_flags( t.full, __proc_opts, effect.proc_flags_ );
    else if ( util::str_in_str_ci( t.full, "procon" ) )
      parse_proc_flags( t.full, __proc2_opts, effect.proc_flags2_ );
    else
    {
      item.sim -> errorf( "Player %s has unknown 'use/equip=' token '%s' at slot %s\n", item.player -> name(), t.full.c_str(), item.slot_name() );
      return false;
    }
  }

  return true;
}

/**
 * Ensure that the proc has enough information to actually proc in simc. This
 * essentially requires a non-zero proc_chance or PPM field in effect, or the
 * driver spell to contain a non-zero RPPM or proc chance. Additionally, we
 * need to have some sort of flags indicating what events trigger the proc.
 *
 * TODO: Action support
 */
bool proc::usable_proc( const special_effect_t& effect )
{
  // Valid proc flags (either old- or new style), we can proc this effect.
  if ( effect.trigger_type == PROC_NONE && effect.trigger_mask == 0 && effect.proc_flags() == 0 )
  {
    if ( effect.item && effect.item -> sim -> debug )
      effect.item -> sim -> out_debug.printf( "%s unusable proc %s: No proc flags / trigger type",
          effect.item -> player -> name(), effect.name().c_str() );
    return false;
  }
    
  // A non-zero chance to proc it through one of the proc chance triggers
  if ( effect.ppm() == 0 && effect.rppm() == 0 && effect.proc_chance() == 0 )
  {
    if ( effect.item && effect.item -> sim -> debug )
      effect.item -> sim -> out_debug.printf( "%s unusable proc %s: No RPPM / PPM / Proc chance",
          effect.item -> player -> name(), effect.name().c_str() );
    return false;
  }

  // Require a valid buff or an action
  if ( effect.buff_type() == SPECIAL_EFFECT_BUFF_NONE && effect.action_type() == SPECIAL_EFFECT_ACTION_NONE )
  {
    if ( effect.item && effect.item -> sim -> debug )
      effect.item -> sim -> out_debug.printf( "%s unusable proc %s: No constructible buff or action",
          effect.item -> player -> name(), effect.name().c_str() );
    return false;
  }

  return true;
}

/**
 * Initialize the proc callback. This method is called by each actor through
 * player_t::register_callbacks(), which is invoked as the last thing in the
 * actor initialization process.
 */
void dbc_proc_callback_t::initialize()
{
  if ( listener -> sim -> debug )
    listener -> sim -> out_debug.printf( "Initializing proc %s: %s",
        effect.name().c_str(), effect.to_string().c_str() );
  
  // Initialize proc chance triggers. Note that this code only chooses one, and
  // prioritizes RPPM > PPM > proc chance.
  if ( effect.rppm() > 0 )
    rppm = real_ppm_t( *listener, effect.rppm(), effect.rppm_scale, effect.driver() -> id() );
  else if ( effect.ppm() > 0 )
    ppm = effect.ppm();
  else if ( effect.proc_chance() != 0 )
    proc_chance = effect.proc_chance();

  // Initialize cooldown, if applicable
  if ( effect.cooldown() > timespan_t::zero() )
  {
    cooldown = listener -> get_cooldown( effect.cooldown_name() );
    cooldown -> duration = effect.cooldown();
  }

  // Initialize proc action
  proc_action = effect.create_action();

  // Initialize the potential proc buff through special_effect_t. Can return 0,
  // in which case the proc does not trigger a buff.
  proc_buff = effect.create_buff();
  
  if ( effect.weapon_proc && effect.item )
  {
    weapon = effect.item -> weapon();
  }
  // TODO: Create action (through special_effect_t)
  
  // Register callback to new proc system
  listener -> callbacks.register_callback( effect.proc_flags(), effect.proc_flags2(), this );
}

/**
 * Cooldown name needs some special handling. Cooldowns in WoW are global,
 * regardless of the number of procs (i.e., weapon enchants). Thus, we straight
 * up use the driver's name, or override it with the special_effect_t name_str
 * if it's defined.
 */
std::string special_effect_t::cooldown_name() const
{
  if ( ! name_str.empty() )
  {
    assert( name_str.size() );
    return name_str;
  }

  std::string n = driver() -> name_cstr();
  assert( ! n.empty() );
  util::tokenize( n );
  // Append the spell ID of the driver to the cooldown name. In some cases, the
  // drivers of different trinket procs are actually named identically, causing
  // issues when the trinkets are worn.
  n += "_" + util::to_string( driver() -> id() );

  return n;
}

const item_t dbc_proc_callback_t::default_item_ = item_t();

