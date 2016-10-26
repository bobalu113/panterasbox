object ring, host;

int ring_init( object who, object what );
int ring_dest();
int destruct_signal( object dester, object destee );
int query_remove_shadow( object remover );
object ring_stuff();
object *all_ring_stuff();
object ring_stuff_shadow();
object host();
int ring_cursed();
int ring_uncursed();
int ring_blessed();


int
ring_init( object who, object what )
{
  seteuid( getuid() );
  if( !clonep() )
    return 0;
  host = who;
  ring = what;
  catch( shadow( host, 1 ) );
  if( !query_shadowing(THISO) )
    return 0;
  return 1;
}

int
ring_dest()
{
  if( !THISO )
    return 0;
  if( !clonep() )
    return 0;
  unshadow();
  if( !query_shadowing(THISO) && ring )
    ring->shadow_removed();
  destruct(THISO);
  if( THISO )
    return 0;
  return 1;
}

int
destruct_signal( object dester, object destee )
{
  if( destee != THISO )
    return host->destruct_signal( dester, destee );
  if( dester != THISO )
    ring->shadow_removed();
  return 0;
}

int
query_remove_shadow( object remover )
{
  ring_dest();
  return 1;
}

object
ring_stuff()
{
  if( THISO && query_shadowing(THISO) && ( ENV(ring) == host ) )
    return ring;
  else
  {
    ring_dest();
    return 0;
  }
}

object *
all_ring_stuff()
{
  object *stack;
  if( pointerp( stack = host->all_ring_stuff() ) )
    return ({ ring }) + stack;
  else
    return ({ ring });
}

object
ring_stuff_shadow()
{
  if( THISO && query_shadowing(THISO) && ( ENV(ring) == host ) )
    return THISO;
  else
  {
    ring_dest();
    return 0;
  }
}

object
host()
{
  if( THISO && query_shadowing(THISO) && ( ENV(ring) == host ) )
    return host;
  else
  {
    ring_dest();
    return 0;
  }
}

int
ring_cursed()
{
  if( objectp( ring ) )
    return ring->query_cursed();
  return -1;
}

int
ring_uncursed()
{
  if( objectp( ring ) )
    return ring->query_uncursed();
  return -1;
}

int
ring_blessed()
{
  if( objectp( ring ) )
    return ring->query_blessed();
  return -1;
}
