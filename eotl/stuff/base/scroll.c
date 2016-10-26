inherit StuffCode;

int read_delay = 1;

void create();
set( string var, mixed val );
mixed query( string var );
string short();
string long( string id );
string post_long();
int id( string str );
void init();
int do_read( string str );
void read( object who );
void cursed_read( object who );
void blessed_read( object who );
int try_read( object who );
void read_signal( object who );


void
create()
{
  ::create();
  if( !query("value") )
    set( "value", 60 );
  if( !query("short") )
  {
    set( "short", "a scroll of gibberish" );
    set( "ids", ({ "a scroll of gibberish",
                   "scroll of gibberish",
                   "gibberish scroll",
                   "gibberish" }) );
  }
  return;
}

set( string var, mixed val )
{
  switch( var )
  {
    case "read_delay":
      read_delay = val;
      return 1;
  }
  return ::set( var, val );
}

mixed
query( string var )
{
  switch( var )
  {
    case "read_delay":
      return read_delay;
    case ScrollP:
      return 1;
  }
  return ::query( var );
}
  
// Special identify majicks.
string
short()
{
  string out;

  if( THISI )
    last_view = time();

  if( THISO->query_identified(THISP) & ID_ITEM )
    out = ::short();
  else if( strlen( id_label ) )
    out = sprintf( "a scroll labeled %s", id_label ); 
  else
    out = "an unlabeled scroll";

  return out + post_short();
}

string
long( string id )
{
  int tmp;
  string out;
  tmp = sizeof( explode( id_label, " " ) );
  out = format( sprintf( "This medium-sized sheet of parchment is rolled up "
                "tightly and bound with a single wax label reading the %s "
                "\"%s.\"  Reading the rest of the scroll's text might have "
                "unpredictable effects.", ( tmp>1 ? "words" : "word" ),
                id_label ) );
  return out + THISO->post_long();
}

string
post_long()
{
  return "\nA watermark on the back of the parchment reads the words, "
         "\"Stuff, Inc.\"";
}

// Special identify majicks.
int
id( string str )
{
  string il, lstr;
  if( str == THISO->short() )
    return 1;
  il = lower_case( id_label );
  lstr = lower_case( str );
  if( lstr == "stuff" || lstr == "scroll" || lstr == "a scroll" )
    return 1;
  if( ( THISO->query_identified(THISP) & ID_ITEM ) && ::id(str) )
    return 1;
  if( ( lstr == sprintf( "a scroll labeled %s", il ) ) ||
      ( lstr == sprintf( "scroll labeled %s", il ) ) ||
      ( lstr == sprintf( "a %s scroll", il ) ) ||
      ( lstr == sprintf( "%s scroll", il ) ) )
    return 1;
  return 0;
}

void
init()
{
  ::init();
  //  stupid combat causing players to flee before init() 
  if( !THISP || 
      ( !present( THISO, ENV(THISP) && !present( THISO, THISP ) ) ) )
    return;
  add_action( "do_read", "read" );
  return;
}

// This is the action function for reading the scroll.
int
do_read( string str )
{
  if( ENV(THISO) != THISP )
    return 0;
  if( !stringp(str) || str=="" )
  {
    notify_fail( sprintf( "%s what?\n", CAP( query_verb() ) ) );
    return 0;
  }
  if( present(str,THISP) != THISO )
  {
    notify_fail( sprintf( "You don't have anything like that to %s.\n", 
                 query_verb() ) );
    return 0;
  }

  if( THISO->try_read(THISP) )
    return 1;
 
  THISP->add_combat_delay(read_delay);
 
  tell_player( THISP, "You open the scroll and read aloud the cryptic text." );
  tell_room( ENV(THISP), format( PNAME + " opens a scroll and begins reciting "
             "an unintelligible verse." ), ({ THISP }) );
 
  if( THISO->query_cursed() )
    THISO->cursed_read( THISP );
  else if( THISO->query_blessed() )
    THISO->blessed_read( THISP );
  else
    THISO->read( THISP );

  THISO->read_signal( THISP );
  StuffServer->pull_item( THISO );

  return 1;
}

void
read( object who )
{
  tell_object( who, "Nothing happens.\n" );
  return;
}

void
cursed_read( object who )
{
  return read( who );
}

void
blessed_read( object who )
{
  return read( who );
}

// Some scaffolding
int 
try_read( object who )
{
  return 0;
}

varargs void 
read_signal( object who )
{
  return;
}

private int query_no_copy()
{
  return 1;
}
