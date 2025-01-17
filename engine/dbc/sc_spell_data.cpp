// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.hpp"

namespace { // anonymous namespace ==========================================

enum sdata_field_type_t
{
  SD_TYPE_INT = 0,
  SD_TYPE_UNSIGNED,
  SD_TYPE_DOUBLE,
  SD_TYPE_STR
};

struct sdata_field_t
{
  sdata_field_type_t type;
  std::string        name;
};

const sdata_field_t _talent_data_fields[] =
{
  { SD_TYPE_STR,      "name"          },
  { SD_TYPE_UNSIGNED, "id",           },
  { SD_TYPE_UNSIGNED, "flags"         },
  { SD_TYPE_UNSIGNED, "tab"           },
  { SD_TYPE_UNSIGNED, ""              }, // Class (spell_class_expr_t)
  { SD_TYPE_UNSIGNED, ""              }, // Spec
  { SD_TYPE_UNSIGNED, "col"           },
  { SD_TYPE_UNSIGNED, "row"           },
  { SD_TYPE_UNSIGNED, ""              }, // Talent rank spell ids, unused for now
  { SD_TYPE_UNSIGNED, ""              },
  { SD_TYPE_UNSIGNED, ""              },
};

const sdata_field_t _set_bonus_data_fields[] =
{
  { SD_TYPE_STR,      "name"          },
  { SD_TYPE_UNSIGNED, "tier",         },
  { SD_TYPE_UNSIGNED, "bonus"         },
  { SD_TYPE_UNSIGNED, "class_id"      },
  { SD_TYPE_UNSIGNED, ""              },
  { SD_TYPE_UNSIGNED, ""              },
  { SD_TYPE_UNSIGNED, ""              },
  { SD_TYPE_UNSIGNED, ""              },
  { SD_TYPE_UNSIGNED, "spec"          },
  { SD_TYPE_UNSIGNED, "spell_id"      },
};

const sdata_field_t _effect_data_fields[] =
{
  { SD_TYPE_UNSIGNED, "id"             },
  { SD_TYPE_UNSIGNED, "flags"          },
  { SD_TYPE_UNSIGNED, "spell_id"       },
  { SD_TYPE_UNSIGNED, "index"          },
  { SD_TYPE_INT,      "type"           },
  { SD_TYPE_INT,      "sub_type"       },
  { SD_TYPE_DOUBLE,   "m_average"      },
  { SD_TYPE_DOUBLE,   "m_delta"        },
  { SD_TYPE_DOUBLE,   "m_bonus"        },
  { SD_TYPE_DOUBLE,   "coefficient"    },
  { SD_TYPE_DOUBLE,   "ap_coefficient" },
  { SD_TYPE_DOUBLE,   "amplitude"      },
  { SD_TYPE_DOUBLE,   "radius"         },
  { SD_TYPE_DOUBLE,   "max_radius"     },
  { SD_TYPE_INT,      "base_value"     },
  { SD_TYPE_INT,      "misc_value"     },
  { SD_TYPE_INT,      "misc_value2"    },
  { SD_TYPE_UNSIGNED, ""               }, // Family flags 1
  { SD_TYPE_UNSIGNED, ""               }, // Family flags 2
  { SD_TYPE_UNSIGNED, ""               }, // Family flags 3
  { SD_TYPE_UNSIGNED, ""               }, // Family flags 4
  { SD_TYPE_INT,      "trigger_spell"  },
  { SD_TYPE_DOUBLE,   "m_chain"        },
  { SD_TYPE_DOUBLE,   "p_combo_points" },
  { SD_TYPE_DOUBLE,   "p_level"        },
  { SD_TYPE_INT,      "damage_range"   },
};

const sdata_field_t _spell_data_fields[] =
{
  { SD_TYPE_STR,      "name"          },
  { SD_TYPE_UNSIGNED, "id",           },
  { SD_TYPE_UNSIGNED, "flags"         },
  { SD_TYPE_DOUBLE,   "speed"         },
  { SD_TYPE_UNSIGNED, ""              }, // School, requires custom thing
  { SD_TYPE_UNSIGNED, ""              }, // Class (spell_class_expr_t)
  { SD_TYPE_UNSIGNED, ""              }, // Race (spell_race_expr_t)
  { SD_TYPE_INT,      "scaling"       },
  { SD_TYPE_UNSIGNED, "max_scaling_level" },
  { SD_TYPE_UNSIGNED, "level"         },
  { SD_TYPE_UNSIGNED, "max_level"     },
  { SD_TYPE_DOUBLE,   "min_range"     },
  { SD_TYPE_DOUBLE,   "max_range"     },
  { SD_TYPE_UNSIGNED, "cooldown"      },
  { SD_TYPE_UNSIGNED, "gcd"           },
  { SD_TYPE_UNSIGNED, "category"      },
  { SD_TYPE_DOUBLE,   "duration"      },
  { SD_TYPE_UNSIGNED, ""              }, // Runes (spell_rune_expr_t)
  { SD_TYPE_UNSIGNED, "power_gain"    },
  { SD_TYPE_UNSIGNED, "max_stack"     },
  { SD_TYPE_UNSIGNED, "proc_chance"   },
  { SD_TYPE_UNSIGNED, "initial_stack" },
  { SD_TYPE_UNSIGNED, ""              }, // Proc flags, no support for now
  { SD_TYPE_UNSIGNED, "icd"           },
  { SD_TYPE_DOUBLE,   "rppm"          },
  { SD_TYPE_UNSIGNED, "equip_class"   },
  { SD_TYPE_UNSIGNED, "equip_imask"   },
  { SD_TYPE_UNSIGNED, "equip_scmask"  },
  { SD_TYPE_INT,      "cast_min"      },
  { SD_TYPE_INT,      "cast_max"      },
  { SD_TYPE_INT,      "cast_div"      },
  { SD_TYPE_DOUBLE,   "m_scaling"     },
  { SD_TYPE_UNSIGNED, "scaling_level" },
  { SD_TYPE_UNSIGNED, "replace_spellid" },
  { SD_TYPE_UNSIGNED, ""              }, // Attributes, 0..11, not done for now
  { SD_TYPE_UNSIGNED, ""              },
  { SD_TYPE_UNSIGNED, ""              },
  { SD_TYPE_UNSIGNED, ""              },
  { SD_TYPE_UNSIGNED, ""              },
  { SD_TYPE_UNSIGNED, ""              },
  { SD_TYPE_UNSIGNED, ""              },
  { SD_TYPE_UNSIGNED, ""              },
  { SD_TYPE_UNSIGNED, ""              },
  { SD_TYPE_UNSIGNED, ""              },
  { SD_TYPE_UNSIGNED, ""              },
  { SD_TYPE_UNSIGNED, ""              },
  { SD_TYPE_UNSIGNED, ""              }, // Family flags 1
  { SD_TYPE_UNSIGNED, ""              }, // Family flags 1
  { SD_TYPE_UNSIGNED, ""              }, // Family flags 1
  { SD_TYPE_UNSIGNED, ""              }, // Family flags 1
  { SD_TYPE_UNSIGNED, "family"        }, // Family
  { SD_TYPE_STR,      "desc"          },
  { SD_TYPE_STR,      "tooltip"       },
  { SD_TYPE_STR,      "desc_vars"     },
  { SD_TYPE_STR,      "icon"          },
  { SD_TYPE_STR,      "active_icon"   },
  { SD_TYPE_STR,      "rank"          },
};

const std::string _class_strings[] =
{
  "",
  "warrior",
  "paladin",
  "hunter",
  "rogue",
  "priest",
  "deathknight",
  "shaman",
  "mage",
  "warlock",
  "monk",
  "druid"
};

const std::string _race_strings[] =
{
  "",
  "human",
  "orc",
  "dwarf",
  "night_elf",
  "undead",
  "tauren",
  "gnome",
  "troll",
  "goblin",
  "blood_elf",
  "draenei",
  "", "", "", "", "", "", "", "", "", "",
  "worgen",
  "pandaren",
  "pandaren_alliance",
  "pandaren_horde"
};

const std::string _pet_class_strings[] =
{
  "",
  "cunning",
  "ferocity",
  "tenacity",
};

const struct expr_data_map_t
{
  std::string name;
  expr_data_e type;
} map[] =
{
  { "spell", DATA_SPELL },
  { "talent", DATA_TALENT },
  { "effect", DATA_EFFECT },
  { "talent_spell", DATA_TALENT_SPELL },
  { "class_spell", DATA_CLASS_SPELL },
  { "race_spell", DATA_RACIAL_SPELL },
  { "mastery", DATA_MASTERY_SPELL },
  { "spec_spell", DATA_SPECIALIZATION_SPELL },
  { "glyph", DATA_GLYPH_SPELL },
  { "set_bonus_spell", DATA_SET_BONUS_SPELL },
  { "perk_spell", DATA_PERK_SPELL },
  { "set_bonus", DATA_SET_BONUS },
};

expr_data_e parse_data_type( const std::string& name )
{
  for ( int i = 0; i < static_cast<int>( sizeof_array( map ) ); ++i )
  {
    if ( util::str_compare_ci( map[ i ].name, name ) )
      return map[ i ].type;
  }

  return ( expr_data_e ) - 1;
}

unsigned class_str_to_mask( const std::string& str )
{
  int cls_id = -1;

  for ( unsigned int i = 0; i < sizeof( _class_strings ) / sizeof( std::string ); i++ )
  {
    if ( _class_strings[ i ].empty() )
      continue;

    if ( ! util::str_compare_ci( _class_strings[ i ], str ) )
      continue;

    cls_id = i;
    break;
  }

  return 1 << ( ( cls_id < 1 ) ? 0 : cls_id - 1 );
}

unsigned race_str_to_mask( const std::string& str )
{
  int race_id = -1;

  for ( unsigned int i = 0; i < sizeof( _race_strings ) / sizeof( std::string ); i++ )
  {
    if ( _race_strings[ i ].empty() )
      continue;

    if ( ! util::str_compare_ci( _race_strings[ i ], str ) )
      continue;

    race_id = i;
    break;
  }

  return 1 << ( ( race_id < 1 ) ? 0 : race_id - 1 );
}

unsigned school_str_to_mask( const std::string& str )
{
  unsigned mask = 0;

  if ( util::str_in_str_ci( str, "physical" ) || util::str_in_str_ci( str, "strike" ) )
    mask |= 0x1;

  if ( util::str_in_str_ci( str, "holy" ) || util::str_in_str_ci( str, "light" ) )
    mask |= 0x2;

  if ( util::str_in_str_ci( str, "fire" ) || util::str_in_str_ci( str, "flame" ) )
    mask |= 0x4;

  if ( util::str_in_str_ci( str, "nature" ) || util::str_in_str_ci( str, "storm" ) )
    mask |= 0x8;

  if ( util::str_in_str_ci( str, "frost" ) )
    mask |= 0x10;

  if ( util::str_in_str_ci( str, "shadow" ) )
    mask |= 0x20;

  if ( util::str_in_str_ci( str, "arcane" ) || util::str_in_str_ci( str, "spell" ) )
    mask |= 0x40;

  // Special case: Arcane + Holy
  if ( util::str_compare_ci( "divine", str ) )
    mask = 0x42;

  return mask;
}

// Generic spell list based expression, holds intersection, union for list
// For these expression types, you can only use two spell lists as parameters
struct spell_list_expr_t : public spell_data_expr_t
{
  spell_list_expr_t( sim_t* sim, const std::string& name, expr_data_e type = DATA_SPELL, bool eq = false ) :
    spell_data_expr_t( sim, name, type, eq, TOK_SPELL_LIST ) { }

  virtual int evaluate()
  {
    unsigned spell_id;

    // Based on the data type, see what list of spell ids we should handle, and populate the
    // result_spell_list accordingly
    switch ( data_type )
    {
      case DATA_SPELL:
      {
        for ( const spell_data_t* spell = spell_data_t::list( sim -> dbc.ptr ); spell -> id(); spell++ )
          result_spell_list.push_back( spell -> id() );
        break;
      }
      case DATA_TALENT:
      {
        for ( const talent_data_t* talent = talent_data_t::list( sim -> dbc.ptr ); talent -> id(); talent++ )
          result_spell_list.push_back( talent -> id() );
        break;
      }
      case DATA_EFFECT:
      {
        for ( const spelleffect_data_t* effect = spelleffect_data_t::list( sim -> dbc.ptr ); effect -> id(); effect++ )
          result_spell_list.push_back( effect -> id() );
        break;
      }
      case DATA_TALENT_SPELL:
      {
        for ( const talent_data_t* talent = talent_data_t::list( sim -> dbc.ptr ); talent -> id(); talent++ )
        {
          if ( ! talent -> spell_id() )
            continue;
          result_spell_list.push_back( talent -> spell_id() );
        }
        break;
      }
      case DATA_CLASS_SPELL:
      {
        for ( unsigned cls = 0; cls < sim -> dbc.specialization_max_class(); cls++ )
        {
          for ( unsigned tree = 0; tree < sim -> dbc.class_ability_tree_size(); tree++ )
          {
            for ( unsigned n = 0; n < sim -> dbc.class_ability_size(); n++ )
            {
              if ( ! ( spell_id = sim -> dbc.class_ability( cls, tree, n ) ) )
                continue;

              result_spell_list.push_back( spell_id );
            }
          }
        }
        break;
      }
      case DATA_RACIAL_SPELL:
      {
        for ( unsigned race = 0; race < sim -> dbc.race_ability_tree_size(); race++ )
        {
          for ( unsigned cls = 0; cls < sim -> dbc.specialization_max_class() - 1; cls++ )
          {
            for ( unsigned n = 0; n < sim -> dbc.race_ability_size(); n++ )
            {
              if ( ! ( spell_id = sim -> dbc.race_ability( race, cls, n ) ) )
                continue;

              result_spell_list.push_back( spell_id );
            }
          }
        }
        break;
      }
      case DATA_MASTERY_SPELL:
      {
        for ( unsigned cls = 0; cls < sim -> dbc.specialization_max_class(); cls++ )
        {
          for ( unsigned tree = 0; tree < sim -> dbc.specialization_max_per_class(); tree++ )
          {
            for ( unsigned n = 0; n < sim -> dbc.mastery_ability_size(); n++ )
            {
              if ( ! ( spell_id = sim -> dbc.mastery_ability( cls, tree, n ) ) )
                continue;

              result_spell_list.push_back( spell_id );
            }
          }
        }
        break;
      }
      case DATA_SPECIALIZATION_SPELL:
      {
        for ( unsigned cls = 0; cls < sim -> dbc.specialization_max_class(); cls++ )
        {
          for ( unsigned tree = 0; tree < sim -> dbc.specialization_max_per_class(); tree++ )
          {
            for ( unsigned n = 0; n < sim -> dbc.specialization_ability_size(); n++ )
            {
              if ( ! ( spell_id = sim -> dbc.specialization_ability( cls, tree, n ) ) )
                continue;

              result_spell_list.push_back( spell_id );
            }
          }
        }
        break;
      }
      case DATA_GLYPH_SPELL:
      {
        for ( unsigned cls = 0; cls < sim -> dbc.specialization_max_class(); cls++ )
        {
          for ( unsigned type = 0; type < GLYPH_MAX; type++ )
          {
            for ( unsigned n = 0; n < sim -> dbc.glyph_spell_size(); n++ )
            {
              if ( ! ( spell_id = sim -> dbc.glyph_spell( cls, type, n ) ) )
                continue;

              result_spell_list.push_back( spell_id );
            }
          }
        }
        break;
      }
      case DATA_SET_BONUS_SPELL:
      {
        for ( unsigned cls = 0; cls < sim -> dbc.specialization_max_class(); cls++ )
        {
          for ( unsigned tier = 0; tier < N_TIER; tier++ )
          {
            for ( unsigned n = 0; n < sim -> dbc.set_bonus_spell_size(); n++ )
            {
              if ( ! ( spell_id = sim -> dbc.set_bonus_spell( cls, tier, n ) ) )
                continue;

              result_spell_list.push_back( spell_id );
            }
          }
        }
        break;
      }

      case DATA_PERK_SPELL:
      {
        for ( unsigned cls = 0; cls < sim -> dbc.specialization_max_class(); cls++ )
        {
          for ( unsigned tree = 0; tree < sim -> dbc.specialization_max_per_class(); tree++ )
          {
          for ( unsigned n = 0; n < sim -> dbc.perk_ability_size(); n++ )
          {
              if ( ! ( spell_id = sim -> dbc.perk_ability( cls, tree, n ) ) )
                continue;

              result_spell_list.push_back( spell_id );
            }
          }
        }
        break;
      }
      case DATA_SET_BONUS:
      {
        for ( size_t i = 0, max = dbc::n_set_bonus( sim -> dbc.ptr ); i < max; ++i )
        {
          result_spell_list.push_back( (int)i ); // back: just insert the array index
        }
        break;
      }
      default:
        return TOK_UNKNOWN;
    }

    result_spell_list.resize( range::unique( range::sort( result_spell_list ) ) - result_spell_list.begin() );

    return TOK_SPELL_LIST;
  }

  // Intersect two spell lists
  virtual std::vector<uint32_t> operator&( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;

    // Only or two spell lists together
    if ( other.result_tok != TOK_SPELL_LIST )
    {
      sim -> errorf( "Unsupported right side operand '%s' (%d) for operator &",
                     other.name_str.c_str(),
                     other.result_tok );
    }
    else
      range::set_intersection( result_spell_list, other.result_spell_list, std::back_inserter( res ) );

    return res;
  }

  // Merge two spell lists, uniqueing entries
  virtual std::vector<uint32_t> operator|( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;

    // Only or two spell lists together
    if ( other.result_tok != TOK_SPELL_LIST )
    {
      sim -> errorf( "Unsupported right side operand '%s' (%d) for operator |",
                     other.name_str.c_str(),
                     other.result_tok );
    }
    else
      range::set_union( result_spell_list, other.result_spell_list, std::back_inserter( res ) );

    return res;
  }

  // Subtract two spell lists, other from this
  virtual std::vector<uint32_t>operator-( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;

    // Only or two spell lists together
    if ( other.result_tok != TOK_SPELL_LIST )
    {
      sim -> errorf( "Unsupported right side operand '%s' (%d) for operator -",
                     other.name_str.c_str(),
                     other.result_tok );
    }
    else
      range::set_difference( result_spell_list, other.result_spell_list, std::back_inserter( res ) );

    return res;
  }
};

struct sd_expr_binary_t : public spell_list_expr_t
{
  int                operation;
  spell_data_expr_t* left;
  spell_data_expr_t* right;

  sd_expr_binary_t( sim_t* sim, const std::string& n, int o, spell_data_expr_t* l, spell_data_expr_t* r ) :
    spell_list_expr_t( sim, n ), operation( o ), left( l ), right( r ) { }

  virtual int evaluate()
  {
    int  left_result =  left -> evaluate();

    right -> evaluate();
    result_tok      = TOK_UNKNOWN;

    if ( left_result != TOK_SPELL_LIST )
    {
      sim -> errorf( "Inconsistent input types (%s and %s) for binary operator '%s', left must always be a spell list.\n",
                     left -> name(), right -> name(), name() );
      sim -> cancel();
    }
    else
    {
      result_tok = TOK_SPELL_LIST;
      // Data type follows from left side operand
      data_type   = left -> data_type;

      switch ( operation )
      {
        case TOK_EQ:    result_spell_list = *left == *right; break;
        case TOK_NOTEQ: result_spell_list = *left != *right; break;
        case TOK_OR:    result_spell_list = *left | *right; break;
        case TOK_AND:   result_spell_list = *left & *right; break;
        case TOK_SUB:   result_spell_list = *left - *right; break;
        case TOK_LT:    result_spell_list = *left < *right; break;
        case TOK_LTEQ:  result_spell_list = *left <= *right; break;
        case TOK_GT:    result_spell_list = *left > *right; break;
        case TOK_GTEQ:  result_spell_list = *left >= *right; break;
        case TOK_IN:    result_spell_list = left -> in( *right ); break;
        case TOK_NOTIN: result_spell_list = left -> not_in( *right ); break;
        default:
          sim -> errorf( "Unsupported spell query operator %d", operation );
          result_spell_list = std::vector<uint32_t>();
          result_tok = TOK_UNKNOWN;
          break;
      }
    }

    return result_tok;
  }
};

struct spell_data_filter_expr_t : public spell_list_expr_t
{
  int                offset;
  sdata_field_type_t field_type;

  spell_data_filter_expr_t( sim_t* sim, expr_data_e type, const std::string& f_name, bool eq = false ) :
    spell_list_expr_t( sim, f_name, type, eq ), offset( 0 ), field_type( SD_TYPE_INT )
  {
    const sdata_field_t      * fields = 0;
    unsigned             fsize;
    if ( type == DATA_TALENT )
    {
      fields = _talent_data_fields;
      fsize  = sizeof( _talent_data_fields );
    }
    else if ( type == DATA_SET_BONUS )
    {
      fields = _set_bonus_data_fields;
      fsize  = sizeof( _set_bonus_data_fields );
    }
    else if ( effect_query || type == DATA_EFFECT )
    {
      fields = _effect_data_fields;
      fsize  = sizeof( _effect_data_fields );
    }
    else
    {
      fields = _spell_data_fields;
      fsize  = sizeof( _spell_data_fields );
    }

    // Figure out our offset then
    for ( unsigned int i = 0; i < fsize / sizeof( sdata_field_t ); i++ )
    {
      if ( fields[ i ].name.empty() || ! util::str_compare_ci( f_name, fields[ i ].name ) )
      {
        switch ( fields[ i ].type )
        {
          case SD_TYPE_INT:
          case SD_TYPE_UNSIGNED:
            offset += sizeof( int );
            break;
          case SD_TYPE_DOUBLE:
            offset += sizeof( double );
            break;
          case SD_TYPE_STR:
            offset += sizeof( const char* );
            break;
          default:
            sim -> errorf( "Unknown field type %d for %s.",
                           fields[ i ].type,
                           fields[ i ].name.c_str() );
            break;
        }

        continue;
      }

      field_type = fields[ i ].type;
      break;
    }
  }

  virtual bool compare( const char* data, const spell_data_expr_t& other, token_e t ) const
  {
    switch ( field_type )
    {
      case SD_TYPE_INT:
      {
        int int_v  = *reinterpret_cast< const int* >( data + offset );
        int oint_v = static_cast<int>( other.result_num );
        switch ( t )
        {
          case TOK_LT:     return int_v <  oint_v;
          case TOK_LTEQ:   return int_v <= oint_v;
          case TOK_GT:     return int_v >  oint_v;
          case TOK_GTEQ:   return int_v >= oint_v;
          case TOK_EQ:     return int_v == oint_v;
          case TOK_NOTEQ:  return int_v != oint_v;
          default:         return false;
        }
        break;
      }
      case SD_TYPE_UNSIGNED:
      {
        unsigned unsigned_v  = *reinterpret_cast< const unsigned* >( data + offset );
        unsigned ounsigned_v = static_cast<unsigned>( other.result_num );
        switch ( t )
        {
          case TOK_LT:     return unsigned_v <  ounsigned_v;
          case TOK_LTEQ:   return unsigned_v <= ounsigned_v;
          case TOK_GT:     return unsigned_v >  ounsigned_v;
          case TOK_GTEQ:   return unsigned_v >= ounsigned_v;
          case TOK_EQ:     return unsigned_v == ounsigned_v;
          case TOK_NOTEQ:  return unsigned_v != ounsigned_v;
          default:         return false;
        }
        break;
      }
      case SD_TYPE_DOUBLE:
      {
        double double_v = *reinterpret_cast<const double*>( data + offset );
        switch ( t )
        {
          case TOK_LT:     return double_v <  other.result_num;
          case TOK_LTEQ:   return double_v <= other.result_num;
          case TOK_GT:     return double_v >  other.result_num;
          case TOK_GTEQ:   return double_v >= other.result_num;
          case TOK_EQ:     return double_v == other.result_num;
          case TOK_NOTEQ:  return double_v != other.result_num;
          default:         return false;
        }
        break;
      }
      case SD_TYPE_STR:
      {
        const char* c_str = *reinterpret_cast<const char * const*>( data + offset );
        std::string string_v = c_str ? c_str : "";
        util::tokenize( string_v );
        const std::string& ostring_v = other.result_str;

        switch ( t )
        {
          case TOK_EQ:    return util::str_compare_ci( string_v, ostring_v );
          case TOK_NOTEQ: return ! util::str_compare_ci( string_v, ostring_v );
          case TOK_IN:    return util::str_in_str_ci( string_v, ostring_v );
          case TOK_NOTIN: return ! util::str_in_str_ci( string_v, ostring_v );
          default:        return false;
        }
        break;
      }
      default:
        break;
    }
    return false;
  }

  void build_list( std::vector<uint32_t>& res, const spell_data_expr_t& other, token_e t ) const
  {
    for ( std::vector<uint32_t>::const_iterator i = result_spell_list.begin(); i != result_spell_list.end(); ++i )
    {
      // Don't bother comparing if this spell id is already in the result set.
      if ( range::find( res, *i ) != res.end() )
        continue;

      if ( effect_query )
      {
        const spell_data_t& spell = *sim -> dbc.spell( *i );

        // Compare against every spell effect
        for ( size_t j = 0; j < spell.effect_count(); j++ )
        {
          const spelleffect_data_t& effect = spell.effectN( j + 1 );

          if ( effect.id() > 0 && sim -> dbc.effect( effect.id() ) &&
               compare( reinterpret_cast<const char*>( &effect ), other, t ) )
          {
            res.push_back( *i );
            break;
          }
        }
      }
      else
      {
        const char* p_data;
        if ( data_type == DATA_TALENT )
          p_data = reinterpret_cast<const char*>( sim -> dbc.talent( *i ) );
        if ( data_type == DATA_SET_BONUS )
          p_data = reinterpret_cast<const char*>( (dbc::set_bonus( sim -> dbc.ptr ) + *i) );
        else if ( data_type == DATA_EFFECT )
          p_data = reinterpret_cast<const char*>( sim -> dbc.effect( *i ) );
        else
          p_data = reinterpret_cast<const char*>( sim -> dbc.spell( *i ) );
        if ( p_data && compare( p_data, other, t ) )
          res.push_back( *i );
      }
    }
  }

  virtual std::vector<uint32_t> operator==( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;

    if ( other.result_tok != TOK_NUM && other.result_tok != TOK_STR )
    {
      sim -> errorf( "Unsupported expression operator == for left=%s(%d), right=%s(%d)",
                     name_str.c_str(),
                     result_tok,
                     other.name_str.c_str(),
                     other.result_tok );
    }
    else
      build_list( res, other, TOK_EQ );

    return res;
  }

  virtual std::vector<uint32_t> operator!=( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;

    if ( other.result_tok != TOK_NUM && other.result_tok != TOK_STR )
    {
      sim -> errorf( "Unsupported expression operator != for left=%s(%d), right=%s(%d)",
                     name_str.c_str(),
                     result_tok,
                     other.name_str.c_str(),
                     other.result_tok );
    }
    else
      build_list( res, other, TOK_NOTEQ );

    return res;
  }

  virtual std::vector<uint32_t> operator<( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;

    if ( other.result_tok != TOK_NUM ||
         ( field_type != SD_TYPE_INT && field_type != SD_TYPE_UNSIGNED && field_type != SD_TYPE_DOUBLE )  )
    {
      sim -> errorf( "Unsupported expression operator < for left=%s(%d), right=%s(%d) or field '%s' is not a number",
                     name_str.c_str(),
                     result_tok,
                     other.name_str.c_str(),
                     other.result_tok,
                     name_str.c_str() );
    }
    else
      build_list( res, other, TOK_LT );

    return res;
  }

  virtual std::vector<uint32_t> operator<=( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;

    if ( other.result_tok != TOK_NUM ||
         ( field_type != SD_TYPE_INT && field_type != SD_TYPE_UNSIGNED && field_type != SD_TYPE_DOUBLE )  )
    {
      sim -> errorf( "Unsupported expression operator <= for left=%s(%d), right=%s(%d) or field '%s' is not a number",
                     name_str.c_str(),
                     result_tok,
                     other.name_str.c_str(),
                     other.result_tok,
                     name_str.c_str() );
    }
    else
      build_list( res, other, TOK_LTEQ );

    return res;
  }

  virtual std::vector<uint32_t> operator>( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;

    if ( other.result_tok != TOK_NUM ||
         ( field_type != SD_TYPE_INT && field_type != SD_TYPE_UNSIGNED && field_type != SD_TYPE_DOUBLE )  )
    {
      sim -> errorf( "Unsupported expression operator > for left=%s(%d), right=%s(%d) or field '%s' is not a number",
                     name_str.c_str(),
                     result_tok,
                     other.name_str.c_str(),
                     other.result_tok,
                     name_str.c_str() );
    }
    else
      build_list( res, other, TOK_GT );

    return res;
  }

  virtual std::vector<uint32_t> operator>=( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;

    if ( other.result_tok != TOK_NUM ||
         ( field_type != SD_TYPE_INT && field_type != SD_TYPE_UNSIGNED && field_type != SD_TYPE_DOUBLE )  )
    {
      sim -> errorf( "Unsupported expression operator >= for left=%s(%d), right=%s(%d) or field '%s' is not a number",
                     name_str.c_str(),
                     result_tok,
                     other.name_str.c_str(),
                     other.result_tok,
                     name_str.c_str() );
    }
    else
      build_list( res, other, TOK_GTEQ );

    return res;
  }

  virtual std::vector<uint32_t> in( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;

    if ( other.result_tok != TOK_STR || field_type != SD_TYPE_STR )
    {
      sim -> errorf( "Unsupported expression operator ~ for left=%s(%d), right=%s(%d) or field '%s' is not a string",
                     name_str.c_str(),
                     result_tok,
                     other.name_str.c_str(),
                     other.result_tok,
                     name_str.c_str() );
    }
    else
      build_list( res, other, TOK_IN );

    return res;
  }

  virtual std::vector<uint32_t> not_in( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;

    if ( other.result_tok != TOK_STR || field_type != SD_TYPE_STR )
    {
      sim -> errorf( "Unsupported expression operator !~ for left=%s(%d), right=%s(%d) or field '%s' is not a string",
                     name_str.c_str(),
                     result_tok,
                     other.name_str.c_str(),
                     other.result_tok,
                     name_str.c_str() );
    }
    else
      build_list( res, other, TOK_NOTIN );

    return res;
  }
};

struct spell_rune_expr_t : public spell_list_expr_t
{
  static unsigned rune_cost( const std::string& s )
  {
    const char _runes[] = { 'b', 'u', 'f', 'd' };
    int n_runes[]       = { 0, 0, 0, 0 };
    unsigned rune_mask  = 0;

    for ( unsigned int i = 0; i < s.size(); i++ )
    {
      for ( unsigned int j = 0; j < 4; j++ )
      {
        if ( s[ i ] == _runes[ j ] )
          n_runes[ j ]++;
      }
    }

    for ( unsigned int i = 0; i < 4; i++ )
    {
      for ( int j = 0; j < std::min( 2, n_runes[ i ] ); j++ )
        rune_mask |= ( 1 << ( i * 2 + j ) );
    }

    return rune_mask;
  }

  spell_rune_expr_t( sim_t* sim, expr_data_e type ) : spell_list_expr_t( sim, "rune", type ) { }

  virtual std::vector<uint32_t> operator==( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;
    unsigned              r = rune_cost( other.result_str );

    for ( std::vector<uint32_t>::const_iterator i = result_spell_list.begin(); i != result_spell_list.end(); ++i )
    {
      const spell_data_t* spell = sim -> dbc.spell( *i );
      if ( ! spell )
        continue;

      if ( ( spell -> rune_cost() & r ) == r )
        res.push_back( *i );
    }

    return res;
  }

  virtual std::vector<uint32_t> operator!=( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;
    unsigned              r = rune_cost( other.result_str );

    for ( std::vector<uint32_t>::const_iterator i = result_spell_list.begin(); i != result_spell_list.end(); ++i )
    {
      const spell_data_t* spell = sim -> dbc.spell( *i );
      if ( ! spell )
        continue;

      if ( ( spell -> rune_cost() & r ) != r )
        res.push_back( *i );
    }

    return res;
  }
};

struct spell_class_expr_t : public spell_list_expr_t
{
  spell_class_expr_t( sim_t* sim, expr_data_e type ) : spell_list_expr_t( sim, "class", type ) { }

  virtual std::vector<uint32_t> operator==( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;
    uint32_t              class_mask;

    if ( other.result_tok == TOK_STR )
      class_mask = class_str_to_mask( other.result_str );
    // Other types will not be allowed, e.g. you cannot do class=list
    else
      return res;

    player_e class_type = util::translate_class_str( other.result_str );

    for ( std::vector<uint32_t>::const_iterator i = result_spell_list.begin(); i != result_spell_list.end(); ++i )
    {
      if ( data_type == DATA_TALENT )
      {
        if ( ! sim -> dbc.talent( *i ) )
          continue;

        if ( sim -> dbc.talent( *i ) -> mask_class() & class_mask )
          res.push_back( *i );
      }
      if ( data_type == DATA_SET_BONUS )
      {
        if ( ! (dbc::set_bonus( sim -> dbc.ptr ) + *i) )
          continue;

        if ( util::translate_class_id( ( dbc::set_bonus( sim -> dbc.ptr ) + *i) -> class_id ) == class_type )
          res.push_back( *i );
      }
      else
      {
        const spell_data_t* spell = sim -> dbc.spell( *i );

        if ( ! spell )
          continue;

        if ( spell -> class_mask() & class_mask )
          res.push_back( *i );
      }
    }

    return res;
  }

  virtual std::vector<uint32_t> operator!=( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;
    uint32_t              class_mask;

    if ( other.result_tok == TOK_STR )
      class_mask = class_str_to_mask( other.result_str );
    // Other types will not be allowed, e.g. you cannot do class=list
    else
      return res;

    for ( std::vector<uint32_t>::const_iterator i = result_spell_list.begin(); i != result_spell_list.end(); ++i )
    {
      if ( data_type == DATA_TALENT )
      {
        if ( ! sim -> dbc.talent( *i ) )
          continue;

        if ( ( sim -> dbc.talent( *i ) -> mask_class() & class_mask ) == 0 )
          res.push_back( *i );
      }
      else
      {
        const spell_data_t* spell = sim -> dbc.spell( *i );
        if ( ! spell )
          continue;

        if ( ( spell -> class_mask() & class_mask ) == 0 )
          res.push_back( *i );
      }
    }

    return res;
  }
};

struct spell_race_expr_t : public spell_list_expr_t
{
  spell_race_expr_t( sim_t* sim, expr_data_e type ) : spell_list_expr_t( sim, "race", type ) { }

  virtual std::vector<uint32_t> operator==( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;
    uint32_t              race_mask;

    // Talents are not race specific
    if ( data_type == DATA_TALENT )
      return res;

    if ( other.result_tok == TOK_STR )
      race_mask = race_str_to_mask( other.result_str );
    // Other types will not be allowed, e.g. you cannot do race=list
    else
      return res;

    for ( std::vector<uint32_t>::const_iterator i = result_spell_list.begin(); i != result_spell_list.end(); ++i )
    {
      const spell_data_t* spell = sim -> dbc.spell( *i );

      if ( ! spell )
        continue;

      if ( spell -> race_mask() & race_mask )
        res.push_back( *i );
    }

    return res;
  }

  virtual std::vector<uint32_t> operator!=( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;
    uint32_t              class_mask;

    // Talents are not race specific
    if ( data_type == DATA_TALENT )
      return res;

    if ( other.result_tok == TOK_STR )
      class_mask = race_str_to_mask( other.result_str );
    // Other types will not be allowed, e.g. you cannot do class=list
    else
      return res;

    for ( std::vector<uint32_t>::const_iterator i = result_spell_list.begin(); i != result_spell_list.end(); ++i )
    {
      const spell_data_t* spell = sim -> dbc.spell( *i );
      if ( ! spell )
        continue;

      if ( ( spell -> class_mask() & class_mask ) == 0 )
        res.push_back( *i );
    }

    return res;
  }
};

struct spell_attribute_expr_t : public spell_list_expr_t
{
  spell_attribute_expr_t( sim_t* sim, expr_data_e type ) : spell_list_expr_t( sim, "attribute", type ) { }

  virtual std::vector<uint32_t> operator==( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;

    // Only for spells
    if ( data_type == DATA_EFFECT || data_type == DATA_TALENT )
      return res;

    // Numbered attributes only
    if ( other.result_tok != TOK_NUM )
      return res;

    uint32_t attridx = ( unsigned ) other.result_num / ( sizeof( unsigned ) * 8 );
    uint32_t flagidx = ( unsigned ) other.result_num % ( sizeof( unsigned ) * 8 );

    assert( attridx < NUM_SPELL_FLAGS && flagidx < 32 );

    for ( std::vector<uint32_t>::const_iterator i = result_spell_list.begin(); i != result_spell_list.end(); ++i )
    {
      const spell_data_t* spell = sim -> dbc.spell( *i );

      if ( ! spell )
        continue;

      if ( spell -> attribute( attridx ) & ( 1 << flagidx ) )
        res.push_back( *i );
    }

    return res;
  }
};

struct spell_school_expr_t : public spell_list_expr_t
{
  spell_school_expr_t( sim_t* sim, expr_data_e type ) : spell_list_expr_t( sim, "school", type ) { }

  virtual std::vector<uint32_t> operator==( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;
    uint32_t              school_mask;

    if ( other.result_tok == TOK_STR )
      school_mask = school_str_to_mask( other.result_str );
    // Other types will not be allowed, e.g. you cannot do class=list
    else
      return res;

    for ( std::vector<uint32_t>::const_iterator i = result_spell_list.begin(); i != result_spell_list.end(); ++i )
    {
      const spell_data_t* spell = sim -> dbc.spell( *i );

      if ( ! spell )
        continue;

      if ( ( spell -> school_mask() & school_mask ) == school_mask )
        res.push_back( *i );
    }

    return res;
  }

  virtual std::vector<uint32_t> operator!=( const spell_data_expr_t& other )
  {
    std::vector<uint32_t> res;
    uint32_t              school_mask;

    if ( other.result_tok == TOK_STR )
      school_mask = school_str_to_mask( other.result_str );
    // Other types will not be allowed, e.g. you cannot do school=list
    else
      return res;

    for ( std::vector<uint32_t>::const_iterator i = result_spell_list.begin(); i != result_spell_list.end(); ++i )
    {
      const spell_data_t* spell = sim -> dbc.spell( *i );

      if ( ! spell )
        continue;

      if ( ( spell -> school_mask() & school_mask ) == 0 )
        res.push_back( *i );
    }

    return res;
  }
};

spell_data_expr_t* build_expression_tree( sim_t* sim,
                                          const std::vector<expr_token_t>& tokens )
{
  auto_dispose< std::vector<spell_data_expr_t*> > stack;

  size_t num_tokens = tokens.size();
  for ( size_t i = 0; i < num_tokens; i++ )
  {
    const expr_token_t& t = tokens[ i ];

    if ( t.type == TOK_NUM )
      stack.push_back( new spell_data_expr_t( sim, t.label, atof( t.label.c_str() ) ) );
    else if ( t.type == TOK_STR )
    {
      spell_data_expr_t* e = spell_data_expr_t::create_spell_expression( sim, t.label );

      if ( ! e )
      {
        sim -> errorf( "Unable to decode expression function '%s'\n", t.label.c_str() );
        return 0;
      }
      stack.push_back( e );
    }
    else if ( expression_t::is_binary( t.type ) )
    {
      if ( stack.size() < 2 )
        return 0;
      spell_data_expr_t* right = stack.back(); stack.pop_back();
      spell_data_expr_t* left  = stack.back(); stack.pop_back();
      if ( ! left || ! right )
        return 0;
      stack.push_back( new sd_expr_binary_t( sim, t.label, t.type, left, right ) );
    }
  }

  if ( stack.size() != 1 )
    return 0;

  spell_data_expr_t* res = stack.back();
  stack.pop_back();
  return res;
}

} // anonymous namespace ====================================================

spell_data_expr_t* spell_data_expr_t::create_spell_expression( sim_t* sim, const std::string& name_str )
{
  std::vector<std::string> splits = util::string_split( name_str, "." );

  if ( splits.empty() || splits.size() > 3 )
    return 0;

  expr_data_e data_type = parse_data_type( splits[ 0 ] );

  if ( splits.size() == 1 )
  {
    // No split, access raw list or create a normal expression
    if ( data_type == ( expr_data_e ) - 1 )
      return new spell_data_expr_t( sim, name_str, name_str );
    else
      return new spell_list_expr_t( sim, splits[ 0 ], data_type );
  }

  if ( data_type == ( expr_data_e ) - 1 )
    return 0;

  // Effect handling, set flag and remove effect keyword from tokens
  bool effect_query = false;
  if ( util::str_compare_ci( splits[ 1 ], "effect" ) )
  {
    if ( data_type == DATA_EFFECT )
    {
      // "effect.effect"?!?
      return 0;
    }
    effect_query = true;
    splits.erase( splits.begin() + 1 );
  }
  else if ( util::str_compare_ci( splits[ 1 ], "class" ) )
    return new spell_class_expr_t( sim, data_type );
  else if ( util::str_compare_ci( splits[ 1 ], "race" ) )
    return new spell_race_expr_t( sim, data_type );
  else if ( util::str_compare_ci( splits[ 1 ], "attribute" ) )
    return new spell_attribute_expr_t( sim, data_type );
  else if ( data_type != DATA_TALENT && util::str_compare_ci( splits[ 1 ], "school" ) )
    return new spell_school_expr_t( sim, data_type );
  else if ( data_type != DATA_TALENT && util::str_compare_ci( splits[ 1 ], "rune" ) )
    return new spell_rune_expr_t( sim, data_type );

  const sdata_field_t* fields;
  unsigned fsize;
  if ( data_type == DATA_TALENT )
  {
    fields = _talent_data_fields;
    fsize  = sizeof( _talent_data_fields );
  }
  else if ( data_type == DATA_SET_BONUS )
  {
    fields = _set_bonus_data_fields;
    fsize  = sizeof( _set_bonus_data_fields );
  }
  else if ( effect_query || data_type == DATA_EFFECT )
  {
    fields = _effect_data_fields;
    fsize  = sizeof( _effect_data_fields );
  }
  else
  {
    fields = _spell_data_fields;
    fsize  = sizeof( _spell_data_fields );
  }

  for ( unsigned int i = 0; i < fsize / sizeof( sdata_field_t ); i++ )
  {
    if ( ! fields[ i ].name.empty() && util::str_compare_ci( splits[ 1 ], fields[ i ].name ) )
    {
      return new spell_data_filter_expr_t( sim, data_type, fields[ i ].name, effect_query );
    }
  }

  return 0;
}

spell_data_expr_t* spell_data_expr_t::parse( sim_t* sim, const std::string& expr_str )
{
  if ( expr_str.empty() ) return 0;

  std::vector<expr_token_t> tokens = expression_t::parse_tokens( 0, expr_str );

  if ( sim -> debug ) expression_t::print_tokens( tokens, sim );

  expression_t::convert_to_unary( tokens );

  if ( sim -> debug ) expression_t::print_tokens( tokens, sim );

  if ( ! expression_t::convert_to_rpn( tokens ) )
  {
    sim -> errorf( "Unable to convert %s into RPN\n", expr_str.c_str() );
    sim -> cancel();
    return 0;
  }

  if ( sim -> debug ) expression_t::print_tokens( tokens, sim );

  spell_data_expr_t* e = build_expression_tree( sim, tokens );

  if ( ! e )
  {
    sim -> errorf( "Unable to build expression tree from %s\n", expr_str.c_str() );
    sim -> cancel();
    return 0;
  }

  return e;
}
