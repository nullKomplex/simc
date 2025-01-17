// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.hpp"

// ==========================================================================
//
// ==========================================================================

namespace
{ // UNNAMED NAMESPACE

// ==========================================================================
// Warrior
// ==========================================================================

struct warrior_t;

enum warrior_stance { STANCE_BATTLE = 2, STANCE_DEFENSE = 4, STANCE_GLADIATOR = 8 };

struct warrior_td_t: public actor_pair_t
{
  dot_t* dots_bloodbath;
  dot_t* dots_deep_wounds;
  dot_t* dots_ravager;
  dot_t* dots_rend;

  buff_t* debuffs_colossus_smash;
  buff_t* debuffs_demoralizing_shout;
  buff_t* debuffs_taunt;

  warrior_t& warrior;
  warrior_td_t( player_t* target, warrior_t& p );
};

struct warrior_t: public player_t
{
public:
  int initial_rage;
  double arms_rage_mult;
  double crit_rage_mult;
  bool swapping; // Disables automated swapping when it's not required to use the ability.
  // Set to true whenever a player uses the swap option inside of stance_t, as we should assume they are intentionally sitting in defensive stance.

  simple_sample_data_t cs_damage;
  simple_sample_data_t priority_damage;
  simple_sample_data_t all_damage;

  // Active
  action_t* active_blood_craze;
  action_t* active_bloodbath_dot;
  action_t* active_deep_wounds;
  action_t* active_enhanced_rend;
  action_t* active_rallying_cry_heal;
  action_t* active_second_wind;
  attack_t* active_sweeping_strikes;

  heal_t* active_t16_2pc;
  warrior_stance active_stance;

  // Buffs
  struct buffs_t
  {
    // All Warriors
    buff_t* battle_stance;
    buff_t* berserker_rage;
    buff_t* defensive_stance;
    buff_t* heroic_charge;
    // Talents
    buff_t* avatar;
    buff_t* bloodbath;
    buff_t* enraged_regeneration;
    buff_t* gladiator_stance;
    buff_t* ignite_weapon;
    buff_t* ravager;
    buff_t* sudden_death;
    // Glyphs
    buff_t* enraged_speed;
    buff_t* hamstring;
    buff_t* heroic_leap_glyph;
    buff_t* raging_blow_glyph;
    buff_t* raging_wind;
    buff_t* rude_interruption;
    // Arms and Fury
    buff_t* die_by_the_sword;
    buff_t* rallying_cry;
    buff_t* recklessness;
    // Fury and Prot
    buff_t* enrage;
    // Fury Only
    buff_t* bloodsurge;
    buff_t* meat_cleaver;
    buff_t* raging_blow;
    // Arms only
    buff_t* slam;
    buff_t* sweeping_strikes;
    // Prot only
    buff_t* bladed_armor;
    buff_t* blood_craze;
    buff_t* last_stand;
    absorb_buff_t* shield_barrier;
    buff_t* shield_block;
    buff_t* shield_charge;
    buff_t* shield_wall;
    buff_t* sword_and_board;
    buff_t* ultimatum;
    buff_t* unyielding_strikes;

    // Tier bonuses
    buff_t* tier15_2pc_tank;
    buff_t* tier16_reckless_defense;
    buff_t* tier17_4pc_arms;
    buff_t* tier17_4pc_fury;
    buff_t* tier17_4pc_fury_driver;
  } buff;

  // Cooldowns
  struct cooldowns_t
  {
    // All Warriors
    cooldown_t* charge;
    cooldown_t* heroic_leap;
    cooldown_t* intervene;
    cooldown_t* rage_from_charge;
    cooldown_t* stance_swap;
    cooldown_t* stance_cooldown;
    // Talents
    cooldown_t* avatar;
    cooldown_t* bladestorm;
    cooldown_t* bloodbath;
    cooldown_t* dragon_roar;
    cooldown_t* shockwave;
    cooldown_t* storm_bolt;
    // Fury And Arms
    cooldown_t* die_by_the_sword;
    cooldown_t* recklessness;
    // Prot Only
    cooldown_t* block;
    cooldown_t* demoralizing_shout;
    cooldown_t* last_stand;
    cooldown_t* rage_from_crit_block;
    cooldown_t* revenge;
    cooldown_t* shield_slam;
    cooldown_t* shield_wall;
  } cooldown;

  // Gains
  struct gains_t
  {
    // All Warriors
    gain_t* avoided_attacks;
    gain_t* charge;
    gain_t* enrage;
    gain_t* melee_main_hand;
    // Fury and Prot
    gain_t* defensive_stance;
    // Fury and Arms
    gain_t* drawn_sword_glyph;
    // Fury Only
    gain_t* bloodthirst;
    gain_t* colossus_smash;
    gain_t* melee_off_hand;
    // Arms Only
    gain_t* melee_crit;
    gain_t* sweeping_strikes;
    gain_t* taste_for_blood;
    // Prot Only
    gain_t* critical_block;
    gain_t* revenge;
    gain_t* shield_slam;
    gain_t* sword_and_board;
    // Tier bonuses
    gain_t* tier15_4pc_tank;
    gain_t* tier16_2pc_melee;
    gain_t* tier16_4pc_tank;
    gain_t* tier17_2pc_arms;
  } gain;

  // Spells
  struct spells_t
  {
    // All Warriors
    const spell_data_t* charge;
    const spell_data_t* intervene;
    const spell_data_t* headlong_rush;
    const spell_data_t* heroic_leap;
  } spell;

  // Glyphs
  struct glyphs_t
  {
    // All Warriors
    const spell_data_t* bull_rush;
    const spell_data_t* cleave;
    const spell_data_t* death_from_above;
    const spell_data_t* enraged_speed;
    const spell_data_t* hamstring;
    const spell_data_t* heroic_leap;
    const spell_data_t* long_charge;
    const spell_data_t* rude_interruption;
    const spell_data_t* unending_rage;
    const spell_data_t* victory_rush;
    // Fury and Arms
    const spell_data_t* colossus_smash;
    const spell_data_t* drawn_sword;
    const spell_data_t* rallying_cry;
    const spell_data_t* recklessness;
    // Fury only
    const spell_data_t* bloodthirst;
    const spell_data_t* raging_blow;
    const spell_data_t* raging_wind;
    // Arms only
    const spell_data_t* sweeping_strikes;
    // Arms and Prot
    const spell_data_t* resonating_power;
    // Prot only
    const spell_data_t* shield_wall;
  } glyphs;

  // Mastery
  struct mastery_t
  {
    const spell_data_t* critical_block; //Protection
    const spell_data_t* unshackled_fury; //Fury
    const spell_data_t* weapons_master; //Arms
  } mastery;

  // Procs
  struct procs_t
  {
    proc_t* raging_blow_wasted;
    proc_t* sudden_death;
    proc_t* sudden_death_wasted;
    proc_t* bloodsurge;
    proc_t* bloodsurge_wasted;

    //Tier bonuses
    proc_t* t15_2pc_melee;
    proc_t* t17_4pc_arms;
    proc_t* t17_2pc_fury;
  } proc;

  real_ppm_t t15_2pc_melee;

  // Spec Passives
  struct spec_t
  {
    //Arms-only
    const spell_data_t* mortal_strike;
    const spell_data_t* readiness_arms;
    const spell_data_t* rend;
    const spell_data_t* seasoned_soldier;
    const spell_data_t* sweeping_strikes;
    const spell_data_t* weapon_mastery;
    //Arms and Prot
    const spell_data_t* thunder_clap;
    //Arms and Fury
    const spell_data_t* colossus_smash;
    const spell_data_t* die_by_the_sword;
    const spell_data_t* rallying_cry;
    const spell_data_t* recklessness;
    const spell_data_t* whirlwind;
    //Fury-only
    const spell_data_t* bloodsurge;
    const spell_data_t* bloodthirst;
    const spell_data_t* crazed_berserker;
    const spell_data_t* cruelty;
    const spell_data_t* meat_cleaver;
    const spell_data_t* raging_blow;
    const spell_data_t* readiness_fury;
    const spell_data_t* singleminded_fury;
    const spell_data_t* wild_strike;
    // Fury and Prot
    const spell_data_t* enrage;
    //Prot-only
    const spell_data_t* bastion_of_defense;
    const spell_data_t* bladed_armor;
    const spell_data_t* blood_craze;
    const spell_data_t* devastate;
    const spell_data_t* last_stand;
    const spell_data_t* readiness_protection;
    const spell_data_t* resolve;
    const spell_data_t* revenge;
    const spell_data_t* riposte;
    const spell_data_t* shield_mastery;
    const spell_data_t* shield_slam;
    const spell_data_t* sword_and_board;
    const spell_data_t* ultimatum;
    const spell_data_t* unwavering_sentinel;
  } spec;

  // Talents
  struct talents_t
  {
    const spell_data_t* juggernaut;
    const spell_data_t* double_time;
    const spell_data_t* warbringer;

    const spell_data_t* enraged_regeneration;
    const spell_data_t* second_wind;
    const spell_data_t* impending_victory;

    const spell_data_t* taste_for_blood;
    const spell_data_t* unyielding_strikes;
    const spell_data_t* sudden_death;
    const spell_data_t* unquenchable_thirst;
    const spell_data_t* heavy_repercussions;
    const spell_data_t* slam;
    const spell_data_t* furious_strikes;

    const spell_data_t* storm_bolt;
    const spell_data_t* shockwave;
    const spell_data_t* dragon_roar;

    const spell_data_t* mass_spell_reflection;
    const spell_data_t* safeguard;
    const spell_data_t* vigilance;

    const spell_data_t* avatar;
    const spell_data_t* bloodbath;
    const spell_data_t* bladestorm;

    const spell_data_t* anger_management;
    const spell_data_t* ravager;
    const spell_data_t* ignite_weapon; // Arms/Fury talent
    const spell_data_t* gladiators_resolve; // Protection talent
  } talents;

  // Perks
  struct
  {
    //All Specs
    const spell_data_t* improved_heroic_leap;
    //Arms and Fury
    const spell_data_t* improved_die_by_the_sword;
    //Arms only
    const spell_data_t* enhanced_rend;
    const spell_data_t* enhanced_sweeping_strikes;
    //Fury only
    const spell_data_t* enhanced_whirlwind;
    const spell_data_t* improved_colossus_smash;
    //Protection only
    const spell_data_t* improved_block;
    const spell_data_t* improved_defensive_stance;
    const spell_data_t* improved_heroic_throw;
  } perk;

  warrior_t( sim_t* sim, const std::string& name, race_e r = RACE_NIGHT_ELF ):
    player_t( sim, WARRIOR, name, r ),
    buff( buffs_t() ),
    cooldown( cooldowns_t() ),
    gain( gains_t() ),
    glyphs( glyphs_t() ),
    mastery( mastery_t() ),
    proc( procs_t() ),
    t15_2pc_melee( *this ),
    spec( spec_t() ),
    talents( talents_t() )
  {
    // Active
    active_bloodbath_dot      = 0;
    active_blood_craze        = 0;
    active_deep_wounds        = 0;
    active_rallying_cry_heal  = 0;
    active_second_wind        = 0;
    active_sweeping_strikes   = 0;
    active_enhanced_rend      = 0;
    active_t16_2pc            = 0;
    active_stance             = STANCE_BATTLE;

    // Cooldowns
    cooldown.avatar                   = get_cooldown( "avatar" );
    cooldown.bladestorm               = get_cooldown( "bladestorm" );
    cooldown.block                    = get_cooldown( "block" );
    cooldown.block                    -> duration = timespan_t::from_seconds( 1.5 );
    cooldown.bloodbath                = get_cooldown( "bloodbath" );
    cooldown.charge                   = get_cooldown( "charge" );
    cooldown.demoralizing_shout       = get_cooldown( "demoralizing_shout" );
    cooldown.die_by_the_sword         = get_cooldown( "die_by_the_sword" );
    cooldown.dragon_roar              = get_cooldown( "dragon_roar" );
    cooldown.heroic_leap              = get_cooldown( "heroic_leap" );
    cooldown.intervene                = get_cooldown( "intervene" );
    cooldown.last_stand               = get_cooldown( "last_stand" );
    cooldown.rage_from_charge         = get_cooldown( "rage_from_charge" );
    cooldown.rage_from_charge         -> duration = timespan_t::from_seconds( 12.0 );
    cooldown.rage_from_crit_block     = get_cooldown( "rage_from_crit_block" );
    cooldown.rage_from_crit_block    -> duration = timespan_t::from_seconds( 3.0 );
    cooldown.recklessness             = get_cooldown( "recklessness" );
    cooldown.revenge                  = get_cooldown( "revenge" );
    cooldown.shield_slam              = get_cooldown( "shield_slam" );
    cooldown.shield_wall              = get_cooldown( "shield_wall" );
    cooldown.shockwave                = get_cooldown( "shockwave" );
    cooldown.stance_swap              = get_cooldown( "stance_swap" );
    cooldown.stance_swap             -> duration = timespan_t::from_seconds( 1.5 );
    cooldown.stance_cooldown          = get_cooldown( "stance_cooldown" );
    cooldown.stance_cooldown         -> duration = timespan_t::from_seconds( 6.0 );
    cooldown.storm_bolt               = get_cooldown( "storm_bolt" );

    initial_rage = 0;
    arms_rage_mult = 2.15;
    crit_rage_mult = 2;
    swapping = false;
    base.distance = 3.0;

    regen_type = REGEN_DISABLED;
  }

  // Character Definition
  virtual void      init_spells();
  virtual void      init_defense();
  virtual void      init_base_stats();
  virtual void      init_scaling();
  virtual void      create_buffs();
  virtual void      init_gains();
  virtual void      init_position();
  virtual void      init_procs();
  virtual void      init_rng();
  virtual void      arise();
  virtual void      combat_begin();
  virtual double    composite_attribute( attribute_e attr ) const;
  virtual double    composite_rating_multiplier( rating_e rating ) const;
  virtual double    composite_player_multiplier( school_e school ) const;
  virtual double    composite_player_critical_damage_multiplier() const;
  virtual double    matching_gear_multiplier( attribute_e attr ) const;
  virtual double    composite_armor_multiplier() const;
  virtual double    composite_block() const;
  virtual double    composite_parry_rating() const;
  virtual double    composite_parry() const;
  virtual double    composite_melee_expertise( weapon_t* ) const;
  virtual double    composite_attack_power_multiplier() const;
  virtual double    composite_melee_attack_power() const;
  virtual double    composite_crit_block() const;
  virtual double    composite_crit_avoidance() const;
  virtual double    composite_melee_speed() const;
  virtual double    composite_melee_crit() const;
  virtual double    composite_spell_crit() const;
  virtual void      reset();
  virtual void      create_options();
  virtual action_t* create_proc_action( const std::string& name );
  virtual bool      create_profile( std::string& profile_str, save_e type, bool save_html );
  virtual void      invalidate_cache( cache_e );
  virtual double    passive_movement_modifier() const;
  virtual double    temporary_movement_modifier() const;

  void              apl_precombat();
  void              apl_default();
  void              apl_fury();
  void              apl_arms();
  void              apl_prot();
  virtual void      init_action_list();

  virtual action_t* create_action( const std::string& name, const std::string& options );
  virtual set_e     decode_set( const item_t& ) const;
  virtual resource_e primary_resource() const { return RESOURCE_RAGE; }
  virtual role_e    primary_role() const;
  virtual stat_e    convert_hybrid_stat( stat_e s ) const;
  virtual void      assess_damage( school_e, dmg_e, action_state_t* s );
  virtual void      copy_from( player_t* source );
  virtual void      merge( player_t& other ) override
  {
    warrior_t& other_p = dynamic_cast<warrior_t&>( other );

    cs_damage.merge( other_p.cs_damage );
    all_damage.merge( other_p.all_damage );
    priority_damage.merge( other_p.priority_damage );

    player_t::merge( other );
  }

  // Custom Warrior Functions
  void enrage();
  void stance_swap();

  target_specific_t<warrior_td_t*> target_data;

  virtual warrior_td_t* get_target_data( player_t* target ) const
  {
    warrior_td_t*& td = target_data[target];

    if ( !td )
    {
      td = new warrior_td_t( target, const_cast<warrior_t&>( *this ) );
    }
    return td;
  }
};

namespace
{ // UNNAMED NAMESPACE

// Template for common warrior action code. See priest_action_t.
template <class Base>
struct warrior_action_t: public Base
{
  int stancemask;
  bool headlongrush;
  bool recklessness;
private:
  typedef Base ab; // action base, eg. spell_t
public:
  typedef warrior_action_t base_t;

  warrior_action_t( const std::string& n, warrior_t* player,
                    const spell_data_t* s = spell_data_t::nil() ):
                    ab( n, player, s ),
                    stancemask( STANCE_BATTLE | STANCE_DEFENSE | STANCE_GLADIATOR ),
                    headlongrush( ab::data().affected_by( player -> spell.headlong_rush -> effectN( 1 ) ) ),
                    recklessness( ab::data().affected_by( player -> spec.recklessness -> effectN( 1 ) ) )
  {
    ab::may_crit = true;
  }

  virtual ~warrior_action_t() {}

  warrior_t* p()
  {
    return debug_cast<warrior_t*>( ab::player );
  }
  const warrior_t* p() const
  {
    return debug_cast<warrior_t*>( ab::player );
  }

  warrior_td_t* td( player_t* t ) const
  {
    return p() -> get_target_data( t );
  }

  virtual void update_ready( timespan_t cd_duration )
  {
    //Head Long Rush reduces the cooldown depending on the amount of haste.
    if ( headlongrush )
    {
      cd_duration = ab::cooldown -> duration;
      cd_duration *= ab::player -> cache.attack_haste();
    }
    ab::update_ready( cd_duration );
  }

  virtual double composite_crit() const
  {
    double cc = ab::composite_crit();

    if ( recklessness )
      cc += p() -> buff.recklessness -> value();

    return cc;
  }

  virtual bool ready()
  {
    if ( !ab::ready() )
      return false;

    if ( p() -> active_stance == STANCE_GLADIATOR ) // Gladiator can use all abilities no matter the stance.
      return true;

    // Attack available in current stance?
    if ( ( ( stancemask & p() -> active_stance ) == 0 ) && p() -> cooldown.stance_swap -> up() )
    {
      p() -> stance_swap();
      return false;
    }
    else if ( ( ( stancemask & p() -> active_stance ) == 0 ) && p() -> cooldown.stance_swap -> down() )
      return false;

    return true;
  }

  virtual void execute()
  {
    if ( p() -> cooldown.stance_swap -> up() && p() -> swapping == false )
    {
      if ( p() -> active_stance == STANCE_DEFENSE &&
           p() -> specialization() != WARRIOR_PROTECTION &&
           ( ( stancemask & STANCE_BATTLE ) != 0 ) )
           p() -> stance_swap();
      else if ( p() -> active_stance == STANCE_BATTLE &&
                p() -> specialization() == WARRIOR_PROTECTION &&
                ( ( stancemask & STANCE_DEFENSE ) != 0 ) )
                p() -> stance_swap();
    }
    ab::execute();
  }

  virtual void impact( action_state_t* s )
  {
    ab::impact( s );

    if ( ab::sim -> log || ab::sim -> debug )
    {
      ab::sim -> out_debug.printf(
        "Strength: %4.4f, AP: %4.4f, Crit: %4.4f%%, Crit Dmg Mult: %4.4f,  Mastery: %4.4f%%, Multistrike: %4.4f%%, Haste: %4.4f%%, Versatility: %4.4f%%, Bonus Armor: %4.4f, Tick Multiplier: %4.4f, Direct Multiplier: %4.4f, Action Multiplier: %4.4f",
        p() -> cache.strength(),
        p() -> cache.attack_power(),
        p() -> cache.attack_crit() * 100,
        p() -> composite_player_critical_damage_multiplier(),
        p() -> cache.mastery_value() * 100,
        p() -> cache.multistrike() * 100,
        ( ( 1 / ( p() -> cache.attack_haste() ) ) - 1 ) * 100,
        p() -> cache.damage_versatility() * 100,
        p() -> cache.bonus_armor(),
        s -> composite_ta_multiplier(),
        s -> composite_da_multiplier(),
        s -> action -> action_multiplier() );
    }
  }

  virtual void tick( dot_t* d )
  {
    ab::tick( d );

    if ( ab::sim -> log || ab::sim -> debug )
    {
      ab::sim -> out_debug.printf(
        "Strength: %4.4f, AP: %4.4f, Crit: %4.4f%%, Crit Dmg Mult: %4.4f,  Mastery: %4.4f%%, Multistrike: %4.4f%%, Haste: %4.4f%%, Versatility: %4.4f%%, Bonus Armor: %4.4f, Tick Multiplier: %4.4f, Direct Multiplier: %4.4f, Action Multiplier: %4.4f",
        p() -> cache.strength(),
        p() -> cache.attack_power(),
        p() -> cache.attack_crit() * 100,
        p() -> composite_player_critical_damage_multiplier(),
        p() -> cache.mastery_value() * 100,
        p() -> cache.multistrike() * 100,
        ( ( 1 / ( p() -> cache.attack_haste() ) ) - 1 ) * 100,
        p() -> cache.damage_versatility() * 100,
        p() -> cache.bonus_armor(),
        d -> state -> composite_ta_multiplier(),
        d -> state -> composite_da_multiplier(),
        d -> state -> action -> action_ta_multiplier() );
    }
  }

  virtual void assess_damage( dmg_e type,
                              action_state_t* s )
  {
    ab::assess_damage( type, s );

    if ( td( s -> target ) -> debuffs_colossus_smash -> up() && s -> result_amount > 0 )
      p() -> cs_damage.add( s -> result_amount );

    if ( ( s -> target == p() -> sim -> target ) && s -> result_amount > 0 )
      p() -> priority_damage.add( s -> result_amount );

    if ( s -> result_amount > 0 && s -> target != p() )
      p() -> all_damage.add( s -> result_amount );
  }

  virtual void consume_resource()
  {
    ab::consume_resource();

    double rage = ab::resource_consumed;

    if ( p() -> talents.anger_management -> ok() )
      anger_management(rage);

    if ( ab::result_is_miss( ab::execute_state -> result ) && rage > 0 && !ab::aoe )
      p() -> resource_gain( RESOURCE_RAGE, rage*0.8, p() -> gain.avoided_attacks );
  }

  void anger_management( double rage )
  {
    if ( rage > 0 )
    {
      //Anger management takes the amount of rage spent and reduces the cooldown of abilities by 1 second per 30 rage.
      rage /= p() -> talents.anger_management -> effectN( 1 ).base_value();
      rage *= -1;
      p() -> cooldown.heroic_leap -> adjust( timespan_t::from_seconds( rage ) ); //All specs

      if ( p() -> specialization() != WARRIOR_PROTECTION )
      {
        // Fourth Tier Talents
        if ( p() -> talents.storm_bolt -> ok() )
          p() -> cooldown.storm_bolt -> adjust( timespan_t::from_seconds( rage ) );
        else if ( p() -> talents.dragon_roar -> ok() )
          p() -> cooldown.dragon_roar -> adjust( timespan_t::from_seconds( rage ) );
        else if ( p() -> talents.shockwave -> ok() )
          p() -> cooldown.shockwave -> adjust( timespan_t::from_seconds( rage ) );
        // Sixth tier talents
        if ( p() -> talents.bladestorm -> ok() )
          p() -> cooldown.bladestorm -> adjust( timespan_t::from_seconds( rage ) );
        else if ( p() -> talents.bloodbath -> ok() )
          p() -> cooldown.bloodbath -> adjust( timespan_t::from_seconds( rage ) );
        else if ( p() -> talents.avatar -> ok() )
          p() -> cooldown.avatar -> adjust( timespan_t::from_seconds( rage ) );

        p() -> cooldown.recklessness -> adjust( timespan_t::from_seconds( rage ) );
        p() -> cooldown.die_by_the_sword -> adjust( timespan_t::from_seconds( rage ) );
      }
      else if ( p() -> specialization() == WARRIOR_PROTECTION )
      {
        p() -> cooldown.demoralizing_shout -> adjust( timespan_t::from_seconds( rage ) );
        p() -> cooldown.last_stand -> adjust( timespan_t::from_seconds( rage ) );
        p() -> cooldown.shield_wall -> adjust( timespan_t::from_seconds( rage ) );
      }
    }
  }

  virtual timespan_t gcd() const
  {
    timespan_t t = ab::action_t::gcd();

    if ( t == timespan_t::zero() )
      return t;

    t *= ab::player -> cache.attack_haste();
    if ( t < ab::min_gcd )
      t = ab::min_gcd;

    return t;
  }
};

// ==========================================================================
// Warrior Heals
// ==========================================================================

struct warrior_heal_t: public warrior_action_t < heal_t >
{
  warrior_heal_t( const std::string& n, warrior_t* p, const spell_data_t* s = spell_data_t::nil() ):
    base_t( n, p, s )
  {
    may_crit = tick_may_crit = hasted_ticks = false;
    target = p;
  }
};

// ==========================================================================
// Warrior Attack
// ==========================================================================

struct warrior_attack_t: public warrior_action_t < melee_attack_t >
{
  warrior_attack_t( const std::string& n, warrior_t* p,
                    const spell_data_t* s = spell_data_t::nil() ):
                    base_t( n, p, s )
  {
    special = true;
  }

  virtual double target_armor( player_t* t ) const
  {
    double a = base_t::target_armor( t );

    a *= 1.0 - td( t ) -> debuffs_colossus_smash -> current_value;

    return a;
  }

  virtual void   execute();

  virtual void   impact( action_state_t* s );

  virtual double calculate_weapon_damage( double attack_power )
  {
    double dmg = base_t::calculate_weapon_damage( attack_power );

    // Catch the case where weapon == 0 so we don't crash/retest below.
    if ( dmg == 0.0 )
      return 0;

    if ( weapon -> slot == SLOT_OFF_HAND )
    {
      dmg *= 1.0 + p() -> spec.crazed_berserker -> effectN( 2 ).percent();

      if ( p() -> main_hand_weapon.group() == WEAPON_1H &&
           p() -> off_hand_weapon.group() == WEAPON_1H )
           dmg *= 1.0 + p() -> spec.singleminded_fury -> effectN( 2 ).percent();
    }
    return dmg;
  }

  // helper functions

  void trigger_bloodbath_dot( player_t* t, double dmg )
  {
    if ( !p() -> buff.bloodbath -> check() )
      return;

    residual_action::trigger(
      p() -> active_bloodbath_dot, // ignite spell
      t, // target
      p() -> buff.bloodbath -> data().effectN( 1 ).percent() * dmg );
  }

  void trigger_rage_gain( action_state_t* s )
  {
    // MoP: base rage gain is 3.5 * weaponspeed and half that for off-hand
    // Defensive/Gladiator stance: -100%
    // Arms warriors get 2.4 times the rage per swing, and 4.8 times the rage on crit swings.
    // They get half rage while in defensive stance.

    if ( p() -> active_stance != STANCE_BATTLE && p() -> specialization() != WARRIOR_ARMS )
      return;

    weapon_t*  w = weapon;
    double rage_gain = 3.5 * w -> swing_time.total_seconds();

    if ( p() -> specialization() == WARRIOR_ARMS && p() -> active_stance == STANCE_BATTLE )
      rage_gain *= p() -> arms_rage_mult;

    if ( p() -> specialization() == WARRIOR_ARMS && s -> result == RESULT_CRIT )
      rage_gain *= p() -> crit_rage_mult;

    if ( w -> slot == SLOT_OFF_HAND )
      rage_gain /= 2.0;

    rage_gain = floor( rage_gain * 10 ) / 10.0;

    if ( p() -> specialization() == WARRIOR_ARMS && s -> result == RESULT_CRIT )
    {
      p() -> resource_gain( RESOURCE_RAGE,
                            rage_gain,
                            p() -> gain.melee_crit );
      return;
    }

    p() -> resource_gain( RESOURCE_RAGE,
                          rage_gain,
                          w -> slot == SLOT_OFF_HAND ? p() -> gain.melee_off_hand : p() -> gain.melee_main_hand );
  }
};

// trigger_bloodbath ========================================================

struct bloodbath_dot_t: public residual_action::residual_periodic_action_t < warrior_attack_t >
{
  bloodbath_dot_t( warrior_t* p ):
    base_t( "bloodbath", p, p -> find_spell( 113344 ) )
  {
    dual = true;
  }
};

// ==========================================================================
// Static Functions
// ==========================================================================

// trigger_sweeping_strikes =================================================

static void trigger_sweeping_strikes( action_state_t* s )
{
  struct sweeping_strikes_attack_t: public warrior_attack_t
  {
    double pct_damage;
    sweeping_strikes_attack_t( warrior_t* p ):
      warrior_attack_t( "sweeping_strikes_attack", p, p -> spec.sweeping_strikes )
    {
      may_miss = may_dodge = may_parry = may_crit = may_block = callbacks = false;
      aoe = 1;
      weapon_multiplier = 0;
      base_costs[RESOURCE_RAGE] = 0; //Resource consumption already accounted for in the buff application.
      cooldown -> duration = timespan_t::zero(); // Cooldown accounted for in the buff.
      pct_damage = data().effectN( 1 ).percent();
    }

    double target_armor( player_t* ) const
    {
      return 0; // Armor accounted for in previous attack.
    }

    double composite_player_multiplier() const
    {
      return 1; // No double dipping
    }

    void execute()
    {
      base_dd_max *= pct_damage; //Deals 50% of original damage
      base_dd_min *= pct_damage;

      warrior_attack_t::execute();
    }

    size_t available_targets( std::vector< player_t* >& tl ) const
    {
      tl.clear();

      for ( size_t i = 0, actors = sim -> actor_list.size(); i < actors; i++ )
      {
        player_t* t = sim -> actor_list[i];

        if ( !t -> is_sleeping() && t -> is_enemy() && ( t != target ) )
          tl.push_back( t );
      }

      return tl.size();
    }

    void impact( action_state_t* s )
    {
      warrior_attack_t::impact( s );

      if ( result_is_hit( s -> result ) && p() -> glyphs.sweeping_strikes -> ok() )
        p() -> resource_gain( RESOURCE_RAGE, p() -> glyphs.sweeping_strikes -> effectN( 1 ).base_value(), p() -> gain.sweeping_strikes );
    }
  };

  warrior_t* p = debug_cast<warrior_t*>( s -> action -> player );

  if ( !p -> buff.sweeping_strikes -> check() )
    return;

  if ( !s -> action -> weapon )
    return;

  if ( !s -> action -> result_is_hit( s -> result ) && !s -> action -> result_is_multistrike( s -> result ) )
    return;

  if ( s -> action -> sim -> active_enemies == 1 )
    return;

  if ( !p -> active_sweeping_strikes )
  {
    p -> active_sweeping_strikes = new sweeping_strikes_attack_t( p );
    p -> active_sweeping_strikes -> init();
  }

  p -> active_sweeping_strikes -> base_dd_min = s -> result_amount;
  p -> active_sweeping_strikes -> base_dd_max = s -> result_amount;
  p -> active_sweeping_strikes -> execute();

  return;
}

// trigger_t15_2pc_melee ====================================================

static bool trigger_t15_2pc_melee( warrior_attack_t* a )
{
  if ( !a -> player -> sets.has_set_bonus( SET_T15_2PC_MELEE ) )
    return false;

  warrior_t* p = a -> p();

  bool procced;

  if ( ( procced = p -> t15_2pc_melee.trigger() ) != false )
  {
    p -> proc.t15_2pc_melee -> occur();
    if ( p -> specialization() != WARRIOR_ARMS )
      p -> enrage();
  }

  return procced;
}

// ==========================================================================
// Warrior Attacks
// ==========================================================================

// warrior_attack_t::execute ================================================

void warrior_attack_t::execute()
{
  base_t::execute();
}

// warrior_attack_t::impact =================================================

void warrior_attack_t::impact( action_state_t* s )
{
  base_t::impact( s );

  if ( s -> result_amount > 0 )
  {
    if ( ( result_is_hit(s -> result) || result_is_multistrike(s -> result) ) )
    {
      if ( p() -> talents.second_wind->ok() )
      {
        if ( p() -> resources.current[RESOURCE_HEALTH] < p() -> resources.max[RESOURCE_HEALTH] * 0.35 )
        {
          p()->active_second_wind->base_dd_min = s->result_amount;
          p()->active_second_wind->base_dd_max = s->result_amount;
          p()->active_second_wind->execute();
        }
      }
      if ( p() -> buff.rallying_cry -> up() && p() -> glyphs.rallying_cry -> ok() )
      {
        p() -> active_rallying_cry_heal -> base_dd_min = s -> result_amount;
        p() -> active_rallying_cry_heal -> base_dd_max = s -> result_amount;
        p() -> active_rallying_cry_heal -> execute();
      }

      if ( !proc ) // No procs allowed.
      {
        if ( p() -> buff.sweeping_strikes -> up() && !aoe )
          trigger_sweeping_strikes(s);
        if ( special )
        {
          if ( p() -> buff.bloodbath -> up() && this -> id != 156287 ) // Ravager does not trigger bloodbath.
            trigger_bloodbath_dot(s -> target, s -> result_amount);
          if ( p() -> sets.has_set_bonus(SET_T16_2PC_MELEE) && td(s -> target) ->  debuffs_colossus_smash -> up() && // Melee tier 16 2 piece.
              ( this ->  weapon == &( p() -> main_hand_weapon ) || this -> id == 100130 ) && // Only procs once per ability used.
              this -> id != 12328 ) // Doesn't proc from sweeping strikes.
              p() -> resource_gain(RESOURCE_RAGE,
              p() -> sets.set(SET_T16_2PC_MELEE) -> effectN(1).base_value(),
              p() -> gain.tier16_2pc_melee);
        }
      }
    }
  }
}

// Melee Attack =============================================================

struct melee_t: public warrior_attack_t
{
  int sync_weapons;
  bool first;

  melee_t( const std::string& name, warrior_t* p, int sw ):
    warrior_attack_t( name, p, spell_data_t::nil() ),
    sync_weapons( sw ), first( true )
  {
    school = SCHOOL_PHYSICAL;
    special = false;
    background = repeating = auto_attack = may_glance = true;
    trigger_gcd = timespan_t::zero();

    if ( p -> dual_wield() )
      base_hit -= 0.19;
  }

  void reset()
  {
    warrior_attack_t::reset();
    first = true;
  }

  timespan_t execute_time() const
  {
    timespan_t t = warrior_attack_t::execute_time();
    if ( first )
      return ( weapon -> slot == SLOT_OFF_HAND ) ? ( sync_weapons ? std::min( t / 2, timespan_t::zero() ) : t / 2 ) : timespan_t::zero();
    else
      return t;
  }

  void execute()
  {
    if ( first )
      first = false;

    // If we're casting, we should clip a swing
    if ( time_to_execute > timespan_t::zero() && player -> executing )
      schedule_execute();
    else
      warrior_attack_t::execute();
  }

  school_e get_school() const
  {
    if ( p() -> buff.ignite_weapon -> up() )
      return SCHOOL_FIRE;
    else
      return warrior_attack_t::get_school();
  }

  void impact( action_state_t* s )
  {
    warrior_attack_t::impact( s );

    if ( result_is_hit( s -> result ) || result_is_block( s -> block_result ) )
    {
      trigger_t15_2pc_melee( this );
      if ( p() -> talents.sudden_death -> ok() )
      {
        bool sudden_death = false;
        if ( p() -> buff.sudden_death -> check() )
          sudden_death = true;
        if ( p() -> buff.sudden_death -> trigger() )
        {
          p() -> proc.sudden_death -> occur();
          if ( sudden_death )
            p() -> proc.sudden_death_wasted -> occur();
        }
      }
      trigger_rage_gain( s );
      if ( p() -> perk.enhanced_rend -> ok() )
      {
        p() -> active_enhanced_rend -> target = s -> target;
        p() -> active_enhanced_rend -> execute();
      }
    }

    if ( p() -> specialization() == WARRIOR_PROTECTION )
    {
      if ( result_is_multistrike( s -> result ) )
      {
        p() -> buff.blood_craze -> trigger();
        residual_action::trigger(
          p() -> active_blood_craze, // ignite spell
          p(), // target
          p() -> spec.blood_craze -> effectN( 1 ).trigger() -> effectN( 1 ).percent() *
          p() -> resources.max[RESOURCE_HEALTH] * 3 );
      }
    }
  }
};

// Auto Attack ==============================================================

struct auto_attack_t: public warrior_attack_t
{
  int sync_weapons;

  auto_attack_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "auto_attack", p ),
    sync_weapons( 0 )
  {
    option_t options[] =
    {
      opt_bool( "sync_weapons", sync_weapons ),
      opt_null()
    };
    parse_options( options, options_str );

    assert( p -> main_hand_weapon.type != WEAPON_NONE );

    p -> main_hand_attack = new melee_t( "auto_attack_mh", p, sync_weapons );
    p -> main_hand_attack -> weapon = &( p -> main_hand_weapon );
    p -> main_hand_attack -> base_execute_time = p -> main_hand_weapon.swing_time;

    if ( p -> off_hand_weapon.type != WEAPON_NONE )
    {
      p -> off_hand_attack = new melee_t( "auto_attack_oh", p, sync_weapons );
      p -> off_hand_attack -> weapon = &( p -> off_hand_weapon );
      p -> off_hand_attack -> base_execute_time = p -> off_hand_weapon.swing_time;
      p -> off_hand_attack -> id = 1;
    }

    trigger_gcd = timespan_t::zero();
  }

  void execute()
  {
    p() -> main_hand_attack -> schedule_execute();

    if ( p() -> off_hand_attack )
      p() -> off_hand_attack -> schedule_execute();
  }

  bool ready()
  {
    if ( p() -> current.distance_to_move > 5 )
      return false;

    return( p() -> main_hand_attack -> execute_event == 0 ); // not swinging
  }
};

// Bladestorm ===============================================================

struct bladestorm_tick_t: public warrior_attack_t
{
  bladestorm_tick_t( warrior_t* p, const std::string& name ):
    warrior_attack_t( name, p, p -> talents.bladestorm -> effectN( 1 ).trigger() )
  {
    dual = true;
    aoe = -1;
    if ( p -> specialization() == WARRIOR_ARMS )
      weapon_multiplier *= 2;
  }

  double action_multiplier() const
  {
    double am = warrior_attack_t::action_multiplier();

    if ( p() -> specialization() == WARRIOR_PROTECTION && p() -> has_shield_equipped() )
      am *= 1.0 + 1 / 3;

    return am;
  }

  void execute()
  {
    warrior_attack_t::execute();
    if ( p() -> buff.sweeping_strikes -> up() )
      trigger_sweeping_strikes( execute_state );
  }
};

struct bladestorm_t: public warrior_attack_t
{
  attack_t* bladestorm_mh;
  attack_t* bladestorm_oh;

  bladestorm_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "bladestorm", p, p -> talents.bladestorm ),
    bladestorm_mh( new bladestorm_tick_t( p, "bladestorm_mh" ) ),
    bladestorm_oh( 0 )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;

    channeled = tick_zero = true;
    callbacks = false;

    bladestorm_mh -> weapon = &( player -> main_hand_weapon );
    add_child( bladestorm_mh );

    if ( player -> off_hand_weapon.type != WEAPON_NONE )
    {
      bladestorm_oh = new bladestorm_tick_t( p, "bladestorm_oh" );
      bladestorm_oh -> weapon = &( player -> off_hand_weapon );
      add_child( bladestorm_oh );
    }
  }

  void tick( dot_t* )
  {
    bladestorm_mh -> execute();

    if ( bladestorm_mh -> result_is_hit( execute_state -> result ) && bladestorm_oh )
      bladestorm_oh -> execute();
  }

  void update_ready( timespan_t cd_duration )
  {
    if ( p() -> specialization() != WARRIOR_PROTECTION )
      cd_duration = cooldown -> duration / ( 1 + player -> cache.readiness() );

    warrior_attack_t::update_ready( cd_duration );
  }
  // Bladestorm is not modified by haste effects
  double composite_haste() const { return 1.0; }
};

// Bloodthirst Heal =========================================================

struct bloodthirst_heal_t: public warrior_heal_t
{
  double base_pct_heal;
  bloodthirst_heal_t( warrior_t* p ):
    warrior_heal_t( "bloodthirst_heal", p, p -> find_spell( 117313 ) ),
    base_pct_heal( 0 )
  {
    pct_heal   = data().effectN( 1 ).percent();
    pct_heal  *= 1.0 + p -> glyphs.bloodthirst -> effectN( 2 ).percent();
    base_pct_heal = pct_heal;
    background = true;
  }

  resource_e current_resource() const { return RESOURCE_NONE; }

  double calculate_direct_amount( action_state_t* state )
  {
    pct_heal = base_pct_heal;

    if ( p() -> buff.raging_blow_glyph -> up() )
    {
      pct_heal *= 1.0 + p() -> buff.raging_blow_glyph -> data().effectN( 1 ).percent();
      p() -> buff.raging_blow_glyph -> expire();
    }

    return warrior_heal_t::calculate_direct_amount( state );
  }
};

// Bloodthirst ==============================================================

struct bloodthirst_t: public warrior_attack_t
{
  bloodthirst_heal_t* bloodthirst_heal;
  bloodthirst_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "bloodthirst", p, p -> spec.bloodthirst ),
    bloodthirst_heal( NULL )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_DEFENSE;

    if ( p -> talents.unquenchable_thirst -> ok() )
      cooldown -> duration = timespan_t::zero();

    weapon           = &( p -> main_hand_weapon );
    bloodthirst_heal = new bloodthirst_heal_t( p );
    weapon_multiplier += p -> sets.set( SET_T14_2PC_MELEE ) -> effectN( 2 ).percent();
  }

  double composite_crit() const
  {
    double c = warrior_attack_t::composite_crit();

    c += data().effectN( 4 ).percent();

    return c;
  }

  void impact( action_state_t* s )
  {
    warrior_attack_t::impact( s );

    if ( result_is_hit( s -> result ) )
    {
      bloodthirst_heal -> execute();
      int bloodsurge = 0;

      if ( p() -> buff.bloodsurge -> check() )
        bloodsurge = p() -> buff.bloodsurge -> current_stack;
      if ( p() -> buff.bloodsurge -> trigger( 3 ) )
      {
        p() -> proc.bloodsurge -> occur();
        if ( bloodsurge > 0 )
        {
          do
          {
            p() -> proc.bloodsurge_wasted -> occur();
            bloodsurge--;
          }
          while ( bloodsurge > 0 );
        }
      }
    }

    p() -> resource_gain( RESOURCE_RAGE,
                          data().effectN( 3 ).resource( RESOURCE_RAGE ),
                          p() -> gain.bloodthirst );

    if ( s -> result == RESULT_CRIT )
      p() -> enrage();
  }

  void update_ready( timespan_t cd_duration )
  {
    if ( p() -> talents.unquenchable_thirst -> ok() )
      return;

    warrior_attack_t::update_ready( cd_duration );
  }

  bool ready()
  {
    if ( !p() -> cooldown.stance_cooldown -> up() )
      return false;

    return warrior_attack_t::ready();
  }
};

// Charge ===================================================================

struct charge_t: public warrior_attack_t
{
  bool first_charge;
  charge_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "charge", p, p -> spell.charge ),
    first_charge( true )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
    base_teleport_distance = data().max_range();
    base_teleport_distance += p -> glyphs.long_charge -> effectN( 1 ).base_value();
    movement_directionality = MOVEMENT_OMNI;

    if ( p -> talents.double_time -> ok() )
      cooldown -> charges = 2;
    else if ( p -> talents.juggernaut -> ok() )
      cooldown -> duration += p -> talents.juggernaut -> effectN( 1 ).time_value();
  }

  void execute()
  {
    warrior_attack_t::execute();

    if ( first_charge == true )
      first_charge = !first_charge;

    p() -> buff.heroic_charge -> expire();

    if ( p() -> cooldown.rage_from_charge -> up() )
    {
      p() -> cooldown.rage_from_charge -> start();
      p() -> resource_gain( RESOURCE_RAGE,
                            p() -> glyphs.bull_rush -> effectN( 2 ).resource( RESOURCE_RAGE ) +
                            data().effectN( 2 ).resource( RESOURCE_RAGE ),
                            p() -> gain.charge );
    }
  }

  void reset()
  {
    action_t::reset();

    first_charge = true;
  }

  bool ready()
  {
    if ( first_charge == true || p() -> buff.heroic_charge -> up() ) // Assumes that we charge into combat, instead of setting initial distance to 20 yards.
      return warrior_attack_t::ready();

    if ( p() -> current.distance_to_move > base_teleport_distance ||
         p() -> current.distance_to_move < data().min_range() ) // Cannot charge unless target is in range.
         return false;

    return warrior_attack_t::ready();
  }
};

// Colossus Smash ===========================================================

struct colossus_smash_t: public warrior_attack_t
{
  colossus_smash_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "colossus_smash", p, p -> spec.colossus_smash )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE;

    weapon = &( player -> main_hand_weapon );
  }

  double action_multiplier() const
  {
    double am = warrior_attack_t::action_multiplier();

    if ( p() -> mastery.weapons_master -> ok() )
      am *= 1.0 + p() -> cache.mastery_value();

    return am;
  }

  void execute()
  {
    warrior_attack_t::execute();

    if ( p() -> specialization() == WARRIOR_ARMS )
    {
      if ( p() -> sets.has_set_bonus( SET_T17_2PC_MELEE ) )
        p() -> resource_gain( RESOURCE_RAGE,
        p() -> sets.set( SET_T17_2PC_MELEE ) -> effectN( 1 ).trigger() -> effectN( 1 ).resource( RESOURCE_RAGE ),
        p() -> gain.tier17_2pc_arms );
      if ( p() -> sets.has_set_bonus( SET_T17_4PC_MELEE ) )
        if ( p() -> buff.tier17_4pc_arms -> trigger() )
          p() -> proc.t17_4pc_arms -> occur();
    }
  }

  void impact( action_state_t* s )
  {
    warrior_attack_t::impact( s );

    if ( result_is_hit( s -> result ) )
    {
      if ( p() -> glyphs.colossus_smash -> ok() )
        td( s -> target ) -> debuffs_colossus_smash -> trigger( 1, p() -> glyphs.colossus_smash -> effectN( 3 ).percent() );
      else
        td( s -> target ) -> debuffs_colossus_smash -> trigger( 1, data().effectN( 2 ).percent() );

      if ( s -> result == RESULT_CRIT && p() -> specialization() != WARRIOR_ARMS )
        p() -> enrage();

      p() -> resource_gain( RESOURCE_RAGE,
                            p() -> perk.improved_colossus_smash -> effectN( 1 ).base_value(),
                            p() -> gain.colossus_smash );
    }
  }
};

// Demoralizing Shout =======================================================

struct demoralizing_shout: public warrior_attack_t
{
  demoralizing_shout( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "demoralizing_shout", p, p -> find_specialization_spell( "Demoralizing Shout" ) )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
  }

  void impact( action_state_t* s )
  {
    warrior_attack_t::impact( s );
    if ( result_is_hit( s -> result ) )
      td( s -> target ) -> debuffs_demoralizing_shout -> trigger( 1, data().effectN( 1 ).percent() );
  }

  void update_ready( timespan_t cd_duration )
  {
    cd_duration = cooldown -> duration / ( 1 + player -> cache.readiness() );
    warrior_attack_t::update_ready( cd_duration );
  }
};

// Devastate ================================================================

struct devastate_off_hand_t: public warrior_attack_t
{
  devastate_off_hand_t(warrior_t* p, const char* name, const spell_data_t* s):
    warrior_attack_t(name, p, s)
  {
    dual = true;
    may_miss = may_dodge = may_parry = false;
    weapon = &( p -> off_hand_weapon );
  }
};

struct devastate_t: public warrior_attack_t
{
  devastate_off_hand_t* oh_attack;
  devastate_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "devastate", p, p -> specialization() == WARRIOR_PROTECTION ? 
                                      p -> find_spell( 20243 ) : p -> spec.devastate ),
                                      oh_attack( 0 )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_GLADIATOR | STANCE_DEFENSE;
    weapon = &( p -> main_hand_weapon );
    if ( p -> specialization() == WARRIOR_FURY )
    {
      oh_attack = new devastate_off_hand_t(p, "devastate_offhand", p -> find_spell( 174894 ) );
      add_child(oh_attack);
    }
  }

  void execute()
  {
    warrior_attack_t::execute();

    if ( result_is_hit( execute_state -> result ) )
    {
      if ( p() -> specialization() == WARRIOR_PROTECTION )
      {
        if ( p() -> buff.sword_and_board -> trigger() )
          p() -> cooldown.shield_slam -> reset( true );
        p() -> active_deep_wounds -> target = execute_state -> target;
        p() -> active_deep_wounds -> execute();
        if ( p() -> talents.unyielding_strikes -> ok() )
        {
          if ( p() -> buff.unyielding_strikes -> current_stack != 5 )
            p() -> buff.unyielding_strikes -> trigger(1);
        }
      }
      else
      {
        if ( oh_attack )
          oh_attack -> execute();
        if ( rng().roll( 0.3 ) ) // No spell data for the reset yet.
          p() -> cooldown.revenge -> reset( true );
      }
    }
  }

  void impact( action_state_t* s )
  {
    warrior_attack_t::impact( s );

    if ( p() -> specialization() != WARRIOR_ARMS )
    {
      if ( s -> result == RESULT_CRIT )
        p() -> enrage();
    }
  }
};

// Dragon Roar ==============================================================

struct dragon_roar_t: public warrior_attack_t
{
  dragon_roar_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "dragon_roar", p, p -> talents.dragon_roar )
  {
    parse_options( NULL, options_str );
    aoe = -1;
    may_dodge = may_parry = may_block = false;
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
  }

  double target_armor( player_t* ) const { return 0; }

  double composite_crit() const { return 1.0; }

  double composite_target_crit( player_t* ) const { return 0.0; }

  void update_ready( timespan_t cd_duration )
  {
    if ( p() -> specialization() != WARRIOR_PROTECTION )
      cd_duration = cooldown -> duration / ( 1 + player -> cache.readiness() );

    warrior_attack_t::update_ready( cd_duration );
  }
};

// Execute ==================================================================

struct execute_off_hand_t: public warrior_attack_t
{
  execute_off_hand_t( warrior_t* p, const char* name, const spell_data_t* s ):
    warrior_attack_t( name, p, s )
  {
    dual = true;
    may_miss = may_dodge = may_parry = may_block = false;

    weapon = &( p -> off_hand_weapon );
    if ( p -> main_hand_weapon.group() == WEAPON_1H &&
         p -> off_hand_weapon.group() == WEAPON_1H )
         weapon_multiplier *= 1.0 + p -> spec.singleminded_fury -> effectN( 3 ).percent();
  }
};

struct execute_t: public warrior_attack_t
{
  execute_off_hand_t* oh_attack;
  execute_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "execute", p, p -> find_specialization_spell( "Execute" ) )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
    weapon = &( p -> main_hand_weapon );

    if ( p -> main_hand_weapon.group() == WEAPON_1H &&
         p -> off_hand_weapon.group() == WEAPON_1H )
         weapon_multiplier *= 1.0 + p -> spec.singleminded_fury -> effectN( 3 ).percent();

    if ( p -> spec.crazed_berserker -> ok() )
    {
      oh_attack = new execute_off_hand_t( p, "execute_oh", p -> find_spell( 163558 ) );
      add_child( oh_attack );
    }
  }

  double action_multiplier() const
  {
    double am = warrior_attack_t::action_multiplier();

    if ( p() -> mastery.weapons_master -> ok() )
    {
      if ( !p() -> buff.sudden_death -> check() )
        am *= 6.0 * std::min( 40.0, p() -> resources.current[RESOURCE_RAGE] ) / 40;

      am *= 1.0 + p() -> cache.mastery_value();
    }
    return am;
  }

  double cost() const
  {
    double c = warrior_attack_t::cost();

    if ( p() -> mastery.weapons_master -> ok() )
      c = std::min( 40.0, std::max( p() -> resources.current[RESOURCE_RAGE], c ) );

    if ( p() -> buff.sudden_death -> up() )
      c = 0;

    return c;
  }

  void execute()
  {
    warrior_attack_t::execute();

    if ( p() -> spec.crazed_berserker -> ok() && result_is_hit( execute_state -> result ) &&
         p() -> off_hand_weapon.type != WEAPON_NONE ) // If MH fails to land, or if there is no OH weapon for Fury, oh attack does not execute.
         oh_attack -> execute();

    p() -> buff.sudden_death -> expire();
  }

  bool ready()
  {
    if ( p() -> main_hand_weapon.type == WEAPON_NONE )
      return false;

    if ( p() -> buff.sudden_death -> check() )
      return warrior_attack_t::ready();

    if ( target -> health_percentage() > 20 )
      return false;

    return warrior_attack_t::ready();
  }
};

// Hamstring ==============================================================

struct hamstring_t: public warrior_attack_t
{
  bool rage_spent;
  hamstring_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "hamstring", p, p -> find_class_spell( "Hamstring" ) ),
    rage_spent( false )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
    weapon = &( p -> main_hand_weapon );
  }

  double cost() const
  {
    double c = warrior_attack_t::cost();

    if ( p() -> buff.hamstring -> up() )
      c = 0;

    return c;
  }

  void execute()
  {
    if ( cost() > 0 )
      rage_spent = true;
    else
      rage_spent = false;
    warrior_attack_t::execute();
    p() -> buff.hamstring -> expire();
    if ( rage_spent )
      p() -> buff.hamstring -> trigger();
  }
};

// Ignite Weapon ============================================================

struct ignite_weapon_t: public warrior_attack_t
{
  ignite_weapon_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "ignite_weapon", p, p -> talents.ignite_weapon )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
    weapon  = &( player -> main_hand_weapon );

    dot_duration = timespan_t::zero(); // Effect 4 shows up as periodic damage on target, but the actual "dot" shows up on autoattacks.
    use_off_gcd = true;
  }

  void execute()
  {
    warrior_attack_t::execute();

    p() -> buff.ignite_weapon -> trigger();
  }

  double action_multiplier() const
  {
    double am = warrior_attack_t::action_multiplier();

    // The 140% is hardcoded in the tooltip
    if ( weapon -> group() == WEAPON_1H ||
         weapon -> group() == WEAPON_SMALL )
         am *= 1.40;

    if ( p() -> mastery.weapons_master -> ok() )
      am *= 1.0 + p() -> cache.mastery_value();

    return am;
  }

  bool ready()
  {
    if ( p() -> main_hand_weapon.type == WEAPON_NONE )
      return false;

    return warrior_attack_t::ready();
  }
};

// Heroic Strike ============================================================

struct heroic_strike_t: public warrior_attack_t
{
  heroic_strike_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "heroic_strike", p, p -> find_class_spell( "Heroic Strike" ) )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;

    weapon  = &( player -> main_hand_weapon );

    if ( p -> glyphs.cleave -> ok() )
      aoe = 2;

    use_off_gcd = true;
  }

  double action_multiplier() const
  {
    double am = warrior_attack_t::action_multiplier();

    // The 140% is hardcoded in the tooltip
    if ( weapon -> group() == WEAPON_1H ||
         weapon -> group() == WEAPON_SMALL )
         am *= 1.40;

    if ( p() -> buff.shield_charge -> up() )
      am *= 1.0 + p() -> buff.shield_charge -> default_value;

    return am;
  }

  double cost() const
  {
    double c = warrior_attack_t::cost();

    if ( p() -> buff.unyielding_strikes -> up() )
      c -= p() -> buff.unyielding_strikes -> current_stack * 6;

    if ( p() -> buff.ultimatum -> check() )
      c *= 1 + p() -> buff.ultimatum -> data().effectN( 1 ).percent();

    return c;
  }

  double composite_crit() const
  {
    double cc = warrior_attack_t::composite_crit();

    if ( p() -> buff.ultimatum -> check() )
      cc += p() -> buff.ultimatum -> data().effectN( 2 ).percent();

    return cc;
  }

  void execute()
  {
    warrior_attack_t::execute();

    p() -> buff.ultimatum -> expire();
  }

  bool ready()
  {
    if ( p() -> main_hand_weapon.type == WEAPON_NONE )
      return false;

    return warrior_attack_t::ready();
  }
};

// Heroic Throw ==============================================================

struct heroic_throw_t: public warrior_attack_t
{
  heroic_throw_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "heroic_throw", p, p -> find_class_spell( "Heroic Throw" ) )
  {
    parse_options( NULL, options_str );

    weapon  = &( player -> main_hand_weapon );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
    may_dodge = may_parry = may_block = false;

    if ( p -> perk.improved_heroic_throw -> ok() )
      cooldown -> duration = timespan_t::zero();
  }

  bool ready()
  {
    if ( p() -> current.distance_to_move > data().max_range() ||
         p() -> current.distance_to_move < data().min_range() ) // Cannot heroic throw unless target is in range.
         return false;

    if ( p() -> main_hand_weapon.type == WEAPON_NONE )
      return false;

    return warrior_attack_t::ready();
  }
};

// Heroic Leap ==============================================================

struct heroic_leap_t: public warrior_attack_t
{
  heroic_leap_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "heroic_leap", p, p -> spell.heroic_leap )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
    aoe = -1;
    may_dodge = may_parry = may_miss = may_block = false;
    movement_directionality = MOVEMENT_OMNI;
    base_teleport_distance = data().max_range();
    attack_power_mod.direct = data().effectN( 2 ).trigger() -> effectN( 1 ).ap_coeff();

    cooldown -> duration = p -> cooldown.heroic_leap -> duration;
    cooldown -> duration += p -> glyphs.death_from_above -> effectN( 1 ).time_value();
  }

  void impact( action_state_t* s )
  {
    if ( p() -> current.distance_to_move >  // Hack so that heroic leap doesn't deal damage if the target is far away.
         ( data().effectN( 2 ).trigger() -> effectN( 1 ).radius() +
         p() -> perk.improved_heroic_leap -> effectN( 1 ).base_value() ) )
         s -> result_amount = 0;

    p() -> buff.heroic_leap_glyph -> trigger();

    warrior_attack_t::impact( s );
  }

  void update_ready( timespan_t cd_duration )
  {
    cd_duration = cooldown -> duration / ( 1 + player -> cache.readiness() );

    warrior_attack_t::update_ready( cd_duration );
  }
};

// 2 Piece Tier 16 Tank Set Bonus ===========================================

struct tier16_2pc_tank_heal_t: public warrior_heal_t
{
  tier16_2pc_tank_heal_t( warrior_t* p ):
    warrior_heal_t( "tier16_2pc_tank_heal", p )
  {}
  resource_e current_resource() const { return RESOURCE_NONE; }
};

// Impending Victory ========================================================

struct impending_victory_heal_t: public warrior_heal_t
{
  double base_pct_heal;
  impending_victory_heal_t( warrior_t* p ):
    warrior_heal_t( "impending_victory_heal", p, p -> find_spell( 118340 ) )
  {
    pct_heal = data().effectN( 1 ).percent();
    base_pct_heal = pct_heal;
  }

  double calculate_direct_amount( action_state_t* state )
  {
    pct_heal = base_pct_heal;

    if ( p() -> buff.tier15_2pc_tank -> check() )
      pct_heal += p() -> buff.tier15_2pc_tank -> value();

    return warrior_heal_t::calculate_direct_amount( state );
  }

  resource_e current_resource() const { return RESOURCE_NONE; }
};

struct impending_victory_t: public warrior_attack_t
{
  impending_victory_heal_t* impending_victory_heal;

  impending_victory_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "impending_victory", p, p -> talents.impending_victory ),
    impending_victory_heal( new impending_victory_heal_t( p ) )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
    parse_effect_data( data().effectN( 2 ) ); //Both spell effects list an ap coefficient, #2 is correct.
  }

  void execute()
  {
    warrior_attack_t::execute();

    if ( result_is_hit( execute_state -> result ) )
      impending_victory_heal -> execute();

    p() -> buff.tier15_2pc_tank -> decrement();
  }
};

// Intervene ===============================================================
// Note: Conveniently ignores that you can only intervene a friendly target.
// For the time being, we're just going to assume that there is a friendly near the target
// that we can intervene to. Maybe in the future with a more complete movement system, we will
// fix this to work in a raid simulation that includes multiple melee.

struct intervene_t: public warrior_attack_t
{
  intervene_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "intervene", p, p -> spell.intervene )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
    base_teleport_distance = data().max_range();
    movement_directionality = MOVEMENT_OMNI;
  }

  bool ready()
  {
    if ( p() -> current.distance_to_move > base_teleport_distance )
      return false;

    return warrior_attack_t::ready();
  }
};

// Heroic Charge  ==============================================================
// Currently assumes that the player has a swing timer, and never clips autoattacks.
// In practice this is extremely easy to do.

struct heroic_charge_t: public warrior_attack_t
{
  action_t*leap;
  action_t*charge;
  heroic_charge_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "heroic_charge", p, spell_data_t::nil() )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_DEFENSE | STANCE_GLADIATOR | STANCE_BATTLE;
    leap = new heroic_leap_t( p, options_str );
    charge = new charge_t( p, options_str );
    add_child( leap );
    add_child( charge );
    dual = true;
    school = SCHOOL_PHYSICAL; // Used to give the pie chart a different color.
    min_gcd = timespan_t::from_millis( 750 );
  }

  timespan_t gcd() const
  {
    if ( p() -> cooldown.heroic_leap -> up() )
      return timespan_t::from_millis( 750 );
    else
      return timespan_t::from_millis( 1250 );
  }

  void execute()
  {
    if ( p() -> cooldown.heroic_leap -> up() )
      leap -> execute();

    warrior_attack_t::execute();
    p() -> buff.heroic_charge -> trigger();
    charge -> execute();
  }

  bool ready()
  {
    if ( p() -> cooldown.rage_from_charge -> up() && p() -> cooldown.charge -> up() )
      return warrior_attack_t::ready();
    else
      return false;
  }
};

// Mortal Strike ============================================================

struct mortal_strike_t: public warrior_attack_t
{
  mortal_strike_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "mortal_strike", p, p -> spec.mortal_strike )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_DEFENSE;

    weapon = &( p -> main_hand_weapon );
    weapon_multiplier += p -> sets.set( SET_T14_2PC_MELEE ) -> effectN( 1 ).percent();
  }

  double action_multiplier() const
  {
    double am = warrior_attack_t::action_multiplier();

    if ( p() -> mastery.weapons_master -> ok() )
      am *= 1.0 + p() -> cache.mastery_value();

    return am;
  }

  void impact( action_state_t* s )
  {
    warrior_attack_t::impact( s );

    if ( result_is_hit( s -> result ) )
    {
      if ( sim -> overrides.mortal_wounds )
        s -> target -> debuffs.mortal_wounds -> trigger();
    }
  }

  bool ready()
  {
    if ( p() -> main_hand_weapon.type == WEAPON_NONE )
      return false;

    if ( !p() -> cooldown.stance_cooldown -> up() )
      return false;

    return warrior_attack_t::ready();
  }
};

// Pummel ===================================================================

struct pummel_t: public warrior_attack_t
{
  pummel_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "pummel", p, p -> find_class_spell( "Pummel" ) )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;

    may_miss = may_block = may_dodge = may_parry = false;
  }

  void execute()
  {
    warrior_attack_t::execute();
    p() -> buff.rude_interruption -> trigger();
  }
};

// Raging Blow ==============================================================

struct raging_blow_attack_t: public warrior_attack_t
{
  raging_blow_attack_t( warrior_t* p, const char* name, const spell_data_t* s ):
    warrior_attack_t( name, p, s )
  {
    may_miss = may_dodge = may_parry = may_block = false;
    dual = true;
  }

  void execute()
  {
    aoe = p() -> buff.meat_cleaver -> stack();
    if ( aoe ) ++aoe;

    warrior_attack_t::execute();
  }
};

struct raging_blow_t: public warrior_attack_t
{
  raging_blow_attack_t* mh_attack;
  raging_blow_attack_t* oh_attack;

  raging_blow_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "raging_blow", p, p -> spec.raging_blow ),
    mh_attack( NULL ), oh_attack( NULL )
  {
    // Parent attack is only to determine miss/dodge/parry
    weapon_multiplier = attack_power_mod.direct = 0;
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_DEFENSE;

    mh_attack = new raging_blow_attack_t( p, "raging_blow_mh", data().effectN( 1 ).trigger() );
    mh_attack -> weapon = &( p -> main_hand_weapon );
    add_child( mh_attack );

    oh_attack = new raging_blow_attack_t( p, "raging_blow_oh", data().effectN( 2 ).trigger() );
    oh_attack -> weapon = &( p -> off_hand_weapon );
    add_child( oh_attack );
  }

  void impact( action_state_t* s )
  {
    warrior_attack_t::impact( s );
    if ( result_is_hit( s -> result ) && td( s -> target ) -> debuffs_colossus_smash -> up() )
      td( s -> target ) -> debuffs_colossus_smash -> extend_duration( s -> target, timespan_t::from_seconds( 2 ) );
  }

  void execute()
  {
    // check attack
    attack_t::execute();
    if ( result_is_hit( execute_state -> result ) )
    {
      mh_attack -> execute();
      oh_attack -> execute();
      if ( mh_attack -> execute_state -> result == RESULT_CRIT &&
           oh_attack -> execute_state -> result == RESULT_CRIT )
           p() -> buff.raging_blow_glyph -> trigger();
      if ( mh_attack -> execute_state -> result == RESULT_CRIT ||
           oh_attack -> execute_state -> result == RESULT_CRIT )
      {
        if ( rng().roll( p() -> sets.set( SET_T17_2PC_MELEE ) -> _proc_chance ) )
        {
          p() -> enrage();
          p() -> proc.t17_2pc_fury -> occur();
        }
      }
      p() -> buff.raging_wind -> trigger();
      p() -> buff.meat_cleaver -> expire();
    }
    p() -> buff.raging_blow -> decrement(); // Raging blow buff decrements even if the attack doesn't land.
  }

  bool ready()
  {
    if ( !p() -> buff.raging_blow -> check() )
      return false;

    if ( !p() -> cooldown.stance_cooldown -> up() )
      return false;

    // Needs weapons in both hands
    if ( p() -> main_hand_weapon.type == WEAPON_NONE ||
         p() -> off_hand_weapon.type == WEAPON_NONE )
         return false;

    return warrior_attack_t::ready();
  }
};

// Ravager ==============================================================

struct ravager_tick_t: public warrior_attack_t
{
  ravager_tick_t( warrior_t* p, const std::string& name ):
    warrior_attack_t( name, p, p -> find_spell( 156287 ) )
  {
    aoe = -1;
    dual = true;
  }
};

struct ravager_t: public warrior_attack_t
{
  attack_t* ravager;
  ravager_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "ravager", p, p -> talents.ravager ),
    ravager( new ravager_tick_t( p, "ravager_tick" ) )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
    hasted_ticks = callbacks = false;
    add_child( ravager );
  }

  void execute()
  {
    p() -> buff.ravager -> trigger();

    warrior_attack_t::execute();
  }

  void tick( dot_t* )
  {
    ravager -> execute();
  }

  double composite_haste() const { return 1.0; }
};

// Revenge ==================================================================

struct revenge_t: public warrior_attack_t
{
  stats_t* absorb_stats;
  double rage_gain;
  revenge_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "revenge", p, p -> spec.revenge ),
    absorb_stats( 0 ), rage_gain( 0 )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_GLADIATOR | STANCE_DEFENSE;

    aoe = 3;
    rage_gain = data().effectN( 2 ).resource( RESOURCE_RAGE );
  }

  double action_multiplier() const
  {
    double am = warrior_attack_t::action_multiplier();

    if ( p() -> buff.shield_charge -> up() )
      am *= 1.0 + p() -> buff.shield_charge -> default_value;

    return am;
  }

  void execute()
  {
    warrior_attack_t::execute();

    if ( result_is_hit( execute_state -> result ) )
    {
      p() -> resource_gain( RESOURCE_RAGE, rage_gain, p() -> gain.revenge );

      if ( p() -> sets.has_set_bonus(SET_T15_4PC_TANK) )
      {
        if ( td(target) -> debuffs_demoralizing_shout -> up() )
          p() -> resource_gain(RESOURCE_RAGE,
          rage_gain * p() -> sets.set(SET_T15_4PC_TANK) -> effectN(1).percent(),
          p() -> gain.tier15_4pc_tank);
      }
    }
  }

  void impact( action_state_t* s )
  {
    warrior_attack_t::impact( s );

    if ( result_is_hit( s -> result ) )
    {
      if ( rng().roll( p() -> sets.set( SET_T15_2PC_TANK ) -> proc_chance() ) )
        p() -> buff.tier15_2pc_tank -> trigger();
    }
  }

  bool ready()
  {
    if ( p() -> main_hand_weapon.type == WEAPON_NONE )
      return false;

    return warrior_attack_t::ready();
  }
};

// Blood Craze ==============================================================

struct blood_craze_t: public residual_action::residual_periodic_action_t < warrior_heal_t >
{
  blood_craze_t( warrior_t* p ):
    base_t( "blood_craze", p, p -> spec.blood_craze )
  {
    hasted_ticks = harmful = tick_may_crit = false;
    base_tick_time = p -> spec.blood_craze -> effectN( 1 ).trigger() -> effectN( 1 ).period();
    dot_duration = p -> spec.blood_craze -> effectN( 1 ).trigger() -> duration();
    dot_behavior = DOT_REFRESH;
  }
};

// Second Wind ==============================================================

struct second_wind_t: public warrior_heal_t
{
  second_wind_t( warrior_t* p ):
    warrior_heal_t( "second_wind", p, p -> talents.second_wind )
  {
    callbacks = false;
  }

  void execute()
  {
    base_dd_max *= 0.1; //Heals for 10% of original damage
    base_dd_min *= 0.1;

    warrior_heal_t::execute();
  }
};

// Enraged Regeneration ===============================================

struct enraged_regeneration_t: public warrior_heal_t
{
  enraged_regeneration_t( warrior_t* p, const std::string& options_str ):
    warrior_heal_t( "enraged_regeneration", p, p -> talents.enraged_regeneration )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
    dot_duration = data().duration();
    base_tick_time = data().effectN( 2 ).period();

    pct_heal = data().effectN( 1 ).percent();
    tick_pct_heal = data().effectN( 2 ).percent();
  }

  void execute()
  {
    p() -> buff.enraged_regeneration -> trigger();

    warrior_heal_t::execute();
  }
};

// Shield Slam ==============================================================

struct shield_slam_t: public warrior_attack_t
{
  double rage_gain;
  shield_slam_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "shield_slam", p, p -> find_specialization_spell( "Shield Slam" ) ),
    rage_gain( 0.0 )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_GLADIATOR | STANCE_DEFENSE;
    cooldown = p -> cooldown.shield_slam;

    rage_gain = data().effectN( 3 ).resource( RESOURCE_RAGE );
    attack_power_mod.direct = 3.696; //Hard-coded in tooltip.
  }

  double action_multiplier() const
  {
    double am = warrior_attack_t::action_multiplier();

    if ( p() -> buff.shield_charge -> up() )
    {
      am *= 1.0 + p() -> buff.shield_charge -> default_value;
      if ( p() -> talents.heavy_repercussions -> ok() )
        am *= 1.0 + p() -> talents.heavy_repercussions -> effectN( 1 ).percent();
    }
    else if ( p() -> buff.shield_block -> up() && p() -> talents.heavy_repercussions -> ok() )
      am *= 1.0 + p() -> talents.heavy_repercussions -> effectN( 1 ).percent();

    return am;
  }

  void execute()
  {
    warrior_attack_t::execute();

    double rage_from_snb = 0;

    if ( result_is_hit( execute_state -> result ) )
    {
      if ( p() -> active_stance != STANCE_BATTLE )
      {
        p() -> resource_gain( RESOURCE_RAGE,
                              rage_gain,
                              p() -> gain.shield_slam );

        if ( p() -> buff.sword_and_board -> up() )
        {
          rage_from_snb = p() -> buff.sword_and_board -> data().effectN( 1 ).resource( RESOURCE_RAGE );
          p() -> resource_gain( RESOURCE_RAGE,
                                rage_from_snb,
                                p() -> gain.sword_and_board );
        }
        p() -> buff.sword_and_board -> expire();
      }
    }

    if ( td( target ) -> debuffs_demoralizing_shout -> up() && p() -> sets.has_set_bonus( SET_T15_4PC_TANK ) )
      p() -> resource_gain( RESOURCE_RAGE,
      ( rage_gain + rage_from_snb ) * p() -> sets.set( SET_T15_4PC_TANK ) -> effectN( 1 ).percent(),
      p() -> gain.tier15_4pc_tank );
  }

  void impact( action_state_t* s )
  {
    warrior_attack_t::impact( s );

    if ( rng().roll( p() -> sets.set( SET_T15_2PC_TANK ) -> proc_chance() ) )
      p() -> buff.tier15_2pc_tank -> trigger();

    if ( s -> result == RESULT_CRIT )
    {
      p() -> enrage();
      p() -> buff.ultimatum -> trigger();
    }
  }

  bool ready()
  {
    if ( !p() -> has_shield_equipped() )
      return false;

    return warrior_attack_t::ready();
  }
};

// Slam =====================================================================

struct slam_t: public warrior_attack_t
{
  slam_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "slam", p, p -> talents.slam )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_DEFENSE;
    weapon = &( p -> main_hand_weapon );
    base_costs[RESOURCE_RAGE] = 10;
  }

  double cost() const
  {
    double c = warrior_attack_t::cost();

    c *= 1.0 + p() -> buff.slam -> current_stack;

    return c;
  }

  void impact( action_state_t* s )
  {
    warrior_attack_t::impact( s );

    if ( result_is_hit( s -> result ) )
      p() -> buff.slam -> trigger( 1 );
  }

  double action_multiplier() const
  {
    double am = warrior_attack_t::action_multiplier();

    am *= 1.0 + p() -> cache.mastery_value();

    if ( p() -> buff.slam -> up() )
      am *= 1.0 + ( ( (double)p() -> buff.slam -> current_stack ) / 2 );

    return am;
  }

  bool ready()
  {
    if ( p() -> main_hand_weapon.type == WEAPON_NONE )
      return false;

    if ( !p() -> cooldown.stance_cooldown -> up() )
      return false;

    return warrior_attack_t::ready();
  }
};

// Shockwave ================================================================

struct shockwave_t: public warrior_attack_t
{
  shockwave_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "shockwave", p, p -> talents.shockwave )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
    may_dodge = may_parry = may_block = false;
    aoe = -1;
  }

  void update_ready( timespan_t cd_duration )
  {
    if ( p() -> specialization() != WARRIOR_PROTECTION )
      cd_duration = cooldown -> duration / ( 1 + player -> cache.readiness() );

    if ( result_is_hit( execute_state -> result ) )
      if ( execute_state -> n_targets >= 3 )
      {
      if ( cd_duration > timespan_t::from_seconds( 20 ) )
        cd_duration += timespan_t::from_seconds( -20 );
      else
        cd_duration = timespan_t::zero();
      }

    warrior_attack_t::update_ready( cd_duration );
  }
};

// Storm Bolt ===============================================================

struct storm_bolt_off_hand_t: public warrior_attack_t
{
  storm_bolt_off_hand_t( warrior_t* p, const char* name, const spell_data_t* s ):
    warrior_attack_t( name, p, s )
  {
    may_dodge = may_parry = may_block = may_miss = false;
    dual = true;

    weapon = &( p -> off_hand_weapon );
    // assume the target is stun-immune
    weapon_multiplier *= 4.00;
  }
};

struct storm_bolt_t: public warrior_attack_t
{
  storm_bolt_off_hand_t* oh_attack;
  storm_bolt_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "storm_bolt", p, p -> talents.storm_bolt ),
    oh_attack( 0 )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
    may_dodge = may_parry = may_block = false;
    // Assuming that our target is stun immune
    weapon_multiplier *= 4.00;

    if ( p -> specialization() == WARRIOR_FURY )
    {
      oh_attack = new storm_bolt_off_hand_t( p, "storm_bolt_offhand", data().effectN( 4 ).trigger() );
      add_child( oh_attack );
    }
  }

  void execute()
  {
    warrior_attack_t::execute(); // for fury, this is the MH attack

    if ( oh_attack && result_is_hit( execute_state -> result ) &&
         p() -> off_hand_weapon.type != WEAPON_NONE ) // If MH fails to land, OH does not execute.
         oh_attack -> execute();
  }

  void update_ready( timespan_t cd_duration )
  {
    if ( p() -> specialization() != WARRIOR_PROTECTION )
      cd_duration = cooldown -> duration / ( 1 + player -> cache.readiness() );

    warrior_attack_t::update_ready( cd_duration );
  }

  bool ready()
  {
    if ( p() -> main_hand_weapon.type == WEAPON_NONE )
      return false;

    return warrior_attack_t::ready();
  }
};

// Thunder Clap =============================================================

struct thunder_clap_t: public warrior_attack_t
{
  thunder_clap_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "thunder_clap", p, p -> spec.thunder_clap )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
    aoe = -1;
    may_dodge = may_parry = may_block = false;
    cooldown -> duration = data().cooldown();
    cooldown -> duration *= 1 + p -> glyphs.resonating_power -> effectN( 2 ).percent();
    attack_power_mod.direct *= 1.0 + p -> glyphs.resonating_power -> effectN( 1 ).percent();
  }

  void impact( action_state_t* s )
  {
    warrior_attack_t::impact( s );

    if ( result_is_hit( s -> result ) && p() -> specialization() == WARRIOR_PROTECTION )
    {
      p() -> active_deep_wounds -> target = s -> target;
      p() -> active_deep_wounds -> execute();
    }
  }

  double cost() const
  {
    double c = base_t::cost();

    if ( p() -> specialization() == WARRIOR_PROTECTION && p() -> active_stance == STANCE_DEFENSE )
      c = 0;

    return c;
  }
};

// Victory Rush =============================================================

struct victory_rush_heal_t: public warrior_heal_t
{
  victory_rush_heal_t( warrior_t* p ):
    warrior_heal_t( "victory_rush_heal", p, p -> find_spell( 118779 ) )
  {
    pct_heal = data().effectN( 1 ).percent() * ( 1 + p -> glyphs.victory_rush -> effectN( 1 ).percent() );
  }
  resource_e current_resource() const { return RESOURCE_NONE; }
};

struct victory_rush_t: public warrior_attack_t
{
  victory_rush_heal_t* victory_rush_heal;

  victory_rush_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "victory_rush", p, p -> find_class_spell( "Victory Rush" ) ),
    victory_rush_heal( new victory_rush_heal_t( p ) )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;

    cooldown -> duration = timespan_t::from_seconds( 1000.0 );
  }

  void execute()
  {
    warrior_attack_t::execute();

    if ( result_is_hit( execute_state -> result ) )
      victory_rush_heal -> execute();

    p() -> buff.tier15_2pc_tank -> decrement();
  }

  bool ready()
  {
    if ( p() -> buff.tier15_2pc_tank -> check() )
      return true;

    return warrior_attack_t::ready();
  }
};

// Whirlwind ================================================================

struct whirlwind_off_hand_t: public warrior_attack_t
{
  whirlwind_off_hand_t( warrior_t* p ):
    warrior_attack_t( "whirlwind_oh", p, p -> find_spell( 44949 ) )
  {
    dual = true;
    aoe = -1;
    weapon = &( p -> off_hand_weapon );
  }

  double action_multiplier() const
  {
    double am = warrior_attack_t::action_multiplier();

    if ( p() -> buff.raging_wind ->  up() )
      am *= 1.0 + p() -> buff.raging_wind -> data().effectN( 1 ).percent();

    return am;
  }
};

struct whirlwind_t: public warrior_attack_t
{
  whirlwind_off_hand_t* oh_attack;
  whirlwind_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "whirlwind_mh", p, p -> spec.whirlwind ),
    oh_attack( 0 )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_DEFENSE;
    aoe = -1;

    if ( p -> specialization() == WARRIOR_FURY )
    {
      oh_attack = new whirlwind_off_hand_t( p );
      add_child( oh_attack );
    }
    else
      weapon_multiplier *= 2;

    weapon = &( p -> main_hand_weapon );
  }

  double action_multiplier() const
  {
    double am = warrior_attack_t::action_multiplier();

    if ( p() -> buff.raging_wind ->  up() )
      am *= 1.0 + p() -> buff.raging_wind -> data().effectN( 1 ).percent();

    return am;
  }

  void execute()
  {
    warrior_attack_t::execute();

    if ( p() -> buff.sweeping_strikes -> up() )
      trigger_sweeping_strikes( execute_state );

    if ( oh_attack )
      oh_attack -> execute();

    p() -> buff.meat_cleaver -> trigger();
    if ( p() -> perk.enhanced_whirlwind -> ok() )
      p() -> buff.meat_cleaver -> trigger();
    p() -> buff.raging_wind -> expire();
  }

  bool ready()
  {
    if ( p() -> main_hand_weapon.type == WEAPON_NONE )
      return false;

    if ( !p() -> cooldown.stance_cooldown -> up() )
      return false;

    return warrior_attack_t::ready();
  }
};

// Wild Strike ==============================================================

struct wild_strike_t: public warrior_attack_t
{
  wild_strike_t( warrior_t* p, const std::string& options_str ):
    warrior_attack_t( "wild_strike", p, p -> spec.wild_strike )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_DEFENSE;

    if ( p -> talents.furious_strikes -> ok() )
      base_costs[RESOURCE_RAGE] = 20;
    weapon  = &( player -> off_hand_weapon );
    min_gcd = data().gcd();
  }

  double cost() const
  {
    double c = warrior_attack_t::cost();

    if ( p() -> buff.bloodsurge -> up() )
      c += p() -> buff.bloodsurge -> data().effectN( 2 ).resource( RESOURCE_RAGE );

    return c;
  }

  void execute()
  {
    warrior_attack_t::execute();

    p() -> buff.bloodsurge -> decrement();
  }

  bool ready()
  {
    if ( !p() -> cooldown.stance_cooldown -> up() )
      return false;

    if ( p() -> off_hand_weapon.type == WEAPON_NONE )
      return false;

    return warrior_attack_t::ready();
  }
};

// ==========================================================================
// Warrior Spells
// ==========================================================================

struct warrior_spell_t: public warrior_action_t < spell_t >
{
  warrior_spell_t( const std::string& n, warrior_t* p, const spell_data_t* s = spell_data_t::nil() ):
    base_t( n, p, s )
  {
    may_miss = may_glance = may_block = may_dodge = may_parry = false;
  }
};

// Avatar ===================================================================

struct avatar_t: public warrior_spell_t
{
  avatar_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "avatar", p, p -> talents.avatar )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
  }

  void execute()
  {
    warrior_spell_t::execute();

    p() -> buff.avatar -> trigger();
  }

  void update_ready( timespan_t cd_duration )
  {
    if ( p() -> specialization() != WARRIOR_PROTECTION )
      cd_duration = cooldown -> duration / ( 1 + player -> cache.readiness() );

    warrior_spell_t::update_ready( cd_duration );
  }
};

// Battle Shout =============================================================

struct battle_shout_t: public warrior_spell_t
{
  battle_shout_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "battle_shout", p, p -> find_class_spell( "Battle Shout" ) )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
    callbacks = false;
  }

  void execute()
  {
    warrior_spell_t::execute();

    if ( !sim -> overrides.attack_power_multiplier )
      sim -> auras.attack_power_multiplier -> trigger( 1, buff_t::DEFAULT_VALUE(), -1.0, data().duration() );
  }
};

// Berserker Rage ===========================================================

struct berserker_rage_t: public warrior_spell_t
{
  berserker_rage_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "berserker_rage", p, p -> find_class_spell( "Berserker Rage" ) )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
  }

  void execute()
  {
    warrior_spell_t::execute();

    p() -> buff.berserker_rage -> trigger();
    if ( p() -> specialization() != WARRIOR_ARMS )
      p() -> enrage();
  }
};

// Bloodbath ================================================================

struct bloodbath_t: public warrior_spell_t
{
  bloodbath_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "bloodbath", p, p -> talents.bloodbath )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
  }

  void execute()
  {
    warrior_spell_t::execute();

    p() -> buff.bloodbath -> trigger();
  }

  void update_ready( timespan_t cd_duration )
  {
    if ( p() -> specialization() != WARRIOR_PROTECTION )
      cd_duration = cooldown -> duration / ( 1 + player -> cache.readiness() );

    warrior_spell_t::update_ready( cd_duration );
  }
};

// Commanding Shout =========================================================

struct commanding_shout_t: public warrior_spell_t
{
  commanding_shout_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "commanding_shout", p, p -> find_class_spell( "Commanding Shout" ) )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
    callbacks = false;
  }

  void execute()
  {
    warrior_spell_t::execute();

    if ( !sim -> overrides.stamina )
      sim -> auras.stamina -> trigger( 1, buff_t::DEFAULT_VALUE(), -1.0, data().duration() );
  }
};

// Deep Wounds ==============================================================

struct deep_wounds_t: public warrior_spell_t
{
  deep_wounds_t( warrior_t* p ):
    warrior_spell_t( "deep_wounds", p, p -> find_specialization_spell( "Deep Wounds" ) -> effectN( 2 ).trigger() )
  {
    background = tick_may_crit = true;
    hasted_ticks = false;
    dot_behavior = DOT_REFRESH;
  }
};

// Die By the Sword  ==============================================================

struct die_by_the_sword_t: public warrior_spell_t
{
  die_by_the_sword_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "die_by_the_sword", p, p -> spec.die_by_the_sword )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_DEFENSE | STANCE_BATTLE;
  }

  void execute()
  {
    warrior_spell_t::execute();
    p() -> buff.die_by_the_sword -> trigger();
  }

  void update_ready( timespan_t cd_duration )
  {
    cd_duration = cooldown -> duration / ( 1 + player -> cache.readiness() );
    warrior_spell_t::update_ready( cd_duration );
  }

  bool ready()
  {
    if ( p() -> main_hand_weapon.type == WEAPON_NONE )
      return false;

    return warrior_spell_t::ready();
  }
};

// Enhanced Rend ==============================================================

struct enhanced_rend_t: public warrior_spell_t
{
  enhanced_rend_t( warrior_t* p ):
    warrior_spell_t( "enhanced_rend", p, p -> find_spell( 174736 ) )
  {
    dual = true;
  }

  double target_armor(player_t*) const
  {
    return 0.0;
  }
};

// Last Stand ===============================================================

struct last_stand_t: public warrior_spell_t
{
  last_stand_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "last_stand", p, p -> spec.last_stand )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
    cooldown -> duration = data().cooldown();
    cooldown -> duration += p -> sets.set( SET_T14_2PC_TANK ) -> effectN( 1 ).time_value();
  }

  void execute()
  {
    warrior_spell_t::execute();
    p() -> buff.last_stand -> trigger();
  }

  void update_ready( timespan_t cd_duration )
  {
    cd_duration = cooldown -> duration / ( 1 + player -> cache.readiness() );
    warrior_spell_t::update_ready( cd_duration );
  }
};

// Rallying Cry ===============================================================

struct rallying_cry_heal_t: public warrior_heal_t
{
  double percent;
  rallying_cry_heal_t( warrior_t* p ):
    warrior_heal_t( "glyph_of_rallying_cry", p, p -> glyphs.rallying_cry )
  {
    percent = data().effectN( 1 ).percent();
  }

  void execute()
  {
    base_dd_max *= percent; //Heals for 20% of original damage
    base_dd_min *= percent;

    warrior_heal_t::execute();
  }
};

struct rallying_cry_t: public warrior_spell_t
{
  rallying_cry_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "rallying_cry", p, p -> find_specialization_spell( "Rallying Cry" ) )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
  }

  void execute()
  {
    warrior_spell_t::execute();
    p() -> buff.rallying_cry -> trigger();
  }
};

// Recklessness =============================================================

struct recklessness_t: public warrior_spell_t
{
  double bonus_crit;
  recklessness_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "recklessness", p, p -> spec.recklessness ),
    bonus_crit( 0.0 )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE;
    bonus_crit = data().effectN( 1 ).percent();
    bonus_crit *= ( 1 - p -> glyphs.recklessness -> effectN( 1 ).percent() );
    cooldown -> duration = data().cooldown();
    cooldown -> duration += p -> sets.set( SET_T14_4PC_MELEE ) -> effectN( 1 ).time_value();
  }

  void execute()
  {
    warrior_spell_t::execute();

    p() -> buff.recklessness -> trigger( 1, bonus_crit );
    if ( p() -> sets.has_set_bonus( SET_T17_4PC_MELEE ) && p() -> specialization() == WARRIOR_FURY )
      p() -> buff.tier17_4pc_fury_driver -> trigger();
  }

  void update_ready( timespan_t cd_duration )
  {
    cd_duration = cooldown -> duration / ( 1 + player -> cache.readiness() );
    warrior_spell_t::update_ready( cd_duration );
  }
};

// Rend ==============================================================

struct rend_burst_t: public warrior_spell_t
{
  rend_burst_t( warrior_t* p ):
    warrior_spell_t( "rend_burst", p, p -> find_spell( 94009 ) )
  {
    dual = true;
    may_multistrike = 1;
  }

  double target_armor( player_t* ) const
  {
    return 0.0;
  }
};

struct rend_t: public warrior_spell_t
{
  rend_burst_t* burst;
  rend_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "rend", p, p -> spec.rend ),
    burst( new rend_burst_t( p ) )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_DEFENSE;
    hasted_ticks = tick_zero = false;
    tick_may_crit = true;
    may_multistrike = 1;
    add_child( burst );
  }

  void impact( action_state_t* s )
  {
    if ( result_is_hit( s -> result ) )
    {
      dot_t* dot = get_dot( s -> target );
      if ( dot -> is_ticking() && dot -> remains() < dot -> current_action -> base_tick_time )
        burst -> execute();
    }
    warrior_spell_t::impact( s );
  }

  void tick( dot_t* d )
  {
    warrior_spell_t::tick( d );
    if ( p() -> talents.taste_for_blood -> ok() )
      p() -> resource_gain( RESOURCE_RAGE,
      p() -> talents.taste_for_blood -> effectN( 1 ).trigger() -> effectN( 1 ).resource( RESOURCE_RAGE ),
      p() -> gain.taste_for_blood );
  }

  void last_tick( dot_t* d )
  {
    warrior_spell_t::last_tick( d );
    burst -> execute();
  }

  bool ready()
  {
    if ( p() -> main_hand_weapon.type == WEAPON_NONE )
      return false;

    return warrior_spell_t::ready();
  }
};

// Shield Barrier ===========================================================

struct shield_barrier_t: public warrior_action_t < absorb_t >
{
  shield_barrier_t( warrior_t* p, const std::string& options_str ):
    base_t( "shield_barrier", p, p -> specialization() == WARRIOR_PROTECTION ? 
                                 p -> find_spell( 112048 ) : p -> find_specialization_spell( "Shield Barrier" ) )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_GLADIATOR | STANCE_DEFENSE;
    use_off_gcd = true;
    may_crit = false;
    target = player;
    attack_power_mod.direct = 2.75; // Effect #1 is not correct.
  }

  double cost() const
  {
    return std::min( 60.0, std::max( p() -> resources.current[RESOURCE_RAGE], 20.0 ) );
  }

  void impact( action_state_t* s )
  {
    //1) Buff does not stack with itself.
    //2) Will overwrite existing buff if new one is bigger.
    double amount;

    amount = s -> result_amount;
    amount *= cost() / 20;
    if ( !p() -> buff.shield_barrier -> check() ||
         ( p() -> buff.shield_barrier -> check() && p() -> buff.shield_barrier -> current_value < amount )
         )
    {
      p() -> buff.shield_barrier -> trigger( 1, amount );
      stats -> add_result( 0.0, amount, ABSORB, s -> result, s -> block_result, p() );
    }
  }

  bool ready()
  {
    if ( !p() -> has_shield_equipped() )
      return false;

    return base_t::ready();
  }
};

// Shield Block =============================================================

struct shield_block_t: public warrior_spell_t
{
  shield_block_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "shield_block", p, p -> find_class_spell( "Shield Block" ) )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_DEFENSE | STANCE_BATTLE;
    cooldown -> duration = timespan_t::from_seconds( 9.0 );
    cooldown -> charges = 2;
    use_off_gcd = true;
  }

  double cost() const
  {
    double c = warrior_spell_t::cost();

    c += p() -> sets.set( SET_T14_4PC_TANK ) -> effectN( 1 ).resource( RESOURCE_RAGE );

    return c;
  }

  void execute()
  {
    warrior_spell_t::execute();
    p() -> cooldown.block -> start();

    if ( p() -> buff.shield_block -> check() )
      p() -> buff.shield_block -> extend_duration( p(), timespan_t::from_seconds( 6.0 ) );
    else
      p() -> buff.shield_block -> trigger();
  }

  bool ready()
  {
    if ( !p() -> has_shield_equipped() )
      return false;

    if ( !p() -> cooldown.block -> up() )
      return false;

    return warrior_spell_t::ready();
  }
};

// Shield Charge ============================================================

struct shield_charge_t: public warrior_spell_t
{
  shield_charge_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "shield_charge", p, p -> find_spell( 156321 ) )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_GLADIATOR;
    cooldown -> charges = 2;
    cooldown -> duration = timespan_t::from_seconds( 15 );
    base_teleport_distance = data().max_range();
    movement_directionality = MOVEMENT_OMNI;
    use_off_gcd = true;
  }

  void execute()
  {
    warrior_spell_t::execute();

    if ( p() -> buff.shield_charge -> check() )
      p() -> buff.shield_charge -> extend_duration( p(), timespan_t::from_seconds( 6.0 ) );
    else
      p() -> buff.shield_charge -> trigger();
  }

  bool ready()
  {
    if ( !p() -> has_shield_equipped() )
      return false;

    if ( p() -> current.distance_to_move > base_teleport_distance )
      return false;

    return warrior_spell_t::ready();
  }
};

// Shield Wall ==============================================================

struct shield_wall_t: public warrior_spell_t
{
  shield_wall_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "shield_wall", p, p -> find_class_spell( "Shield Wall" ) )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
    harmful = false;
    cooldown -> duration = data().cooldown();
    cooldown -> duration += p -> spec.bastion_of_defense -> effectN( 2 ).time_value();
    cooldown -> duration += p -> glyphs.shield_wall -> effectN( 1 ).time_value();
  }

  void execute()
  {
    warrior_spell_t::execute();

    double value = p() -> buff.shield_wall -> data().effectN( 1 ).percent();
    value += p() -> glyphs.shield_wall -> effectN( 2 ).percent();

    p() -> buff.shield_wall -> trigger( 1, value );
  }

  void update_ready( timespan_t cd_duration )
  {
    cd_duration = cooldown -> duration / ( 1 + player -> cache.readiness() );
    warrior_spell_t::update_ready( cd_duration );
  }
};

// Spell Reflection  ==============================================================

struct spell_reflection_t: public warrior_spell_t
{
  spell_reflection_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "spell_reflection", p, p -> find_class_spell( "Spell Reflection" ) )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
  }
};

// Mass Spell Reflection  ==============================================================

struct mass_spell_reflection_t: public warrior_spell_t
{
  mass_spell_reflection_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "spell_reflection", p, p -> talents.mass_spell_reflection )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_DEFENSE | STANCE_GLADIATOR | STANCE_BATTLE;
  }
};

// The swap/damage taken options are intended to make it easier for players to simulate possible gains/losses from
// swapping stances while in combat, without having to create a bunch of messy actions for it.
// (Instead, we have a bunch of messy code!)

// Stance ==============================================================

struct stance_t: public warrior_spell_t
{
  warrior_stance switch_to_stance;
  warrior_stance starting_stance;
  warrior_stance original_switch;
  std::string stance_str;
  double swap;

  stance_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "stance", p ),
    switch_to_stance( STANCE_BATTLE ), stance_str( "" ), swap( 0 )
  {
    option_t options[] =
    {
      opt_string( "choose", stance_str ),
      opt_float( "swap", swap ),
      opt_null()
    };
    parse_options( options, options_str );

    if ( p -> specialization() != WARRIOR_PROTECTION )
      starting_stance = STANCE_BATTLE;
    else if ( p -> primary_role() == ROLE_ATTACK && p -> talents.gladiators_resolve )
      starting_stance = STANCE_GLADIATOR;
    else
      starting_stance = STANCE_DEFENSE;

    if ( !stance_str.empty() )
    {
      if ( stance_str == "battle" )
      {
        switch_to_stance = STANCE_BATTLE;
        original_switch = switch_to_stance;
      }
      else if ( stance_str == "def" || stance_str == "defensive" )
      {
        switch_to_stance = STANCE_DEFENSE;
        original_switch = switch_to_stance;
      }
      else if ( stance_str == "glad" || stance_str == "gladiator" )
      {
        switch_to_stance = STANCE_GLADIATOR;
        original_switch = switch_to_stance;
      }
    }

    if ( swap == 0 )
      cooldown -> duration = p -> cooldown.stance_swap -> duration;
    else
    {
      p -> swapping = true;
      cooldown -> duration = ( timespan_t::from_seconds( swap ) );
    }

    callbacks = harmful = false;
    use_off_gcd = true;
  }

  void execute()
  {
    if ( p() -> active_stance != switch_to_stance )
    {
      switch ( p() -> active_stance )
      {
      case STANCE_BATTLE: p() -> buff.battle_stance -> expire(); break;
      case STANCE_DEFENSE:
      {
        p() -> buff.defensive_stance -> expire();
        p() -> recalculate_resource_max( RESOURCE_HEALTH );
        break;
      }
      case STANCE_GLADIATOR: p() -> buff.gladiator_stance -> expire(); break;
      }

      switch ( switch_to_stance )
      {
      case STANCE_BATTLE: p() -> buff.battle_stance -> trigger(); break;
      case STANCE_DEFENSE:
      {
        p() -> buff.defensive_stance -> trigger();
        p() -> recalculate_resource_max( RESOURCE_HEALTH ); // Force stamina modifier, otherwise it doesn't apply until stat_loss is called
        break;
      }
      case STANCE_GLADIATOR: p() -> buff.gladiator_stance -> trigger(); break;
      }
      p() -> cooldown.stance_swap -> start();
    }

    if ( swap > 0 )
    {
      if ( swap >= 3.0 && p() -> active_stance != starting_stance )
        switch_to_stance = starting_stance;
      if ( swap < 3.0 )
        cooldown -> start();
      if ( swap >= 3.0 && p() -> active_stance == starting_stance )
      {
        switch_to_stance = original_switch;
        cooldown -> start();
        cooldown -> adjust( -1 * p() -> cooldown.stance_swap -> duration );
      }
    }
  }

  bool ready()
  {
    if ( p() -> cooldown.stance_swap -> down() ||
         cooldown -> down() ||
         ( swap == 0 && p() -> active_stance == switch_to_stance ) ||
         p() -> buff.gladiator_stance -> check() )
         return false;

    return warrior_spell_t::ready();
  }
};

// Sweeping Strikes =========================================================

struct sweeping_strikes_t: public warrior_spell_t
{
  sweeping_strikes_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "sweeping_strikes", p, p -> spec.sweeping_strikes )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE;
    cooldown -> duration  = data().cooldown();
    cooldown -> duration += p -> perk.enhanced_sweeping_strikes -> effectN( 2 ).time_value();
  }

  void execute()
  {
    warrior_spell_t::execute();
    p() -> buff.sweeping_strikes -> trigger();
  }
};

// Taunt =======================================================================

struct taunt_t: public warrior_spell_t
{
  taunt_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "taunt", p, p -> find_class_spell( "Taunt" ) )
  {
    parse_options( NULL, options_str );
    use_off_gcd = true;
    stancemask = STANCE_DEFENSE | STANCE_GLADIATOR;
  }

  void impact( action_state_t* s )
  {
    if ( s -> target -> is_enemy() )
      target -> taunt( player );

    warrior_spell_t::impact( s );
  }
};

// Vigilance =======================================================================

struct vigilance_t: public warrior_spell_t
{
  vigilance_t( warrior_t* p, const std::string& options_str ):
    warrior_spell_t( "vigilance", p, p -> talents.vigilance )
  {
    parse_options( NULL, options_str );
    stancemask = STANCE_BATTLE | STANCE_GLADIATOR | STANCE_DEFENSE;
  }
};

} // UNNAMED NAMESPACE

// warrior_t::create_action  ================================================

action_t* warrior_t::create_action( const std::string& name,
                                    const std::string& options_str )
{
  if ( name == "auto_attack"          ) return new auto_attack_t          ( this, options_str );
  if ( name == "avatar"               ) return new avatar_t               ( this, options_str );
  if ( name == "battle_shout"         ) return new battle_shout_t         ( this, options_str );
  if ( name == "berserker_rage"       ) return new berserker_rage_t       ( this, options_str );
  if ( name == "bladestorm"           ) return new bladestorm_t           ( this, options_str );
  if ( name == "bloodbath"            ) return new bloodbath_t            ( this, options_str );
  if ( name == "bloodthirst"          ) return new bloodthirst_t          ( this, options_str );
  if ( name == "charge"               ) return new charge_t               ( this, options_str );
  if ( name == "colossus_smash"       ) return new colossus_smash_t       ( this, options_str );
  if ( name == "demoralizing_shout"   ) return new demoralizing_shout     ( this, options_str );
  if ( name == "devastate"            ) return new devastate_t            ( this, options_str );
  if ( name == "die_by_the_sword"     ) return new die_by_the_sword_t     ( this, options_str );
  if ( name == "dragon_roar"          ) return new dragon_roar_t          ( this, options_str );
  if ( name == "enraged_regeneration" ) return new enraged_regeneration_t ( this, options_str );
  if ( name == "execute"              ) return new execute_t              ( this, options_str );
  if ( name == "hamstring"            ) return new hamstring_t            ( this, options_str );
  if ( name == "heroic_charge"        ) return new heroic_charge_t        ( this, options_str );
  if ( name == "heroic_leap"          ) return new heroic_leap_t          ( this, options_str );
  if ( name == "heroic_strike"        ) return new heroic_strike_t        ( this, options_str );
  if ( name == "heroic_throw"         ) return new heroic_throw_t         ( this, options_str );
  if ( name == "ignite_weapon"        ) return new ignite_weapon_t        ( this, options_str );
  if ( name == "impending_victory"    ) return new impending_victory_t    ( this, options_str );
  if ( name == "intervene" || name == "safeguard" ) return new intervene_t            ( this, options_str );
  if ( name == "last_stand"           ) return new last_stand_t           ( this, options_str );
  if ( name == "mortal_strike"        ) return new mortal_strike_t        ( this, options_str );
  if ( name == "pummel"               ) return new pummel_t               ( this, options_str );
  if ( name == "raging_blow"          ) return new raging_blow_t          ( this, options_str );
  if ( name == "rallying_cry"         ) return new rallying_cry_t         ( this, options_str );
  if ( name == "ravager"              ) return new ravager_t              ( this, options_str );
  if ( name == "recklessness"         ) return new recklessness_t         ( this, options_str );
  if ( name == "rend"                 ) return new rend_t                 ( this, options_str );
  if ( name == "revenge"              ) return new revenge_t              ( this, options_str );
  if ( name == "shield_barrier"       ) return new shield_barrier_t       ( this, options_str );
  if ( name == "shield_block"         ) return new shield_block_t         ( this, options_str );
  if ( name == "shield_charge"        ) return new shield_charge_t        ( this, options_str );
  if ( name == "shield_slam"          ) return new shield_slam_t          ( this, options_str );
  if ( name == "shield_wall"          ) return new shield_wall_t          ( this, options_str );
  if ( name == "shockwave"            ) return new shockwave_t            ( this, options_str );
  if ( name == "slam"                 ) return new slam_t                 ( this, options_str );
  if ( name == "spell_reflection" || name == "mass_spell_reflection" )
  {
    if ( talents.mass_spell_reflection->ok() )
      return new mass_spell_reflection_t(this, options_str);
    else
      return new spell_reflection_t(this, options_str);
  }
  if ( name == "stance"               ) return new stance_t               ( this, options_str );
  if ( name == "storm_bolt"           ) return new storm_bolt_t           ( this, options_str );
  if ( name == "sweeping_strikes"     ) return new sweeping_strikes_t     ( this, options_str );
  if ( name == "taunt"                ) return new taunt_t                ( this, options_str );
  if ( name == "thunder_clap"         ) return new thunder_clap_t         ( this, options_str );
  if ( name == "victory_rush"         ) return new victory_rush_t         ( this, options_str );
  if ( name == "vigilance"            ) return new vigilance_t            ( this, options_str );
  if ( name == "whirlwind"            ) return new whirlwind_t            ( this, options_str );
  if ( name == "wild_strike"          ) return new wild_strike_t          ( this, options_str );

  return player_t::create_action( name, options_str );
}

// warrior_t::init_spells ===================================================

void warrior_t::init_spells()
{
  player_t::init_spells();

  // Mastery
  mastery.critical_block        = find_mastery_spell( WARRIOR_PROTECTION );
  mastery.weapons_master        = find_mastery_spell( WARRIOR_ARMS );
  mastery.unshackled_fury       = find_mastery_spell( WARRIOR_FURY );

  // Spec Passives
  spec.bastion_of_defense       = find_specialization_spell( "Bastion of Defense" );
  spec.bladed_armor             = find_specialization_spell( "Bladed Armor" );
  spec.blood_craze              = find_specialization_spell( "Blood Craze" );
  spec.bloodsurge               = find_specialization_spell( "Bloodsurge" );
  spec.bloodthirst              = find_specialization_spell( "Bloodthirst" );
  spec.colossus_smash           = find_specialization_spell( "Colossus Smash" );
  spec.crazed_berserker         = find_specialization_spell( "Crazed Berserker" );
  spec.cruelty                  = find_specialization_spell( "Cruelty" );
  spec.devastate                = find_specialization_spell( "Devastate" );
  spec.die_by_the_sword         = find_specialization_spell( "Die By the Sword" );
  spec.enrage                   = find_specialization_spell( "Enrage" );
  spec.last_stand               = find_specialization_spell( "Last Stand" );
  spec.meat_cleaver             = find_specialization_spell( "Meat Cleaver" );
  spec.mortal_strike            = find_specialization_spell( "Mortal Strike" );
  spec.raging_blow              = find_specialization_spell( "Raging Blow" );
  spec.rallying_cry             = find_specialization_spell( "Rallying Cry" );
  spec.readiness_arms           = find_specialization_spell( "Readiness: Arms" );
  spec.readiness_fury           = find_specialization_spell( "Readiness: Fury" );
  spec.readiness_protection     = find_specialization_spell( "Readiness: Protection" );
  spec.recklessness             = find_specialization_spell( "Recklessness" );
  spec.rend                     = find_specialization_spell( "Rend" );
  spec.resolve                  = find_specialization_spell( "Resolve" );
  spec.revenge                  = find_specialization_spell( "Revenge" );
  spec.riposte                  = find_specialization_spell( "Riposte" );
  spec.seasoned_soldier         = find_specialization_spell( "Seasoned Soldier" );
  spec.shield_mastery           = find_specialization_spell( "Shield Mastery" );
  spec.shield_slam              = find_specialization_spell( "Shield Slam" );
  spec.singleminded_fury        = find_specialization_spell( "Single-Minded Fury" );
  spec.sweeping_strikes         = find_specialization_spell( "Sweeping Strikes" );
  spec.sword_and_board          = find_specialization_spell( "Sword and Board" );
  spec.thunder_clap             = find_specialization_spell( "Thunder Clap" );
  spec.ultimatum                = find_specialization_spell( "Ultimatum" );
  spec.unwavering_sentinel      = find_specialization_spell( "Unwavering Sentinel" );
  spec.weapon_mastery           = find_specialization_spell( "Weapon Mastery" );
  spec.whirlwind                = find_specialization_spell( "Whirlwind" );
  spec.wild_strike              = find_specialization_spell( "Wild Strike" );

  // Talents
  talents.juggernaut            = find_talent_spell( "Juggernaut" );
  talents.double_time           = find_talent_spell( "Double Time" );
  talents.warbringer            = find_talent_spell( "Warbringer" );

  talents.enraged_regeneration  = find_talent_spell( "Enraged Regeneration" );
  talents.second_wind           = find_talent_spell( "Second Wind" );
  talents.impending_victory     = find_talent_spell( "Impending Victory" );

  talents.heavy_repercussions   = find_talent_spell( "Heavy Repercussions" );
  talents.unyielding_strikes    = find_talent_spell( "Unyielding Strikes" );
  talents.sudden_death          = find_talent_spell( "Sudden Death" );
  talents.taste_for_blood       = find_talent_spell( "Taste for Blood" );
  talents.unquenchable_thirst   = find_talent_spell( "Unquenchable Thirst" );
  talents.slam                  = find_talent_spell( "Slam" );
  talents.furious_strikes       = find_talent_spell( "Furious Strikes" );

  talents.storm_bolt            = find_talent_spell( "Storm Bolt" );
  talents.shockwave             = find_talent_spell( "Shockwave" );
  talents.dragon_roar           = find_talent_spell( "Dragon Roar" );

  talents.mass_spell_reflection = find_talent_spell( "Mass Spell Reflection" );
  talents.safeguard             = find_talent_spell( "Safeguard" );
  talents.vigilance             = find_talent_spell( "Vigilance" );

  talents.avatar                = find_talent_spell( "Avatar" );
  talents.bloodbath             = find_talent_spell( "Bloodbath" );
  talents.bladestorm            = find_talent_spell( "Bladestorm" );

  talents.anger_management      = find_talent_spell( "Anger Management" );
  talents.ravager               = find_talent_spell( "Ravager" );
  talents.ignite_weapon         = find_talent_spell( "Ignite Weapon" );
  talents.gladiators_resolve    = find_talent_spell( "Gladiator's Resolve" );

  //Perks
  perk.improved_heroic_leap          = find_perk_spell( "Improved Heroic Leap" );

  perk.enhanced_sweeping_strikes     = find_perk_spell( "Enhanced Sweeping Strikes" );
  perk.improved_die_by_the_sword     = find_perk_spell( "Improved Die by The Sword" );
  perk.improved_colossus_smash       = find_perk_spell( "Improved Colossus Smash" );
  perk.enhanced_rend                 = find_perk_spell( "Enhanced Rend" );

  perk.enhanced_whirlwind            = find_perk_spell( "Enhanced Whirlwind" );

  perk.improved_heroic_throw         = find_perk_spell( "Improved Heroic Throw" );
  perk.improved_defensive_stance     = find_perk_spell( "Improved Defensive Stance" );
  perk.improved_block                = find_perk_spell( "Improved Block" );

  // Glyphs
  glyphs.bloodthirst            = find_glyph_spell( "Glyph of Bloodthirst" );
  glyphs.bull_rush              = find_glyph_spell( "Glyph of Bull Rush" );
  glyphs.cleave                 = find_glyph_spell( "Glyph of Cleave" );
  glyphs.colossus_smash         = find_glyph_spell( "Glyph of Colossus Smash" );
  glyphs.death_from_above       = find_glyph_spell( "Glyph of Death From Above" );
  glyphs.drawn_sword            = find_glyph_spell( "Glyph of the Drawn Sword" );
  glyphs.enraged_speed          = find_glyph_spell( "Glyph of Enraged Speed" );
  glyphs.hamstring              = find_glyph_spell( "Glyph of Hamstring" );
  glyphs.heroic_leap            = find_glyph_spell( "Glyph of Heroic Leap" );
  glyphs.long_charge            = find_glyph_spell( "Glyph of Long Charge" );
  glyphs.raging_blow            = find_glyph_spell( "Glyph of Raging Blow" );
  glyphs.raging_wind            = find_glyph_spell( "Glyph of Raging Wind" );
  glyphs.rallying_cry           = find_glyph_spell( "Glyph of Rallying Cry" );
  glyphs.recklessness           = find_glyph_spell( "Glyph of Recklessness" );
  glyphs.resonating_power       = find_glyph_spell( "Glyph of Resonating Power" );
  glyphs.rude_interruption      = find_glyph_spell( "Glyph of Rude Interruption" );
  glyphs.shield_wall            = find_glyph_spell( "Glyph of Shield Wall" );
  glyphs.sweeping_strikes       = find_glyph_spell( "Glyph of Sweeping Strikes" );
  glyphs.unending_rage          = find_glyph_spell( "Glyph of Unending Rage" );
  glyphs.victory_rush           = find_glyph_spell( "Glyph of Victory Rush" );

  // Generic spells
  spell.charge                  = find_class_spell( "Charge" );
  spell.intervene               = find_class_spell( "Intervene" );
  spell.headlong_rush           = find_spell( 158836 );
  spell.heroic_leap             = find_class_spell( "Heroic Leap" );

  // Active spells
  active_blood_craze        = new blood_craze_t( this );
  active_bloodbath_dot      = new bloodbath_dot_t( this );
  active_deep_wounds        = new deep_wounds_t( this );
  active_enhanced_rend      = new enhanced_rend_t( this );
  active_rallying_cry_heal  = new rallying_cry_heal_t( this );
  active_second_wind        = new second_wind_t( this );
  active_t16_2pc            = new tier16_2pc_tank_heal_t( this );

  static set_bonus_description_t set_bonuses =
  {
    // 0's are for healers/casters.
    //      M2P     M4P     T2P     T4P
    { 0, 0, 105797, 105907, 105908, 105911, 0, 0 }, // Tier13
    { 0, 0, 123142, 123144, 123146, 123147, 0, 0 }, // Tier14
    { 0, 0, 138120, 138126, 138280, 138281, 0, 0 }, // Tier15
    { 0, 0, 144436, 144441, 144503, 144502, 0, 0 }, // Tier16
    { 0, 0, 0, 0, 165338, 165351, 0, 0 }, // Tier17
  };

  if ( specialization() == WARRIOR_FURY )
  {
    set_bonuses[4][2] = 165337; //T17 2PC
    set_bonuses[4][3] = 165349; //T17 4PC
  }
  else if ( specialization() == WARRIOR_ARMS )
  {
    set_bonuses[4][2] = 165336; //T17 2PC
    set_bonuses[4][3] = 165345; //T17 4PC
  }

  sets.register_spelldata( set_bonuses );
}

// warrior_t::init_defense ==================================================

void warrior_t::init_defense()
{
  player_t::init_defense();
}

// warrior_t::init_base =====================================================

void warrior_t::init_base_stats()
{
  player_t::init_base_stats();

  resources.base[RESOURCE_RAGE] = 100;

  if ( glyphs.unending_rage -> ok() )
    resources.base[RESOURCE_RAGE] += glyphs.unending_rage -> effectN( 1 ).resource( RESOURCE_RAGE );

  base.attack_power_per_strength = 1.0;
  base.attack_power_per_agility  = 0.0;

  // Avoidance diminishing Returns constants/conversions now handled in player_t::init_base_stats().
  // Base miss, dodge, parry, and block are set in player_t::init_base_stats().
  // Just need to add class- or spec-based modifiers here.

  base.dodge += spec.bastion_of_defense -> effectN( 3 ).percent();

  base_gcd = timespan_t::from_seconds( 1.5 );
}

//Pre-combat Action Priority List============================================

void warrior_t::apl_precombat()
{
  action_priority_list_t* precombat = get_action_priority_list( "precombat" );

  // Flask
  if ( sim -> allow_flasks && level >= 80 )
  {
    std::string flask_action = "flask,type=";
    if ( primary_role() == ROLE_ATTACK )
      flask_action += "greater_draenic_strength_flask";
    else if ( primary_role() == ROLE_TANK )
      flask_action += "greater_draenic_stamina_flask";
    precombat -> add_action( flask_action );
  }

  // Food
  if ( sim -> allow_food )
  {
    std::string food_action = "food,type=";
    if ( level >= 90 && specialization() != WARRIOR_PROTECTION )
      food_action += "blackrock_barbecue";
    else
      food_action += "nagrand_tempura";

    if ( level < 90 )
    {
      if ( primary_role() == ROLE_ATTACK )
        food_action += "black_pepper_ribs_and_shrimp";
      else if ( primary_role() == ROLE_TANK )
        food_action += "chun_tian_spring_rolls";
    }
    precombat -> add_action( food_action );
  }

  if ( specialization() != WARRIOR_PROTECTION )
    precombat -> add_action( "stance,choose=battle" );
  else
  {
    precombat -> add_action( "stance,choose=gladiator,if=talent.gladiators_resolve.enabled" );
    precombat -> add_action( "stance,choose=defensive,if=!talent.gladiators_resolve.enabled" );
  }

  precombat -> add_action( "snapshot_stats", "Snapshot raid buffed stats before combat begins and pre-potting is done." );

  //Pre-pot
  if ( sim -> allow_potions )
  {
    if ( level >= 90 )
    {
      if ( primary_role() == ROLE_ATTACK )
        precombat -> add_action( "potion,name=draenic_strength" );
      else if ( primary_role() == ROLE_TANK )
        precombat -> add_action( "potion,name=draenic_armor" );
    }
    //Pre-pot
    else if ( level >= 80 )
    {
      if ( primary_role() == ROLE_ATTACK )
        precombat -> add_action( "potion,name=mogu_power" );
      else if ( primary_role() == ROLE_TANK )
        precombat -> add_action( "potion,name=mountains" );
    }
  }
}

// Fury Warrior Action Priority List ========================================

void warrior_t::apl_fury()
{
  std::vector<std::string> racial_actions     = get_racial_actions();

  action_priority_list_t* default_list        = get_action_priority_list( "default" );
  action_priority_list_t* single_target       = get_action_priority_list( "single_target" );
  action_priority_list_t* two_targets         = get_action_priority_list( "two_targets" );
  action_priority_list_t* three_targets       = get_action_priority_list( "three_targets" );
  action_priority_list_t* aoe                 = get_action_priority_list( "aoe" );

  default_list -> add_action( this, "Charge" );
  default_list -> add_action( "auto_attack" );

  int num_items = (int)items.size();
  for ( int i = 0; i < num_items; i++ )
  {
    if ( items[i].has_special_effect( SPECIAL_EFFECT_SOURCE_NONE, SPECIAL_EFFECT_USE ) )
      default_list -> add_action( "use_item,name=" + items[i].name_str + ",if=debuff.colossus_smash.up" );
  }

  if ( sim -> allow_potions )
  {
    if ( level >= 90 )
      default_list -> add_action( "potion,name=draenic_strength,if=(target.health.pct<20&buff.recklessness.up)|target.time_to_die<=25" );
    else if ( level >= 80 )
      default_list -> add_action( "potion,name=mogu_power,if=(target.health.pct<20&buff.recklessness.up)|target.time_to_die<=25" );
  }

  default_list -> add_action( this, "Recklessness", "if=!talent.bloodbath.enabled&(((cooldown.colossus_smash.remains<2|debuff.colossus_smash.remains>=5)&target.time_to_die>192)|target.health.pct<20)|buff.bloodbath.up&(target.time_to_die>192|target.health.pct<20)|target.time_to_die<=12",
                              "This incredibly long line can be translated to 'Use recklessness on cooldown with colossus smash; unless the boss will die before the ability is usable again, and then combine with execute instead.'" );
  default_list -> add_talent( this, "Avatar", "if=(buff.recklessness.up|target.time_to_die<=25)" );
  default_list -> add_action( this, "Berserker Rage", "if=debuff.colossus_smash.up&buff.raging_blow.stack<2" );

  for ( size_t i = 0; i < racial_actions.size(); i++ )
    default_list -> add_action( racial_actions[i] + ",if=buff.bloodbath.up|(!talent.bloodbath.enabled&debuff.colossus_smash.up)|buff.recklessness.up" );

  default_list -> add_action( "run_action_list,name=single_target,if=active_enemies=1" );
  default_list -> add_action( "run_action_list,name=two_targets,if=active_enemies=2" );
  default_list -> add_action( "run_action_list,name=three_targets,if=active_enemies=3" );
  default_list -> add_action( "run_action_list,name=aoe,if=active_enemies>3" );

  single_target -> add_talent( this, "Bloodbath", "if=(cooldown.colossus_smash.remains<2|debuff.colossus_smash.remains>=5|target.time_to_die<=20)" );
  single_target -> add_talent( this, "Ignite Weapon", "if=(target.health.pct>=20&(buff.ignite_weapon.remains<2|rage>90|(buff.enrage.up&rage>60)))" );
  single_target -> add_action( this, "Heroic Leap", "if=debuff.colossus_smash.up" );
  single_target -> add_action( this, "Bloodthirst", "if=!(target.health.pct<20&debuff.colossus_smash.up&rage>=30&buff.enrage.up)&!talent.unquenchable_thirst.enabled" );
  single_target -> add_action( this, "Bloodthirst", "if=talent.unquenchable_thirst.enabled&buff.enrage.down&rage<100" );
  single_target -> add_talent( this, "Storm Bolt", "if=cooldown.colossus_smash.remains>5|debuff.colossus_smash.up" );
  single_target -> add_talent( this, "Dragon Roar", "if=(!debuff.colossus_smash.up&(buff.bloodbath.up|!talent.bloodbath.enabled))" );
  single_target -> add_talent( this, "Ravager", "if=cooldown.colossus_smash.remains<4" );
  single_target -> add_action( this, "Colossus Smash" );
  single_target -> add_action( this, "Raging Blow", "if=debuff.colossus_smash.up" );
  single_target -> add_action( this, "Execute", "if=debuff.colossus_smash.up|rage>70|target.time_to_die<12|buff.sudden_death.up" );
  single_target -> add_action( this, "Wild Strike", "if=buff.bloodsurge.up|(((debuff.colossus_smash.up|rage>70)&target.health.pct>20)&!talent.ignite_weapon.enabled&!talent.unquenchable_thirst.enabled)" );
  single_target -> add_action( this, "Raging Blow", "if=cooldown.colossus_smash.remains>4" );
  single_target -> add_action( "bladestorm,if=enabled&buff.enrage.remains>3,interrupt_if=buff.enrage.down" );
  single_target -> add_action( this, "Bloodthirst", "if=talent.unquenchable_thirst.enabled" );
  single_target -> add_talent( this, "Shockwave" );

  two_targets -> add_talent( this, "Bloodbath" );
  two_targets -> add_action( this, "Heroic Leap", "if=buff.enrage.up" );
  two_targets -> add_talent( this, "Ignite Weapon", "if=(target.health.pct>=20&rage>100)|buff.ignite_weapon.down" );
  two_targets -> add_talent( this, "Ravager" );
  two_targets -> add_talent( this, "Bladestorm" );
  two_targets -> add_talent( this, "Dragon Roar", "if=!debuff.colossus_smash.up&(buff.bloodbath.up|!talent.bloodbath.enabled)" );
  two_targets -> add_action( this, "Colossus Smash" );
  two_targets -> add_talent( this, "Storm Bolt", "if=debuff.colossus_smash.up" );
  two_targets -> add_action( this, "Bloodthirst", "if=(talent.unquenchable_thirst.enabled&buff.enrage.down)|!talent.unquenchable_thirst.enabled" );
  two_targets -> add_talent( this, "Shockwave" );
  two_targets -> add_action( this, "Raging Blow", "if=buff.meat_cleaver.up" );
  two_targets -> add_action( this, "Whirlwind", "if=!buff.meat_cleaver.up" );
  two_targets -> add_action( this, "Execute" );
  two_targets -> add_action( this, "Bloodthirst" );

  three_targets -> add_talent( this, "Bloodbath" );
  three_targets -> add_action( this, "Heroic Leap", "if=buff.enrage.up" );
  three_targets -> add_talent( this, "Ravager" );
  three_targets -> add_talent( this, "Ignite Weapon", "if=(target.health.pct>=20&rage>100)|buff.ignite_weapon.down" );
  three_targets -> add_talent( this, "Bladestorm", "if=buff.enrage.remains>4" );
  three_targets -> add_talent( this, "Dragon Roar", "if=!debuff.colossus_smash.up&(buff.bloodbath.up|!talent.bloodbath.enabled)" );
  three_targets -> add_action( this, "Bloodthirst", "if=(talent.unquenchable_thirst.enabled&buff.enrage.down)|!talent.unquenchable_thirst.enabled" );
  three_targets -> add_action( this, "Colossus Smash" );
  three_targets -> add_talent( this, "Storm Bolt", "if=debuff.colossus_smash.up" );
  three_targets -> add_action( this, "Raging Blow", "if=buff.meat_cleaver.stack>=2" );
  three_targets -> add_action( this, "Whirlwind" );
  three_targets -> add_action( this, "Bloodthirst" );

  aoe -> add_talent( this, "Bloodbath" );
  aoe -> add_action( this, "Heroic Leap", "if=buff.enrage.up" );
  aoe -> add_talent( this, "Ignite Weapon", "if=(target.health.pct>=20&rage>100)|buff.ignite_weapon.down" );
  aoe -> add_talent( this, "Ravager" );
  aoe -> add_talent( this, "Bladestorm" );
  aoe -> add_action( this, "Bloodthirst", "if=(talent.unquenchable_thirst.enabled&buff.enrage.down)|!talent.unquenchable_thirst.enabled" );
  aoe -> add_action( this, "Raging Blow", "if=buff.meat_cleaver.stack>=3" );
  aoe -> add_action( this, "Whirlwind" );
  aoe -> add_talent( this, "Dragon Roar", "if=debuff.colossus_smash.down&(buff.bloodbath.up|!talent.bloodbath.enabled)" );
  aoe -> add_action( this, "Colossus Smash" );
  aoe -> add_talent( this, "Storm Bolt" );
  aoe -> add_talent( this, "Shockwave" );
  aoe -> add_action( this, "Bloodthirst" );
}

// Arms Warrior Action Priority List ========================================

void warrior_t::apl_arms()
{
  std::vector<std::string> racial_actions     = get_racial_actions();

  action_priority_list_t* default_list        = get_action_priority_list( "default" );
  action_priority_list_t* single_target       = get_action_priority_list( "single_target" );
  action_priority_list_t* aoe                 = get_action_priority_list( "aoe" );

  default_list -> add_action( this, "charge" );
  default_list -> add_action( "auto_attack" );

  int num_items = (int)items.size();
  for ( int i = 0; i < num_items; i++ )
  {
    if ( items[i].has_special_effect( SPECIAL_EFFECT_SOURCE_NONE, SPECIAL_EFFECT_USE ) )
      default_list -> add_action( "use_item,name=" + items[i].name_str + ",if=debuff.colossus_smash.up" );
  }

  if ( sim -> allow_potions )
  {
    if ( level >= 90 )
      default_list -> add_action( "potion,name=draenic_strength,if=(target.health.pct<20&buff.recklessness.up)|target.time_to_die<=25" );
    else if ( level >= 80 )
      default_list -> add_action( "potion,name=mogu_power,if=(target.health.pct<20&buff.recklessness.up)|target.time_to_die<=25" );
  }

  default_list -> add_action( this, "Recklessness", "if=!talent.bloodbath.enabled&((cooldown.colossus_smash.remains<2|debuff.colossus_smash.remains>=5)&(target.time_to_die>192|target.health.pct<20))|buff.bloodbath.up&(target.time_to_die>192|target.health.pct<20)|target.time_to_die<=12",
                              "This incredibly long line (Due to differing talent choices) says 'Use recklessness on cooldown with colossus smash, unless the boss will die before the ability is usable again, and then use it with execute.'" );
  default_list -> add_talent( this, "Avatar", "if=(buff.recklessness.up|target.time_to_die<=25)" );

  for ( size_t i = 0; i < racial_actions.size(); i++ )
    default_list -> add_action( racial_actions[i] + ",if=buff.bloodbath.up|(!talent.bloodbath.enabled&debuff.colossus_smash.up)|buff.recklessness.up" );

  default_list -> add_talent( this, "Bloodbath", "if=(debuff.colossus_smash.remains>0.1|cooldown.colossus_smash.remains<5|target.time_to_die<=20)" );
  default_list -> add_action( this, "Heroic Leap", "if=debuff.colossus_smash.up" );
  default_list -> add_action( "run_action_list,name=aoe,if=active_enemies>=2" );
  default_list -> add_action( "run_action_list,name=single_target,if=active_enemies<2" );

  single_target -> add_action( this, "Rend", "if=ticks_remain<2" );
  single_target -> add_action( this, "Mortal Strike", "if=target.health.pct>20" );
  //single_target -> add_action( "heroic_charge,if=rage<45" ); It's not feasible in raids to do this with the rage tweaks.
  single_target -> add_talent( this, "Ravager", "if=cooldown.colossus_smash.remains<3" );
  single_target -> add_action( this, "Colossus Smash" );
  single_target -> add_talent( this, "Storm Bolt", "if=cooldown.colossus_smash.remains>4|debuff.colossus_smash.up" );
  single_target -> add_talent( this, "Dragon Roar" );
  single_target -> add_action( this, "Execute", "if=rage>60|(rage>40&debuff.colossus_smash.up)|buff.sudden_death.up" );
  single_target -> add_talent( this, "Slam", "if=(debuff.colossus_smash.up|rage>60)&target.health.pct>20" );
  single_target -> add_action( this, "Whirlwind", "if=(rage>60|debuff.colossus_smash.up)&target.health.pct>20&buff.sudden_death.down&!talent.slam.enabled" );

  aoe -> add_action( this, "Sweeping Strikes" );
  aoe -> add_action( this, "Rend", "if=!ticking" );
  aoe -> add_talent( this, "Ravager" );
  aoe -> add_talent( this, "Bladestorm" );
  aoe -> add_talent( this, "Shockwave" );
  aoe -> add_talent( this, "Dragon Roar" );
  aoe -> add_action( this, "Colossus Smash" );
  aoe -> add_action( this, "Mortal Strike", "if=active_enemies<4" );
  aoe -> add_action( this, "Execute", "if=buff.sudden_death.up|active_enemies<4" );
  aoe -> add_action( this, "Whirlwind", "if=rage>40" );
  aoe -> add_action( this, "Rend", "cycle_targets=1,if=!ticking&talent.taste_for_blood.enabled" );
}

// Protection Warrior Action Priority List ========================================

void warrior_t::apl_prot()
{
  std::vector<std::string> racial_actions     = get_racial_actions();

  action_priority_list_t* default_list        = get_action_priority_list( "default" );
  action_priority_list_t* prot                = get_action_priority_list( "prot" );
  action_priority_list_t* prot_aoe            = get_action_priority_list( "prot_aoe" );
  action_priority_list_t* gladiator           = get_action_priority_list( "gladiator" );
  action_priority_list_t* gladiator_aoe       = get_action_priority_list( "gladiator_aoe" );

  default_list -> add_action( this, "charge" );
  default_list -> add_action( "auto_attack" );

  int num_items = (int)items.size();
  for ( int i = 0; i < num_items; i++ )
  {
    if ( items[i].has_special_effect( SPECIAL_EFFECT_SOURCE_NONE, SPECIAL_EFFECT_USE ) )
      default_list -> add_action( "use_item,name=" + items[i].name_str + ",if=buff.bloodbath.up|buff.avatar.up|buff.shield_charge.up" );
  }
  for ( size_t i = 0; i < racial_actions.size(); i++ )
    default_list -> add_action( racial_actions[i] + ",if=buff.bloodbath.up|buff.avatar.up|buff.shield_charge.up" );
  default_list -> add_action( this, "Berserker Rage", "if=buff.enrage.down" );
  default_list -> add_action( "run_action_list,name=gladiator,if=talent.gladiators_resolve.enabled" );
  default_list -> add_action( "run_action_list,name=prot" );

  if ( sim -> allow_potions )
  {
    if ( level >= 90 )
      prot -> add_action( "potion,name=draenic_armor" );
    else if ( level >= 80 )
      prot -> add_action( "potion,name=mountains" );
  }

  prot -> add_action( this, "Heroic Strike", "if=buff.ultimatum.up" );
  prot -> add_action( this, "Shield Block" );
  prot -> add_action( this, "Shield Barrier", "if=incoming_damage_1500ms>health.max*0.3|rage>rage.max-20" );
  prot -> add_action( this, "Shield Wall", "if=incoming_damage_2500ms>health.max*0.6" );
  prot -> add_action( this, "Last Stand", "if=incoming_damage_2500ms>health.max*0.6&buff.shield_wall.down" );
  prot -> add_action( "run_action_list,name=prot_aoe,if=active_enemies>3" );

  prot -> add_action( this, "Shield Slam" );
  prot -> add_action( this, "Revenge" );
  prot -> add_talent( this, "Enraged Regeneration" );
  prot -> add_action( this, "Heroic Strike", "if=buff.ultimatum.up" );
  prot -> add_talent( this, "Ravager" );
  prot -> add_action( this, "Thunder Clap" );
  prot -> add_action( this, "Demoralizing Shout" );
  prot -> add_talent( this, "Impending Victory" );
  prot -> add_action( this, "Victory Rush", "if=!talent.impending_victory.enabled" );
  prot -> add_talent( this, "Bloodbath" );
  prot -> add_talent( this, "Bladestorm" );
  prot -> add_talent( this, "Shockwave" );
  prot -> add_talent( this, "Storm Bolt" );
  prot -> add_talent( this, "Dragon Roar" );
  prot -> add_action( this, "Devastate" );

  prot_aoe -> add_talent( this, "Bloodbath" );
  prot_aoe -> add_talent( this, "Avatar" );
  prot_aoe -> add_action( this, "Thunder Clap", "if=!dot.deep_wounds.ticking" );
  prot_aoe -> add_talent( this, "Bladestorm" );
  prot_aoe -> add_action( this, "Heroic Strike", "if=buff.ultimatum.up|rage>110" );
  prot_aoe -> add_action( this, "Heroic Leap", "if=(buff.bloodbath.up|cooldown.bloodbath.remains>5|!talent.bloodbath.enabled)" );
  prot_aoe -> add_action( this, "Revenge" );
  prot_aoe -> add_action( this, "Shield Slam" );
  prot_aoe -> add_action( this, "Thunder Clap" );
  prot_aoe -> add_talent( this, "Dragon Roar", "if=(buff.bloodbath.up|cooldown.bloodbath.remains>10)|!talent.bloodbath.enabled" );
  prot_aoe -> add_talent( this, "Storm Bolt", "if=(buff.bloodbath.up|cooldown.bloodbath.remains>7)|!talent.bloodbath.enabled" );
  prot_aoe -> add_action( this, "Devastate", "if=cooldown.shield_slam.remains>gcd*0.4" );

  if ( sim -> allow_potions )
    gladiator -> add_action( "potion,name=draenic_strength,if=buff.bloodbath.up|buff.avatar.up|buff.shield_charge.up" );

  gladiator -> add_action( "run_action_list,name=gladiator_aoe,if=active_enemies>3" );
  gladiator -> add_action( "shield_charge,if=buff.shield_charge.down&cooldown.shield_slam.remains=0&(cooldown.bloodbath.remains>15|!talent.bloodbath.enabled)" );
  gladiator -> add_action( this, "Heroic Strike", "if=buff.shield_charge.up|buff.ultimatum.up|rage>=90|target.time_to_die<=3|(talent.unyielding_strikes.enabled&buff.unyielding_strikes.max_stack)" );
  gladiator -> add_talent( this, "Bloodbath" );
  gladiator -> add_talent( this, "Avatar" );
  gladiator -> add_action( this, "Heroic Leap", "if=(buff.bloodbath.up|cooldown.bloodbath.remains>10)|!talent.bloodbath.enabled" );
  gladiator -> add_action( this, "Shield Slam" );
  gladiator -> add_action( this, "Revenge" );
  gladiator -> add_talent( this, "Storm Bolt", "if=(buff.bloodbath.up|cooldown.bloodbath.remains>7)|!talent.bloodbath.enabled" );
  gladiator -> add_talent( this, "Dragon Roar", "if=(buff.bloodbath.up|cooldown.bloodbath.remains>10)|!talent.bloodbath.enabled" );
  gladiator -> add_action( this, "Devastate", "if=cooldown.shield_slam.remains>gcd*0.4" );

  gladiator_aoe -> add_talent( this, "Bloodbath" );
  gladiator_aoe -> add_talent( this, "Avatar" );
  gladiator_aoe -> add_action( "shield_charge,if=buff.shield_charge.down&cooldown.shield_slam.remains=0&(cooldown.bloodbath.remains>15|!talent.bloodbath.enabled)" );
  gladiator_aoe -> add_action( this, "Thunder Clap", "if=!dot.deep_wounds.ticking" );
  gladiator_aoe -> add_talent( this, "Bladestorm" );
  gladiator_aoe -> add_action( this, "Heroic Strike", "if=buff.ultimatum.up|active_enemies<4|rage>110" );
  gladiator_aoe -> add_action( this, "Heroic Leap", "if=(buff.bloodbath.up|cooldown.bloodbath.remains>5|!talent.bloodbath.enabled)" );
  gladiator_aoe -> add_action( this, "Revenge" );
  gladiator_aoe -> add_action( this, "Shield Slam" );
  gladiator_aoe -> add_action( this, "Thunder Clap" );
  gladiator_aoe -> add_talent( this, "Dragon Roar", "if=(buff.bloodbath.up|cooldown.bloodbath.remains>10)|!talent.bloodbath.enabled" );
  gladiator_aoe -> add_talent( this, "Storm Bolt", "if=(buff.bloodbath.up|cooldown.bloodbath.remains>7)|!talent.bloodbath.enabled" );
  gladiator_aoe -> add_action( this, "Devastate", "if=cooldown.shield_slam.remains>gcd*0.4" );
}

// NO Spec Combat Action Priority List

void warrior_t::apl_default()
{
  action_priority_list_t* default_list = get_action_priority_list( "default" );

  default_list -> add_action( this, "Heroic Throw" );
}

// =========================================================================
// Buff Help Classes
// =========================================================================

// Defensive Stance Rage Gain ==============================================

static void defensive_stance( buff_t* buff, int, int )
{
  warrior_t* p = debug_cast<warrior_t*>( buff -> player );

  p -> resource_gain( RESOURCE_RAGE, 1, p -> gain.defensive_stance );
  buff -> refresh();
}

// Fury T17 4 piece ========================================================

static void tier17_4pc_fury( buff_t* buff, int, int )
{
  warrior_t* p = debug_cast<warrior_t*>( buff -> player );

  p -> buff.tier17_4pc_fury -> trigger( 1 );
}

// ==========================================================================
// Warrior Buffs
// ==========================================================================

namespace buffs
{
template <typename Base>
struct warrior_buff_t: public Base
{
public:
  typedef warrior_buff_t base_t;

  warrior_buff_t( warrior_td_t& p, const buff_creator_basics_t& params ):
    Base( params ), warrior( p.warrior )
  {}

  warrior_buff_t( warrior_t& p, const buff_creator_basics_t& params ):
    Base( params ), warrior( p )
  {}

  warrior_td_t& get_td( player_t* t ) const
  {
    return *( warrior.get_target_data( t ) );
  }

protected:
  warrior_t& warrior;
};


struct bloodsurge_t: public warrior_buff_t < buff_t >
{
  int wasted;
  bloodsurge_t( warrior_t& p, const std::string&n, const spell_data_t*s ):
    base_t( p, buff_creator_t( &p, n, s )
    .chance( p.spec.bloodsurge -> effectN( 1 ).percent() ) ), wasted( 0 )
  {}

  void execute( int a, double b, timespan_t t )
  {
    wasted = 3;
    base_t::execute( a, b, t );
  }

  void decrement( int a, double b )
  {
    wasted--;
    base_t::decrement( a, b );
  }

  void reset()
  {
    base_t::reset();
    wasted = 0;
  }

  void expire_override()
  {
    if ( wasted > 0 )
    {
      do
      {
        warrior.proc.bloodsurge_wasted -> occur();
        wasted--;
      }
      while ( wasted > 0 );
    }
    base_t::expire_override();
  }
};

struct defensive_stance_t: public warrior_buff_t < buff_t >
{
  defensive_stance_t( warrior_t& p, const std::string&n, const spell_data_t*s ):
    base_t( p, buff_creator_t( &p, n, s )
    .activated( true )
    .tick_callback( defensive_stance )
    .tick_behavior( BUFF_TICK_REFRESH )
    .add_invalidate( CACHE_EXP )
    .add_invalidate( CACHE_CRIT_AVOIDANCE )
    .add_invalidate( CACHE_CRIT_BLOCK )
    .add_invalidate( CACHE_BLOCK )
    .add_invalidate( CACHE_STAMINA )
    .add_invalidate( CACHE_ARMOR )
    .add_invalidate( CACHE_BONUS_ARMOR )
    .duration( timespan_t::from_seconds( 3 ) )
    .period( timespan_t::from_seconds( 3 ) ) )
  {}

  void execute( int a, double b, timespan_t t )
  {
    warrior.active_stance = STANCE_DEFENSE;
    base_t::execute( a, b, t );
  }
};

struct battle_stance_t: public warrior_buff_t < buff_t >
{
  battle_stance_t( warrior_t& p, const std::string&n, const spell_data_t*s ):
    base_t( p, buff_creator_t( &p, n, s )
    .activated( true ) )
  {}

  void execute( int a, double b, timespan_t t )
  {
    warrior.active_stance = STANCE_BATTLE;
    base_t::execute( a, b, t );
  }
};

struct gladiator_stance_t: public warrior_buff_t < buff_t >
{
  gladiator_stance_t( warrior_t& p, const std::string&n, const spell_data_t*s ):
    base_t( p, buff_creator_t( &p, n, s )
    .activated( true )
    .add_invalidate( CACHE_PLAYER_DAMAGE_MULTIPLIER ) )
  {}

  void execute( int a, double b, timespan_t t )
  {
    warrior.swapping = true; // Once you go into gladiator stance, there's no going back after precombat.
    warrior.active_stance = STANCE_GLADIATOR;
    base_t::execute( a, b, t );
  }
};

struct rallying_cry_t: public warrior_buff_t < buff_t >
{
  int health_gain;
  rallying_cry_t( warrior_t& p, const std::string&n, const spell_data_t*s ):
    base_t( p, buff_creator_t( &p, n, s ) ), health_gain( 0 )
  {}

  bool trigger( int stacks, double value, double chance, timespan_t duration )
  {
    health_gain = (int)floor( warrior.resources.max[RESOURCE_HEALTH] * data().effectN( 1 ).percent() );
    warrior.stat_gain( STAT_MAX_HEALTH, health_gain, (gain_t*)0, (action_t*)0, true );
    return base_t::trigger( stacks, value, chance, duration );
  }

  void expire_override()
  {
    warrior.stat_loss( STAT_MAX_HEALTH, health_gain, (gain_t*)0, (action_t*)0, true );
    base_t::expire_override();
  }
};

struct last_stand_t: public warrior_buff_t < buff_t >
{
  int health_gain;
  last_stand_t( warrior_t& p, const std::string&n, const spell_data_t*s ):
    base_t( p, buff_creator_t( &p, n, s ) ), health_gain( 0 )
  {}

  bool trigger( int stacks, double value, double chance, timespan_t duration )
  {
    health_gain = (int)floor( warrior.resources.max[RESOURCE_HEALTH] * warrior.spec.last_stand -> effectN( 1 ).percent() );
    warrior.stat_gain( STAT_MAX_HEALTH, health_gain, (gain_t*)0, (action_t*)0, true );
    return base_t::trigger( stacks, value, chance, duration );
  }

  void expire_override()
  {
    warrior.stat_loss( STAT_MAX_HEALTH, health_gain, (gain_t*)0, (action_t*)0, true );
    base_t::expire_override();
  }
};

struct debuff_demo_shout_t: public buff_t
{
  debuff_demo_shout_t( warrior_td_t& wtd ):
    buff_t( buff_creator_t( wtd, "demo_shout", wtd.source -> find_specialization_spell( "Demoralizing Shout" ) ) )
  {
    default_value = data().effectN( 1 ).percent();
  }

  void expire_override()
  {
    warrior_t* p = (warrior_t*)player;

    if ( set_bonus_t::has_set_bonus( p, SET_T16_4PC_TANK ) )
      p -> buff.tier16_reckless_defense -> trigger();

    buff_t::expire_override();
  }
};

} // end namespace buffs

// ==========================================================================
// Warrior Character Definition
// ==========================================================================

warrior_td_t::warrior_td_t( player_t* target, warrior_t& p ):
actor_pair_t( target, &p ), warrior( p )
{
  using namespace buffs;

  dots_bloodbath   = target -> get_dot( "bloodbath", &p );
  dots_deep_wounds = target -> get_dot( "deep_wounds", &p );
  dots_ravager     = target -> get_dot( "ravager", &p );
  dots_rend        = target -> get_dot( "rend", &p );

  debuffs_colossus_smash = buff_creator_t( *this, "colossus_smash" )
    .duration( p.glyphs.colossus_smash -> effectN( 1 ).time_value() +
    p.spec.colossus_smash -> duration() );

  debuffs_demoralizing_shout = new buffs::debuff_demo_shout_t( *this );
  debuffs_taunt = buff_creator_t( *this, "taunt", p.find_class_spell( "Taunt" ) );
}

// warrior_t::init_buffs ====================================================

void warrior_t::create_buffs()
{
  player_t::create_buffs();

  using namespace buffs;

  buff.heroic_charge = buff_creator_t( this, "heroic_charge" );

  buff.avatar = buff_creator_t( this, "avatar", talents.avatar )
    .cd( timespan_t::zero() )
    .add_invalidate( CACHE_PLAYER_DAMAGE_MULTIPLIER );

  buff.battle_stance = new buffs::battle_stance_t( *this, "battle_stance", find_class_spell( "Battle Stance" ) );

  buff.berserker_rage = buff_creator_t( this, "berserker_rage", find_class_spell( "Berserker Rage" ) );

  buff.bladed_armor = buff_creator_t( this, "bladed_armor", spec.bladed_armor )
    .add_invalidate( CACHE_ATTACK_POWER );

  buff.bloodbath = buff_creator_t( this, "bloodbath", talents.bloodbath )
    .cd( timespan_t::zero() );

  buff.blood_craze = buff_creator_t( this, "blood_craze", spec.blood_craze -> effectN( 1 ).trigger() );

  buff.bloodsurge = new buffs::bloodsurge_t( *this, "bloodsurge", spec.bloodsurge -> effectN( 1 ).trigger() );

  buff.defensive_stance = new buffs::defensive_stance_t( *this, "defensive_stance", find_class_spell( "Defensive Stance" ) );

  buff.die_by_the_sword = buff_creator_t( this, "die_by_the_sword", spec.die_by_the_sword )
    .default_value( spec.die_by_the_sword -> effectN( 2 ).percent() + perk.improved_die_by_the_sword -> effectN( 1 ).percent() )
    .cd( timespan_t::zero() )
    .add_invalidate( CACHE_PARRY );

  buff.enrage = buff_creator_t( this, "enrage", spec.enrage -> effectN( 1 ).trigger() )
    .add_invalidate( CACHE_PLAYER_DAMAGE_MULTIPLIER );

  buff.enraged_regeneration = buff_creator_t( this, "enraged_regeneration", talents.enraged_regeneration );

  buff.enraged_speed = buff_creator_t( this, "enraged_speed", glyphs.enraged_speed )
    .duration( buff.enrage -> data().duration() );

  buff.gladiator_stance = new buffs::gladiator_stance_t( *this, "gladiator_stance", find_spell( 156291 ) );

  buff.hamstring = buff_creator_t( this, "hamstring", glyphs.hamstring -> effectN( 1 ).trigger() );

  buff.heroic_leap_glyph = buff_creator_t( this, "heroic_leap_glyph", glyphs.heroic_leap );

  buff.ignite_weapon = buff_creator_t( this, "ignite_weapon", talents.ignite_weapon );

  buff.last_stand = new buffs::last_stand_t( *this, "last_stand", spec.last_stand );

  buff.meat_cleaver     = buff_creator_t( this, "meat_cleaver", spec.meat_cleaver -> effectN( 1 ).trigger() )
    .max_stack( perk.enhanced_whirlwind ? 3 : perk.enhanced_whirlwind -> effectN( 2 ).base_value() );

  buff.raging_blow = buff_creator_t( this, "raging_blow", find_spell( 131116 ) )
    .cd( timespan_t::zero() );
  // The buff has a 0.5 second ICD in spell data, but from in game testing this doesn't
  // do anything. Also confirmed from sparkle dragon.

  buff.raging_blow_glyph = buff_creator_t( this, "raging_blow_glyph", glyphs.raging_blow -> effectN( 1 ).trigger() );

  buff.raging_wind = buff_creator_t( this, "raging_wind", glyphs.raging_wind -> effectN( 1 ).trigger() );

  buff.rallying_cry = new buffs::rallying_cry_t( *this, "rallying_cry", find_spell( 97463 ) );

  buff.ravager = buff_creator_t( this, "ravager", talents.ravager )
    .add_invalidate( CACHE_PARRY );

  buff.recklessness = buff_creator_t( this, "recklessness", spec.recklessness )
    .duration( spec.recklessness -> duration() * ( 1.0 + glyphs.recklessness -> effectN( 2 ).percent() ) )
    .cd( timespan_t::zero() ); //Necessary for readiness.

  buff.rude_interruption = buff_creator_t( this, "rude_interruption", glyphs.rude_interruption -> effectN( 1 ).trigger() )
    .default_value( glyphs.rude_interruption -> effectN( 1 ).trigger() -> effectN( 1 ).percent() )
    .add_invalidate( CACHE_PLAYER_DAMAGE_MULTIPLIER );

  buff.shield_barrier = absorb_buff_creator_t(this, "shield_barrier", specialization() == WARRIOR_PROTECTION ?
                                              find_spell( 112048 ) : find_specialization_spell("Shield Barrier") )
    .source( get_stats( "shield_barrier" ) );

  buff.shield_block = buff_creator_t( this, "shield_block", find_spell( 132404 ) )
    .add_invalidate( CACHE_BLOCK );

  buff.shield_charge = buff_creator_t( this, "shield_charge", find_spell( 169667 ) )
    .default_value( find_spell( 169667 ) -> effectN( 1 ).percent() );

  buff.shield_wall = buff_creator_t( this, "shield_wall", find_specialization_spell( "Shield Wall" ) )
    .default_value( find_specialization_spell( "Shield Wall" ) -> effectN( 1 ).percent() )
    .cd( timespan_t::zero() );

  buff.slam = buff_creator_t( this, "slam", talents.slam );

  buff.sudden_death = buff_creator_t( this, "sudden_death", talents.sudden_death -> effectN( 1 ).trigger() )
    .chance( talents.sudden_death -> proc_chance() );

  buff.sweeping_strikes = buff_creator_t( this, "sweeping_strikes", spec.sweeping_strikes )
    .duration( spec.sweeping_strikes -> duration() + perk.enhanced_sweeping_strikes -> effectN( 1 ).time_value() )
    .cd( timespan_t::zero() );

  buff.sword_and_board = buff_creator_t( this, "sword_and_board", find_spell( 50227 ) )
    .chance( spec.sword_and_board -> effectN( 1 ).percent() );

  buff.tier15_2pc_tank = buff_creator_t( this, "tier15_2pc_tank", find_spell( 138279 ) );

  buff.tier16_reckless_defense = buff_creator_t( this, "tier16_reckless_defense", find_spell( 144500 ) );

  buff.tier17_4pc_arms = buff_creator_t( this, "tier17_4pc_arms", sets.set( SET_T17_4PC_MELEE ) -> effectN( 1 ).trigger() )
    .chance( ( sets.has_set_bonus( SET_T17_4PC_MELEE ) ? 1.0 : 0.0 ) * sets.set( SET_T17_4PC_MELEE ) -> proc_chance() );

  buff.tier17_4pc_fury = buff_creator_t( this, "rampage", find_spell( 166588 ) )
    .default_value( find_spell( 166588 ) -> effectN( 1 ).percent() )
    .add_invalidate( CACHE_ATTACK_SPEED )
    .add_invalidate( CACHE_CRIT );

  buff.tier17_4pc_fury_driver = buff_creator_t( this, "rampage_driver", find_spell( 165350 ) )
    .tick_callback( tier17_4pc_fury );

  buff.unyielding_strikes = buff_creator_t( this, "unyielding_strikes", talents.unyielding_strikes -> effectN( 1 ).trigger() )
    .default_value( talents.unyielding_strikes -> effectN( 1 ).trigger() -> effectN( 1 ).resource( RESOURCE_RAGE ) )
    .max_stack( 5 );

  buff.ultimatum = buff_creator_t( this, "ultimatum", spec.ultimatum -> effectN( 1 ).trigger() );
}

// warrior_t::init_scaling ==================================================

void warrior_t::init_scaling()
{
  player_t::init_scaling();

  if ( specialization() == WARRIOR_FURY )
  {
    scales_with[STAT_WEAPON_OFFHAND_DPS] = true;
    scales_with[STAT_WEAPON_OFFHAND_SPEED] = sim -> weapon_speed_scale_factors != 0;
  }
}

// warrior_t::init_gains ====================================================

void warrior_t::init_gains()
{
  player_t::init_gains();

  gain.avoided_attacks        = get_gain( "avoided_attacks" );
  gain.bloodthirst            = get_gain( "bloodthirst" );
  gain.charge                 = get_gain( "charge" );
  gain.colossus_smash         = get_gain( "colossus_smash" );
  gain.critical_block         = get_gain( "critical_block" );
  gain.defensive_stance       = get_gain( "defensive_stance" );
  gain.drawn_sword_glyph      = get_gain( "drawn_sword_glyph" );
  gain.enrage                 = get_gain( "enrage" );
  gain.melee_crit             = get_gain( "melee_crit" );
  gain.melee_main_hand        = get_gain( "melee_main_hand" );
  gain.melee_off_hand         = get_gain( "melee_off_hand" );
  gain.revenge                = get_gain( "revenge" );
  gain.shield_slam            = get_gain( "shield_slam" );
  gain.sweeping_strikes       = get_gain( "sweeping_strikes" );
  gain.sword_and_board        = get_gain( "sword_and_board" );
  gain.taste_for_blood        = get_gain( "taste_for_blood" );

  gain.tier15_4pc_tank        = get_gain( "tier15_4pc_tank" );
  gain.tier16_2pc_melee       = get_gain( "tier16_2pc_melee" );
  gain.tier16_4pc_tank        = get_gain( "tier16_4pc_tank" );
  gain.tier17_2pc_arms        = get_gain( "tier17_2pc_arms" );
}

// warrior_t::init_position ====================================================

void warrior_t::init_position()
{
  player_t::init_position();

  if ( specialization() == WARRIOR_PROTECTION && primary_role() == ROLE_ATTACK )
  {
    base.position = POSITION_BACK;
    position_str = util::position_type_string( base.position );
    if ( sim -> debug )
      sim -> out_debug.printf( "%s: Position adjusted to %s for Gladiator DPS", name(), position_str.c_str() );
  }
}

// warrior_t::init_procs ======================================================

void warrior_t::init_procs()
{
  player_t::init_procs();
  proc.bloodsurge              = get_proc( "bloodsurge" );
  proc.bloodsurge_wasted       = get_proc( "bloodsurge_wasted" );
  proc.raging_blow_wasted      = get_proc( "raging_blow_wasted" );
  proc.sudden_death            = get_proc( "sudden_death" );
  proc.sudden_death_wasted     = get_proc( "sudden_death_wasted" );

  proc.t15_2pc_melee           = get_proc( "t15_2pc_melee" );
  proc.t17_2pc_fury            = get_proc( "t17_2pc_fury" );
  proc.t17_4pc_arms            = get_proc( "t17_4pc_arms" );
}

// warrior_t::init_rng ======================================================

void warrior_t::init_rng()
{
  player_t::init_rng();

  double rppm;
  //Lookup rppm value according to spec
  switch ( specialization() )
  {
  case WARRIOR_ARMS:
    rppm = 1.6;
    break;
  case WARRIOR_FURY:
    rppm = 0.6;
    break;
  case WARRIOR_PROTECTION:
    rppm = 1;
    break;
  default: rppm = 0.0;
    break;
  }
  t15_2pc_melee.set_frequency( rppm * 1.11 );
}

// warrior_t::init_actions ==================================================

void warrior_t::init_action_list()
{
  if ( !action_list_str.empty() )
  {
    player_t::init_action_list();
    return;
  }
  if ( main_hand_weapon.type == WEAPON_NONE )
  {
    if ( !quiet )
      sim -> errorf( "Player %s has no weapon equipped at the Main-Hand slot.", name() );

    quiet = true;
    return;
  }
  clear_action_priority_lists();

  apl_precombat();

  switch ( specialization() )
  {
  case WARRIOR_FURY:
    apl_fury();
    break;
  case WARRIOR_ARMS:
    apl_arms();
    break;
  case WARRIOR_PROTECTION:
    apl_prot();
    break;
  default:
    apl_default(); // DEFAULT
    break;
  }

  // Default
  use_default_action_list = true;
  player_t::init_action_list();
}

// warrior_t::arise() ======================================================

void warrior_t::arise()
{
  player_t::arise();

  if ( active_stance == STANCE_BATTLE )
    buff.battle_stance -> trigger();
  else if ( active_stance == STANCE_GLADIATOR )
    buff.gladiator_stance -> trigger();
  else if ( active_stance == STANCE_DEFENSE )
    buff.defensive_stance -> trigger();
}

// warrior_t::combat_begin ==================================================

void warrior_t::combat_begin()
{
  player_t::combat_begin();

  if ( initial_rage > 0 )
    resources.current[RESOURCE_RAGE] = initial_rage; // User specified rage.

  if ( specialization() == WARRIOR_PROTECTION )
    resolve_manager.start();

  if ( spec.bladed_armor )
    buff.bladed_armor -> trigger();
}

// warrior_t::reset =========================================================

void warrior_t::reset()
{
  player_t::reset();

  active_stance = STANCE_BATTLE;
  swapping = false;

  t15_2pc_melee.reset();
}

// warrior_t::composite_player_multiplier ===================================

double warrior_t::composite_player_multiplier( school_e school ) const
{
  double m = player_t::composite_player_multiplier( school );

  if ( buff.avatar -> up() )
    m *= 1.0 + buff.avatar -> data().effectN( 1 ).percent();

  if ( main_hand_weapon.group() == WEAPON_2H && spec.seasoned_soldier )
    m *= 1.0 + spec.seasoned_soldier -> effectN( 1 ).percent();

  // --- Enrages ---
  if ( buff.enrage -> up() )
  {
    m *= 1.0 + buff.enrage -> data().effectN( 2 ).percent();

    if ( mastery.unshackled_fury -> ok() )
      m *= 1.0 + cache.mastery_value();
  }

  if ( main_hand_weapon.group() == WEAPON_1H &&
       off_hand_weapon.group() == WEAPON_1H )
       m *= 1.0 + spec.singleminded_fury -> effectN( 1 ).percent();

  // --- Buffs / Procs ---
  if ( buff.rude_interruption -> up() )
    m *= 1.0 + buff.rude_interruption -> value();

  if ( active_stance == STANCE_GLADIATOR && school == SCHOOL_PHYSICAL )
    m *= 1.0 + buff.gladiator_stance -> data().effectN( 1 ).percent();

  return m;
}

// What a name.
// warrior_t::composite_player_critical_damage_multiplier ======================

double warrior_t::composite_player_critical_damage_multiplier() const
{
  double cdm = player_t::composite_player_critical_damage_multiplier();

  if ( buff.recklessness -> up() )
    cdm += ( buff.recklessness -> data().effectN( 2 ).percent() *
    ( glyphs.recklessness -> ok() ? glyphs.recklessness -> effectN( 1 ).percent() : 1 ) );

  return cdm;
}

// warrior_t::composite_attribute =============================================

double warrior_t::composite_attribute( attribute_e attr ) const
{
  double a = player_t::composite_attribute( attr );

  switch ( attr )
  {
  case ATTR_STAMINA:
    if ( active_stance == STANCE_DEFENSE )
      a += spec.unwavering_sentinel -> effectN( 1 ).percent() * player_t::composite_attribute( ATTR_STAMINA );
    break;
  default:
    break;
  }

  return a;
}

// warrior_t::composite_armor_multiplier ======================================

double warrior_t::composite_armor_multiplier() const
{
  double a = player_t::composite_armor_multiplier();

  if ( active_stance == STANCE_DEFENSE )
    a *= 1.0 + spec.unwavering_sentinel -> effectN( 3 ).percent() +
    perk.improved_defensive_stance -> effectN( 1 ).percent();

  return a;
}

// warrior_t::composite_melee_expertise =====================================

double warrior_t::composite_melee_expertise( weapon_t* ) const
{
  double e = player_t::composite_melee_expertise();

  if ( active_stance == STANCE_DEFENSE )
    e += spec.unwavering_sentinel -> effectN( 5 ).percent();

  return e;
}

// warrior_t::composite_rating_multiplier =====================================

double warrior_t::composite_rating_multiplier( rating_e rating ) const
{
  double m = player_t::composite_rating_multiplier( rating );

  switch ( rating )
  {
  case RATING_MELEE_CRIT:
    return m *= 1.0 + spec.cruelty -> effectN( 1 ).percent();
  case RATING_SPELL_CRIT:
    return m *= 1.0 + spec.cruelty -> effectN( 1 ).percent();
  case RATING_MASTERY:
    m *= 1.0 + spec.weapon_mastery -> effectN( 1 ).percent();
    m *= 1.0 + spec.shield_mastery -> effectN( 1 ).percent();
    return m;
    break;
  default:
    break;
  }

  return m;
}

// warrior_t::matching_gear_multiplier ======================================

double warrior_t::matching_gear_multiplier( attribute_e attr ) const
{
  if ( ( attr == ATTR_STRENGTH ) && ( specialization() != WARRIOR_PROTECTION ) )
    return 0.05;

  if ( ( attr == ATTR_STAMINA ) && ( specialization() == WARRIOR_PROTECTION ) )
    return 0.05;

  return 0.0;
}

// warrior_t::composite_block ==========================================

double warrior_t::composite_block() const
{
  // this handles base block and and all block subject to diminishing returns
  double block_subject_to_dr = cache.mastery() * mastery.critical_block -> effectN( 2 ).mastery_value();
  double b = player_t::composite_block_dr( block_subject_to_dr );

  // add in spec- and perk-specific block bonuses not subject to DR
  b += spec.bastion_of_defense -> effectN( 1 ).percent();
  b += perk.improved_block -> effectN( 1 ).percent();

  if ( buff.shield_block -> up() )
    b += buff.shield_block -> data().effectN( 1 ).percent();

  return b;
}

// warrior_t::composite_melee_attack_power ==================================

double warrior_t::composite_melee_attack_power() const
{
  double ap = player_t::composite_melee_attack_power();

  ap += buff.bladed_armor -> data().effectN( 1 ).percent() * current.stats.get_stat( STAT_BONUS_ARMOR );

  return ap;
}

// warrior_t::composite_parry_rating() ========================================

double warrior_t::composite_parry_rating() const
{
  double p = player_t::composite_parry_rating();

  // add Riposte
  if ( spec.riposte -> ok() )
    p += composite_melee_crit_rating();

  return p;
}

// warrior_t::composite_parry ==========================================

double warrior_t::composite_parry() const
{
  double parry = player_t::composite_parry();

  if ( buff.ravager -> up() )
    parry += talents.ravager -> effectN( 2 ).percent();

  if ( buff.die_by_the_sword -> up() )
    parry += spec.die_by_the_sword -> effectN( 1 ).percent();

  return parry;
}

// warrior_t::composite_attack_power_multiplier ========================

double warrior_t::composite_attack_power_multiplier() const
{
  double ap = player_t::composite_attack_power_multiplier();

  if ( mastery.critical_block -> ok() )
    ap += cache.mastery() * mastery.critical_block -> effectN( 5 ).mastery_value();

  return ap;
}

// warrior_t::composite_crit_block =====================================

double warrior_t::composite_crit_block() const
{
  double b = player_t::composite_crit_block();

  if ( mastery.critical_block -> ok() )
    b += cache.mastery() * mastery.critical_block -> effectN( 1 ).mastery_value();

  return b;
}

// warrior_t::composite_crit_avoidance ===========================================

double warrior_t::composite_crit_avoidance() const
{
  double c = player_t::composite_crit_avoidance();

  if ( active_stance == STANCE_DEFENSE )
    c += spec.unwavering_sentinel -> effectN( 4 ).percent();

  return c;
}

// warrior_t::composite_attack_speed ========================================

double warrior_t::composite_melee_speed() const
{
  double s = player_t::composite_melee_speed();

  s /= 1.0 + buff.tier17_4pc_fury -> current_stack * buff.tier17_4pc_fury -> default_value;

  return s;
}

// warrior_t::composite_melee_crit =========================================

double warrior_t::composite_melee_crit() const
{
  double c = player_t::composite_melee_crit();

  c += buff.tier17_4pc_fury -> current_stack * buff.tier17_4pc_fury -> default_value;

  return c;
}

// warrior_t::composite_spell_crit =========================================

double warrior_t::composite_spell_crit() const
{
  double c = player_t::composite_spell_crit();

  c += buff.tier17_4pc_fury -> current_stack * buff.tier17_4pc_fury -> default_value;

  return c;
}

// warrior_t::temporary_movement_modifier ==================================

double warrior_t::temporary_movement_modifier() const
{
  double temporary = player_t::temporary_movement_modifier();

  if ( buff.heroic_leap_glyph -> up() )
    temporary = std::max( buff.heroic_leap_glyph -> data().effectN( 1 ).percent(), temporary );

  if ( buff.enraged_speed -> up() )
    temporary = std::max( buff.enraged_speed -> data().effectN( 1 ).percent(), temporary );

  return temporary;
}

// warrior_t::passive_movement_modifier===================================

double warrior_t::passive_movement_modifier() const
{
  double ms = player_t::passive_movement_modifier();

  return ms;
}

// warrior_t::invalidate_cache ==============================================

void warrior_t::invalidate_cache( cache_e c )
{
  player_t::invalidate_cache( c );

  if ( c == CACHE_ATTACK_CRIT && mastery.critical_block -> ok() )
    player_t::invalidate_cache( CACHE_PARRY );

  if ( c == CACHE_MASTERY && mastery.critical_block -> ok() )
  {
    player_t::invalidate_cache( CACHE_BLOCK );
    player_t::invalidate_cache( CACHE_CRIT_BLOCK );
    player_t::invalidate_cache( CACHE_ATTACK_POWER );
  }
  if ( c == CACHE_MASTERY && mastery.unshackled_fury -> ok() )
    player_t::invalidate_cache( CACHE_PLAYER_DAMAGE_MULTIPLIER );
}

// warrior_t::primary_role() ================================================

role_e warrior_t::primary_role() const
{
  // Gladiator Stance is selected pre-combat, which means that anyone who imports their character will need to select ROLE_DPS/ATTACK before
  // importing in order for the simulation to catch that they want to dps, not tank.
  if ( specialization() == WARRIOR_PROTECTION || player_t::primary_role() == ROLE_TANK )
  {
    if ( player_t::primary_role() == ROLE_DPS || player_t::primary_role() == ROLE_ATTACK )
      return ROLE_ATTACK;
    else
      return ROLE_TANK;
  }
  return ROLE_ATTACK;
}

// warrior_t::convert_hybrid_stat ==============================================

stat_e warrior_t::convert_hybrid_stat( stat_e s ) const
{
  // this converts hybrid stats that either morph based on spec or only work
  // for certain specs into the appropriate "basic" stats
  switch ( s )
  {
    // This is a guess at how AGI/INT will work for Warriors, TODO: confirm
  case STAT_AGI_INT:
    return STAT_NONE;
  case STAT_STR_AGI:
    return STAT_STRENGTH;
  case STAT_STR_INT:
    return STAT_STRENGTH;
  case STAT_SPIRIT:
    return STAT_NONE;
  case STAT_BONUS_ARMOR:
    if ( specialization() == WARRIOR_PROTECTION )
      return s;
    else
      return STAT_NONE;
  default: return s;
  }
}

// warrior_t::assess_damage =================================================

void warrior_t::assess_damage( school_e school,
                               dmg_e    dtype,
                               action_state_t* s )
{
  if ( s -> result == RESULT_HIT ||
       s -> result == RESULT_CRIT ||
       s -> result == RESULT_GLANCE )
  {
    if ( active_stance == STANCE_DEFENSE )
      s -> result_amount *= 1.0 + ( buff.defensive_stance -> data().effectN( 1 ).percent() +
      talents.gladiators_resolve -> effectN( 2 ).percent() );

    warrior_td_t* td = get_target_data( s -> action -> player );

    if ( td -> debuffs_demoralizing_shout -> up() )
      s -> result_amount *= 1.0 + td -> debuffs_demoralizing_shout -> value();

    //take care of dmg reduction CDs
    if ( buff.shield_wall -> up() )
      s -> result_amount *= 1.0 + buff.shield_wall -> value();

    if ( buff.die_by_the_sword -> up() )
      s -> result_amount *= 1.0 + buff.die_by_the_sword -> default_value;

    if ( s -> block_result == BLOCK_RESULT_CRIT_BLOCKED )
    {
      if ( cooldown.rage_from_crit_block -> up() )
      {
        cooldown.rage_from_crit_block -> start();
        resource_gain( RESOURCE_RAGE,
                       buff.enrage -> data().effectN( 1 ).resource( RESOURCE_RAGE ),
                       gain.critical_block );
        buff.enrage -> trigger();
        buff.enraged_speed -> trigger();
      }
    }
  }

  if ( s -> result == RESULT_DODGE || s -> result == RESULT_PARRY )
    cooldown.revenge -> reset( true );

  if ( s -> result == RESULT_PARRY && buff.die_by_the_sword -> up() && glyphs.drawn_sword )
    player_t::resource_gain( RESOURCE_RAGE,
    glyphs.drawn_sword -> effectN( 1 ).resource( RESOURCE_RAGE ),
    gain.drawn_sword_glyph );

  player_t::assess_damage( school, dtype, s );

  if ( ( s -> result == RESULT_HIT ||
    s -> result == RESULT_CRIT ||
    s -> result == RESULT_GLANCE ) &&
    buff.tier16_reckless_defense -> up() )
  {
    player_t::resource_gain( RESOURCE_RAGE,
                             floor( s -> result_amount / resources.max[RESOURCE_HEALTH] * 100 ),
                             gain.tier16_4pc_tank );
  }

  if ( sets.has_set_bonus( SET_T16_2PC_TANK ) )
  {
    if ( s -> block_result != BLOCK_RESULT_UNBLOCKED ) //heal if blocked
    {
      double heal_amount = floor( s -> blocked_amount * sets.set( SET_T16_2PC_TANK ) -> effectN( 1 ).percent() );
      active_t16_2pc -> base_dd_min = active_t16_2pc -> base_dd_max = heal_amount;
      active_t16_2pc -> execute();
    }

    if ( s -> self_absorb_amount > 0 ) //always heal if shield_barrier absorbed it. This assumes that shield_barrier is our only own absorb spell.
    {
      double heal_amount = floor( s -> self_absorb_amount * sets.set( SET_T16_2PC_TANK ) -> effectN( 2 ).percent() );
      active_t16_2pc -> base_dd_min = active_t16_2pc -> base_dd_max = heal_amount;
      active_t16_2pc -> execute();
    }
  }
}

// warrior_t::create_options ================================================

void warrior_t::create_options()
{
  player_t::create_options();

  option_t warrior_options[] =
  {
    opt_int( "initial_rage", initial_rage ),
    opt_float( "arms_rage_mult", arms_rage_mult ),
    opt_float( "crit_rage_mult", crit_rage_mult ),
    opt_null()
  };

  option_t::copy( options, warrior_options );
}

// Specialized attacks =========================================================

struct warrior_flurry_of_xuen_t: public warrior_attack_t
{
  warrior_flurry_of_xuen_t( warrior_t* p ):
    warrior_attack_t( "flurry_of_xuen", p, p -> find_spell( 147891 ) )
  {
    attack_power_mod.tick = data().extra_coeff();
    proc = false;
    aoe = 5;
    special = may_miss = may_parry = may_block = may_dodge = may_crit = background = true;
  }
};

struct warrior_lightning_strike_t: public warrior_attack_t
{
  warrior_lightning_strike_t( warrior_t* p ):
    warrior_attack_t( "lightning_strike", p, p -> find_spell( 137597 ) )
  {
    background = true;
    may_dodge = may_parry = false;
  }
};

// warrior_t::create_proc_action =============================================

action_t* warrior_t::create_proc_action( const std::string& name )
{
  if ( name == "flurry_of_xuen" ) return new warrior_flurry_of_xuen_t( this );
  if ( name == "lightning_strike" ) return new warrior_lightning_strike_t( this );

  return 0;
}

// warrior_t::create_profile ================================================

bool warrior_t::create_profile( std::string& profile_str, save_e type, bool save_html )
{
  if ( specialization() == WARRIOR_PROTECTION && primary_role() == ROLE_TANK )
    position_str = "front";

  return player_t::create_profile( profile_str, type, save_html );
}

// warrior_t::copy_from =====================================================

void warrior_t::copy_from( player_t* source )
{
  player_t::copy_from( source );

  warrior_t* p = debug_cast<warrior_t*>( source );

  initial_rage = p -> initial_rage;
  arms_rage_mult = p -> arms_rage_mult;
  crit_rage_mult = p -> crit_rage_mult;
}

// warrior_t::decode_set ====================================================

set_e warrior_t::decode_set( const item_t& item ) const
{
  if ( item.slot != SLOT_HEAD      &&
       item.slot != SLOT_SHOULDERS &&
       item.slot != SLOT_CHEST     &&
       item.slot != SLOT_HANDS     &&
       item.slot != SLOT_LEGS )
  {
    return SET_NONE;
  }

  const char* s = item.name();

  if ( strstr( s, "resounding_rings" ) )
  {
    bool is_melee = ( strstr( s, "helmet" ) ||
                      strstr( s, "pauldrons" ) ||
                      strstr( s, "battleplate" ) ||
                      strstr( s, "legplates" ) ||
                      strstr( s, "gauntlets" ) );

    bool is_tank = ( strstr( s, "faceguard" ) ||
                     strstr( s, "shoulderguards" ) ||
                     strstr( s, "chestguard" ) ||
                     strstr( s, "legguards" ) ||
                     strstr( s, "handguards" ) );

    if ( is_melee ) return SET_T14_MELEE;
    if ( is_tank ) return SET_T14_TANK;
  }

  if ( strstr( s, "last_mogu" ) )
  {
    bool is_melee = ( strstr( s, "helmet" ) ||
                      strstr( s, "pauldrons" ) ||
                      strstr( s, "battleplate" ) ||
                      strstr( s, "legplates" ) ||
                      strstr( s, "gauntlets" ) );

    bool is_tank = ( strstr( s, "faceguard" ) ||
                     strstr( s, "shoulderguards" ) ||
                     strstr( s, "chestguard" ) ||
                     strstr( s, "legguards" ) ||
                     strstr( s, "handguards" ) );

    if ( is_melee ) return SET_T15_MELEE;
    if ( is_tank ) return SET_T15_TANK;
  }

  if ( strstr( s, "prehistoric_marauder" ) )
  {
    bool is_melee = ( strstr( s, "helmet" ) ||
                      strstr( s, "pauldrons" ) ||
                      strstr( s, "battleplate" ) ||
                      strstr( s, "legplates" ) ||
                      strstr( s, "gauntlets" ) );

    bool is_tank = ( strstr( s, "faceguard" ) ||
                     strstr( s, "shoulderguards" ) ||
                     strstr( s, "chestguard" ) ||
                     strstr( s, "legguards" ) ||
                     strstr( s, "handguards" ) );

    if ( is_melee ) return SET_T16_MELEE;
    if ( is_tank ) return SET_T16_TANK;
  }

  if ( strstr( s, "_gladiators_plate_" ) ) return SET_PVP_MELEE;

  return SET_NONE;
}

void warrior_t::stance_swap()
{
  // Blizzard has automated stance swapping with defensive and battle stance. This class will swap to the stance automatically if
  // The ability that we are trying to use is not usable in the current stance.
  // Currently it is not possible to swap in/out of gladiator stance, so there's no need to model it.
  warrior_stance swap;
  switch ( active_stance )
  {
  case STANCE_BATTLE:
  {
    buff.battle_stance -> expire();
    swap = STANCE_DEFENSE;
    break;
  }
  case STANCE_DEFENSE:
  {
    buff.defensive_stance -> expire();
    swap = STANCE_BATTLE;
    recalculate_resource_max( RESOURCE_HEALTH );
    break;
  }
  case STANCE_GLADIATOR:
  {
    buff.gladiator_stance -> expire();
    swap = STANCE_GLADIATOR;
    break;
  }
  default:
    swap = active_stance;
    break;
  }

  switch ( swap )
  {
  case STANCE_BATTLE: buff.battle_stance -> trigger(); break;
  case STANCE_DEFENSE:
  {
    buff.defensive_stance -> trigger();
    recalculate_resource_max( RESOURCE_HEALTH );
    break;
  }
  case STANCE_GLADIATOR:
  {
    buff.gladiator_stance -> trigger();
    break;
  }
  }
  cooldown.stance_swap -> start();
  if ( active_stance == STANCE_DEFENSE )
    cooldown.stance_cooldown -> start();
}

// warrior_t::enrage ========================================================

void warrior_t::enrage()
{
  // Crit CS/Block give rage, and refresh enrage
  // Additionally, BT crits grant 1 charge of Raging Blow

  if ( specialization() == WARRIOR_FURY )
  {
    if ( buff.raging_blow -> stack() == 2 )
      proc.raging_blow_wasted -> occur();

    buff.raging_blow -> trigger();
  }
  resource_gain( RESOURCE_RAGE,
                 buff.enrage -> data().effectN( 1 ).resource( RESOURCE_RAGE ),
                 gain.enrage );
  buff.enrage -> trigger();
  buff.enraged_speed -> trigger();
}

/* Report Extension Class
 * Here you can define class specific report extensions/overrides
 */
class warrior_report_t: public player_report_extension_t
{
public:
  warrior_report_t( warrior_t& player ):
    p( player )
  {}

  virtual void html_customsection( report::sc_html_stream& os ) override
  {
    double cs_damage = p.cs_damage.sum();
    double all_damage = p.all_damage.sum();
    double priority_damage = p.priority_damage.sum();

    // Custom Class Section
    os << "\t\t\t\t<div class=\"player-section custom_section\">\n"
      << "\t\t\t\t\t<h3 class=\"toggle open\">Custom Section</h3>\n"
      << "\t\t\t\t\t<div class=\"toggle-content\">\n";

    os << p.name() << "\n<br>";
    os << "\t\t\t\t\t<p>Percentage of damage dealt to primary target</p>\n";
    os << "%" << ( ( priority_damage / all_damage ) * 100 ) << "</p>\n";
    if ( p.specialization() != WARRIOR_PROTECTION )
    {
      os << "\t\t\t\t\t<p>Percentage of primary target damage that occurs inside of Colossus Smash</p>\n";
      os << "%" << ( ( cs_damage / priority_damage ) * 100 ) << "</p>\n";
    }
    os << "\t\t\t\t\t<p> Dps done to primary target </p>\n";
    os << ( ( priority_damage / all_damage ) * p.collected_data.dps.mean() ) << "</p>\n";

    os << "\t\t\t\t\t\t</div>\n" << "\t\t\t\t\t</div>\n";
  }
private:
  warrior_t& p;
};

// WARRIOR MODULE INTERFACE =================================================

struct warrior_module_t: public module_t
{
  warrior_module_t(): module_t( WARRIOR ) {}

  virtual player_t* create_player( sim_t* sim, const std::string& name, race_e r = RACE_NONE ) const
  {
    warrior_t* p = new warrior_t( sim, name, r );
    p -> report_extension = std::shared_ptr<player_report_extension_t>( new warrior_report_t( *p ) );
    return p;
  }

  virtual bool valid() const { return true; }

  virtual void init( sim_t* /* sim */ ) const
  {
    /*
    for ( size_t i = 0; i < sim -> actor_list.size(); i++ )
    {
    player_t* p = sim -> actor_list[i];
    }*/
  }

  virtual void combat_begin( sim_t* ) const {}

  virtual void combat_end( sim_t* ) const {}
};

} // UNNAMED NAMESPACE

const module_t* module_t::warrior()
{
  static warrior_module_t m;
  return &m;
}
