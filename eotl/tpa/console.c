#include <ansi.h>
#include "tpa.h"

inherit ObjectCode;

void set_departure( string station );
string query_departure();
void set_arrival( string station );
string query_arrival();
void set_schedule( string name );
string query_schedule();
void set_offline(int i);
int query_offline();
void set_prices( mapping map );
void set_price( string station, int price );
mapping query_prices();
int query_price( string station );
void set_reserve( int price );
int query_reserve();
void set_timetable( mapping map );
void add_timetable( string station, int *times );
void clear_timetable( string station );
mixed *query_timetable();
int query_current();
void set_toggle( string *list );
string *query_toggle();
void alarm();
private void reset_current();
private int next_timetable( int c );
private int timetable_sort( mixed *a, mixed *b );
static int room_setup( mixed room );
static int *explode_minutes( int inc, int start, int end );
static object resolve_current();
int in_service();
int do_depart( string arg );
int do_scan( string arg );
int do_toggle( string arg );
void push_message( object who );
void alarm_message( object station );
void toggle_message( object who, object station );
void arrival_message( object who, object station );
void departure_message( object who, object station );
void scan_message( object who );

int offline, current, reserve;
mapping prices;
mixed *timetable;
string departure, arrival, schedule, *toggle;

void
create()
{
  if( clonep() )
    destruct(THISO);
  toggle = ({ });
  prices = ([ ]);
  timetable = 0;
  departure = 0;
  arrival = 0;
  schedule = 0;
  current = 0;
  offline = 0;

  ::create();
  set( "gettable", 0 );
  set( "droppable", 0 );
  set( "weight", 50 );
  set( "value", 0 );

  return;
}

void
init()
{
  ::init();
  add_action( "do_depart", "push" );
  add_action( "do_depart", "press" );
  add_action( "do_scan", "scan" );
  add_action( "do_toggle", "turn" );
  add_action( "do_toggle", "toggle" );
  add_action( "do_toggle", "spin" );

  return;
}

int
id( string id )
{
  int type=0;
  string *ids = ({ });

  if( stringp(departure) )
  {
    ids += ({ "red button", "button" });
    if( stringp(arrival) )
      ids += ({ "purple light" });
    else
      ids += ({ "red light" });

    if( timetable )
      ids += ({ "timer" });
    else if( sizeof(toggle)>1 )
      ids += ({ "dial", "black dial" });

    ids += ({ "display", "digital display", "destination" });
  }
  else if( stringp(arrival)  )
    ids += ({ "blue light" });

  if( stringp(arrival) )
    ids += ({ "slot", "scanner", "card scanner", "card slot", "card reader" });

  ids += ({ "light", "neon light", "post", "console", "tpa console", 
            "eotl tpa console"});

  if( member(ids, lower_case(id)) != -1 )
    return 1;

  return 0;
}

string
long( string id, object who )
{
  int type;  // 1=depart, 2=arrival, 3=both
  int dim;
  string out;
  
  int ansi = (who||THISP)->query_ansi();

  out = "";

  if( !in_service() )
    dim = 1;

  if( stringp(departure) && stringp(arrival) )
    type = 1|2;
  else if( stringp(departure) )
    type = 1;
  else if( stringp(arrival) )
    type = 2;
  else
  {
    dim = 1;
  }

  if( member( ({ "light", "neon light", "purple light", "blue light",
                 "red light" }), id ) != -1 )
  {
    if( dim )
    {
      if( type==1 )
        out = "The red light is meant to signal passers by that this is a "
              "departure station, but it has been shut off.  The station "
              "must be temporarily out of service.";
      else if( type==2 )
        out = "The blue light is meant to signal passers by that this is a "
              "arrival station, but it has been shut off.  The station "
              "must be temporarily out of service.";
      else if( type==3 )
        out = "The red light is meant to signal passers by that this is a "
              "departure station, but it has been shut off.  The station "
              "must be temporarily out of service.";
      else
        out = "The light has been shut off.  The station must be "
              "temporarily out of service.";
    }
    else
    {
      if( type==1 )
        out = "The light shines brightly with a red glow, signaling to "
              "passers by that this station is for depatures only.";
      else if( type==2 )
        out = "The light shines brightly with a blue glow, signaling to "
              "passers by that this station is for arrivals only.";
      else if( type==3 )
        out = "The light shines brightly with a purple glow, signaling to "
              "passers by that this station services both arrivals and "
              "departures.";
    }
  }
  else if( member( ({ "post" }), id ) != -1 )
  {
    out = "A thin metal post has been shoved into the ground here, with "
          "the console facing out at shoulder level.  Engraved across the "
          "face of the post are the words \"End of the Line Teleport "
          "Authority\".";
  }
  else if( member( ({ "slot", "scanner", "card scanner", "card slot",
                      "card reader" }), id ) != -1 )
  {
    out = "Scanning an official Teleport Authority card will add this "
          "station to the card's destination list, allowing the "
          "cardholder to travel back to this station from any of its "
          "connecting stations.";
  }
  else if( member( ({ "button", "red button" }), id ) != -1 )
  {
    out = "Push it.  You know you want to.";
  }
  else if( member( ({ "dial", "black dial" }), id ) != -1 )
  {
    out = "The dial has been placed just next to the button, and it can "
          "be turned left or right.";
  }
  else if( member( ({ "timer" }), id ) != -1 )
  {
    out = "The timer moves too slow to notice it actually turning, but "
          "a faint ticking from inside the device convinces you that "
          "it's working.";
  }
  else
  {
    out += sprintf( "A %sneon light outlines the console, which is affixed "
                    "to a post planted firmly in the ground.  ",
                    ( (type==1) ? "red " :
                      (type==2) ? "blue " :
                      (type==3) ? "purple " : "" ) );
    if( dim )
      out += "The light is conspicuously dim, however, and the entire panel "
             "seems to be powered down.  ";

    if( type&2 )
      out += sprintf( "The top of the console is spanned by a thin slot, "
                      "which appears to be a card scanner of some sort.  "
                      "The words \"%s%s Station%s\" headline the face of the "
                      "unit.  ", ( ansi ? BOLD_BLUE : "" ), arrival,
                      ( ansi ? NORM : "" ) );
    if( type&1 )
    {
      object station = resolve_current();
      string sname;
      if( objectp(station) )
        sname = station->query_arrival();
      int price = TPAServer->get_real_price( departure, sname );
      out += sprintf( "The console is also adorned by an unlabeled red "
                      "button%s.  Across the bottom of the console is a small "
                      "digital display labeled \"Destination\".  It "
                      "currently reads, \"%s%s%s\".",
                      ( timetable ?
                        ", adjacent to a timer, ticking away faintly" :
                        (sizeof(toggle)>1) ?
                        ", adjascent to an equally non-descript black dial" :
                        "" ),
                      ( ansi ? BOLD_RED : "" ),
                      ( objectp(station) && 
                          (price>=0) && 
                          in_service() &&
                          station->in_service() ?
                        ( sname + " Station: " + ( price ?
                                           ( price + " credit" +
                                             (price!=1 ? "s" : "") ) :
                                           "Free" ) ) :
                        "Out of Service" ),
                      ( ansi ? NORM : "" ) );
    }
  }

  out = format( out, THISP->query_page_width() );
  return out;
}


string
short(object who)
{
  string desc = "an EotL TPA console";
  
  if( !((who||THISP)->query_ansi()) )
    return desc;
  
  if( !in_service() )
  {
    return desc;
  }
  else if( stringp(departure) && stringp(arrival) )
  {
    return BOLD_PURPLE + desc + NORM;
  }
  else if( stringp(departure) )
  {
    return BOLD_RED + desc + NORM;
  }
  else if( stringp(arrival) )
  {
    return BOLD_BLUE + desc + NORM;
  }
  else
  {
    return desc;
  }
}

void
set_departure( string station )
{
  if( departure )
  {
    TPAServer->remove_departure( departure );
    departure = 0;
  }
  if( TPAServer->set_departure( station, THISO ) )  
    departure = station;
  return;
}

string
query_departure()
{
  return departure;
}

void
set_arrival( string station )
{
  if( arrival )
  {
    TPAServer->remove_arrival( arrival );
    arrival = 0;
  }
  if( TPAServer->set_arrival( station, THISO ) )  
    arrival = station;
  return;
}

string
query_arrival()
{
  return arrival;
}

void
set_schedule( string name )
{
  schedule = name;
  return;
}

string
query_schedule()
{
  return schedule;
}

void
set_offline(int i)
{
  offline = i;
  return;
}

int
query_offline()
{
  return offline;
}

void
set_prices( mapping map )
{
  walk_mapping( map, (: if( $2 > 99 ) $2 = 99; :) );
  prices = map;
  return;
}

void
set_price( string station, int price )
{
  prices[station] = price;
  return;
}

mapping
query_prices()
{
  return prices;
}

int
query_price( string station )
{
  return prices[station];
}

void
set_reserve(int i)
{
  reserve = i;
  return;
}

int
query_reserve()
{
  return reserve;
}

void
set_timetable( mapping map )
{
  int *times;
  string station;
  
  toggle = ({ });
  timetable = ({ });
 
  foreach( station, times : map )
  {
    foreach( int time : times )
    {
      timetable += ({ ({ time, station }) });
    }
  }

  // remove duplicate times
  timetable = transpose_array( timetable );
  mapping tmp = mkmapping( timetable[0], timetable[1] );
  timetable = transpose_array( ({ m_indices(tmp), m_values(tmp) }) );

  timetable = sort_array( timetable, "timetable_sort" );
  if( timetable[0][0] == 0 )
  {
    if( timetable[<1][0] != 2400 )
    {
      timetable += ({ timetable[0] });
      timetable[<1][0] = 2400;
    }
    timetable = timetable[1..];
  }
    
  reset_current();

  return;
}

void
add_timetable( string station, int *times )
{
  int now;

  toggle = ({ });
  if( !timetable )
    timetable = ({ });
  clear_timetable(station);

  foreach( int time : times )
  {
    timetable += ({ ({ time, station }) });
  }

  // remove duplicate times
  timetable = transpose_array( timetable );
  mapping tmp = mkmapping( timetable[0], timetable[1] );
  timetable = transpose_array( ({ m_indices(tmp), m_values(tmp) }) );

  timetable = sort_array( timetable, "timetable_sort" );
  if( timetable[0][0] == 0 )
  {
    if( timetable[<1][0] != 2400 )
    {
      timetable += ({ timetable[0] });
      timetable[<1][0] = 2400;
    }
    timetable = timetable[1..];
  }
  reset_current();

  return;
}

void
clear_timetable( string station )
{
  timetable = filter_array( timetable, (: $1[1] != $2 :), station );
  reset_current();
  return;
}

mixed *
query_timetable()
{
  return timetable;
}

int
query_current()
{
  return current;
}

void
set_toggle( string *list )
{
  toggle = list;
  timetable = 0;
  offline = 0;
  reset_current();
  return;
}

string *
query_toggle()
{
  return toggle;
}

// other
void
alarm()
{
  if( !objectp(previous_object()) ||
      previous_object() != FINDO(CLOCKDAEMON) )
    return;
  
  if( !timetable )
    return;

  current = next_timetable(current);
  if( in_service() )
    alarm_message( resolve_current() );
  CLOCKDAEMON->set_alarm( "alarm", timetable[next_timetable(current)][0], 
                          THISO );
  return;
}

private void
reset_current()
{
  if( timetable )
  {
    int now = (CLOCKDAEMON->query_24_hour()*100) + CLOCKDAEMON->query_minute();
    int i=0;
    int j=sizeof(timetable);
    for( int i=0, j=sizeof(timetable) ; i<j ; i++ )
    {
      if( timetable[i][0] > now )
        break;
    }
    if( i==0 )
      i = j;
    current = i-1;

    CLOCKDAEMON->set_alarm( "alarm", timetable[next_timetable(current)][0], 
                            THISO );
  }
  else
  {
    if( current >= sizeof(toggle) )
      current = 0;
  }
  return;
}

private int
next_timetable( int c )
{
  c++;
  if( c >= sizeof(timetable) )
    c=0;

  return c;
}

private int
timetable_sort( mixed *a, mixed *b )
{
  return a[0] > b[0];
}

static int
room_setup( mixed room )
{
  if( stringp(room) )
    room = load_object(room);

  if( !objectp(room) )
    return 0;

  if( !move_object(THISO, room) )
    return 0;

  return 1;
}

static int *
explode_minutes( int inc, int start, int end )
{
  int *out;

  if( inc < 0 )
    return 0;
  if( start < 0 )
    start = 0;
  if( end > 2400 )
    end = 2400;

  inc = ( (inc / 60) * 100 ) + ( inc % 60 );

  out = ({ start });
  while( out[<1] <= end )
  {
    int next = out[<1];
    next += inc;
    if( (next % 100) > 60 )
      next += 40;
    out += ({ next });
  }

  out = out[0..<2];
  if( (out[0]==0) && (out[<1]!=2400) )
    out += ({ 2400 });

  return out;
}

static object
resolve_current()
{
  string station;

  if( current < 0 )
    return 0;

  if( timetable && (current<sizeof(timetable)) )
    station = timetable[current][1];
  else if( current<sizeof(toggle) )
    station = toggle[current];

  if( !station )
    return 0;

  return TPAServer->query_arrival(station);
}

int
in_service()
{
  if( offline )
    return 0;

  if( stringp(arrival) )
    return 1;

  if( stringp(departure) )
    return 1;

  return 0;
}


// actions
int
do_depart( string arg )
{
  object station, card;
  string sname;

  if( !stringp(departure) )
    return 0;

  if( !stringp(arg) || 
      (member( ({ "button", "red button" }), lower_case(arg) ) == -1) )
  {
    notify_fail( "Push what?\n" );
    return 0;
  }

  if( !objectp( card = present_clone(TPACard, THISP) ) )
  {
    tell_player( THISP, "The machine pops and buzzes loudly.  The "
           "words \"Missing Card\" flash across the display." );
    tell_room( ENV(THISO), format("The machine pops and buzzes loudly."),
              ({THISP}) );
    return 1;
  }

  push_message(THISP);
  station = resolve_current();

  if( !in_service() || !objectp( station ) || !station->in_service() )
  {
    tell_player( THISP, "The machine pops and buzzes loudly.  The "
           "words \"Out of Service\" flash across the display." );
    tell_room( ENV(THISO), format("The machine pops and buzzes loudly."),
              ({THISP}) );
    return 1;
  }

  sname = station->query_arrival();

  int price = TPAServer->get_real_price( departure, sname );
  if( price < 0 )
  {
    tell_player( THISP, "The machine pops and buzzes loudly.  The "
           "words \"Out of Service\" flash across the display." );
    tell_room( ENV(THISO), format("The machine pops and buzzes loudly."),
              ({THISP}) );
    return 1;
  }

  if( !card->query_station(sname) )
  {
    tell_player( THISP, "The machine pops and buzzes loudly.  The "
           "words \"Station Access Denied\" flash across the display." );
    tell_room( ENV(THISO), format("The machine pops and buzzes loudly."),
              ({THISP}) );
    return 1;
  }

  if( card->query_credits() < price )
  {
    tell_player( THISP, "The machine pops and buzzes loudly.  The "
           "words \"Insufficient Credits\" flash across the display." );
    tell_room( ENV(THISO), format("The machine pops and buzzes loudly."),
              ({THISP}) );
    return 1;
  }

  THISP->move_player( "&&", ENV(station) );
  departure_message( THISP, station );
  station->arrival_message( THISP, THISO );

  if( price )
  {
    int c;
    card->add_credits( -price );
    c = card->query_credits();
    tell_player( THISP, "The balance on your card fades in and out..." +
                 c + " credit" + ( c!=1 ? "s" : "" ) + " remaining." );
  }

  return 1;
}

int
do_scan( string arg )
{
  object card;

  if( !stringp(arrival) )
    return 0;

  if( !stringp( arg ) || !objectp( card = present( arg, THISP ) ) )
  {
    notify_fail("Scan what?\n");
    return 0;
  }

  if( program_name(card) != TPACard )
  {
    tell_player(THISP, "Sliding that through the card reader isn't going "
                "to be good for anyone.");
    return 1;
  }

  scan_message(THISP);
  
  if( !in_service() )
  {
    tell_player( THISP, "Nothing happens.  This console appears to be "
                 "powered down." );
    return 1;
  }
  
  if( card->query_station(query_arrival()) )
  {
    tell_player(THISP, "Nothing happens.  " + query_arrival() + " Station "
                "is already on your station list.");
  }
  else
  {
    card->add_station(query_arrival());
    tell_player(THISP, "Your card glows briefly.  The words \"" +
                query_arrival() + " Station\" appear on the station "
                "list.");
  }
  card->multi_check();
  
  return 1;
}

int
do_toggle( string arg )
{
  int dir;
  object station;

  if( timetable || (sizeof(toggle)<=1) )
    return 0;

  if( !stringp(arg) )
    arg = "";
    
  arg = lower_case(arg);
  if( sscanf( arg, "%s left", arg ) )
    dir = -1;
  if( sscanf( arg, "%s right", arg ) )
    dir = 1;

  if( member( ({ "dial", "black dial" }), arg ) == -1 )
  {
    notify_fail( "Turn what?\n" );
    return 0;
  }

  if( !dir )
  {
    write( "Turn dial which direction?\n" );
    return 1;
  }

  if( !in_service() )
  {
    tell_player( THISP, "Nothing happens.  This console appears to be "
                 "powered down." );
    return 1;
  }

  current += dir;
  if( current >= sizeof(toggle) )
    current = 0;
  if( current < 0 )
    current = sizeof(toggle)-1;

  station = resolve_current();
  toggle_message(THISP, station);
  return 1;
}

// messaging

void
alarm_message( object station )
{
  if( objectp(station) )
    tell_room( ENV(THISO), format("The TPA console dings and the "
                 "words \"" + station->query_arrival() + " Station\" flash "
                 "across the display.") );
  else
    tell_room( ENV(THISO), format("The TPA console dings and the "
                 "words \"Out of Service\" flash across the display.") );
  return;
}

void
push_message( object who )
{
  tell_room( ENV(THISO), format(who->query_name() + " pushes the button on "
             "the console."), ({who}) );
  tell_player( who, "You push the button on the console." );
  return;
}


void
toggle_message( object who, object station )
{
  tell_room( ENV(THISO), format(who->query_name() + " turns the dial on "
             "the console."), ({who}) );
  if( objectp(station) )
    tell_player( who, "You turn the dial on the console.  The words \"" +
                 station->query_arrival() + " Station\" flash across the "
                 "display." );
  else
    tell_player( who, "You turn the dial on the console.  The words "
                 "\"Out of Service\" flash across the display." );
  
  return;
}

void
arrival_message( object who, object station )
{
  tell_room( ENV(THISO), format("The TPA console dings twice and " +
             who->query_name() + " appears out of thin air."), ({who}) );
  return;
}

void
departure_message( object who, object station )
{
  tell_room( ENV(THISO), format(who->query_name() + " beams away to another " 
             "station on the EotL Teleport Authority network.\n"), ({who}) );
  return;
}

void
scan_message( object who )
{
  tell_room( ENV(THISO), format(who->query_name() + " slides " + 
             possessive(who) + " card through the machine."), ({who}) );
  tell_player( who, "You slide your card through the machine." );
  return;
}

