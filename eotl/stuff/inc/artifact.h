int id_default, spawn_limit, spawn_rate,  last_view;
int max_charges, charges, invoke_timeout;
string id_label, *prevent_spawn, item_name;

#define CREATE_LOG StuffLogDir "create.log"

void
create()
{
  object *stack;

  spawn_limit = 1;
  spawn_rate = 0;
  last_view = time();
  max_charges = -1;
  charges = -1;
  invoke_timeout = 3*60;

  // some objects get cursed/blessed automatically
  switch( random(100) )
  {
    case 0..19  : curse(); break;
    case 20..39 : bless(); break;
    default     : break;
  }

  ::create();

  set( ArtifactP, 1 );
  // If the server can't register our item, give up right away.
  if( !StuffServer->push_item(THISO) )
    destruct(THISO);

  // For short descriptions later on.
  id_label = StuffServer->request_label(THISO);

  if( clonep() && pointerp( stack = caller_stack() ) &&
    load_name( stack[0] ) != StuffServer[0..<3] )
    logf( CREATE_LOG, sprintf( "[%s] %s cloned by:\n%O\n", ctime(),
      object_name( THISO ), caller_stack() ) );

  add( "ids", ({ "artifact", "an artifact", "artefact", "an artefact" }) );
  return;
}


string
short()
{
  string out;

  if( THISI )
    last_view = time();

  if( THISO->query_identified(THISP) & ID_ITEM )
    return ::short();
  else if( strlen( id_label ) )
    out = sprintf( "%s %s %s",
                   ( IsVowel(id_label[0]) ? "an" : "a" ),
                   id_label,
                   query( "item_name" ) );
  else
    out = "an artifact";

  return out + THISO->post_short( THISP );
}

string
post_short( object me )
{
  string out;
  out = ::post_short( me ) || "";

  if( THISO->query( "max_charges" ) > 0 &&
  THISO->query_identified( THISP ) & ID_CHARGES )
    out += sprintf(" (%d/%d)", charges, max_charges );
  if( THISO->query_identified( THISP ) & ID_STATUS )
  {
    if( THISO->query_blessed() )
      out += " [blessed]";
    else if( THISO->query_cursed() )
      out += " [cursed]";
    else
      out += " [uncursed]";
  }
  return out;
}

string
long( string id )
{
  string out;

  if( THISO->query_identified(THISP) & ID_ITEM )
    out = ::long(id);
  else
    out = format( sprintf( "It is difficult to identify the specifics of this "
                         "%s, as something seems to bend your mind from "
                         "understanding its purpose.  %s this "
                         "artifact might have unpredictable effects.\n"
                         "Etched into the artifact are the words 'Stuff, Inc.'",
                         query( "item_name" ),
                         THISO->query( "charges" ) > 0 ? "Invoking" :
                         THISO->query( WeaponP ) ? "Wielding" :
                         THISO->query( ArmorP ) ? 
                           ( THISO->query_handed() ? "Holding" : "Wearing" ) :
                         "Using" ) );
  return out;
}

int
id( string str )
{
  string lstr;
  if( str == THISO->short() )
    return 1;
  lstr = lower_case( str );
  if( lstr == "stuff" || lstr == query( "item_name" ) || lstr == "artifact" )
    return 1;
  if( ( THISO->query_identified(THISP) & ID_ITEM ) && ::id(str) )
    return 1;
  if( ( lstr == sprintf( "%s %s %s",
                      ( IsVowel(id_label[0]) ? "an" : "a" ),
                      id_label,
                      query( "item_name" ) ) ) ||
      ( lstr == sprintf( "%s %s", id_label, query( "item_name" ) ) ) )
    return 1;
  return 0;
}

mixed
set( string var, mixed val )
{
  switch( var )
  {
    case "charges":
      val <= max_charges ? charges = val : charges = max_charges;
      return 1;
    case "max_charges":
      max_charges = val;
      return 1;
    case "id_default":
      id_default = val ? 1 : 0;
      return 1;
    case "spawn_limit":
      spawn_limit = val;
      return 1;
    case "spawn_rate":
      spawn_rate = val;
      return 1;
    case "prevent_spawn":
      prevent_spawn = val;
      return 1;
    case "last_view":
      last_view = val;
      return 1;
    case "item_name":
      item_name = val;
      return 1;
    case "invoke_timeout":
      invoke_timeout = val;
      return 1;
    case CursedP:
      if( val )
        curse();
      else
        uncurse();
      return 1;
    case BlessedP:
      if( val )
      {
        bless();
        return 1;
      }
      else
        return 0;
  }
  return ::set( var, val );
}

mixed
query( string var )
{
  switch( var )
  {
    case "charges":
      return charges;
    case "max_charges":
      return max_charges;
    case "id_default":
      return id_default;
    case "spawn_limit":
      return spawn_limit;
    case "spawn_rate":
      return spawn_rate;
    case "prevent_spawn":
      return prevent_spawn;
    case "last_view":
      return last_view;
    case "item_name":
      return item_name;
    case "id_label":
      return id_label;
    case "invoke_timeout":
      return invoke_timeout;
    case StuffP:
      return 1;
    case NoStoreP:
      return 1;
    case BlessedP:
      return THISO->query_blessed();
    case CursedP:
      return THISO->query_cursed();
  }
  return ::query( var );
}

void
init()
{
  ::init();
  if( max_charges )
    add_action( "do_invoke", "invoke" );
  return;
}
