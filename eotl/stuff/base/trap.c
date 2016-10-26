//  Stuff Traps

inherit TrapCode;
int last_view;
#define CREATE_LOG StuffLogDir "create.log"

void create()
{
    object *stack;

    ::create();
    set( StuffP, 1 );
    set( TrapP, 1 );

    set( "prevent_spawn", ({ ShopP, LivingP, ContainerP, NoCombatP }) +
         ( query( "prevent_spawn" ) || ({}) ) );
    last_view = time();

    if( !query( "trap_name" ) )
    {
        set( "trap_name", "a squeaky board" );
        add( "id", "board" );
        add( "id", "squeaky board" );
        set( "long", "This wooden board is unusually squeaky." );
        set( "hidden", 1 );
        set( "ready", 1 );
        set( "reset_on_trip", 1 );
        set( "hide_on_trip", 1 );
        set( "difficulty", 1 );
        set( "repeating", 1 );
        set( "spawn_limit", 3 );
    } 

    // If the server can't register our item, give up right away.
    if( !StuffServer->push_item(THISO) )
        destruct(THISO);

    if( clonep() && pointerp( stack = caller_stack() ) &&
        load_name( stack[0] ) != StuffServer[0..<3] )
        logf( CREATE_LOG, sprintf( "[%s] %s cloned by:\n%O\n", ctime(),
            object_name( THISO ), caller_stack() ) );
}

void effect( object tripper )
{
    tell_room( ENV(tripper), "A board squeaks loudly.\n",
        ({ tripper }) );
    tell_object( tripper, "A board squeaks beneath you.\n" );
}

void init()
{
  //  stupid combat causing players to flee before init() 
  if( !THISP || 
      ( !present( THISO, ENV(THISP) && !present( THISO, THISP ) ) ) )
    return;
    ::init();
    if( THISP && is_player( THISP ) )
        last_view = time();
}

void reset()
{
    if( ( time() - last_view ) > ( 7200 ) )
        StuffServer->pull_item(THISO);
}

// Catches destruction and tells the server to pull it first.
int
destruct_signal( object dester )
{
    StuffServer->pull_item( THISO );
    return 0;
}

// The server calls this every reset to see if it can safely dest the
// object after it's been pulled from the game.  Return 0 to prevent
// destruction.
int
remove()
{
  if( ENV(THISO) != FINDO(StuffServer) )
    return 0;
  if( member( heart_beat_info(), THISO ) != -1 )
    return 0;
  if( member( map( call_out_info(), #'[, 0 ), THISO ) != -1 )
    return 0;
  return 1;
}

// This function is called routinely by the StuffServer to ensure that
// inactive items still have reset() invoked by the driver.
nomask int
touch()
{
  return 1;
}

int do_dismantle( string arg )
{
    if( query( "hidden" ) )
      return 0;
    if( !THISO->id( arg ) )
    {
      write( "Dismantle what?\n" );
      return 1;
    }

    // slightly different than normal TrapCode, you can successfully
    // dismantle a Stuff trap even when it's "ready" (though you do 
    // still suffer the consequences)
    if( query( "ready" ) )
    {
      if( stringp( query( "trip_message" ) ) )
        tell_room( ENV( THISP ), query( "trip_message" ) );
      if( !query( "time_delay" ) )
        do_effect( THISP );
      else
        call_out( "do_effect", query( "time_delay" ), THISP );

    }
    write( "You dismantle " + query( "trap_name" ) + ".\n" );
    StuffServer->pull_item( THISO );
    return 1;
}

string post_long()
{
    string out;
    out = ::post_long() || "";
    out += "The words 'Stuff, Inc.' are scrawled beside the trap.\n";
    return out;
}
