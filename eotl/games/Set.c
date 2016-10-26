#include <acme.h>        
#include <ansi.h>
inherit ObjectCode;
static inherit AcmeArray;
static inherit AcmeMath;
#include AcmeArrayInc
#include AcmeMathInc
 
#define FORECOL ([ "black"    : NORM BLACK,   "hblack"    : BOLD_BLACK,    \
                   "red"      : NORM RED,     "hred"      : BOLD_RED,      \
                   "green"    : NORM GREEN,   "hgreen"    : BOLD_GREEN,    \
                   "yellow"   : NORM YELLOW,  "hyellow"   : BOLD_YELLOW,   \
                   "blue"     : NORM BLUE,    "hblue"     : BOLD_BLUE,     \
                   "magenta"  : NORM MAGENTA, "hmagenta"  : BOLD_MAGENTA,  \
                   "cyan"     : NORM CYAN,    "hcyan"     : BOLD_CYAN,     \
                   "white"    : NORM WHITE,   "hwhite"    : BOLD_WHITE ])
#define BACKCOL ([ "black"    : BACK_BLACK,    \
                   "red"      : BACK_RED,      \
                   "green"    : BACK_GREEN,    \
                   "yellow"   : BACK_YELLOW,   \
                   "blue"     : BACK_BLUE,     \
                   "magenta" : BACK_MAGENTA,  \
                   "cyan"     : BACK_CYAN,     \
                   "white"    : BACK_WHITE ])
 
#define SETTINGS         settings[THISP]
#define SHAPES           SETTINGS["shape"]
#define FORE_COLORS      SETTINGS["fore"]
#define BACK_COLORS      SETTINGS["back"]
#define DEFAULT_SETTINGS ([ \
                         "shape" : ({ "O", "X", "+" }), \
                         "fore"  : ({ BLACK, BOLD_RED, BOLD_WHITE }), \
                         "back"  : ({ BACK_WHITE, BACK_CYAN, BACK_GREEN }) \
                         ])

#define SCOREFILE "/zone/null/games/games/Set/scores"

static int *deck, *dealt, timer;
static float scale;
static mapping sets, settings;
mapping scores;

varargs string strcard( int num, object who );
int deal_card( int num );
status valid_set( int one, int two, int three );
status do_set( string str );
varargs void room_printf( string fmt, varargs int *cards );
float calc_score( int time, float scale );
int calc_exp( float score, object who );
void update_score( string who, float score );
void save_scores();
mapping query_scores();

void
extra_create()
{
  seteuid( getuid() );
  set( "id", ({ "set deck", "deck", "set", "cards", "deck of cards", 
                "deck of set cards", "deck of set", "Set deck", "Set",
                "deck of Set cards", "deck of Set" }) );
  set( "short", "a deck of Set cards" );
  set( "long", 
    "This is a deck of Set cards.  It is purple.\n"
    "These are the commands:\n"
    "  Set           - view dealt cards\n"
    "  Set reset     - shuffle all cards back into deck\n"
    "  Set deal      - deal out 3 more Set cards\n"
    "  Set rearrange - rearrange dealt Set cards\n"
    "  Set sets      - view collected sets\n"
    "  Set mode      - change Settings\n"
    "  Set rules     - view Set rules\n"
    "  Set c1 c2 c3  - collect a set of cards c1, c2, and c3\n" 
    "\nYou must have an ANSI color terminal to play this game.\n" );
  set( "gettable", 1 );
  set( "droppable", 1 );
  set( NoStoreP, 1 );
  sets = ([ ]);
  deck = ({ });
  dealt = ({ });
  settings = ([ 0 : DEFAULT_SETTINGS ]);
  scores = allocate_mapping(0, 2);
  
  if( !clonep() )
  {
    restore_object( SCOREFILE );
    save_scores();
  }
      
  if( clonep() )
    call_out( "do_set", 1, "reset" );
  return;
}
 
void
extra_init()
{
  add_action( "do_set", "Set" );
  return; 
}
 
status
do_set( string arg ) 
{
  int cols, rows, size;
  int i, j, idx;
  string str;
  mixed tmp;
  if( !THISP->query_ansi() )
  {
    notify_fail( "You must have ansi set on to play this game.\n" );
    return 0;
  }
  if( arg )
    str = lower_case( arg );
  if( !member( settings, THISP ) )
  {
    if( !mappingp( THISP->query("SetSettings") ) )
      THISP->set("SetSettings", DEFAULT_SETTINGS);
    SETTINGS = THISP->query("SetSettings");
  }
  
  if( !stringp( str ) || !strlen( str ) )
  {
    cols = THISP->query_page_width()/8;
    size = sizeof( dealt );
    i = 0;
    while( i*cols < size )
      i+=3;
    if( i == 0 )
    {
      write( "No cards have been dealt.\n" );
      return 1;
    }
    rows = i;
    cols = size/rows;
    if( size%rows )
      cols++;
    for( i = 0 ; i < rows ; i++ )
    {
      for( j = 0 ; j < cols ; j++ )
        printf( "  %2d|%s", 
          ( ( idx = ( rows * j ) + i ) + 1 ), 
          ( idx < size ? strcard( dealt[ idx ] ) : strcard( -1 ) ) );
      printf( "\n" );
    }
    return 1;
  }   
  else if( str == "sets" )
  {
    string *setters;
    int x, y;
    setters = m_indices( sets );
    cols = THISP->query_page_width()/6;
    for( i=0 , j=sizeof(setters) ; i < j ; i++ )
    {
      printf( "%s's Sets:\n", CAP( setters[i] ) );
      tmp = sets[setters[i]];
      size = sizeof( tmp );
      x = 0;
      while( x*cols < size )
        x+=3;
      rows = x;
      cols = size/rows;
      if( size%rows )
        cols++;
      for( x = 0 ; x < rows ; x++ )
      {
        for( y = 0 ; y < cols ; y++ )
          printf( "  %s", 
          ( ( idx = ( rows * y ) + x ) < size ? 
            strcard( tmp[ idx ] ) : 
            strcard( -1 ) ) );
        printf( "\n" );
      }
      printf( "\n\n" );
    }
    return 1;
  }
  else if( str == "reset" )
  {
    deck = random_array( 3*3*3*3 );
    dealt = ({ });
    sets = ([ ]);
    scale = to_float(MAXINT);
    write( "You shuffle all cards back into the deck.\n" );
    room_printf( PNAME + " shuffles all cards back into the deck.\n" );
    return 1;
  }
  else if( str == "rules" )
  {
    THISP->more_string( format(
    "The object of the game is to identify a 'set' of three cards from "
    "a pool of 12 dealt cards.  Each card has four features, which can vary "
    "as follows:\n\n"
    "  Symbols:     Each card is labeled with the '#', '-' or '|' symbols;\n"
    "  Number:      Each card has one, two, or three symbols on it;\n"
    "  Colors:      The symbols are either red, magenta, or black;\n"
    "  Background:  The background color is either yellow, green, or white.\n\n"
    "A 'set' consists of three cards in which each of the card's features, "
    "looked at one-by-one, are the same on each card, or are different on "
    "each card.  All of the features must separately satisfy this rule.  In "
    "others words: symbol must either bet the same on the 3 cards, or "
    "different on each of the 3; color must either be the same on the 3 "
    "cards, or different on each of the 3; etc.  If only 2 of the 3 cards "
    "are the same in any feature, then it is _not_ a 'set'.\n\n"
    "To play, shuffle the deck and deal out 12 cards.  When a 'set' is "
    "found, deal 3 more cards to replace the ones that were taken.  You may "
    "deal out more than 12 cards if you can't find a 'set', but your score "
    "will go down.  At the end of the deck, there may still be six or nine "
    "cards remaining which do not form a 'set'.\n\n"
    "You may keep the deck in your inventory to play solitaire, or drop it "
    "in the room to allow other people to play along!  Mail bugs, comments, "
    "queries, etc to Devo.\n", 
    THISP->query_page_width() ) );
    return 1;
  } 
  else if( str == "deal" )
  {
    deal_card(3);
    return 1;
  }
  else if( str == "rearrange" )
  {
    dealt -= ({ -1 });
    printf( "You rearrange the dealt cards.\n" );
    room_printf( PNAME + " rearranges the dealt cards.\n" );
    return 1;
  }
  else if( str[0..3] == "mode" )
  {
    string var, val1, val2, val3;
    if( str == "mode" )
    {
      printf( "Settings are:\n"
              "  Back Colors: %s%sx%sx%sx%s\n"
              "  Fore Colors: %s%sx%sx%sx%s\n"
              "  Shapes     : %s %s %s\n",
              NORM, 
              BACK_COLORS[0], BACK_COLORS[1], BACK_COLORS[2], 
              NORM,
              NORM, 
              FORE_COLORS[0], FORE_COLORS[1], FORE_COLORS[2], 
              NORM,
              SHAPES[0], SHAPES[1], SHAPES[2]
      );
      return 1;
    }
    if( sscanf( arg[4..], "%t%s%.1t%s%.1t%s%.1t%s", var, val1, val2, val3 ) 
        != 4 )
    {
      write( "Usage: Set mode var val1 val2 val3\n" );
      return 1;
    }
    var = lower_case( var );
    if( !member( SETTINGS, var ) )
    {
      printf( "Bad variable.  Possible values:\n"
              "  %-*#s\n",
              THISP->query_page_width(),
              implode( m_indices( SETTINGS ), "\n" ) );
      return 1;
    }

    if( var=="fore" )
    {
      val1 = FORECOL[lower_case(val1)];
      val2 = FORECOL[lower_case(val2)];
      val3 = FORECOL[lower_case(val3)];
      if( !val1 || !val2 || !val3 )
      {
        printf( "Bad foreground color.  Possible values:\n"
                "  %-*#s\n",
                THISP->query_page_width(),
                implode( m_indices( FORECOL ), "\n" ) );
      }
      else
        FORE_COLORS = ({ val1, val2, val3 });
    }
    else if( var=="back" )
    {
      val1 = BACKCOL[lower_case(val1)];
      val2 = BACKCOL[lower_case(val2)];
      val3 = BACKCOL[lower_case(val3)];
      if( !val1 || !val2 || !val3 )
      {
        printf( "Bad background color.  Possible values:\n"
                "  %-*#s\n",
                THISP->query_page_width(),
                implode( m_indices( BACKCOL ), "\n" ) );
      }
      else
        BACK_COLORS = ({ val1, val2, val3 });
    }
    else if( var=="shape" )
    {
      if( !strlen(val1) > 1 || 
          !strlen(val2) > 1 || 
          !strlen(val3) > 1 )
        printf( "Shape must be a single character.\n" );
      else
        SHAPES = ({ val1, val2, val3 });
    }
    THISP->set("SetSettings", SETTINGS);
    return 1;
  }
  else
  {
    int one, two, three;
    if( sscanf( str, "%d %d %d", one, two, three ) != 3 )
    {
      notify_fail( "Usage: Set card1 card2 card3\n" );
      return 0;
    }
    if( one   < 1 || one   > sizeof( dealt ) ||
        two   < 1 || two   > sizeof( dealt ) ||
        three < 1 || three > sizeof( dealt ) )
    {
      write( "That is not a valid card number.\n"
             "Type \"Set\" to view cards.\n" );
      return 1;
    }
    if( (one==two) || (two==three) || (one==three) )
    {
      write( "You may not select the same card more than once.\n" );
      return 1;
    }
    if( !valid_set( dealt[one-1], dealt[two-1], dealt[three-1] ) )
      printf( "These cards do not form a valid Set: %2d|%s  %2d|%s  %2d|%s\n",
        one,   strcard( dealt[one  -1] ), 
        two,   strcard( dealt[two  -1] ), 
        three, strcard( dealt[three-1] ) );
    else
    {
      float score, newscale;
      int exp, dsize;
      
      printf( "You get a Set: %2d|%s  %2d|%s  %2d|%s\n",
        one,   strcard( dealt[one  -1] ), 
        two,   strcard( dealt[two  -1] ), 
        three, strcard( dealt[three-1] ) );
      room_printf( sprintf( "%s gets a Set: %2d|%%s  %2d|%%s  "
                                        "%2d|%%s\n",
                   PNAME, one, two, three ),
        dealt[one  -1], dealt[two  -1], dealt[three-1] );
      score = calc_score( time()-timer, scale );
      exp = calc_exp( score, THISP );
      load_name(THISO)->update_score( THISP->query_real_name(), score );
      printf( "You scored %0f points for %d experience points.\n",  score, exp );
      THISP->add_exp(exp);

      if( !pointerp( sets[PRNAME] ) )
        sets[PRNAME] = ({ });
      sets[PRNAME] += ({ dealt[one-1], dealt[two-1], dealt[three-1] });
      dealt[one-1]   = -1;
      dealt[two-1]   = -1;
      dealt[three-1] = -1;      

      dsize = 4-(sizeof(dealt-({-1}))/3);
      if( dsize > 0 )
        newscale = 1+(1-pow(2, -dsize));
      else
        newscale = pow(2, dsize);
      if( scale < newscale )
        scale = newscale;
    }
    return 1;
  }
}
 
status
valid_set( int one, int two, int three )
{
  int matches, diffs, i;
  matches = diffs = 0;
  if( one < 0 || two < 0 || three < 0 )
    return 0;
  for( i = 4 ; i > 0 ; i-- )
  {
    if( one%3 == two%3 && two%3 == three%3 && one%3 == three%3 )
      matches++;
    else if( one%3 != two%3 && two%3 != three%3 && one%3 != three%3 )
      diffs++;
    one /= 3;
    two /= 3;
    three /= 3;
  }
  return ( (matches+diffs) == 4 );
}
 
int 
deal_card( int num )
{
  int i, place, *places, *cards, dsize;
  float newscale;
  string out;
  
  num = MIN( num, sizeof( deck ) );
  places = ({ });
  cards = ({ });
  if( !sizeof( deck ) )
  {
    write( "No cards left to deal.\n" );
    return 0;
  }
  for( i = 0 ; i < num ; i++ )
  {
    if( ( place = searcha( dealt, -1 ) ) != -1 )
    {
      dealt[ place ] = deck[0];
      places += ({ place });
    }
    else
    {
      dealt += ({ deck[0] });
      places += ({ sizeof( dealt ) - 1 });
    }
    cards += ({ deck[0] });
    deck = deletea( deck, 0 );
  }
  out = sprintf( "%d Set cards are dealt:", sizeof( cards ) );
  for( i = 0 ; i < sizeof( cards ) ; i++ )
    out += sprintf( " %2d|%%s ", places[i]+1 );
  out += "\n";
  apply( #'printf, ({ out }) + map_array( cards, #'strcard, THISP ) );
  apply( #'room_printf, ({ out}) + cards );

  timer = time();
  dsize = 4-(sizeof(dealt-({-1}))/3);
  if( dsize > 0 )
    newscale = 1+(1-pow(2, -dsize));
  else
    newscale = pow(2, dsize);
  if( scale > newscale )
    scale = newscale;

  return sizeof( cards );
}
 
varargs string 
strcard( int num, object who )
{
  int count, shape, fore, back;
  if( !objectp( who ) )
    who = THISP;
  if( objectp( who ) && !member( settings, who ) )
  {
    if( !mappingp( who->query("SetSettings") ) )
      who->set("SetSettings", DEFAULT_SETTINGS);
    settings[who] = who->query("SetSettings");
  }
  if( num < 0 )
    return "   "; 
  count = num % 3;
  num /= 3;
  shape = num % 3;
  num /= 3;
  fore = num % 3;
  num /= 3;
  back = num % 3;
  count++;
  return sprintf( "%s%s%s%s%s%s", 
    settings[who]["fore"][fore], 
    settings[who]["back"][back],
    ( count == 2 || count == 3 ? settings[who]["shape"][shape] : " " ),
    ( count == 1 || count == 3 ? settings[who]["shape"][shape] : " " ),
    ( count == 2 || count == 3 ? settings[who]["shape"][shape] : " " ),
    NORM
  );
}

varargs void
room_printf( string fmt, varargs int *cards )
{
  object who;
  
  if( ENV(THISO) == THISP || ENV(THISO) == 0)
    return;

  foreach( who : all_inventory(ENV(THISO)) )
  {
    string *args;
    if( !interactive(who) )
      continue;
    if( !who->query_ansi() )
      continue;
    if( who == THISP )
      continue;
    args = map_array( cards, #'strcard, who );
    tell_object( who, apply( #'sprintf, ({ fmt }) + args ) );
  }
  return;
}

float 
calc_score( int time, float scale )
{
  float out;
  out = (60.0-time);
  if( out < 0.0 )
    return 0.0;
  out *= scale * 100;
  out /= 60;
  return out;
}

int 
calc_exp( float score, object who )
{
  float exp;
  exp = sqrt( 1 + MAX( who->query_eval()+(random(1000)-500)/100.0, 0 ));
  exp *= score;
  return to_int( round( exp, 0 ) );
}

void 
update_score( string who, float score )
{
  if( clonep() )
    return load_name(THISO)->update_score( who, score );  
  if( !member( scores, who ) )
    scores += ([ who : 1; score ]);
  else
    scores[who,1] = ( ( ( scores[who,0]++ * scores[who,1] ) + score ) / 
                      scores[who,0] );    
}

void 
save_scores()
{
  if( clonep() )
    return load_name(THISO)->save_scores();
 remove_call_out( "save_scores" );
 save_object( SCOREFILE );
 call_out( "save_scores", 300 );
}

mapping
query_scores()
{
  if( clonep() )
    return load_name(THISO)->query_scores();
  return copy_mapping( scores );
}

int
destruct_signal()
{
  save_scores();
  return 0;
}

