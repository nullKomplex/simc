// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

// ==========================================================================
// Druid
// ==========================================================================

struct druid_t : public player_t
{
  // Active
  action_t* active_fury_swipes;
  action_t* active_t10_4pc_caster_dot;

  // Auto-attacks
  attack_t* cat_melee_attack;
  attack_t* bear_melee_attack;

  double equipped_weapon_dps;

  struct primal_madness_buff_t;
  struct tigers_fury_buff_t;
  struct berserk_buff_t;

  struct primal_madness_buff_t : public buff_t
  {
    double energy_value;
    double rage_value;

    primal_madness_buff_t( player_t* player ) : 
      buff_t( player, "primal_madness", 1 ), energy_value( 0.0 ), rage_value( 0.0 )
    {
      druid_t* p = player -> cast_druid();

      if ( p -> talents.primal_madness -> rank() > 0 )
      {
        uint32_t sid = ( p -> talents.primal_madness -> rank() == 2 ) ? 80886 : 80879;
        passive_spell_t spell_primal_madness_energy( p, "primal_madness_energy", sid );
        energy_value = spell_primal_madness_energy.base_value( E_APPLY_AURA, A_MOD_INCREASE_ENERGY );
        rage_value = p -> talents.primal_madness -> base_value( E_APPLY_AURA, A_PROC_TRIGGER_SPELL_WITH_VALUE ) * 0.1;
      }
    };

    virtual void aura_loss()
    {
      druid_t* p = player -> cast_druid();

      buff_t::aura_loss();

      if ( p -> buffs_cat_form -> check() )
      {
        p -> stat_loss( STAT_MAX_ENERGY, energy_value );
      }
    }

    virtual void aura_gain()
    {
      druid_t* p = player -> cast_druid();

      buff_t::aura_gain();

      if ( p -> buffs_cat_form -> check() )
      {
        p -> stat_gain( STAT_MAX_ENERGY, energy_value, p -> gains_primal_madness );
      }
    }

    virtual bool trigger( int stacks=1, double value=-1.0, double chance=-1.0 )
    {
      druid_t* p = player -> cast_druid();

      stacks = 1;
      value  = energy_value;
      chance = p -> talents.primal_madness -> rank(); 
      return buff_t::trigger( stacks, value, chance );
    }
  };

  struct berserk_buff_t : public buff_t
  {
    berserk_buff_t( player_t* player ) : 
      buff_t( player, "berserk", 1, 15.0 + player -> cast_druid() -> glyphs.berserk -> mod_additive( P_DURATION ) )
    {
    }

    virtual void aura_loss()
    {
      druid_t* p = player -> cast_druid();

      buff_t::aura_loss();

      if ( ! p -> buffs_tigers_fury -> check() )
      {
        p -> buffs_primal_madness -> expire();
      }
    }

    virtual void aura_gain()
    {
      druid_t* p = player -> cast_druid();

      buff_t::aura_gain();

      if ( ! p -> buffs_tigers_fury -> check() )
      {
        p -> buffs_primal_madness -> trigger();
      }
    }
  };

  struct tigers_fury_buff_t : public buff_t
  {
    double dmg_value;

    tigers_fury_buff_t( player_t* player ) : 
      buff_t( player, "tigers_fury", 1, 6.0 ), dmg_value( 0.0 )
    {
    }

    virtual void aura_loss()
    {
      druid_t* p = player -> cast_druid();

      buff_t::aura_loss();

      if ( ! p -> buffs_berserk -> check() )
      {
        p -> buffs_primal_madness -> expire();
      }
    }

    virtual void aura_gain()
    {
      druid_t* p = player -> cast_druid();

      buff_t::aura_gain();

      if ( ! p -> buffs_berserk -> check() )
      {
        p -> buffs_primal_madness -> trigger();
      }

      if ( p -> talents.king_of_the_jungle -> rank() )
      {
        p -> resource_gain( RESOURCE_ENERGY, p -> talents.king_of_the_jungle -> effect_base_value( 2 ), p -> gains_tigers_fury );
      }
    }
  };

  // Buffs
  buff_t* buffs_bear_form;
  buff_t* buffs_cat_form;
  buff_t* buffs_combo_points;
  buff_t* buffs_eclipse_lunar;
  buff_t* buffs_eclipse_solar;
  buff_t* buffs_enrage;
  buff_t* buffs_glyph_of_innervate;
  buff_t* buffs_lacerate;
  buff_t* buffs_lunar_shower;
  buff_t* buffs_moonkin_form;
  buff_t* buffs_natures_grace;
  buff_t* buffs_natures_swiftness;
  buff_t* buffs_omen_of_clarity;
  buff_t* buffs_pulverize;
  buff_t* buffs_savage_roar;
  buff_t* buffs_shooting_stars;
  buff_t* buffs_stampede_bear;
  buff_t* buffs_stampede_cat;
  buff_t* buffs_stealthed;
  buff_t* buffs_t10_2pc_caster;
  buff_t* buffs_t11_4pc_caster;
  buff_t* buffs_t11_4pc_melee;
  buff_t* buffs_wild_mushroom;
  berserk_buff_t*        buffs_berserk;
  primal_madness_buff_t* buffs_primal_madness;
  tigers_fury_buff_t*    buffs_tigers_fury;

  // Cooldowns
  cooldown_t* cooldowns_mangle_bear;
  cooldown_t* cooldowns_fury_swipes;

  // DoTs
  dot_t* dots_insect_swarm;
  dot_t* dots_moonfire;
  dot_t* dots_rake;
  dot_t* dots_rip;
  dot_t* dots_sunfire;

  // Gains
  gain_t* gains_bear_melee;
  gain_t* gains_energy_refund;
  gain_t* gains_enrage;
  gain_t* gains_euphoria;
  gain_t* gains_glyph_of_innervate;
  gain_t* gains_incoming_damage;
  gain_t* gains_moonkin_form;
  gain_t* gains_natural_reaction;
  gain_t* gains_omen_of_clarity;
  gain_t* gains_primal_fury;
  gain_t* gains_primal_madness;
  gain_t* gains_tigers_fury;

  // Glyphs
  struct glyphs_t
  {
    glyph_t* berserk;
    glyph_t* ferocious_bite;
    glyph_t* focus;
    glyph_t* innervate;
    glyph_t* insect_swarm;
    glyph_t* lacerate;
    glyph_t* mangle;
    glyph_t* mark_of_the_wild;
    glyph_t* maul;
    glyph_t* monsoon;
    glyph_t* moonfire;
    glyph_t* rip;
    glyph_t* savage_roar;
    glyph_t* shred;
    glyph_t* starfall;
    glyph_t* starfire;
    glyph_t* starsurge;
    glyph_t* tigers_fury;
    glyph_t* typhoon;
    glyph_t* wrath;

    glyphs_t() { memset( ( void* ) this, 0x0, sizeof( glyphs_t ) ); }
  };
  glyphs_t glyphs;

  // Procs
  proc_t* procs_combo_points_wasted;
  proc_t* procs_fury_swipes;
  proc_t* procs_parry_haste;
  proc_t* procs_primal_fury;

  // Random Number Generation
  rng_t* rng_euphoria;
  rng_t* rng_fury_swipes;
  rng_t* rng_blood_in_the_water;
  rng_t* rng_primal_fury;
  rng_t* rng_wrath_eclipsegain;
  
  // Tree specialization passives
  // Balance
  passive_spell_t* spec_moonfury;
  mastery_t* mastery_total_eclipse; // Mastery
  int eclipse_bar_value; // Tracking the current value of the eclipse bar
  int eclipse_bar_direction; // Tracking the current direction of the eclipse bar
  
  // Feral
  passive_spell_t* spec_aggression;
  passive_spell_t* spec_vengeance;
  mastery_t* mastery_razor_claws; // Mastery
  mastery_t* mastery_savage_defender; // Mastery

  // Up-Times
  uptime_t* uptimes_energy_cap;
  uptime_t* uptimes_rage_cap;

  // Talents
  struct talents_t 
  {
    // Balance
    talent_t* balance_of_power;
    talent_t* earth_and_moon;
    talent_t* euphoria;
    talent_t* dreamstate;
    talent_t* force_of_nature;
    talent_t* fungal_growth;
    talent_t* gale_winds;
    talent_t* genesis;
    talent_t* lunar_shower;
    talent_t* moonglow;
    talent_t* moonkin_form;
    talent_t* natures_grace;
    talent_t* natures_majesty;
    talent_t* owlkin_frenzy;
    talent_t* shooting_stars;
    talent_t* solar_beam;
    talent_t* starfall;
    talent_t* starlight_wrath;
    talent_t* sunfire;
    talent_t* typhoon;
    
    // Feral  
    talent_t* berserk;
    talent_t* blood_in_the_water;
    talent_t* brutal_impact;
    talent_t* endless_carnage;
    talent_t* feral_aggression;
    talent_t* feral_charge;
    talent_t* feral_swiftness;
    talent_t* furor;
    talent_t* fury_swipes;
    talent_t* infected_wounds;
    talent_t* king_of_the_jungle;
    talent_t* leader_of_the_pack;
    talent_t* natural_reaction;
    talent_t* nurturing_instict; // NYI as we don't simulate healing
    talent_t* predatory_strikes;
    talent_t* primal_fury;
    talent_t* primal_madness;
    talent_t* pulverize; // NYI
    talent_t* rend_and_tear;
    talent_t* stampede;
    talent_t* survival_instincts; // NYI as we don't simulate damage
    talent_t* thick_hide; // How does the +10% and +33%@bear stack etc
    
    // Resto
    talent_t* blessing_of_the_grove;
    talent_t* fury_of_stormrage; // NYI
    talent_t* heart_of_the_wild;
    talent_t* master_shapeshifter;
    talent_t* natures_swiftness;

    talents_t() { memset( ( void* ) this, 0x0, sizeof( talents_t ) ); }
  };
  talents_t talents;

  druid_t( sim_t* sim, const std::string& name, race_type r = RACE_NONE ) : player_t( sim, DRUID, name, r )
  {
    if ( race == RACE_NONE ) race = RACE_NIGHT_ELF;

    tree_type[ DRUID_BALANCE     ] = TREE_BALANCE;
    tree_type[ DRUID_FERAL       ] = TREE_FERAL;
    tree_type[ DRUID_RESTORATION ] = TREE_RESTORATION;

    active_fury_swipes        = NULL;
    active_t10_4pc_caster_dot = 0;
    
    eclipse_bar_value     = 0;
    eclipse_bar_direction = 0;

    cooldowns_mangle_bear = get_cooldown( "mangle_bear" );
    cooldowns_fury_swipes = get_cooldown( "fury_swipes" );

    distance = 30;

    dots_rip          = get_dot( "rip"          );
    dots_rake         = get_dot( "rake"         );
    dots_insect_swarm = get_dot( "insect_swarm" );
    dots_moonfire     = get_dot( "moonfire"     );
    dots_sunfire      = get_dot( "sunfire"      );

    cat_melee_attack = 0;
    bear_melee_attack = 0;

    equipped_weapon_dps = 0; 

    create_talents();
    create_glyphs();
    create_options();
  }

  // Character Definition
  virtual void      init_talents();
  virtual void      init_spells();
  virtual void      init_base();
  virtual void      init_buffs();
  virtual void      init_scaling();
  virtual void      init_gains();
  virtual void      init_procs();
  virtual void      init_uptimes();
  virtual void      init_rng();
  virtual void      init_actions();
  virtual void      reset();
  virtual void      interrupt();
  virtual void      clear_debuffs();
  virtual void      regen( double periodicity );
  virtual double    available() SC_CONST;
  virtual double    composite_attack_power() SC_CONST;
  virtual double    composite_attack_power_multiplier() SC_CONST;
  virtual double    composite_attack_crit() SC_CONST;
  virtual double    composite_player_multiplier( const school_type school ) SC_CONST;
  virtual double    composite_spell_hit() SC_CONST;
  virtual double    composite_spell_crit() SC_CONST;
  virtual double    composite_attribute_multiplier( int attr ) SC_CONST;
  virtual double    matching_gear_multiplier( const attribute_type attr ) SC_CONST;
  virtual double    composite_block_value() SC_CONST { return 0; }
  virtual double    composite_tank_parry() SC_CONST { return 0; }
  virtual double    composite_tank_block() SC_CONST { return 0; }
  virtual double    composite_tank_crit( const school_type school ) SC_CONST;
  virtual action_expr_t* create_expression( action_t*, const std::string& name );
  virtual action_t* create_action( const std::string& name, const std::string& options );
  virtual pet_t*    create_pet( const std::string& name, const std::string& type = std::string() );
  virtual void      create_pets();
  virtual int       decode_set( item_t& item );
  virtual int       primary_resource() SC_CONST;
  virtual int       primary_role() SC_CONST;
  virtual int       target_swing();

  // Utilities
  double combo_point_rank( double* cp_list ) SC_CONST
  {
    assert( buffs_combo_points -> check() );
    return cp_list[ buffs_combo_points -> stack() - 1 ];
  }

  double combo_point_rank( double cp1, double cp2, double cp3, double cp4, double cp5 ) SC_CONST
  {
    double cp_list[] = { cp1, cp2, cp3, cp4, cp5 };
    return combo_point_rank( cp_list );
  }

  void reset_gcd()
  {
    for ( action_t* a=action_list; a; a = a -> next )
    {
      if ( a -> trigger_gcd != 0 ) a -> trigger_gcd = base_gcd;
    }
  }
};

namespace { // ANONYMOUS NAMESPACE ==========================================

// ==========================================================================
// Druid Cat Attack
// ==========================================================================

struct druid_cat_attack_t : public attack_t
{
  int requires_stealth;
  int requires_position;
  bool requires_combo_points;
  int adds_combo_points;

  druid_cat_attack_t( const char* n, player_t* player, const school_type s=SCHOOL_PHYSICAL, int t=TREE_NONE, bool special=true ) :
      attack_t( n, player, RESOURCE_ENERGY, s, t, special ),
      requires_stealth( 0 ),
      requires_position( POSITION_NONE ),
      requires_combo_points( false ),
      adds_combo_points( 0 )
  {
    tick_may_crit = true;
  }

  druid_cat_attack_t( const char* n, uint32_t id, druid_t* p, bool special = true ) :
      attack_t( n, id, p, 0, special ), 
      adds_combo_points( 0 )
  {
    adds_combo_points     = (int) base_value( E_ADD_COMBO_POINTS );
    may_crit              = true;
    tick_may_crit         = true;
    requires_combo_points = false;
    requires_position     = POSITION_NONE;
    requires_stealth      = false;
  }

  virtual void   parse_options( option_t*, const std::string& options_str );
  virtual double cost() SC_CONST;
  virtual void   execute();
  virtual void   consume_resource();
  virtual void   player_buff();
  virtual bool   ready();
};

// ==========================================================================
// Druid Bear Attack
// ==========================================================================

struct druid_bear_attack_t : public attack_t
{
  druid_bear_attack_t( const char* n, player_t* player, const school_type s=SCHOOL_PHYSICAL, int t=TREE_NONE, bool special=true ) :
      attack_t( n, player, RESOURCE_RAGE, s, t, true )
  {
  }

  druid_bear_attack_t( const char* n, uint32_t id, druid_t* p, bool special = true ) :
      attack_t( n, id, p, 0, special )
  {
    may_crit      = true;
    tick_may_crit = true;
  }

  virtual void   parse_options( option_t*, const std::string& options_str );
  virtual double cost() SC_CONST;
  virtual void   execute();
  virtual double haste() SC_CONST;
  virtual void   consume_resource();
  virtual void   player_buff();
};

// ==========================================================================
// Druid Spell
// ==========================================================================

struct druid_spell_t : public spell_t
{
  double additive_multiplier;

  druid_spell_t( const char* n, player_t* p, const school_type s, int t ) :
      spell_t( n, p, RESOURCE_MANA, s, t ), additive_multiplier( 0.0 )
  {
  }

   druid_spell_t( const char* n, uint32_t id, druid_t* p ) :
      spell_t( n, id, p, 0 ), additive_multiplier( 0.0 )
  {
    may_crit      = true;
    tick_may_crit = true;
  }

  virtual void   consume_resource();
  virtual double cost() SC_CONST;
  virtual double cost_reduction() SC_CONST;
  virtual void   execute();
  virtual double execute_time() SC_CONST;
  virtual double haste() SC_CONST;
  virtual void   parse_options( option_t*, const std::string& options_str );
  virtual void   player_buff();
  virtual void   schedule_execute();
};

// ==========================================================================
// Pet Treants
// ==========================================================================

struct treants_pet_t : public pet_t
{
  struct melee_t : public attack_t
  {
    melee_t( player_t* player ) :
        attack_t( "treant_melee", player )
    {
      //druid_t* o = player -> cast_pet() -> owner -> cast_druid();

      weapon = &( player -> main_hand_weapon );
      base_execute_time = weapon -> swing_time;
      base_dd_min = base_dd_max = 1;
      background = true;
      repeating = true;
      may_crit = true;

      // Model the three Treants as one actor hitting 3x hard
      base_multiplier *= 3.0;
    }
  };

  melee_t* melee;

  treants_pet_t( sim_t* sim, player_t* owner, const std::string& pet_name ) :
      pet_t( sim, owner, pet_name ), melee( 0 )
  {
    main_hand_weapon.type       = WEAPON_BEAST;
    main_hand_weapon.min_dmg    = 580;
    main_hand_weapon.max_dmg    = 580;
    main_hand_weapon.damage     = ( main_hand_weapon.min_dmg + main_hand_weapon.max_dmg ) / 2;
    main_hand_weapon.swing_time = 1.65;
  }
  virtual void init_base()
  {
    pet_t::init_base();

    // At 85 base AP of 932
    attribute_base[ ATTR_STRENGTH  ] = 476;
    attribute_base[ ATTR_AGILITY   ] = 113;
    attribute_base[ ATTR_STAMINA   ] = 361;
    attribute_base[ ATTR_INTELLECT ] = 65;
    attribute_base[ ATTR_SPIRIT    ] = 109;

    base_attack_crit  = .05;
    base_attack_power = -20;
    initial_attack_power_per_strength = 2.0;

    melee = new melee_t( this );
  }
  virtual double composite_attack_power() SC_CONST
  {
    double ap = pet_t::composite_attack_power();
    ap += 0.57 * owner -> composite_spell_power( SCHOOL_MAX );
    return ap;
  }
  virtual double composite_attack_hit() SC_CONST
  {
    return owner -> composite_spell_hit();
  }
  virtual double composite_attack_expertise() SC_CONST
  {
    // Hit scales that if they are hit capped, you're expertise capped.
    return owner -> composite_spell_hit() * 26.0 / 17.0;
  }
  virtual void schedule_ready( double delta_time=0,
                               bool   waiting=false )
  {
    pet_t::schedule_ready( delta_time, waiting );
    if ( ! melee -> execute_event ) melee -> execute();
  }
  virtual void interrupt()
  {
    pet_t::interrupt();
    melee -> cancel();
  }
};

// add_combo_point ==========================================================

static void add_combo_point( druid_t* p )
{
  if ( p -> buffs_combo_points -> current_stack == 5 )
  {
    p -> procs_combo_points_wasted -> occur();
    return;
  }

  p -> buffs_combo_points -> trigger();
}

// trigger_earth_and_moon ===================================================

static void trigger_earth_and_moon( spell_t* s )
{
  druid_t* p = s -> player -> cast_druid();

  if ( p -> talents.earth_and_moon -> rank() == 0 ) return;

  s -> target -> debuffs.earth_and_moon -> trigger( 1, 8 );
  s -> target -> debuffs.earth_and_moon -> source = p;
}

// trigger_eclipse_proc =====================================================

static void trigger_eclipse_proc( druid_t* p )
{
  // All extra procs when eclipse pops
  p -> resource_gain( RESOURCE_MANA, p -> resource_max[ RESOURCE_MANA ] * 0.01 * p -> talents.euphoria -> effect_base_value( 3 ) , p -> gains_euphoria );
  p -> buffs_t11_4pc_caster -> trigger( 3 );
  p -> buffs_natures_grace -> cooldown -> reset();
  
  // When eclipse procs the direction of the bar switches!
  p -> eclipse_bar_direction = - p -> eclipse_bar_direction;
}

// trigger_eclipse_energy_gain ==============================================

static void trigger_eclipse_energy_gain( spell_t* s, int gain )
{
  if ( gain == 0 ) return;
  
  druid_t* p = s -> player -> cast_druid();

  if ( p -> eclipse_bar_direction == 0 )
  {
    // No eclipse gained at all by now => Start of the encounter
    // Set the direction to the direction of the first gain!
    p -> eclipse_bar_direction = ( gain > 0 ? 1 : -1 );
  }
  else
  {
    // If the gain isn't not alligned with the direction of the bar it won't happen
    // +*+ == -*- => >0 => fine
    // -*+ == +*- => <0 => Not alligned, no gain
    if ( p -> eclipse_bar_direction * gain < 0 )
      return;
  }

  int old_eclipse_bar_value = p -> eclipse_bar_value;
  p -> eclipse_bar_value += gain;

  if ( p -> eclipse_bar_value < 0 ) 
  {
    p -> buffs_eclipse_solar -> expire();

    if ( p -> eclipse_bar_value < -100 ) 
      p -> eclipse_bar_value = -100;
  }

  if ( p -> eclipse_bar_value > 0 ) 
  {
    p -> buffs_eclipse_lunar -> expire();

    if ( p -> eclipse_bar_value > 100 ) 
      p -> eclipse_bar_value = 100;
  }
  
  int actual_gain = p -> eclipse_bar_value - old_eclipse_bar_value;
  if ( s -> sim -> log )
  {
    log_t::output( s -> sim, "%s gains %.2f (%.2f) %s from %s (%.2f)",
                   p -> name(), actual_gain, gain,
                   "Eclipse", s -> name(),
                   p -> eclipse_bar_value );
  }
  
  
  // Eclipse proc:
  // Procs when you reach 100 and only then, you have to have an
  // actual gain of eclipse energy (bar value)
  if ( actual_gain != 0 )
  {
    if ( p -> eclipse_bar_value == 100 ) 
    {
      if ( p -> buffs_eclipse_solar -> trigger() )
        trigger_eclipse_proc( p );
    }
    else if ( p -> eclipse_bar_value == -100 ) 
    {
      if ( p -> buffs_eclipse_lunar -> trigger() )
        trigger_eclipse_proc( p );
    }
  }
}

// trigger_energy_refund ====================================================

static void trigger_energy_refund( druid_cat_attack_t* a )
{
  druid_t* p = a -> player -> cast_druid();

  double energy_restored = a -> resource_consumed * 0.80;

  p -> resource_gain( RESOURCE_ENERGY, energy_restored, p -> gains_energy_refund );
}

// trigger_fury_swipes ======================================================

static void trigger_fury_swipes( action_t* a )
{
  druid_t* p = a -> player -> cast_druid();

  if ( a -> proc ) return;

  if ( ! a -> weapon ) return;

  if ( ! p -> talents.fury_swipes -> rank() )
    return;

  if ( p -> cooldowns_fury_swipes -> remains() > 0 )
    return;

  if ( p -> rng_fury_swipes -> roll( p -> talents.fury_swipes -> proc_chance() ) )
  {
    if ( ! p -> active_fury_swipes )
    {
      struct fury_swipes_t : public druid_cat_attack_t
      {
        fury_swipes_t( druid_t* player ) :
            druid_cat_attack_t( "fury_swipes", 80861, player )
        {
          background  = true; 
          proc        = true;
          trigger_gcd = 0;
          reset();
        }
      };
      p -> active_fury_swipes = new fury_swipes_t( p );
    }

    p -> active_fury_swipes    -> execute();
    p -> cooldowns_fury_swipes -> start( 3.0 );
    p -> procs_fury_swipes     -> occur();
  }
}

// trigger_infected_wounds ==================================================

static void trigger_infected_wounds( action_t* a )
{
  druid_t* p = a -> player -> cast_druid();

  if ( p -> talents.infected_wounds -> rank() )
  {
    a -> target -> debuffs.infected_wounds -> trigger();
    a -> target -> debuffs.infected_wounds -> source = p;
  }
}

// trigger_omen_of_clarity ==================================================

static void trigger_omen_of_clarity( action_t* a )
{
  druid_t* p = a -> player -> cast_druid();

  if ( a -> proc ) return;

  if ( p -> buffs_omen_of_clarity -> trigger() )
    p -> buffs_t10_2pc_caster -> trigger( 1, 0.15 );

}

// trigger_primal_fury (Bear) ===============================================

static void trigger_primal_fury( druid_bear_attack_t* a )
{
  druid_t* p = a -> player -> cast_druid();

  if ( ! p -> talents.primal_fury -> rank() )
    return;

  if ( p -> rng_primal_fury -> roll( p -> talents.primal_fury -> rank() * 0.5 ) )
  {
    p -> resource_gain( RESOURCE_RAGE, 5.0, p -> gains_primal_fury );
  }
}

// trigger_primal_fury (Cat) ================================================

static void trigger_primal_fury( druid_cat_attack_t* a )
{
  druid_t* p = a -> player -> cast_druid();

  if ( ! p -> talents.primal_fury -> rank() )
    return;

  if ( ! a -> adds_combo_points )
    return;

  if ( p -> rng_primal_fury -> roll( p -> talents.primal_fury -> rank() * 0.5 ) )
  {
    add_combo_point( p );
    p -> procs_primal_fury -> occur();
  }
}

// trigger_rage_gain ========================================================

static void trigger_rage_gain( druid_bear_attack_t* a )
{
  druid_t* p = a -> player -> cast_druid();

  double rage_gain = 0;

  p -> resource_gain( RESOURCE_RAGE, rage_gain, p -> gains_bear_melee );
}

// trigger_t10_4pc_caster ===================================================

static void trigger_t10_4pc_caster( player_t* player, double direct_dmg, int school )
{
  druid_t* p = player -> cast_druid();

  if ( ! p -> set_bonus.tier10_4pc_caster() ) return;

  struct t10_4pc_caster_dot_t : public druid_spell_t
  {
    t10_4pc_caster_dot_t( player_t* player ) : druid_spell_t( "tier10_4pc_balance", player, SCHOOL_NATURE, TREE_BALANCE )
    {
      may_miss        = false;
      may_resist      = false;
      background      = true;
      proc            = true;
      trigger_gcd     = 0;
      base_cost       = 0;
      base_multiplier = 1.0;
      tick_power_mod  = 0;
      base_tick_time  = 2.0;
      num_ticks       = 2;
      hasted_ticks    = false;
      id              = 71023;
      reset();
    }
    void player_buff() {}
    void target_debuff( player_t* t, int dmg_type ) {}
  };

  if ( ! p -> active_t10_4pc_caster_dot ) p -> active_t10_4pc_caster_dot = new t10_4pc_caster_dot_t( p );

  double dmg = direct_dmg * 0.07;

  dot_t* dot = p -> active_t10_4pc_caster_dot -> dot;

  if ( dot -> ticking )
  {
    dmg +=  p -> active_t10_4pc_caster_dot -> base_td * dot -> ticks();

    p -> active_t10_4pc_caster_dot -> cancel();
  }

  p -> active_t10_4pc_caster_dot -> base_td = dmg / p -> active_t10_4pc_caster_dot -> num_ticks;
  p -> active_t10_4pc_caster_dot -> execute();
}

// ==========================================================================
// Druid Cat Attack
// ==========================================================================

// druid_cat_attack_t::parse_options ========================================

void druid_cat_attack_t::parse_options( option_t*          options,
                                        const std::string& options_str )
{
  option_t base_options[] =
  {
    { NULL, OPT_UNKNOWN, NULL }
  };
  std::vector<option_t> merged_options;
  attack_t::parse_options( merge_options( merged_options, options, base_options ), options_str );
}

// druid_cat_attack_t::cost =================================================

double druid_cat_attack_t::cost() SC_CONST
{
  druid_t* p = player -> cast_druid();
  double c = attack_t::cost();
  if ( c == 0 ) return 0;
  if ( harmful &&  p -> buffs_omen_of_clarity -> check() ) return 0;
  if ( p -> buffs_berserk -> check() ) c *= 1.0 + p -> talents.berserk -> effect_base_value( 1 ) / 100.0;
  return c;
}

// druid_cat_attack_t::consume_resource =====================================

void druid_cat_attack_t::consume_resource()
{
  druid_t* p = player -> cast_druid();
  attack_t::consume_resource();
  if ( harmful && p -> buffs_omen_of_clarity -> up() )
  {
    // Treat the savings like a energy gain.
    double amount = attack_t::cost();
    if ( amount > 0 )
    {
      p -> gains_omen_of_clarity -> add( amount );
      p -> buffs_omen_of_clarity -> expire();
    }
  }
}

// druid_cat_attack_t::execute ==============================================

void druid_cat_attack_t::execute()
{
  druid_t* p = player -> cast_druid();

  attack_t::execute();

  if ( result_is_hit() )
  {
    if ( requires_combo_points ) p -> buffs_combo_points -> expire();
    if (     adds_combo_points ) add_combo_point ( p );

    if ( result == RESULT_CRIT )
    {
      trigger_primal_fury( this );
    }
  }
  else
  {
    trigger_energy_refund( this );
  }

  if ( harmful ) p -> buffs_stealthed -> expire();
}

// druid_cat_attack_t::player_buff ==========================================

void druid_cat_attack_t::player_buff()
{
  druid_t* p = player -> cast_druid();

  attack_t::player_buff();

  player_multiplier *= 1.0 + p -> buffs_tigers_fury -> value();
  
  if ( school == SCHOOL_BLEED )
    player_multiplier *= 1.0 + p -> mastery_razor_claws -> base_value( E_APPLY_AURA, A_DUMMY ) * p -> composite_mastery();
}

// druid_cat_attack_t::ready ================================================

bool druid_cat_attack_t::ready()
{
  if ( ! attack_t::ready() )
    return false;

  druid_t*  p = player -> cast_druid();

  if ( ! p -> buffs_cat_form -> check() )
    return false;

  if ( requires_position != POSITION_NONE )
    if ( p -> position != requires_position )
      return false;

  if ( requires_stealth )
    if ( ! p -> buffs_stealthed -> check() )
      return false;

  if ( requires_combo_points && ! p -> buffs_combo_points -> check() )
    return false;

  return true;
}

// Cat Melee Attack =========================================================

struct cat_melee_t : public druid_cat_attack_t
{
  cat_melee_t( player_t* player ) :
    druid_cat_attack_t( "cat_melee", player, SCHOOL_PHYSICAL, TREE_NONE, /*special*/false )
  {
    background  = true;
    repeating   = true;
    may_crit    = true;
    trigger_gcd = 0;
    base_cost   = 0;
  }

  virtual void player_buff()
  {
    druid_cat_attack_t::player_buff();
    druid_t* p = player -> cast_druid();
    player_multiplier *= 1.0 + p -> buffs_savage_roar -> value();
  }

  virtual double execute_time() SC_CONST
  {
    if ( ! player -> in_combat )
      return 0.01;

    return druid_cat_attack_t::execute_time();
  }

  virtual void execute()
  {
    druid_cat_attack_t::execute();
    if ( result_is_hit() )
    {
      trigger_fury_swipes( this );
      trigger_omen_of_clarity( this );
    }
  }
};

// Claw =====================================================================

struct claw_t : public druid_cat_attack_t
{
  claw_t( druid_t* player, const std::string& options_str ) :
    druid_cat_attack_t( "claw", 1082, player )
  {
    parse_options( NULL, options_str );
  }
};

// Feral Charge (Cat) ======================================================

struct feral_charge_cat_t : public druid_cat_attack_t
{
  double stampede_cost_reduction, stampede_duration;

  feral_charge_cat_t( druid_t* player, const std::string& options_str ) :
      druid_cat_attack_t( "feral_charge_cat", 49376, player ), stampede_cost_reduction( 0.0 ), stampede_duration( 0.0 )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );

    if ( p -> talents.stampede -> rank() )
    {
      uint32_t sid = ( p -> talents.stampede -> rank() == 2 ) ? 81022 : 81021;
      passive_spell_t stampede_spell( p, "stampede_cat_spell", sid );
      stampede_cost_reduction = stampede_spell.mod_additive( P_RESOURCE_COST );
      // 81022 is bugged and says FLAT_MODIFIER not PCT_MODIFIER so adjust it
      if ( sid == 81022 )
      {
        stampede_cost_reduction /= 100.0;
      }
      stampede_cost_reduction = -stampede_cost_reduction;
      stampede_duration = stampede_spell.duration();
    }

    p -> buffs_stampede_cat -> buff_duration = stampede_duration;

    may_miss   = false;
    may_dodge  = false;
    may_parry  = false;
    may_block  = false;
    may_glance = false;
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();

    druid_cat_attack_t::execute();
    
    p -> buffs_stampede_cat -> trigger( 1, stampede_cost_reduction );
  }

  virtual bool ready()
  {
    bool ranged = ( player -> position == POSITION_RANGED_FRONT || 
		    player -> position == POSITION_RANGED_BACK );

    if ( player -> in_combat && ! ranged )
    {
      return false;
    }
    
    return druid_cat_attack_t::ready();
  }
};

// Ferocious Bite ===========================================================

struct ferocious_bite_t : public druid_cat_attack_t
{
  double base_dmg_per_point;
  double excess_energy;

  ferocious_bite_t( druid_t* player, const std::string& options_str ) :
      druid_cat_attack_t( "ferocious_bite", 22568, player )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );

    base_dmg_per_point    = p -> player_data.effect_bonus( p -> player_data.spell_effect_id( id, 1 ), p -> type, p -> level);
    base_multiplier      *= 1.0 + p -> talents.feral_aggression -> mod_additive( P_GENERIC );
    requires_combo_points = true;
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();

    base_dd_adder    = base_dmg_per_point * p -> buffs_combo_points -> stack();
    direct_power_mod = 0.109 * p -> buffs_combo_points -> stack();

    // consumes up to 35 additional energy to increase damage by up to 100%.
    // Assume 100/35 = 2.857% per additional energy consumed
    
    // Glyph: Your Ferocious Bite ability no longer converts extra energy into additional damage.
    if ( p -> glyphs.ferocious_bite -> enabled() )
    {
      excess_energy = 0;
    }
    else
    {
      excess_energy = ( p -> resource_current[ RESOURCE_ENERGY ] - druid_cat_attack_t::cost() );
  
      if ( excess_energy > 35 )
      {
        excess_energy = 35;
      }
      else if ( excess_energy < 0 )
      {
        excess_energy = 0;
      }
    }

    druid_cat_attack_t::execute();

    if ( result_is_hit() && target -> health_percentage() <= p -> talents.blood_in_the_water -> base_value() )
    {
      if ( p -> dots_rip -> ticking && p -> rng_blood_in_the_water -> roll( p -> talents.blood_in_the_water -> proc_chance() ) )
      {
        p -> dots_rip -> action -> refresh_duration();
      }
    }
  }

  virtual void consume_resource()
  {
    // Ferocious Bite consumes 35+x energy, with 0 <= x <= 35.
    // Consumes the base_cost and handles Omen of Clarity
    druid_cat_attack_t::consume_resource();

    if ( result_is_hit() )
    {
      // Let the additional energy consumption create it's own debug log entries.
      if ( sim -> debug )
        log_t::output( sim, "%s consumes an additional %.1f %s for %s", player -> name(),
                       excess_energy, util_t::resource_type_string( resource ), name() );

      player -> resource_loss( resource, excess_energy );
      stats -> consume_resource( excess_energy );
    }
  }

  virtual void player_buff()
  {
    druid_t* p = player -> cast_druid();

    druid_cat_attack_t::player_buff();

    player_multiplier *= 1.0 + excess_energy / 35.0;

    if ( target -> debuffs.bleeding -> check() )
    {
      player_crit += 0.01 * p -> talents.rend_and_tear -> effect_base_value( 2 );
    }
  }
};

// Maim =====================================================================

struct maim_t : public druid_cat_attack_t
{
  double base_dmg_per_point;

  maim_t( druid_t* player, const std::string& options_str ) :
    druid_cat_attack_t( "maim", 22570, player )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );
    
    base_dmg_per_point    = p -> player_data.effect_bonus( p -> player_data.spell_effect_id( id, 1 ), p -> type, p -> level);
    requires_combo_points = true;
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();

    base_dd_adder = base_dmg_per_point * p -> buffs_combo_points -> stack();

    druid_cat_attack_t::execute();
  }
};

// Mangle (Cat) =============================================================

struct mangle_cat_t : public druid_cat_attack_t
{
  mangle_cat_t( druid_t* player, const std::string& options_str ) :
      druid_cat_attack_t( "mangle_cat", 33876, player )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );

    adds_combo_points = 1; // Not in the DBC
    base_multiplier  *= 1.0 + p -> glyphs.mangle -> mod_additive( P_GENERIC );
  }

  virtual void execute()
  {
    druid_cat_attack_t::execute();
    if ( result_is_hit() )
    {
      druid_t* p = player -> cast_druid();
      target -> debuffs.mangle -> trigger();
      target -> debuffs.mangle -> source = p;
      trigger_infected_wounds( this );
      p -> buffs_t11_4pc_melee -> trigger();
    }
  }
};

// Pounce ===================================================================

struct pounce_bleed_t : public druid_cat_attack_t
{
  pounce_bleed_t( druid_t* player ) :
    druid_cat_attack_t( "pounce_bleed", 9007, player )
  {
    dual           = true;
    background     = true;
    stats          = player -> get_stats( "pounce" );
    tick_power_mod = 0; // FIXME: Does pounce scale?
  }

  virtual void tick()
  {
    druid_cat_attack_t::tick();
    update_stats( DMG_OVER_TIME );
  }
};

struct pounce_t : public druid_cat_attack_t
{
  pounce_bleed_t* pounce_bleed;

  pounce_t( druid_t* player, const std::string& options_str ) :
    druid_cat_attack_t( "pounce", 9005, player )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );

    requires_stealth = true;
    pounce_bleed     = new pounce_bleed_t( p );
  }

  virtual void execute()
  {
    druid_cat_attack_t::execute();
    if ( result_is_hit() )
      pounce_bleed -> execute();
  }
};

// Rake =====================================================================

struct rake_t : public druid_cat_attack_t
{
  rake_t( druid_t* player, const std::string& options_str ) :
    druid_cat_attack_t( "rake", 1822, player )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );

    dot_behavior        = DOT_REFRESH;
    direct_power_mod    = 0.23;
    tick_power_mod      = 0.14; // 0.42 / 3 ticks
    num_ticks          += p -> talents.endless_carnage -> rank();
    base_td_multiplier *= 1.0 + p -> sets -> set( SET_T11_2PC_MELEE ) -> mod_additive( P_GENERIC );
  }
};

// Ravage ===================================================================

struct ravage_t : public druid_cat_attack_t
{
  ravage_t( druid_t* player, const std::string& options_str ) :
    druid_cat_attack_t( "ravage", 6785, player )
  {
    parse_options( NULL, options_str );

    requires_position = POSITION_BACK;
    requires_stealth  = true;
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();

    druid_cat_attack_t::execute();

    p -> buffs_stampede_cat -> expire();
    requires_stealth = true;

    if ( result_is_hit() )
      trigger_infected_wounds( this );
  }

  virtual double cost() SC_CONST
  {
    druid_t* p = player -> cast_druid();

    double c = druid_cat_attack_t::cost();

    c *= 1.0 - p -> buffs_stampede_cat -> value();

    return c;
  }

  virtual void consume_resource()
  {
    druid_t* p = player -> cast_druid();
    attack_t::consume_resource();
    if ( p -> buffs_omen_of_clarity -> up() && ! p -> buffs_stampede_cat -> check() )
    {
      // Treat the savings like a energy gain.
      double amount = attack_t::cost();
      if ( amount > 0 )
      {
        p -> gains_omen_of_clarity -> add( amount );
        p -> buffs_omen_of_clarity -> expire();
      }
    }
  }

  virtual void player_buff()
  {
    druid_t* p = player -> cast_druid();
    if ( p -> talents.predatory_strikes -> rank() )
    {
      if ( target -> health_percentage() >= p -> talents.predatory_strikes -> effect_base_value( 2 ) )
        player_crit += p -> talents.predatory_strikes -> effect_base_value( 1 ) / 100.0;
    }

    druid_cat_attack_t::player_buff();
  }

  virtual bool ready()
  {
    druid_t* p = player -> cast_druid();

    if ( p -> buffs_stampede_cat -> up() )
    {
      requires_stealth = false;
    }

    return druid_cat_attack_t::ready();  
  }
};

// Rip ======================================================================

struct rip_t : public druid_cat_attack_t
{
  double base_dmg_per_point;
  double ap_per_point;

  rip_t( druid_t* player, const std::string& options_str ) :
    druid_cat_attack_t( "rip", 1079, player )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );

    base_dmg_per_point    = p -> player_data.effect_bonus( p -> player_data.spell_effect_id( id, 1 ), p -> type, p -> level);
    ap_per_point          = 0.023;
    requires_combo_points = true;
    base_multiplier      *= 1.0 + p -> glyphs.rip -> mod_additive( P_TICK_DAMAGE );

    if ( p -> set_bonus.tier10_2pc_melee() )
      base_cost -= 10;
  }
  
  virtual void execute()
  {
    // We have to save these values for refreshes by Blood in the Water, so
    // we simply reset them to zeroes on each execute and check them in ::player_buff.
    base_td        = 0.0;
    tick_power_mod = 0.0;

    druid_cat_attack_t::execute();
  }

  virtual void player_buff()
  {
    druid_t* p = player -> cast_druid();
    if ( base_td == 0.0 && tick_power_mod == 0.0 )
    {
      base_td        = base_td_init + p -> buffs_combo_points -> stack() * base_dmg_per_point;
      tick_power_mod = p -> buffs_combo_points -> stack() * ap_per_point;
    }

    druid_cat_attack_t::player_buff();
  }

  virtual bool ready()
  {
    druid_t* p = player -> cast_druid();

    if ( p -> dots_rip -> ticking )
    {
      double b = base_td_init + p -> buffs_combo_points -> stack() * base_dmg_per_point;
      double t = p -> buffs_combo_points -> stack() * ap_per_point;
      double current_value = b + t * p -> composite_attack_power();
      double saved_value = base_td + tick_power_mod * player_attack_power;

      if ( current_value < saved_value )
      {
        // A more powerful spell is active message.
        // Note we're making the assumption that's it is based off the simple sum of: basedmg + tick_mod * AP
        // We know it doesn't involve crit at all due to testing. It's unsure if player_multiplier will be needed or not.
        // Or if it's based off some other set of rules.
        return false;
      }
    }
    return druid_cat_attack_t::ready();
  }
};

// Savage Roar ==============================================================

struct savage_roar_t : public druid_cat_attack_t
{
  double buff_value;
  savage_roar_t( druid_t* player, const std::string& options_str ) :
    druid_cat_attack_t( "savage_roar", 52610, player )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );

    buff_value            = 0.50; // 30 in the DBC with no scaling factors
    buff_value           += p -> glyphs.savage_roar ->base_value() / 100.0;
    harmful               = false;
    requires_combo_points = true;
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();
    double duration = 9.0 + 5.0 * p -> buffs_combo_points -> stack();
    duration += p -> talents.endless_carnage -> effect_base_value( 2 ) / 1000.0;

    // execute clears CP, so has to be after calculation duration
    druid_cat_attack_t::execute();

    p -> buffs_savage_roar -> buff_duration = duration;
    p -> buffs_savage_roar -> trigger( 1, buff_value );
  }
};

// Shred ====================================================================

struct shred_t : public druid_cat_attack_t
{
  int extend_rip;

  shred_t( druid_t* player, const std::string& options_str ) :
    druid_cat_attack_t( "shred", 5221, player ),
    extend_rip( 0 )
  {
    option_t options[] =
    {
      { "extend_rip", OPT_BOOL, &extend_rip },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    requires_position  = POSITION_BACK;
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();
    druid_cat_attack_t::execute();
    if ( result_is_hit() )
    {
      if ( p -> glyphs.shred -> enabled() &&
           p -> dots_rip -> ticking  &&
           p -> dots_rip -> added_ticks < 4 )
      {
        // Glyph adds 1/1/2 ticks on execute
        int extra_ticks = ( p -> dots_rip -> added_ticks < 2 ) ? 1 : 2;
        p -> dots_rip -> action -> extend_duration( extra_ticks );
      }
      trigger_infected_wounds( this );
    }
  }

  virtual void target_debuff( player_t* t, int dmg_type )
  {
    druid_cat_attack_t::target_debuff( t, dmg_type );

    druid_t* p = player -> cast_druid();    
    
    if ( t -> debuffs.mangle -> up() || t -> debuffs.blood_frenzy_bleed -> up() || t -> debuffs.hemorrhage -> up() )
      target_multiplier *= 1.30;

    if ( t -> debuffs.bleeding -> up() )
      target_multiplier *= 1.0 + 0.01 * p -> talents.rend_and_tear -> effect_base_value( 1 );
  }

  virtual bool ready()
  {
    druid_t* p = player -> cast_druid();

    if ( extend_rip )
      if ( ! p -> glyphs.shred -> enabled() ||
           ! p -> dots_rip -> ticking ||
           ( p -> dots_rip -> added_ticks == 4 ) )
        return false;

    return druid_cat_attack_t::ready();
  }
};

// Skull Bash (Cat) =========================================================

struct skull_bash_cat_t : public druid_cat_attack_t
{
  skull_bash_cat_t( druid_t* player, const std::string& options_str ) :
    druid_cat_attack_t( "skull_bash_cat", 22570, player )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );

    cooldown -> duration  += p -> talents.brutal_impact -> effect_base_value( 3 ) / 1000.0;
  }

  virtual bool ready()
  {
    if ( ! target -> debuffs.casting -> check() )
      return false;

    return druid_cat_attack_t::ready();
  }
};

// Swipe (Cat) ==============================================================

struct swipe_cat_t : public druid_cat_attack_t
{
  swipe_cat_t( druid_t* player, const std::string& options_str ) :
    druid_cat_attack_t( "swipe_cat", 62078, player )
  {
    parse_options( NULL, options_str );
    
    aoe = -1;
  }
};

// Tigers Fury ==============================================================

struct tigers_fury_t : public druid_cat_attack_t
{
  tigers_fury_t( druid_t* player, const std::string& options_str ) :
      druid_cat_attack_t( "tigers_fury", 5217, player )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );

    harmful = false;
    cooldown -> duration += p -> glyphs.tigers_fury -> mod_additive( P_COOLDOWN );
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();

    druid_cat_attack_t::execute();

    p -> buffs_tigers_fury -> trigger( 1, effect_base_value( 1 ) / 100.0 );
  }

  virtual bool ready()
  {
    druid_t* p = player -> cast_druid();

    if ( p -> buffs_berserk -> check() )
      return false;

    return druid_cat_attack_t::ready();
  }
};

// ==========================================================================
// Druid Bear Attack
// ==========================================================================

// druid_bear_attack_t::parse_options ======================================

void druid_bear_attack_t::parse_options( option_t*          options,
                                         const std::string& options_str )
{
  option_t base_options[] =
  {
    { NULL, OPT_UNKNOWN, NULL }
  };
  std::vector<option_t> merged_options;
  attack_t::parse_options( merge_options( merged_options, options, base_options ), options_str );
}

// druid_bear_attack_t::cost ===============================================

double druid_bear_attack_t::cost() SC_CONST
{
  druid_t* p = player -> cast_druid();
  double c = attack_t::cost();
  if ( harmful && p -> buffs_omen_of_clarity -> check() ) return 0;
  return c;
}

// druid_bear_attack_t::consume_resource ===================================

void druid_bear_attack_t::consume_resource()
{
  druid_t* p = player -> cast_druid();
  attack_t::consume_resource();
  if ( harmful && p -> buffs_omen_of_clarity -> up() )
  {
    // Treat the savings like a rage gain.
    double amount = attack_t::cost();
    if ( amount > 0 )
    {
      p -> gains_omen_of_clarity -> add( amount );
      p -> buffs_omen_of_clarity -> expire();
    }
  }
}

// druid_bear_attack_t::execute ============================================

void druid_bear_attack_t::execute()
{
  attack_t::execute();
  if ( result_is_hit() )
  {
    if ( result == RESULT_CRIT )
    {
      trigger_primal_fury( this );
    }
  }
}

double druid_bear_attack_t::haste() SC_CONST
{
  druid_t* p = player -> cast_druid();

  double h = attack_t::haste();
  
  if ( p -> buffs_stampede_bear -> up() )
  {
    h *= p -> buffs_stampede_bear -> value();
  }

  return h;
}

// druid_bear_attack_t::player_buff ========================================

void druid_bear_attack_t::player_buff()
{
  druid_t* p = player -> cast_druid();
  attack_t::player_buff();

  if ( p -> talents.master_shapeshifter -> rank() )
  {
    player_multiplier *= 1.0 + p -> talents.master_shapeshifter -> base_value() * 0.01;
  }
  if ( p -> talents.king_of_the_jungle -> rank() && p -> buffs_enrage -> up() )
  {
    player_multiplier *= 1.0 + p -> talents.king_of_the_jungle -> effect_base_value( 1 ) / 100.0;
  }
}

// Bear Melee Attack =======================================================

struct bear_melee_t : public druid_bear_attack_t
{
  bear_melee_t( player_t* player ) :
    druid_bear_attack_t( "bear_melee", player, SCHOOL_PHYSICAL, TREE_NONE, /*special*/false )
  {
    background  = true;
    repeating   = true;
    trigger_gcd = 0;
    base_cost   = 0;
    may_crit    = true;
  }

  virtual double execute_time() SC_CONST
  {
    if ( ! player -> in_combat ) return 0.01;
    return druid_bear_attack_t::execute_time();
  }

  virtual void execute()
  {
    druid_bear_attack_t::execute();
    if ( result_is_hit() )
    {
      trigger_fury_swipes( this );
      trigger_rage_gain( this );
      trigger_omen_of_clarity( this );
    }
  }
};

// Feral Charge (Bear) ======================================================

struct feral_charge_bear_t : public druid_bear_attack_t
{
  double stampede_haste, stampede_duration;

  feral_charge_bear_t( druid_t* player, const std::string& options_str ) :
      druid_bear_attack_t( "feral_charge_bear", 16979, player ), stampede_haste( 0.0 ), stampede_duration( 0.0 )
  {
    druid_t* p = player -> cast_druid();
  
    parse_options( NULL, options_str );

    if ( p -> talents.stampede -> rank() )
    {
      uint32_t sid = ( p -> talents.stampede -> rank() == 2 ) ? 78893 : 78892;
      passive_spell_t stampede_spell( p, "stampede_bear_spell", sid );
      stampede_haste = stampede_spell.base_value( E_APPLY_AURA, A_MOD_HASTE ) / 100.0;
      stampede_duration = stampede_spell.duration();
    }

    stampede_haste = 1.0 / ( 1.0 + stampede_haste );
    p -> buffs_stampede_bear -> buff_duration = stampede_duration;

    may_miss   = false;
    may_dodge  = false;
    may_parry  = false;
    may_block  = false;
    may_glance = false;
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();

    druid_bear_attack_t::execute();
    
    p -> buffs_stampede_bear -> trigger( 1, stampede_haste );
  }

  virtual bool ready()
  {
    bool ranged = ( player -> position == POSITION_RANGED_FRONT || 
		    player -> position == POSITION_RANGED_BACK );

    if ( player -> in_combat && ! ranged )
    {
      return false;
    }
    
    return druid_bear_attack_t::ready();
  }
};


// Lacerate ================================================================

struct lacerate_t : public druid_bear_attack_t
{
  lacerate_t( druid_t* player, const std::string& options_str ) :
      druid_bear_attack_t( "lacerate",  33745, player )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );

    direct_power_mod = 0.115;
    tick_power_mod   = 0.0077;
    dot_behavior     = DOT_REFRESH;
    base_crit       += p -> glyphs.lacerate -> mod_additive( P_CRIT );
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();
    druid_bear_attack_t::execute();
    if ( result_is_hit() )
    {
      p -> buffs_lacerate -> trigger();
      base_td_multiplier  = p -> buffs_lacerate -> current_stack;
      base_td_multiplier *= 1.0 + p -> sets -> set( SET_T11_2PC_MELEE ) -> mod_additive( P_GENERIC );
    }
  }

  virtual void last_tick()
  {
    druid_t* p = player -> cast_druid();
    druid_bear_attack_t::last_tick();
    p -> buffs_lacerate -> expire();
  }
};

// Mangle (Bear) ===========================================================

struct mangle_bear_t : public druid_bear_attack_t
{
  mangle_bear_t( druid_t* player, const std::string& options_str ) :
      druid_bear_attack_t( "mangle_bear", 33878, player )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );

    base_multiplier *= 1.0 + p -> glyphs.mangle -> mod_additive( P_GENERIC );
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();
    druid_bear_attack_t::execute();
    if ( p -> buffs_berserk -> up() )
      cooldown -> reset();
    if ( result_is_hit() )
    {
      target -> debuffs.mangle -> trigger();
      trigger_infected_wounds( this );
    }
  }
};

// Maul ====================================================================

struct maul_t : public druid_bear_attack_t
{
  maul_t( druid_t* player, const std::string& options_str ) :
      druid_bear_attack_t( "maul",  6807, player )
  {
    parse_options( NULL, options_str );
    druid_t*  p = player -> cast_druid();

    weapon = &( player -> main_hand_weapon );
    
    aoe = 1;
    base_add_multiplier = p -> glyphs.maul -> effect_base_value( 3 ) / 100.0;
  }

  virtual void execute()
  {
    druid_bear_attack_t::execute();
    if ( result_is_hit() )
    {
      // trigger_omen_of_clarity( this ); //FIX ME is this still true?
      trigger_infected_wounds( this );
    }
  }

  virtual void player_buff()
  {
    druid_t*  p = player -> cast_druid();
    player_t* t = target;

    druid_bear_attack_t::player_buff();

    // FIXME: What are target debuffs doing in player_buff???

    if ( t -> debuffs.mangle -> up() || t -> debuffs.blood_frenzy_bleed -> up() || t -> debuffs.hemorrhage -> up() )
      player_multiplier *= 1.30;

    if ( t -> debuffs.bleeding -> check() )
    {
      player_multiplier *= 1.0 + p -> talents.rend_and_tear -> effect_base_value( 1 ) / 100.0;
    }
  }
};

// Skull Bash (Bear) ========================================================

struct skull_bash_bear_t : public druid_bear_attack_t
{
  skull_bash_bear_t( druid_t* player, const std::string& options_str ) :
    druid_bear_attack_t( "skull_bash_bear", 80964, player )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );

    cooldown -> duration  += p -> talents.brutal_impact -> effect_base_value( 2 ) / 1000.0;
  }

  virtual bool ready()
  {
    if ( ! target -> debuffs.casting -> check() )
      return false;

    return druid_bear_attack_t::ready();
  }
};

// Swipe (Bear) ============================================================

struct swipe_bear_t : public druid_bear_attack_t
{
  swipe_bear_t( druid_t* player, const std::string& options_str ) :
      druid_bear_attack_t( "swipe_bear", 779, player )
  {
    parse_options( NULL, options_str );

    aoe               = -1;
    direct_power_mod  = extra_coeff();
    weapon            = &( player -> main_hand_weapon );
    weapon_multiplier = 0;
  }
};

// ==========================================================================
// Druid Spell
// ==========================================================================

// druid_spell_t::parse_options ============================================

void druid_spell_t::parse_options( option_t*          options,
                                   const std::string& options_str )
{
  option_t base_options[] =
  {
    { NULL, OPT_UNKNOWN, NULL }
  };
  std::vector<option_t> merged_options;
  spell_t::parse_options( merge_options( merged_options, options, base_options ), options_str );
}

// druid_spell_t::cost_reduction ===========================================

double druid_spell_t::cost_reduction() SC_CONST
{
  druid_t* p = player -> cast_druid();
  double   cr = 0.0;
  cr += p -> talents.moonglow -> base_value();
  return cr;
}

// druid_spell_t::cost =====================================================

double druid_spell_t::cost() SC_CONST
{
  druid_t* p = player -> cast_druid();
  if ( harmful && p -> buffs_omen_of_clarity -> check() ) return 0;
  double c = spell_t::cost();
  c *= 1.0 + cost_reduction();
  if ( c < 0 ) c = 0.0;  
  return c;
}

// druid_spell_t::haste ====================================================

double druid_spell_t::haste() SC_CONST
{
  druid_t* p = player -> cast_druid();
  double h =  spell_t::haste();

  h *= 1.0 / ( 1.0 +  p -> buffs_natures_grace -> value() );

  return h;
}

// druid_spell_t::execute_time =============================================

double druid_spell_t::execute_time() SC_CONST
{
  druid_t* p = player -> cast_druid();
  if ( p -> buffs_natures_swiftness -> check() )
    return 0;

  return spell_t::execute_time();
}

// druid_spell_t::schedule_execute =========================================

void druid_spell_t::schedule_execute()
{
  druid_t* p = player -> cast_druid();

  spell_t::schedule_execute();

  if ( base_execute_time > 0 )
  {
    p -> buffs_natures_swiftness -> expire();
  }
}

// druid_spell_t::execute ==================================================

void druid_spell_t::execute()
{
  druid_t* p = player -> cast_druid();

  spell_t::execute();

  if ( result == RESULT_CRIT )
    p -> buffs_t11_4pc_caster -> decrement();

  if ( ! aoe )
  {
    trigger_omen_of_clarity( this );
  }
}

// druid_spell_t::consume_resource =========================================

void druid_spell_t::consume_resource()
{
  druid_t* p = player -> cast_druid();
  spell_t::consume_resource();
  if ( harmful && p -> buffs_omen_of_clarity -> up() )
  {
    // Treat the savings like a mana gain.
    double amount = spell_t::cost();
    if ( amount > 0 )
    {
      p -> gains_omen_of_clarity -> add( amount );
      p -> buffs_omen_of_clarity -> expire();
    }
  }
}

// druid_spell_t::player_buff ==============================================

void druid_spell_t::player_buff()
{
  druid_t* p = player -> cast_druid();

  spell_t::player_buff();

  if ( p -> buffs_moonkin_form -> check() )
    player_multiplier *= 1.0 + p -> talents.master_shapeshifter -> base_value() * 0.01;

  if ( school == SCHOOL_ARCANE || school == SCHOOL_NATURE || school == SCHOOL_SPELLSTORM )
  {
    player_multiplier *= 1.0 + 0.01 * p -> talents.balance_of_power -> effect_base_value( 1 );
    
    player_multiplier *= 1.0 + p -> buffs_t10_2pc_caster -> value();

    // Moonfury is actually additive with other player_multipliers, like glyphs, etc.
    if ( p -> spec_moonfury -> ok() )
      additive_multiplier += p -> spec_moonfury -> mod_additive( P_GENERIC );

    if ( p -> buffs_moonkin_form -> check() )
      player_multiplier *= 1.10;
  }

  player_multiplier *= 1.0 + p -> talents.earth_and_moon -> effect_base_value( 2 ) / 100.0;

  // Add in Additive Multipliers
  player_multiplier *= 1.0 + additive_multiplier;
  // Reset Additive_Multiplier
  additive_multiplier = 0.0;

  player_crit += 0.33 * p -> buffs_t11_4pc_caster -> stack();
}

// Auto Attack =============================================================

struct auto_attack_t : public action_t
{
  auto_attack_t( player_t* player, const std::string& options_str ) :
    action_t( ACTION_OTHER, "auto_attack", player )
  {
    trigger_gcd = 0;
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();
    p -> main_hand_attack -> weapon = &( p -> main_hand_weapon );
    p -> main_hand_attack -> base_execute_time = p -> main_hand_weapon.swing_time;
    p -> main_hand_attack -> schedule_execute();
  }

  virtual bool ready()
  {
    druid_t* p = player -> cast_druid();
    if ( p -> buffs.moving -> check() )
      return false;

    return( p -> main_hand_attack -> execute_event == 0 ); // not swinging
  }
};

// Bear Form Spell ========================================================

struct bear_form_t : public druid_spell_t
{
  bear_form_t( druid_t* player, const std::string& options_str ) :
      druid_spell_t( "bear_form", 5487, player )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );

    // Override these as we can do it before combat
    trigger_gcd       = 0;
    base_execute_time = 0;
    harmful           = false;

    if ( ! p -> bear_melee_attack )
      p -> bear_melee_attack = new bear_melee_t( p );
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();

    spell_t::execute();

    weapon_t* w = &( p -> main_hand_weapon );

    if ( w -> type != WEAPON_BEAST )
    {
      w -> type = WEAPON_BEAST;
      w -> school = SCHOOL_PHYSICAL;
      w -> damage = 54.8 * 2.5;
      w -> swing_time = 2.5;
    }

    // Force melee swing to restart if necessary
    if ( p -> main_hand_attack ) p -> main_hand_attack -> cancel();

    p -> main_hand_attack = p -> bear_melee_attack;
    p -> main_hand_attack -> weapon = w;

    if ( p -> buffs_cat_form     -> check() ) p -> buffs_cat_form     -> expire();
    if ( p -> buffs_moonkin_form -> check() ) p -> buffs_moonkin_form -> expire();

    p -> buffs_bear_form -> start();
    p -> base_gcd = 1.0;
    p -> reset_gcd();

    p -> dodge += 0.02 * p -> talents.feral_swiftness -> rank() + p -> talents.natural_reaction -> effect_base_value( 1 ) / 100.0;
    p -> armor_multiplier += 3.7 * ( 1.0 + 0.11 * p -> talents.thick_hide -> rank() );

    if ( p -> talents.leader_of_the_pack -> rank() )
    {
      sim -> auras.leader_of_the_pack -> trigger();
    }
  }

  virtual bool ready()
  {
    druid_t* d = player -> cast_druid();
    return ! d -> buffs_bear_form -> check();
  }
};

// Berserk ==================================================================

struct berserk_t : public druid_spell_t
{
  berserk_t( druid_t* player, const std::string& options_str ) :
    druid_spell_t( "berserk", 50334, player )
  {
    druid_t* p = player -> cast_druid();
    check_talent( p -> talents.berserk -> rank() );

    parse_options( NULL, options_str );

    harmful = false;
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();
    druid_spell_t::execute();
    p -> buffs_berserk -> trigger();
    if ( p -> talents.primal_madness -> rank() )
      p -> resource_gain( RESOURCE_RAGE, p -> talents.primal_madness -> effect_base_value( 1 ) / 10.0, p -> gains_primal_madness );
    p -> cooldowns_mangle_bear -> reset();
  }
};

// Cat Form Spell =========================================================

struct cat_form_t : public druid_spell_t
{
  cat_form_t( druid_t* player, const std::string& options_str ) :
      druid_spell_t( "cat_form", 768, player )
  {
    druid_t* p = player -> cast_druid();
    
    parse_options( NULL, options_str );

    // Override for precombat casting
    trigger_gcd       = 0;
    base_execute_time = 0;
    harmful           = false;

    if ( ! p -> cat_melee_attack )
      p -> cat_melee_attack = new cat_melee_t( p );
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();

    spell_t::execute();

    weapon_t* w = &( p -> main_hand_weapon );

    if ( w -> type != WEAPON_BEAST )
    {
      // FIXME: If we really want to model switching between forms, the old values need to be saved somewhere
      w -> type = WEAPON_BEAST;
      w -> school = SCHOOL_PHYSICAL;
      w -> min_dmg /= w -> swing_time;
      w -> max_dmg /= w -> swing_time;
      w -> damage = ( w -> min_dmg + w -> max_dmg ) / 2;
      w -> swing_time = 1.0;
    }

    // Force melee swing to restart if necessary
    if ( p -> main_hand_attack ) p -> main_hand_attack -> cancel();

    p -> main_hand_attack = p -> cat_melee_attack;
    p -> main_hand_attack -> weapon = w;

    if ( p -> buffs_bear_form    -> check() ) p -> buffs_bear_form    -> expire();
    if ( p -> buffs_moonkin_form -> check() ) p -> buffs_moonkin_form -> expire();

    p -> dodge += 0.02 * p -> talents.feral_swiftness -> rank();

    p -> buffs_cat_form -> start();
    p -> base_gcd = 1.0;
    p -> reset_gcd();

    if ( p -> talents.leader_of_the_pack -> rank() )
    {
      sim -> auras.leader_of_the_pack -> trigger();
    }
  }

  virtual bool ready()
  {
    druid_t* d = player -> cast_druid();
    return ! d -> buffs_cat_form -> check();
  }
};

// Enrage ==================================================================

struct enrage_t : public druid_spell_t
{
  enrage_t( druid_t* player, const std::string& options_str ) :
    druid_spell_t( "enrage", 5229, player )
  {
    parse_options( NULL, options_str );

    harmful = false;
  }

  virtual void execute()
  {
    druid_spell_t::execute();

    druid_t* p = player -> cast_druid();
    p -> buffs_enrage -> trigger();
    p -> resource_gain( RESOURCE_RAGE, effect_base_value( 2 ) / 10.0, p -> gains_enrage );
    if ( p -> talents.primal_madness -> rank() )
      p -> resource_gain( RESOURCE_RAGE, p -> talents.primal_madness -> effect_base_value( 1 ) / 10.0, p -> gains_primal_madness );
  }

  virtual bool ready()
  {
    druid_t* p = player -> cast_druid();
    if ( ! p -> buffs_bear_form -> check() )
      return false;

    return druid_spell_t::ready();
  }
};

// Faerie Fire (Feral) ======================================================

struct faerie_fire_feral_t : public druid_spell_t
{
  faerie_fire_feral_t( druid_t* player, const std::string& options_str ) :
    druid_spell_t( "faerie_fire_feral", 60089, player )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );    
    
    base_attack_power_multiplier = extra_coeff();
    base_spell_power_multiplier  = 0;
    direct_power_mod             = 1.0;
    cooldown -> duration         = p -> player_data.spell_cooldown( 16857 ); // Cooldown is stored in another version of FF
    trigger_gcd                  = p -> player_data.spell_gcd( 16857 );
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();
    if ( p -> buffs_bear_form -> check() )
    {
      // The damage component is only active in (Dire) Bear Form
      spell_t::execute();
    }
    else
    {
      if ( sim -> log ) log_t::output( sim, "%s performs %s", p -> name(), name() );
      update_ready();
    }
    target -> debuffs.faerie_fire -> trigger( 1 + p -> talents.feral_aggression -> rank(), 0.04 );
  }
};

// Faerie Fire Spell =======================================================

struct faerie_fire_t : public druid_spell_t
{
  faerie_fire_t( druid_t* player, const std::string& options_str ) :
      druid_spell_t( "faerie_fire", 770, player )
  {
    parse_options( NULL, options_str );
  }

  virtual void execute()
  {
    druid_t*  p = player -> cast_druid();
    druid_spell_t::execute();
    if ( result_is_hit() )
    {
      target -> debuffs.faerie_fire -> trigger( 1, 0.04 );
      target -> debuffs.faerie_fire -> source = p;
    }
  }
};

// Innervate Spell =========================================================

struct innervate_t : public druid_spell_t
{
  int trigger;
  player_t* innervate_target;

  innervate_t( druid_t* player, const std::string& options_str ) :
      druid_spell_t( "innervate", 29166, player ), trigger( 0 )
  {
    druid_t* p = player -> cast_druid();

    std::string target_str;
    option_t options[] =
    {
      { "trigger", OPT_INT,    &trigger    },
      { "target",  OPT_STRING, &target_str },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    harmful = false;

    // If no target is set, assume we have innervate for ourself
    innervate_target = target_str.empty() ? p : sim -> find_player( target_str );
    assert ( innervate_target != 0 );
  }
  
  virtual void execute()
  {
    druid_t* p = player -> cast_druid();
 
    druid_spell_t::execute();

    // 20% over 10s base
    double gain = 0.20;

    if ( innervate_target == player )
    {
      gain += 0.01 * p -> talents.dreamstate -> effect_base_value( 1 );
    }
    else
    {
      // Either Dreamstate increases innervate OR you get glyph of innervate
      p -> buffs_glyph_of_innervate -> trigger( 1, p -> resource_max[ RESOURCE_MANA ] * gain / 20.0 );
    }
    innervate_target -> buffs.innervate -> trigger( 1, p -> resource_max[ RESOURCE_MANA ] * gain / 10.0);
  }

  virtual bool ready()
  {
    if ( ! druid_spell_t::ready() )
      return false;

    if ( trigger < 0 )
      return ( player -> resource_current[ RESOURCE_MANA ] + trigger ) < 0;

    return ( player -> resource_max    [ RESOURCE_MANA ] -
             player -> resource_current[ RESOURCE_MANA ] ) > trigger;
  }
};

// Insect Swarm Spell ======================================================

struct insect_swarm_t : public druid_spell_t
{
  cooldown_t* starsurge_cd;
  insect_swarm_t( druid_t* player, const std::string& options_str ) :
      druid_spell_t( "insect_swarm", 5570, player ),
      starsurge_cd( 0 )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );
    
    // Genesis, additional time is given in ms. Current structure requires it to be converted into ticks
    num_ticks   += (int) ( p -> talents.genesis -> mod_additive( P_DURATION ) / 2.0 ); 
    dot_behavior = DOT_REFRESH;

    if ( p -> set_bonus.tier11_2pc_caster() )
      base_crit += 0.05;
    
    starsurge_cd = p -> get_cooldown( "starsurge" );
  }

  virtual void player_buff()
  {
    druid_t* p = player -> cast_druid();
    // Glyph of Insect Swarm is Additive with Moonfury
    additive_multiplier += p -> glyphs.insect_swarm -> mod_additive( P_TICK_DAMAGE );
    druid_spell_t::player_buff();
  }

  virtual void tick()
  {
    druid_spell_t::tick();
    druid_t* p = player -> cast_druid();
    if ( p -> buffs_shooting_stars -> trigger() )
      starsurge_cd -> reset();
  }
  
  virtual void execute()
  {
    druid_spell_t::execute();
    druid_t* p = player -> cast_druid();
    if ( result_is_hit() )
    {
      p -> buffs_natures_grace -> trigger( 1, p -> talents.natures_grace -> base_value() / 100.0 );
    }
  }
};

// Mark of the Wild Spell =====================================================

struct mark_of_the_wild_t : public druid_spell_t
{
  mark_of_the_wild_t( player_t* player, const std::string& options_str ) :
      druid_spell_t( "mark_of_the_wild", player, SCHOOL_NATURE, TREE_RESTORATION )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );

    trigger_gcd = 0;
    id          = 1126;
    base_cost  *= 1.0 + p -> glyphs.mark_of_the_wild -> mod_additive( P_RESOURCE_COST ) / 100.0;
    harmful     = false;
  }

  virtual void execute()
  {
    if ( sim -> log ) log_t::output( sim, "%s performs %s", player -> name(), name() );

    for ( player_t* p = sim -> player_list; p; p = p -> next )
    {
      if ( p -> ooc_buffs() )
      {
        p -> buffs.mark_of_the_wild -> trigger();
      }
    }
  }

  virtual bool ready()
  {
    return ! player -> buffs.mark_of_the_wild -> check();
  }
};

// Moonfire Spell ===========================================================

struct moonfire_t : public druid_spell_t
{
  cooldown_t* starsurge_cd;

  moonfire_t( druid_t* player, const std::string& options_str ) :
      druid_spell_t( "moonfire", 8921, player ), 
      starsurge_cd( 0 )
  {
    druid_t* p = player -> cast_druid();

    parse_options( NULL, options_str );

    // Genesis, additional time is given in ms. Current structure requires it to be converted into ticks
    num_ticks   += (int) ( p -> talents.genesis -> mod_additive( P_DURATION ) / 2.0 ); 
    dot_behavior = DOT_REFRESH;
    
    if ( p -> primary_tree() == TREE_BALANCE )
      base_crit_bonus_multiplier *= 1.0 + p -> spec_moonfury -> mod_additive( P_CRIT_DAMAGE );

    if ( p -> set_bonus.tier11_2pc_caster() )
      base_crit += 0.05;

    starsurge_cd = p -> get_cooldown( "starsurge" );
  }

  virtual void player_buff()
  {
    druid_t* p = player -> cast_druid();    
    // Lunar Shower and BoG are additive with Moonfury and only apply to DD
    additive_multiplier += p -> buffs_lunar_shower -> mod_additive( P_GENERIC ) * p -> buffs_lunar_shower -> stack()
                         + p -> talents.blessing_of_the_grove -> effect_base_value( 2 ) / 100.0;

    druid_spell_t::player_buff();
  }

  void player_buff_tick()
  {
    druid_t* p = player -> cast_druid();

    // Lunar Shower and BoG are additive with Moonfury and only apply to DD
    // So after the DD redo the player_buff w/o Lunar Shower and BotG
    additive_multiplier += p -> glyphs.moonfire -> mod_additive( P_TICK_DAMAGE );
    druid_spell_t::player_buff();
  }

  virtual void tick()
  {
    druid_spell_t::tick();
    druid_t* p = player -> cast_druid();
    if ( p -> buffs_shooting_stars -> trigger() )
      starsurge_cd -> reset();
  }

  virtual void execute()
  {
    druid_spell_t::execute();

    druid_t* p = player -> cast_druid();
    // Recalculate all those multipliers w/o Lunar Shower/BotG
    player_buff_tick();

    if ( result_is_hit() )
    {
      if ( p -> dots_sunfire -> ticking )
        p -> dots_sunfire -> action -> cancel();

      // If moving trigger all 3 stacks, because it will stack up immediately
      p -> buffs_lunar_shower -> trigger( p -> buffs.moving -> check() ? 3 : 1 );
      p -> buffs_natures_grace -> trigger( 1, p -> talents.natures_grace -> base_value() / 100.0 );
    }
  }

  virtual double cost_reduction() SC_CONST
  {
    // Cost reduction from moonglow and lunar shower is additive
    druid_t* p = player -> cast_druid();
    double cr = druid_spell_t::cost_reduction();
    cr += ( p -> buffs_lunar_shower -> mod_additive ( P_RESOURCE_COST ) * p -> buffs_lunar_shower -> stack() );
    return cr;    
  }

  virtual bool ready()
  {
    if ( ! druid_spell_t::ready() )
      return false;

    druid_t* p = player -> cast_druid();

    if ( p -> talents.sunfire -> rank() && p -> buffs_eclipse_solar -> check() )
      return false;

    return true;
  }
};

// Moonkin Form Spell =====================================================

struct moonkin_form_t : public druid_spell_t
{
  moonkin_form_t( druid_t* player, const std::string& options_str ) :
      druid_spell_t( "moonkin_form", 24858, player )
  {
    druid_t* p = player -> cast_druid();
    check_talent( p -> talents.moonkin_form -> rank() );

    parse_options( NULL, options_str );

    // Override these as we can precast before combat begins
    trigger_gcd       = 0;
    base_execute_time = 0;
    base_cost         = 0;
    harmful           = false;
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();

    spell_t::execute();

    if ( p -> buffs_bear_form -> check() ) p -> buffs_bear_form -> expire();
    if ( p -> buffs_cat_form  -> check() ) p -> buffs_cat_form  -> expire();

    p -> buffs_moonkin_form -> start();

    sim -> auras.moonkin -> trigger();

    p -> armor_multiplier += 2.2;
  }

  virtual bool ready()
  {
    druid_t* p = player -> cast_druid();
    return ! p -> buffs_moonkin_form -> check();
  }
};

// Natures Swiftness Spell ==================================================

struct druids_swiftness_t : public druid_spell_t
{
  cooldown_t* sub_cooldown;
  dot_t*      sub_dot;

  druids_swiftness_t( druid_t* player, const std::string& options_str ) :
    druid_spell_t( "natures_swiftness", 17116, player ),
    sub_cooldown(0), sub_dot(0)
  {
    druid_t* p = player -> cast_druid();
    check_talent( p -> talents.natures_swiftness -> rank() );

    parse_options( NULL, options_str );

    if ( ! options_str.empty() )
    {
      // This will prevent Natures Swiftness from being called before the desired "fast spell" is ready to be cast.
      sub_cooldown = p -> get_cooldown( options_str );
      sub_dot      = p -> get_dot     ( options_str );
    }

    harmful = false;
  }

  virtual void execute()
  {
    druid_spell_t::execute();
    druid_t* p = player -> cast_druid();
    p -> buffs_natures_swiftness -> trigger();
  }

  virtual bool ready()
  {
    if ( sub_cooldown )
      if ( sub_cooldown -> remains() > 0 )
        return false;

    if ( sub_dot )
      if ( sub_dot -> remains() > 0 )
        return false;

    return druid_spell_t::ready();
  }
};

// Starfire Spell ===========================================================

struct starfire_t : public druid_spell_t
{
  std::string prev_str;
  int extend_moonfire;

  starfire_t( druid_t* player, const std::string& options_str ) :
      druid_spell_t( "starfire", 2912, player ),
      extend_moonfire( 0 )
  {
    druid_t* p = player -> cast_druid();

    option_t options[] =
    {
      { "extendmf", OPT_BOOL,   &extend_moonfire },
      { "prev",     OPT_STRING, &prev_str        },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );
    
    base_execute_time += p -> talents.starlight_wrath -> mod_additive( P_CAST_TIME );

    if ( p -> primary_tree() == TREE_BALANCE )
      base_crit_bonus_multiplier *= 1.0 + p -> spec_moonfury -> mod_additive( P_CRIT_DAMAGE );
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();
    druid_spell_t::execute();

    if ( result_is_hit() )
    {
      trigger_earth_and_moon( this );

      if ( result == RESULT_CRIT )
      {
        trigger_t10_4pc_caster( player, direct_dmg, school );
      }
      if ( p -> glyphs.starfire -> enabled() )
      {
        if ( p -> dots_moonfire -> ticking )
        {
          if ( p -> dots_moonfire -> added_ticks < 6 )
            p -> dots_moonfire -> action -> extend_duration( 2 );
        }
        else if ( p -> dots_sunfire -> ticking )
        {
          if ( p -> dots_sunfire -> added_ticks < 6 )
            p -> dots_sunfire -> action -> extend_duration( 2 );
        }
      }

      if ( ! p -> buffs_eclipse_solar -> check() )
      {
        // BUG (FEATURE?) ON LIVE 
        // #1 Euphoria does not proc, if you are more than 35 into the side the
        // Eclipse bar is moving towards, >35 for Starfire/towards Solar
        int gain = effect_base_value( 2 );
        if ( ! p -> buffs_eclipse_lunar -> check() 
          && p -> rng_euphoria -> roll( 0.01 * p -> talents.euphoria -> effect_base_value( 1 ) )
          && !( p -> bugs && p -> eclipse_bar_value > 35 ) )
        {
          gain *= 2;
        }
        trigger_eclipse_energy_gain( this, gain );
      }
    }
  }

  virtual bool ready()
  {
    druid_t* p = player -> cast_druid();

    if ( extend_moonfire )
    {
      if ( ! p -> glyphs.starfire -> enabled() )
        return false;

      if ( p -> dots_moonfire -> ticking )
      {
        if ( p -> dots_moonfire -> added_ticks > 5 )
          return false;
      }
      else if ( p -> dots_sunfire -> ticking )
      {
        if ( p -> dots_sunfire -> added_ticks > 5 )
          return false;
      }
      else
        return false;
    }

    if ( ! prev_str.empty() )
    {
      if ( ! p -> last_foreground_action )
        return false;

      if ( p -> last_foreground_action -> name_str != prev_str )
        return false;
    }

    return druid_spell_t::ready();
  }
};

// Starfall Spell ===========================================================

struct starfall_t : public druid_spell_t
{
  spell_t* starfall_star;

  starfall_t( druid_t* player, const std::string& options_str ) :
      druid_spell_t( "starfall", 48505, player )
  {
    struct starfall_star_t : public druid_spell_t
    {
      starfall_star_t( druid_t* player ) :
          druid_spell_t( "starfall_star", 50288, player )
      {
        druid_t* p = player -> cast_druid();

        background = true;
        dual       = true;
        stats      = player -> get_stats( "starfall" );

        if ( p -> primary_tree() == TREE_BALANCE )
          base_crit_bonus_multiplier *= 1.0 + p -> spec_moonfury -> mod_additive( P_CRIT_DAMAGE );
          
        aoe = 1;
      }

      virtual void execute()
      {
        druid_spell_t::execute();
        tick_dmg = direct_dmg;
        stats -> add( direct_dmg, DMG_DIRECT, result, time_to_execute );
      }

      virtual void player_buff()
      {
        druid_t* p = player -> cast_druid();
        // Glyph of Focus is Additive with Moonfury
        additive_multiplier += p -> glyphs.focus -> mod_additive( P_GENERIC );
        druid_spell_t::player_buff();
      }
    };

    druid_t* p = player -> cast_druid();
    check_talent( p -> talents.starfall -> rank() );

    parse_options( NULL, options_str );

    num_ticks      = 10;
    base_tick_time = 1.0;
    hasted_ticks   = false;
    cooldown -> duration += p -> glyphs.starfall -> mod_additive( P_COOLDOWN );

    may_miss = false; // This spell only triggers the buff

    starfall_star = new starfall_star_t( p );
  }

  virtual void tick()
  {
    if ( sim -> debug ) log_t::output( sim, "%s ticks (%d of %d)", name(), dot -> current_tick, dot -> num_ticks );
    starfall_star -> execute();
    update_time( DMG_DIRECT );
  }
};

// Starsurge Spell ==========================================================

struct starsurge_t : public druid_spell_t
{
  cooldown_t* starfall_cd;

  starsurge_t( druid_t* player, const std::string& options_str ) :
      druid_spell_t( "starsurge", 78674, player ),
      starfall_cd( 0 )
  {
    druid_t* p = player -> cast_druid();
    
    check_spec( TREE_BALANCE );
    
    parse_options( NULL, options_str );

    if ( p -> primary_tree() == TREE_BALANCE )
      base_crit_bonus_multiplier *= 1.0 + p -> spec_moonfury -> mod_additive( P_CRIT_DAMAGE );
    
    starfall_cd = p -> get_cooldown( "starfall" );
  }
  
  virtual void travel( player_t* t, int    travel_result,
                       double travel_dmg )
  {
    druid_t* p = player -> cast_druid();
    druid_spell_t::travel( t, travel_result, travel_dmg );

    if ( travel_result == RESULT_CRIT || travel_result == RESULT_HIT )
    {
      // gain is positive for p -> eclipse_bar_direction==0
      // else it is towards p -> eclipse_bar_direction
      int gain = effect_base_value( 2 );
      if ( p -> eclipse_bar_direction < 0 ) gain = -gain;

      trigger_eclipse_energy_gain( this, gain );
      starfall_cd -> ready -= p -> glyphs.starsurge -> base_value();
    }
  }

  virtual void schedule_execute()
  {
    druid_spell_t::schedule_execute();
    druid_t* p = player -> cast_druid();
    p -> buffs_shooting_stars -> expire();
  }
  
  virtual double execute_time() SC_CONST
  {
    druid_t* p = player -> cast_druid();
    if ( p -> buffs_shooting_stars -> up() )
      return 0;

    return druid_spell_t::execute_time();
  }  
};

// Stealth ==================================================================

struct stealth_t : public spell_t
{
  stealth_t( player_t* player, const std::string& options_str ) :
      spell_t( "stealth", player )
  {
    parse_options( NULL, options_str );

    trigger_gcd = 0;
    harmful     = false;
  }

  virtual void execute()
  {
    druid_t* p = player -> cast_druid();
    if ( sim -> log ) log_t::output( sim, "%s performs %s", p -> name(), name() );
    p -> buffs_stealthed -> trigger();
  }

  virtual bool ready()
  {
    druid_t* p = player -> cast_druid();
    return ! p -> buffs_stealthed -> check();
  }
};

// Sunfire Spell ============================================================

struct sunfire_t : public druid_spell_t
{
  cooldown_t* starsurge_cd;

  // Identical to moonfire, except damage type and usability

  sunfire_t( druid_t* player, const std::string& options_str ) :
      druid_spell_t( "sunfire", 93402, player ), 
      starsurge_cd( 0 )
  {
    druid_t* p = player -> cast_druid();
    check_talent( p -> talents.sunfire -> rank() );

    parse_options( NULL, options_str );

    // Genesis, additional time is given in ms. Current structure requires it to be converted into ticks
    num_ticks   += (int) ( p -> talents.genesis -> mod_additive( P_DURATION ) / 2.0 ); 
    dot_behavior = DOT_REFRESH;
    
    if ( p -> primary_tree() == TREE_BALANCE )
      base_crit_bonus_multiplier *= 1.0 + p -> spec_moonfury -> mod_additive( P_CRIT_DAMAGE );

    if ( p -> set_bonus.tier11_2pc_caster() )
      base_crit += 0.05;
      
    starsurge_cd = p -> get_cooldown( "starsurge" );
  }

  virtual void player_buff()
  {
    druid_t* p = player -> cast_druid();    
    // Lunar Shower and BoG are additive with Moonfury and only apply to DD
    additive_multiplier += p -> buffs_lunar_shower -> mod_additive( P_GENERIC ) * p -> buffs_lunar_shower -> stack()
                         + p -> talents.blessing_of_the_grove -> effect_base_value( 2 ) / 100.0;

    druid_spell_t::player_buff();
  }

  void player_buff_tick()
  {
    druid_t* p = player -> cast_druid();

    // Lunar Shower and BoG are additive with Moonfury and only apply to DD
    // So after the DD redo the player_buff w/o Lunar Shower and BotG
    additive_multiplier += p -> glyphs.moonfire -> mod_additive( P_TICK_DAMAGE );
    druid_spell_t::player_buff();
  }

  virtual void tick()
  {
    druid_spell_t::tick();
    druid_t* p = player -> cast_druid();
    if ( p -> buffs_shooting_stars -> trigger() )
      starsurge_cd -> reset();
  }

  virtual void execute()
  {
    druid_spell_t::execute();

    druid_t* p = player -> cast_druid();
    // Recalculate all those multipliers w/o Lunar Shower/BotG
    player_buff_tick();

    if ( result_is_hit() )
    {
      if ( p -> dots_moonfire -> ticking )
        p -> dots_moonfire -> action -> cancel();

      // If moving trigger all 3 stacks, because it will stack up immediately
      p -> buffs_lunar_shower -> trigger( p -> buffs.moving -> check() ? 3 : 1 );
      p -> buffs_natures_grace -> trigger( 1, p -> talents.natures_grace -> base_value() / 100.0 );
    }
  }

  virtual double cost_reduction() SC_CONST
  {
    // Costreduction from moonglow and lunar shower is additive
    druid_t* p = player -> cast_druid();
    double cr = druid_spell_t::cost_reduction();
    cr += ( p -> buffs_lunar_shower -> mod_additive( P_RESOURCE_COST ) * p -> buffs_lunar_shower -> stack() );
    return cr;    
  }

  virtual bool ready()
  {
    if ( ! druid_spell_t::ready() )
      return false;

    druid_t* p = player -> cast_druid();

    if ( ! p -> buffs_eclipse_solar -> check() )
      return false;

    return true;
  }
};

// Treants Spell ============================================================

struct treants_spell_t : public druid_spell_t
{
  treants_spell_t( druid_t* player, const std::string& options_str ) :
      druid_spell_t( "treants", 33831, player )
  {
    druid_t* p = player -> cast_druid();
    check_talent( p -> talents.force_of_nature -> ok() );

    parse_options( NULL, options_str );

    harmful = false;
  }

  virtual void execute()
  {
    consume_resource();
    update_ready();
    player -> summon_pet( "treants", 30.0 );
  }
};

// Typhoon Spell ============================================================

struct typhoon_t : public druid_spell_t
{
  typhoon_t( druid_t* player, const std::string& options_str ) :
      druid_spell_t( "typhoon", 50516, player )
  {
    druid_t* p = player -> cast_druid();
    check_talent( p -> talents.typhoon -> ok() );

    parse_options( NULL, options_str );

    aoe                   = -1;
    base_dd_min           = p -> player_data.effect_min( effect_id( 2 ), p -> type, p -> level );
    base_dd_max           = p -> player_data.effect_max( effect_id( 2 ), p -> type, p -> level );
    base_multiplier      *= 1.0 + p -> talents.gale_winds -> effect_base_value( 1 ) / 100.0;
    direct_power_mod      = p -> player_data.effect_coeff( effect_id( 2 ) );
    cooldown -> duration += p -> glyphs.monsoon -> mod_additive( P_COOLDOWN );
    base_cost            *= 1.0 + p -> glyphs.typhoon -> mod_additive( P_RESOURCE_COST );
  }
};

// Wild Mushroom ============================================================

struct wild_mushroom_t : public druid_spell_t
{
  wild_mushroom_t( druid_t* player, const std::string& options_str ) :
      druid_spell_t( "wild_mushroom", 88747, player )
    {
      parse_options( NULL, options_str );

      harmful = false;
    }

    virtual void execute()
    {
      druid_spell_t::execute();

      druid_t* p = player -> cast_druid();
      p -> buffs_wild_mushroom -> increment();
    }
};

// Wild Mushroom: Detonate ==================================================

struct wild_mushroom_detonate_t : public druid_spell_t
{
  wild_mushroom_detonate_t( druid_t* player, const std::string& options_str ) :
      druid_spell_t( "wild_mushroom_detonate", 88751, player )
    {
      druid_t* p = player -> cast_druid();

      parse_options( NULL, options_str );

      // Actual ability is 88751, all damage is in spell 78777
      uint32_t damage_id = 78777;
      direct_power_mod   = p -> player_data.effect_coeff( damage_id, E_SCHOOL_DAMAGE );
      base_dd_min        = p -> player_data.effect_min( damage_id, p -> level, E_SCHOOL_DAMAGE );
      base_dd_max        = p -> player_data.effect_max( damage_id, p -> level, E_SCHOOL_DAMAGE );
      school             = spell_id_t::get_school_type( p -> player_data.spell_school_mask( damage_id ) );
      stats -> school    = school;
      aoe                = -1;
    }

    virtual void execute()
    {
      druid_spell_t::execute();

      druid_t* p = player -> cast_druid();
      p -> buffs_wild_mushroom -> expire();
    }

    virtual void player_buff()
    {
      druid_spell_t::player_buff();

      druid_t* p = player -> cast_druid();
      player_multiplier *= p -> buffs_wild_mushroom -> stack();
    }

    virtual bool ready()
    {
      druid_t* p = player -> cast_druid();

      if ( ! p -> buffs_wild_mushroom -> stack() )
          return false;

      return druid_spell_t::ready();
    }
};

// Wrath Spell ==============================================================

struct wrath_t : public druid_spell_t
{
  std::string prev_str;

  wrath_t( druid_t* player, const std::string& options_str ) :
      druid_spell_t( "wrath", 5176, player )
  {
    druid_t* p = player -> cast_druid();

    option_t options[] =
    {
      { "prev",    OPT_STRING, &prev_str    },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );
    
    base_execute_time += p -> talents.starlight_wrath -> mod_additive( P_CAST_TIME );
    if ( p -> primary_tree() == TREE_BALANCE )
      base_crit_bonus_multiplier *= 1.0 + p -> spec_moonfury -> mod_additive( P_CRIT_DAMAGE );
  }

  virtual void player_buff()
  {
    druid_t* p = player -> cast_druid();
    // Glyph of Wrath is additive with Moonfury
    if ( p -> dots_insect_swarm -> ticking )
      additive_multiplier += p -> glyphs.wrath -> effect_base_value( 1 ) / 100.0;

    druid_spell_t::player_buff();
  }

  virtual void travel( player_t* t, int    travel_result,
                       double travel_dmg )
  {
    druid_t* p = player -> cast_druid();
    druid_spell_t::travel( t, travel_result, travel_dmg );
    if ( result_is_hit() )
    {
      if ( travel_result == RESULT_CRIT )
      {
        trigger_t10_4pc_caster( player, travel_dmg, SCHOOL_NATURE );
      }
      trigger_earth_and_moon( this );

      if ( ! p -> buffs_eclipse_lunar -> check() )
      {
        // Wrath's Eclipse gain is a bit tricky, as it is NOT just 40/3 or 
        // rather 40/3*2 in case of Euphoria procs. This is how it behaves:
        // 66.6% for -13 Eclipse / -27 if Euphoria proc
        // 33.3% for -14 Eclipse / -26 if Euphoria proc
        // This gives averages of -13.33 and -26.66, but as Power's are tracked
        // as an integer internally, you can't recieve fractions. To get from
        // Solar into Lunar you need to get -200 energy, this means 15 Wrath
        // on average, but because the gains are not fractions you have a not
        // negliable chance to need 15 Wrath!
        
        // Wrath's second effect base value is positive in the DBC files
        int gain = - effect_base_value( 2 );
        
        // BUG (FEATURE?) ON LIVE 
        // #1 Euphoria does not proc, if you are more than 35 into the side the
        // Eclipse bar is moving towards, <-35 for Wrath/towards Lunar
        if ( ! p -> buffs_eclipse_solar -> check() 
          && p -> rng_euphoria -> roll( 0.01 * p -> talents.euphoria -> effect_base_value( 1 ) )
          && !( p -> bugs && p -> eclipse_bar_value < -35 ) )
        {
          gain *= 2;
          if ( p -> rng_wrath_eclipsegain -> roll( 2.0/3.0 ) )
            gain += 1;
        }
        else 
        {
          if ( p -> rng_wrath_eclipsegain -> roll( 1.0/3.0 ) )
            gain += 1;
        }
        trigger_eclipse_energy_gain( this, gain );
      }
    }
  }

  virtual bool ready()
  {
    if ( ! druid_spell_t::ready() )
      return false;

    druid_t* p = player -> cast_druid();

    if ( ! prev_str.empty() )
    {
      if ( ! p -> last_foreground_action )
        return false;

      if ( p -> last_foreground_action -> name_str != prev_str )
        return false;
    }

    return true;
  }
};

} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// Druid Character Definition
// ==========================================================================

// druid_t::create_action  ==================================================

action_t* druid_t::create_action( const std::string& name,
                                  const std::string& options_str )
{
  if ( name == "auto_attack"            ) return new            auto_attack_t( this, options_str );
  if ( name == "berserk"                ) return new                berserk_t( this, options_str );
  if ( name == "bear_form"              ) return new              bear_form_t( this, options_str );
  if ( name == "cat_form"               ) return new               cat_form_t( this, options_str );
  if ( name == "claw"                   ) return new                   claw_t( this, options_str );
  if ( name == "enrage"                 ) return new                 enrage_t( this, options_str );
  if ( name == "faerie_fire"            ) return new            faerie_fire_t( this, options_str );
  if ( name == "faerie_fire_feral"      ) return new      faerie_fire_feral_t( this, options_str );
  if ( name == "feral_charge_bear"      ) return new      feral_charge_bear_t( this, options_str );
  if ( name == "feral_charge_cat"       ) return new       feral_charge_cat_t( this, options_str );
  if ( name == "ferocious_bite"         ) return new         ferocious_bite_t( this, options_str );
  if ( name == "insect_swarm"           ) return new           insect_swarm_t( this, options_str );
  if ( name == "innervate"              ) return new              innervate_t( this, options_str );
  if ( name == "lacerate"               ) return new               lacerate_t( this, options_str );
  if ( name == "maim"                   ) return new                   maim_t( this, options_str );
  if ( name == "mangle_bear"            ) return new            mangle_bear_t( this, options_str );
  if ( name == "mangle_cat"             ) return new             mangle_cat_t( this, options_str );
  if ( name == "mark_of_the_wild"       ) return new       mark_of_the_wild_t( this, options_str );
  if ( name == "maul"                   ) return new                   maul_t( this, options_str );
  if ( name == "moonfire"               ) return new               moonfire_t( this, options_str );
  if ( name == "moonkin_form"           ) return new           moonkin_form_t( this, options_str );
  if ( name == "natures_swiftness"      ) return new       druids_swiftness_t( this, options_str );
  if ( name == "pounce"                 ) return new                 pounce_t( this, options_str );
  if ( name == "rake"                   ) return new                   rake_t( this, options_str );
  if ( name == "ravage"                 ) return new                 ravage_t( this, options_str );
  if ( name == "rip"                    ) return new                    rip_t( this, options_str );
  if ( name == "savage_roar"            ) return new            savage_roar_t( this, options_str );
  if ( name == "shred"                  ) return new                  shred_t( this, options_str );
  if ( name == "skull_bash_bear"        ) return new        skull_bash_bear_t( this, options_str );
  if ( name == "skull_bash_cat"         ) return new         skull_bash_cat_t( this, options_str );
  if ( name == "starfire"               ) return new               starfire_t( this, options_str );
  if ( name == "starfall"               ) return new               starfall_t( this, options_str );
  if ( name == "starsurge"              ) return new              starsurge_t( this, options_str );
  if ( name == "stealth"                ) return new                stealth_t( this, options_str );
  if ( name == "sunfire"                ) return new                sunfire_t( this, options_str );
  if ( name == "swipe_bear"             ) return new             swipe_bear_t( this, options_str );
  if ( name == "swipe_cat"              ) return new              swipe_cat_t( this, options_str );
  if ( name == "tigers_fury"            ) return new            tigers_fury_t( this, options_str );
  if ( name == "treants"                ) return new          treants_spell_t( this, options_str );
  if ( name == "typhoon"                ) return new                typhoon_t( this, options_str );
  if ( name == "wild_mushroom"          ) return new          wild_mushroom_t( this, options_str );
  if ( name == "wild_mushroom_detonate" ) return new wild_mushroom_detonate_t( this, options_str );
  if ( name == "wrath"                  ) return new                  wrath_t( this, options_str );

  return player_t::create_action( name, options_str );
}

// druid_t::create_pet ======================================================

pet_t* druid_t::create_pet( const std::string& pet_name,
                            const std::string& pet_type )
{
  pet_t* p = find_pet( pet_name );

  if ( p ) return p;

  if ( pet_name == "treants" ) return new treants_pet_t( sim, this, pet_name );

  return 0;
}

// druid_t::create_pets =====================================================

void druid_t::create_pets()
{
  create_pet( "treants" );
}

// druid_t::init_talents =====================================================

void druid_t::init_talents()
{
  talents.balance_of_power      = find_talent( "Balance of Power" );
  talents.berserk               = find_talent( "Berserk" );
  talents.blessing_of_the_grove = find_talent( "Blessing of the Grove" );
  talents.blood_in_the_water    = find_talent( "Blood in the Water" );
  talents.brutal_impact         = find_talent( "Brutal Impact" );
  talents.earth_and_moon        = find_talent( "Earth and Moon" );
  talents.endless_carnage       = find_talent( "Endless Carnage" );
  talents.euphoria              = find_talent( "Euphoria" );
  talents.dreamstate            = find_talent( "Dreamstate" );               
  talents.feral_aggression      = find_talent( "Feral Aggression" );
  talents.feral_charge          = find_talent( "Feral Charge" );
  talents.feral_swiftness       = find_talent( "Feral Swiftness" );
  talents.force_of_nature       = find_talent( "Force of Nature" );
  talents.fungal_growth         = find_talent( "Fungal Growth" );
  talents.furor                 = find_talent( "Furor" );
  talents.fury_of_stormrage     = find_talent( "Fury of Stormrage" );
  talents.fury_swipes           = find_talent( "Fury Swipes" );
  talents.gale_winds            = find_talent( "Gale Winds" );
  talents.genesis               = find_talent( "Genesis" );
  talents.heart_of_the_wild     = find_talent( "Heart of the Wild" );
  talents.infected_wounds       = find_talent( "Infected Wounds" );
  talents.king_of_the_jungle    = find_talent( "King of the Jungle" );
  talents.leader_of_the_pack    = find_talent( "Leader of the Pack" );
  talents.lunar_shower          = find_talent( "Lunar Shower" );
  talents.master_shapeshifter   = find_talent( "Master Shapeshifter" );
  talents.moonglow              = find_talent( "Moonglow" );
  talents.moonkin_form          = find_talent( "Moonkin Form" );
  talents.natural_reaction      = find_talent( "Natural Reaction" );
  talents.natures_majesty       = find_talent( "Nature's Majesty" );
  talents.natures_swiftness     = find_talent( "Nature's Swiftness" ); 
  talents.natures_grace         = find_talent( "Nature's Grace" );
  talents.nurturing_instict     = find_talent( "Nurturing Instinct" );
  talents.owlkin_frenzy         = find_talent( "Owlkin Frenzy" );
  talents.predatory_strikes     = find_talent( "Predatory Strikes" );
  talents.primal_fury           = find_talent( "Primal Fury" );
  talents.primal_madness        = find_talent( "Primal Madness" );
  talents.pulverize             = find_talent( "Pulverize" );
  talents.rend_and_tear         = find_talent( "Rend and Tear" );
  talents.shooting_stars        = find_talent( "Shooting Stars" );
  talents.solar_beam            = find_talent( "Solar Beam" );
  talents.stampede              = find_talent( "Stampede" );
  talents.starfall              = find_talent( "Starfall" );
  talents.starlight_wrath       = find_talent( "Starlight Wrath" );
  talents.sunfire               = find_talent( "Sunfire" );
  talents.survival_instincts    = find_talent( "Survival Instincts" );
  talents.thick_hide            = find_talent( "Thick Hide" );
  talents.typhoon               = find_talent( "Typhoon" );

  player_t::init_talents();
}

// druid_t::init_spells =====================================================

void druid_t::init_spells()
{
  player_t::init_spells();

  // Specializations
  
  // Balance
  spec_moonfury         = new passive_spell_t( this, "moonfury",      16913 );
  mastery_total_eclipse = new mastery_t      ( this, "total_eclipse", 77492, TREE_BALANCE );

  // Feral
  spec_aggression         = new passive_spell_t( this, "aggression",      84735 );
  spec_vengeance          = new passive_spell_t( this, "vengeance",       84840 );
  mastery_razor_claws     = new mastery_t      ( this, "razor_claws",     77493, TREE_FERAL );
  mastery_savage_defender = new mastery_t      ( this, "savage_defender", 77494, TREE_FERAL );

  // Glyphs
  glyphs.berserk          = find_glyph( "Glyph of Berserk" );
  glyphs.ferocious_bite   = find_glyph( "Glyph of Ferocious Bite" );
  glyphs.focus            = find_glyph( "Glyph of Focus" );
  glyphs.innervate        = find_glyph( "Glyph of Innervate" );
  glyphs.insect_swarm     = find_glyph( "Glyph of Insect Swarm" );
  glyphs.lacerate         = find_glyph( "Glyph of Lacerate" );
  glyphs.mangle           = find_glyph( "Glyph of Mangle" );
  glyphs.mark_of_the_wild = find_glyph( "Glyph of Mark of the Wild" );
  glyphs.maul             = find_glyph( "Glyph of Maul" );
  glyphs.monsoon          = find_glyph( "Glyph of Monsoon" );
  glyphs.moonfire         = find_glyph( "Glyph of Moonfire" );
  glyphs.rip              = find_glyph( "Glyph of Rip" );
  glyphs.savage_roar      = find_glyph( "Glyph of Savage Roar" );
  glyphs.shred            = find_glyph( "Glyph of Shred" );
  glyphs.starfall         = find_glyph( "Glyph of Starfall" );
  glyphs.starfire         = find_glyph( "Glyph of Starfire" );
  glyphs.starsurge        = find_glyph( "Glyph of Starsurge" );
  glyphs.tigers_fury      = find_glyph( "Glyph of Tiger's Fury" );
  glyphs.typhoon          = find_glyph( "Glyph of Typhoon" );
  glyphs.wrath            = find_glyph( "Glyph of Wrath" );

  // Tier Bonuses
  static uint32_t set_bonuses[N_TIER][N_TIER_BONUS] = 
  {
    //  C2P    C4P    M2P    M4P    T2P    T4P    H2P    H4P
    { 70718, 70723, 70724, 70726,     0,     0,     0,     0 }, // Tier10
    { 90160, 90163, 90162, 90165,     0,     0,     0,     0 }, // Tier11
    {     0,     0,     0,     0,     0,     0,     0,     0 },
  };

  sets = new set_bonus_array_t( this, set_bonuses );
}

// druid_t::init_base =======================================================

void druid_t::init_base()
{
  player_t::init_base();

  initial_spell_power_per_spirit = 0.0;

  base_attack_power = level * (level > 80 ? 3.0 : 2.0);

  attribute_multiplier_initial[ ATTR_INTELLECT ]   *= 1.0 + talents.heart_of_the_wild -> effect_base_value( 1 ) * 0.01;
  initial_attack_power_per_agility  = 0.0;
  initial_attack_power_per_strength = 2.0;
  initial_spell_power_per_intellect = 1.0;

  // FIXME! Level-specific!  Should be form-specific!
  base_miss    = 0.05;
  initial_armor_multiplier  = 1.0 + util_t::talent_rank( talents.thick_hide -> rank(), 3, 0.04, 0.07, 0.10 );

  diminished_kfactor    = 0.009720;
  diminished_dodge_capi = 0.008555;
  diminished_parry_capi = 0.008555;

  resource_base[ RESOURCE_ENERGY ] = 100;
  resource_base[ RESOURCE_RAGE   ] = 100;
  mana_per_intellect           = 15;
  base_energy_regen_per_second = 10;
  
  // Furor: +5/10/15% max mana
  resource_base[ RESOURCE_MANA ] *= 1.0 + talents.furor -> effect_base_value( 2 ) * 0.01;
  mana_per_intellect             *= 1.0 + talents.furor -> effect_base_value( 2 ) * 0.01;
  
  switch ( primary_tree() )
  {
  case TREE_BALANCE:
    break;
  case TREE_RESTORATION:
    // Meditation for choosing resto
    mana_regen_while_casting = 0.50;
    break;
  case TREE_FERAL:
    break;
  default:
    break;
  }

  base_gcd = 1.5;

}

// druid_t::init_buffs ======================================================

void druid_t::init_buffs()
{
  player_t::init_buffs();

  // buff_t( sim, player, name, max_stack, duration, cooldown, proc_chance, quiet )
  // These have either incorrect or no values in the DBC or don't really exist
  buffs_glyph_of_innervate = new buff_t( this, "glyph_of_innervate", 1,  10.0,     0, glyphs.innervate -> enabled() );
  buffs_natures_grace      = new buff_t( this, "natures_grace"     , 1,  15.0,  60.0, talents.natures_grace -> ok() );
  buffs_omen_of_clarity    = new buff_t( this, "omen_of_clarity"   , 1,  15.0,     0, 3.5 / 60.0 );
  buffs_pulverize          = new buff_t( this, "pulverize"         , 1,  10.0 + talents.endless_carnage -> effect_base_value( 2 ) / 1000.0 );
  buffs_stampede_bear      = new buff_t( this, "stampede_bear"     , 1,   8.0,     0, talents.stampede -> ok() );
  buffs_stampede_cat       = new buff_t( this, "stampede_cat"      , 1,  10.0,     0, talents.stampede -> ok() );
  buffs_t10_2pc_caster     = new buff_t( this, "t10_2pc_caster"    , 1,   6.0,     0, set_bonus.tier10_2pc_caster() );
  buffs_t11_4pc_caster     = new buff_t( this, "t11_4pc_caster"    , 3,   8.0,     0, set_bonus.tier11_4pc_caster() );
  buffs_t11_4pc_melee      = new buff_t( this, "t11_4pc_melee"     , 3,  30.0,     0, set_bonus.tier11_4pc_melee()  );
  buffs_wild_mushroom      = new buff_t( this, "wild_mushroom"     , 3,     0,     0, 0, true );
  
  // buff_t ( sim, id, name, chance, duration, quiet, reverse, rng_type )
  buffs_eclipse_lunar      = new buff_t( this, 48518, "lunar_eclipse" );
  buffs_eclipse_solar      = new buff_t( this, 48517, "solar_eclipse" );
  buffs_enrage             = new buff_t( this, player_data.find_class_spell( type, "Enrage" ), "enrage" );
  buffs_lacerate           = new buff_t( this, player_data.find_class_spell( type, "Lacerate" ), "lacerate" );
  buffs_lunar_shower       = new buff_t( this, talents.lunar_shower -> effect_trigger_spell( 1 ), "lunar_shower" );
  buffs_natures_swiftness  = new buff_t( this, talents.natures_swiftness -> spell_id(), "natures_swiftness" );
  buffs_shooting_stars     = new buff_t( this, talents.shooting_stars -> effect_trigger_spell( 1 ), "shooting_stars", talents.shooting_stars -> proc_chance() );
  
  // simple
  buffs_bear_form    = new buff_t( this, 5487,  "bear_form" );
  buffs_cat_form     = new buff_t( this, 768,   "cat_form" );
  buffs_combo_points = new buff_t( this, "combo_points", 5 );
  buffs_moonkin_form = new buff_t( this, 24858, "moonkin_form" );
  buffs_savage_roar  = new buff_t( this, 52610, "savage_roar" );
  buffs_stealthed    = new buff_t( this, 5215,  "stealthed" );

  buffs_berserk        = new        berserk_buff_t( this );
  buffs_primal_madness = new primal_madness_buff_t( this );
  buffs_tigers_fury    = new    tigers_fury_buff_t( this );
}

// druid_t::init_scaling ====================================================

void druid_t::init_scaling()
{
  player_t::init_scaling();

  equipped_weapon_dps = main_hand_weapon.damage / main_hand_weapon.swing_time;

  scales_with[ STAT_WEAPON_SPEED  ] = 0;

  if ( primary_tree() == TREE_FERAL )
  {
    scales_with[ STAT_SPIRIT ] = 0;
  }

  // Balance of Power treats Spirit like Spell Hit Rating
  if( talents.balance_of_power -> rank() && sim -> scaling -> scale_stat == STAT_SPIRIT )
  {
    double v = sim -> scaling -> scale_value;
    if ( ! sim -> scaling -> positive_scale_delta )
    {
      invert_spirit_scaling = 1;
      attribute_initial[ ATTR_SPIRIT ] -= v * 2;
    }
    else
      attribute_initial[ ATTR_SPIRIT ] += v * 2;
  }
}

// druid_t::init_gains ======================================================

void druid_t::init_gains()
{
  player_t::init_gains();

  gains_bear_melee         = get_gain( "bear_melee"         );
  gains_energy_refund      = get_gain( "energy_refund"      );
  gains_enrage             = get_gain( "enrage"             );
  gains_euphoria           = get_gain( "euphoria"           );
  gains_glyph_of_innervate = get_gain( "glyph_of_innervate" );
  gains_incoming_damage    = get_gain( "incoming_damage"    );
  gains_moonkin_form       = get_gain( "moonkin_form"       );
  gains_natural_reaction   = get_gain( "natural_reaction"   );
  gains_omen_of_clarity    = get_gain( "omen_of_clarity"    );
  gains_primal_fury        = get_gain( "primal_fury"        );
  gains_primal_madness     = get_gain( "primal_madness"     );
  gains_tigers_fury        = get_gain( "tigers_fury"        );
}

// druid_t::init_procs ======================================================

void druid_t::init_procs()
{
  player_t::init_procs();

  procs_combo_points_wasted = get_proc( "combo_points_wasted" );
  procs_fury_swipes         = get_proc( "fury_swipes"         );
  procs_parry_haste         = get_proc( "parry_haste"         );
  procs_primal_fury         = get_proc( "primal_fury"         );
}

// druid_t::init_uptimes ====================================================

void druid_t::init_uptimes()
{
  player_t::init_uptimes();

  uptimes_energy_cap   = get_uptime( "energy_cap" );
  uptimes_rage_cap     = get_uptime( "rage_cap"   );
}

// druid_t::init_rng ========================================================

void druid_t::init_rng()
{
  player_t::init_rng();

  rng_blood_in_the_water  = get_rng( "blood_in_the_water" );
  rng_euphoria            = get_rng( "euphoria"           );
  rng_fury_swipes         = get_rng( "fury_swipes"        );  
  rng_primal_fury         = get_rng( "primal_fury"        );
  rng_wrath_eclipsegain   = get_rng( "wrath_eclipsegain"  );
}

// druid_t::init_actions ====================================================

void druid_t::init_actions()
{
  if ( primary_role() == ROLE_ATTACK && main_hand_weapon.type == WEAPON_NONE )
  {
    sim -> errorf( "Player %s has no weapon equipped at the Main-Hand slot.", name() );
    quiet = true;
    return;
  }

  if ( action_list_str.empty() )
  {
    std::string use_str = "";
    int num_items = ( int ) items.size();
    for ( int i=0; i < num_items; i++ )
    {
      if ( items[ i ].use.active() )
      {
        use_str += "/use_item,name=";
        use_str += items[ i ].name();
      }
    }

    if ( primary_tree() == TREE_FERAL )
    {
      if ( primary_role() == ROLE_TANK )
      {
        if ( level > 80 )
        {
          action_list_str += "flask,type=steelskin/food,type=seafood_magnifique_feast";
        }
        else
        {
          action_list_str += "flask,type=endless_rage/food,type=rhinolicious_wormsteak";
        }
        action_list_str += "/mark_of_the_wild";
        action_list_str += "/bear_form";
        action_list_str += "/auto_attack";
        action_list_str += "/snapshot_stats";
        if ( race == RACE_TROLL ) action_list_str += "/berserking";
        action_list_str += "/skull_bash_bear";
        action_list_str += "/faerie_fire_feral,debuff_only=1";  // Use on pull.
        action_list_str += "/mangle_bear,debuff.mangle.remains<=0.5";
        action_list_str += "/lacerate,if=dot.lacerate.remains<=6.9"; // This seems to be the sweet spot to prevent Lacerate falling off.
        if ( talents.berserk -> rank() ) action_list_str+="/berserk";
        action_list_str += use_str;
        action_list_str += "/mangle_bear";
        action_list_str += "/faerie_fire_feral";
        action_list_str += "/swipe_bear,berserk=0,lacerate_stack>=5";
      }
      else
      {
        if ( level > 80 )
        {
          action_list_str += "flask,type=winds";
          action_list_str += "/food,type=seafood_magnifique_feast";
        }
        else
        {
          action_list_str += "flask,type=endless_rage";
          action_list_str += "/food,type=hearty_rhino";
        }
        action_list_str += "/mark_of_the_wild";
        action_list_str += "/cat_form";
        action_list_str += "/snapshot_stats";

        if ( level > 80 )
        {
          action_list_str += "/tolvir_potion,if=!in_combat";
        }
        else
        {
          action_list_str += "/speed_potion,if=!in_combat";
        }

        action_list_str += "/feral_charge_cat,if=!in_combat";
        action_list_str += "/auto_attack";
        action_list_str += "/skull_bash_cat";
        action_list_str += "/tigers_fury,if=energy<=26";
        if ( level > 80 )
        {
          action_list_str += "/tolvir_potion,if=buff.bloodlust.react|target.time_to_die<=40";
        }
        else
        {
          action_list_str += "/speed_potion,if=buff.bloodlust.react|target.time_to_die<=40";
        }
        if ( race == RACE_TROLL ) action_list_str += "/berserking";
        action_list_str += "/mangle_cat,if=set_bonus.tier11_4pc_melee&(buff.t11_4pc_melee.stack<3|buff.t11_4pc_melee.remains<3)";
        action_list_str += "/faerie_fire_feral,if=debuff.faerie_fire.stack<3|!(debuff.sunder_armor.up|debuff.expose_armor.up)";
        action_list_str += "/mangle_cat,if=debuff.mangle.remains<=2&(!debuff.mangle.up|debuff.mangle.remains>=0.0)";
        action_list_str += "/ravage,if=buff.stampede_cat.up&buff.stampede_cat.remains<=1";
        if ( talents.berserk -> rank() )action_list_str += "/berserk,if=time_to_max_energy>=2.0&!buff.tigers_fury.up&cooldown.tigers_fury.remains>15";
        action_list_str += "/ferocious_bite,if=buff.combo_points.stack>=1&dot.rip.ticking&dot.rip.remains<=1&target.health_pct<=25";
        action_list_str += "/ferocious_bite,if=buff.combo_points.stack>=5&dot.rip.ticking&target.health_pct<=25";
        action_list_str += use_str;
        action_list_str += "/rip,if=buff.combo_points.stack>=5&target.time_to_die>=6&dot.rip.remains<2.0&(buff.berserk.up|dot.rip.remains<=cooldown.tigers_fury.remains)";
        action_list_str += "/rake,if=target.time_to_die>=8.5&buff.tigers_fury.up&dot.rake.remains<9.0&(!dot.rake.ticking|dot.rake.multiplier<multiplier)";
        action_list_str += "/rake,if=target.time_to_die>=dot.rake.remains&dot.rake.remains<3.0&(buff.berserk.up|energy>=71|(cooldown.tigers_fury.remains+0.8)>=dot.rake.remains)";
        action_list_str += "/shred,if=buff.omen_of_clarity.react";
        action_list_str += "/savage_roar,if=buff.combo_points.stack>=1&buff.savage_roar.remains<=1";
        action_list_str += "/savage_roar,if=target.time_to_die>=9&buff.combo_points.stack>=5&dot.rip.ticking&dot.rip.remains<=12&@(dot.rip.remains-buff.savage_roar.remains)<=3";
        action_list_str += "/ferocious_bite,if=(target.time_to_die<=4&buff.combo_points.stack>=5)|target.time_to_die<=1";
        action_list_str += "/ferocious_bite,if=level<=80&buff.combo_points.stack>=5&dot.rip.remains>=8.0&buff.savage_roar.remains>=4.0";
        action_list_str += "/ferocious_bite,if=level>80&buff.combo_points.stack>=5&dot.rip.remains>=14.0&buff.savage_roar.remains>=10.0";
        action_list_str += "/shred,extend_rip=1,if=dot.rip.ticking&dot.rip.remains<=4&target.health_pct>25";
        action_list_str += "/ravage,if=buff.stampede_cat.up&!buff.omen_of_clarity.react&buff.tigers_fury.up";
        action_list_str += "/mangle_cat,if=set_bonus.tier11_4pc_melee&buff.t11_4pc_melee.stack<3";
        action_list_str += "/shred,if=buff.combo_points.stack<=4&dot.rake.remains>3.0&dot.rip.remains>3.0&(time_to_max_energy<=2.0|(buff.berserk.up&energy>=20))";
        action_list_str += "/shred,if=cooldown.tigers_fury.remains<=3.0";
        action_list_str += "/shred,if=target.time_to_die<=dot.rake.duration";
        action_list_str += "/shred,if=buff.combo_points.stack=0&(buff.savage_roar.remains<=2.0|dot.rake.remains>=5.0)";
        action_list_str += "/shred,if=!dot.rip.ticking|time_to_max_energy<=1.0";
      }
    }
    else
    {
      if ( level > 80 )
      {
        action_list_str += "flask,type=draconic_mind/food,type=seafood_magnifique_feast";
      }
      else
      {
        action_list_str += "flask,type=frost_wyrm/food,type=fish_feast";
      }
      action_list_str += "/mark_of_the_wild";
      if ( talents.moonkin_form -> rank() ) 
        action_list_str += "/moonkin_form";
      action_list_str += "/snapshot_stats";
      action_list_str += "/volcanic_potion,if=!in_combat";
      action_list_str += "/volcanic_potion,if=buff.bloodlust.react|target.time_to_die<=40";
      action_list_str += "/faerie_fire,if=debuff.faerie_fire.stack<3&!(debuff.sunder_armor.up|debuff.expose_armor.up)";
      if ( ptr )
        action_list_str += "/wild_mushroom_detonate,if=buff.wild_mushroom.stack=3";
      if ( race == RACE_TROLL )
        action_list_str += "/berserking";
      if ( talents.typhoon -> rank() ) 
        action_list_str += "/typhoon,moving=1";
      if ( talents.starfall -> rank() && glyphs.focus -> enabled() ) 
        action_list_str += "/starfall";
      if ( talents.sunfire -> rank() )
        action_list_str += "/sunfire,if=!ticking&!dot.moonfire.remains>0";
      action_list_str += "/moonfire,if=!ticking";
      if ( talents.sunfire -> rank() )
        action_list_str += "&!dot.sunfire.remains>0";
      if ( set_bonus.tier11_4pc_caster() ) // refreshing moonfire right after eclipse locks in the crit
        action_list_str += "&buff.lunar_eclipse.react";
      if ( talents.starfall -> rank() && ! glyphs.focus -> enabled() )
        action_list_str += "/starfall";
      action_list_str += "/insect_swarm,if=!ticking";
      if ( primary_tree() == TREE_BALANCE )
        action_list_str += "/starsurge";
      action_list_str += "/innervate,if=mana_pct<50";
      if ( talents.force_of_nature -> rank() )
        action_list_str += "/treants,time>=5";
      action_list_str += use_str;
      action_list_str += "/starfire,if=eclipse_dir=1";
      action_list_str += "/wrath,if=eclipse_dir=-1";
      action_list_str += "/starfire";
      if ( ptr )
        action_list_str += "/wild_mushroom,moving=1,if=buff.wild_mushroom.stack<3";
      action_list_str += "/moonfire,moving=1";
      action_list_str += "/sunfire,moving=1";
    }
    action_list_default = 1;
  }

  player_t::init_actions();

}

// druid_t::reset ===========================================================

void druid_t::reset()
{
  player_t::reset();

  eclipse_bar_value = 0;
  base_gcd = 1.5;
}

// druid_t::interrupt =======================================================

void druid_t::interrupt()
{
  player_t::interrupt();

  if ( main_hand_attack ) main_hand_attack -> cancel();
}

// druid_t::clear_debuffs ===================================================

void druid_t::clear_debuffs()
{
  player_t::clear_debuffs();

  buffs_combo_points -> expire();
}

// druid_t::regen ===========================================================

void druid_t::regen( double periodicity )
{
  int resource_type = primary_resource();

  if ( resource_type == RESOURCE_ENERGY )
  {
    uptimes_energy_cap -> update( resource_current[ RESOURCE_ENERGY ] ==
                                  resource_max    [ RESOURCE_ENERGY ] );
  }
  else if ( resource_type == RESOURCE_MANA)
  {
    if ( buffs_glyph_of_innervate -> check() )
      resource_gain( RESOURCE_MANA, buffs_glyph_of_innervate -> value() * periodicity, gains_glyph_of_innervate );
  }
  else if ( resource_type == RESOURCE_RAGE )
  {
    if ( buffs_enrage -> up() )
      resource_gain( RESOURCE_RAGE, 1.0 * periodicity, gains_enrage );

    uptimes_rage_cap -> update( resource_current[ RESOURCE_RAGE ] ==
                                resource_max    [ RESOURCE_RAGE ] );
  }

  player_t::regen( periodicity );
}

// druid_t::available =======================================================

double druid_t::available() SC_CONST
{
  if ( primary_resource() != RESOURCE_ENERGY ) return 0.1;

  double energy = resource_current[ RESOURCE_ENERGY ];

  if ( energy > 25 ) return 0.1;

  return std::max( ( 25 - energy ) / energy_regen_per_second(), 0.1 );
}

// druid_t::composite_attack_power ==========================================

double druid_t::composite_attack_power() SC_CONST
{
  double ap = player_t::composite_attack_power();

  if ( buffs_bear_form -> check() || buffs_cat_form  -> check() )
  {
      ap += 2.0 * ( agility() - 10.0 );
  }

  if ( buffs_t11_4pc_melee -> check() )
  {
    ap *= 1.0 + buffs_t11_4pc_melee -> stack() * 0.01;
  }

  return floor( ap );
}

// druid_t::composite_attack_power_multiplier ===============================

double druid_t::composite_attack_power_multiplier() SC_CONST
{
  double multiplier = player_t::composite_attack_power_multiplier();

  if ( buffs_cat_form -> check() )
  {
    multiplier *= 1.0 + talents.heart_of_the_wild -> effect_base_value( 2 ) * 0.01;
  }
  if ( spec_aggression -> ok() )
  {
    multiplier *= 1.0 + spec_aggression -> base_value();
  }
  return multiplier;
}

// druid_t::composite_attack_crit ==========================================

double druid_t::composite_attack_crit() SC_CONST
{
  double c = player_t::composite_attack_crit();

  if ( buffs_cat_form -> check() )
  {
    c += talents.master_shapeshifter -> base_value() / 100.0;
  }

  return floor( c * 10000.0 ) / 10000.0;
}

// druid_t::composite_player_multiplier =======================================

double druid_t::composite_player_multiplier( const school_type school) SC_CONST
{
  double m = player_t::composite_player_multiplier( school );

  // Both eclipse buffs need their own checks
  if ( school == SCHOOL_ARCANE || school == SCHOOL_SPELLSTORM )
    if ( buffs_eclipse_lunar -> up() )
      m *= ( 1.0 + buffs_eclipse_lunar -> base_value() / 100.0 ) * 1.0 
            + composite_mastery() * mastery_total_eclipse -> base_value( E_APPLY_AURA, A_DUMMY );

  if ( school == SCHOOL_NATURE || school == SCHOOL_SPELLSTORM )
    if ( buffs_eclipse_solar -> up() )
      m *= ( 1.0 + buffs_eclipse_solar -> base_value() / 100.0 ) * 1.0
            + composite_mastery() *  mastery_total_eclipse -> base_value( E_APPLY_AURA, A_DUMMY );

  return m;
}

// druid_t::composite_spell_hit =============================================

double druid_t::composite_spell_hit() SC_CONST
{
  double hit = player_t::composite_spell_hit();
  // BoP does not convert base spirit into hit!
  hit += ( spirit() - attribute_base[ ATTR_SPIRIT ] ) * ( 0.01 * talents.balance_of_power -> effect_base_value( 2 ) ) / rating.spell_hit;

  return hit;
}

// druid_t::composite_spell_crit ============================================

double druid_t::composite_spell_crit() SC_CONST
{
  double crit = player_t::composite_spell_crit();
  crit += talents.natures_majesty -> base_value() / 100.0;
  return crit;
}

// druid_t::composite_attribute_multiplier ==================================

double druid_t::composite_attribute_multiplier( int attr ) SC_CONST
{
  double m = player_t::composite_attribute_multiplier( attr );

  // The matching_gear_multiplier is done statically for performance reasons,
  // unfortunately that's before we're in cat form or bear form, so let's compensate here
  if ( attr == ATTR_STAMINA && buffs_bear_form -> check() )
  {
    m *= 1.0 + talents.heart_of_the_wild -> effect_base_value( 1 ) * 0.01;
    if ( primary_tree() == TREE_FERAL)
      m *= 1.05;
  }
  else if ( attr == ATTR_AGILITY && buffs_cat_form -> check() )
  {
    if ( primary_tree() == TREE_FERAL )
      m *= 1.05;
  }

  return m;
}

// druid_t::matching_gear_multiplier ==================================

double druid_t::matching_gear_multiplier( const attribute_type attr ) SC_CONST
{
  switch ( primary_tree() )
  {
  case TREE_BALANCE:
  case TREE_RESTORATION:
    if ( attr == ATTR_INTELLECT )
      return 0.05;
    break;
  default:
    break;
  }

  return 0.0;
}

// druid_t::composite_tank_crit =============================================

double druid_t::composite_tank_crit( const school_type school ) SC_CONST
{
  double c = player_t::composite_tank_crit( school );

  if ( school == SCHOOL_PHYSICAL )
  {
    c += talents.thick_hide -> effect_base_value( 3 ) / 100.0;
    if ( c < 0 ) c = 0;
  }

  return c;
}

// druid_t::create_expression ===============================================

action_expr_t* druid_t::create_expression( action_t* a, const std::string& name_str )
{
  if ( name_str == "eclipse" )
  {
    struct eclipse_expr_t : public action_expr_t
    {
      eclipse_expr_t( action_t* a) : action_expr_t( a, "eclipse", TOK_NUM ) {}
      virtual int evaluate() { result_num = action -> player -> cast_druid() -> eclipse_bar_value; return TOK_NUM; }
    };
    return new eclipse_expr_t( a );
  }
  else if ( name_str == "eclipse_dir" )
  {
    struct eclipse_dir_expr_t : public action_expr_t
    {
      eclipse_dir_expr_t( action_t* a) : action_expr_t( a, "eclipse_dir", TOK_NUM ) {}
      virtual int evaluate() { result_num = action -> player -> cast_druid() -> eclipse_bar_direction; return TOK_NUM; }
    };
    return new eclipse_dir_expr_t( a );
  }
  return player_t::create_expression( a, name_str );
}

// druid_t::decode_set ======================================================

int druid_t::decode_set( item_t& item )
{
  if ( item.slot != SLOT_HEAD      &&
       item.slot != SLOT_SHOULDERS &&
       item.slot != SLOT_CHEST     &&
       item.slot != SLOT_HANDS     &&
       item.slot != SLOT_LEGS      )
  {
    return SET_NONE;
  }

  const char* s = item.name();

  bool is_caster = ( strstr( s, "cover"     ) ||
                     strstr( s, "mantle"    ) ||
                     strstr( s, "vestment"  ) ||
                     strstr( s, "trousers"  ) ||
                     strstr( s, "gloves"    ) );

  bool is_melee = ( strstr( s, "headguard"    ) ||
                    strstr( s, "shoulderpads" ) ||
                    strstr( s, "raiment"      ) ||
                    strstr( s, "legguards"    ) ||
                    strstr( s, "handgrips"    ) );

  if ( strstr( s, "lasherweave" ) )
  {
    if ( is_caster ) return SET_T10_CASTER;
    if ( is_melee  ) return SET_T10_MELEE;
  }

  if ( strstr( s, "stormriders" ) )
  {
    bool is_caster = ( strstr( s, "cover"         ) ||
                       strstr( s, "shoulderwraps" ) ||
                       strstr( s, "vestment"      ) ||
                       strstr( s, "leggings"      ) ||
                       strstr( s, "gloves"        ) );

    bool is_melee = ( strstr( s, "headpiece"    ) ||
                      strstr( s, "spaulders"    ) ||
                      strstr( s, "raiment"      ) ||
                      strstr( s, "legguards"    ) ||
                      strstr( s, "grips"        ) );
    if ( is_caster ) return SET_T11_CASTER;
    if ( is_melee  ) return SET_T11_MELEE;
  }

  return SET_NONE;
}

// druid_t::primary_role ====================================================

int druid_t::primary_role() SC_CONST
{

  if ( primary_tree() == TREE_BALANCE )
  {
    return ROLE_SPELL;
  }

  else if ( primary_tree() == TREE_FERAL )
  {
    if ( player_t::primary_role() == ROLE_TANK )
        return ROLE_TANK;

    // Implement Automatic Tank detection

    return ROLE_ATTACK;
  }

  else if ( primary_tree() == TREE_RESTORATION )
  {
    if ( player_t::primary_role() == ROLE_SPELL )
      return ROLE_SPELL;

    return ROLE_HEAL;
  }

  return ROLE_NONE;

}

// druid_t::primary_resource ================================================

int druid_t::primary_resource() SC_CONST
{
  if ( primary_role() == ROLE_SPELL || primary_role() == ROLE_HEAL ) return RESOURCE_MANA;
  if ( primary_role() == ROLE_TANK ) return RESOURCE_RAGE;
  return RESOURCE_ENERGY;
}

// druid_t::target_swing ====================================================

int druid_t::target_swing()
{
  int result = player_t::target_swing();

  if ( sim -> log ) log_t::output( sim, "%s swing result: %s", sim -> target -> name(), util_t::result_type_string( result ) );

  if ( result == RESULT_HIT    ||
       result == RESULT_CRIT   ||
       result == RESULT_GLANCE )
  {
    resource_gain( RESOURCE_RAGE, 100.0, gains_incoming_damage );  // FIXME! Assume it caps rage every time.
  }
  if ( result == RESULT_DODGE )
  {
    if ( talents.natural_reaction -> rank() )
    {
      resource_gain( RESOURCE_RAGE, talents.natural_reaction -> effect_base_value( 2 ), gains_natural_reaction );
    }
  }

  return result;
}

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_druid  ==================================================

player_t* player_t::create_druid( sim_t*             sim,
                                  const std::string& name,
                                  race_type r )
{
  return new druid_t( sim, name, r );
}

// player_t::druid_init =====================================================

void player_t::druid_init( sim_t* sim )
{
  sim -> auras.leader_of_the_pack = new aura_t( sim, "leader_of_the_pack" );
  sim -> auras.moonkin            = new aura_t( sim, "moonkin" );

  for ( unsigned int i = 0; i < sim -> actor_list.size(); i++ )
  {
    player_t* p = sim -> actor_list[i];
    p -> buffs.innervate              = new buff_t( p, "innervate",        1, 10.0 );
    p -> buffs.mark_of_the_wild       = new buff_t( p, "mark_of_the_wild", !p -> is_pet() );
    p -> debuffs.earth_and_moon       = new debuff_t( p, "earth_and_moon",       1,  12.0 );
    p -> debuffs.faerie_fire          = new debuff_t( p, "faerie_fire",          3, 300.0 );
    p -> debuffs.infected_wounds      = new debuff_t( p, "infected_wounds",      1,  12.0 );
    p -> debuffs.mangle               = new debuff_t( p, "mangle",               1,  60.0 );
  }

}

// player_t::druid_combat_begin =============================================

void player_t::druid_combat_begin( sim_t* sim )
{
  if ( sim -> overrides.leader_of_the_pack     ) sim -> auras.leader_of_the_pack -> override();
  if ( sim -> overrides.moonkin_aura           ) sim -> auras.moonkin            -> override();

  for ( player_t* p = sim -> player_list; p; p = p -> next )
  {
    if ( p -> ooc_buffs() )
    {
      if ( sim -> overrides.mark_of_the_wild ) p -> buffs.mark_of_the_wild -> override();
    }
  }

  for ( target_t* t = sim -> target_list; t; t = t -> next )
  {
    if ( sim -> overrides.earth_and_moon       ) t -> debuffs.earth_and_moon       -> override( 1, 8 );
    if ( sim -> overrides.faerie_fire          ) t -> debuffs.faerie_fire          -> override( 3, 0.04 );
    if ( sim -> overrides.infected_wounds      ) t -> debuffs.infected_wounds      -> override();
    if ( sim -> overrides.mangle               ) t -> debuffs.mangle               -> override();
  }
}

