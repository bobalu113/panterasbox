object amulet, host;

int amulet_init( object who, object what );
int amulet_dest();
int destruct_signal( object dester, object destee );
int query_remove_shadow( object remover );
object amulet_stuff();
object amulet_stuff_shadow();
object host();
int amulet_cursed();
int amulet_uncursed();
int amulet_blessed();


int
amulet_init( object who, object what )
{
  if( !clonep() )
    return 0;
  host = who;
  amulet = what;
  catch( shadow( host, 1 ) );
  if( !query_shadowing(THISO) )
    return 0;
  return 1;
}

int
amulet_dest()
{
  if( !clonep() )
    return 0;
  if( !THISO )
    return 0;
  unshadow();
  if( !query_shadowing(THISO) && amulet )
    amulet->shadow_removed();
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
    amulet->shadow_removed();
  return 0;
}

int
query_remove_shadow( object remover )
{
  amulet_dest();
  return 1;
}

object
amulet_stuff()
{
  if( THISO && query_shadowing(THISO) && ( ENV(amulet) == host ) )
    return amulet;
  else
  {
    amulet_dest();
    return 0;
  }
}

object
amulet_stuff_shadow()
{
  if( THISO && query_shadowing(THISO) && ( ENV(amulet) == host ) )
    return THISO;
  else
  {
    amulet_dest();
    return 0;
  }
}

object
host()
{
  if( THISO && query_shadowing(THISO) && ( ENV(amulet) == host ) )
    return host;
  else
  {
    amulet_dest();
    return 0;
  }
}

int
amulet_cursed()
{
  if( objectp( amulet ) )
    return amulet->query_cursed();
  return -1;
}

int
amulet_uncursed()
{
  if( objectp( amulet ) )
    return amulet->query_uncursed();
  return -1;
}

int
amulet_blessed()
{
  if( objectp( amulet ) )
    return amulet->query_blessed();
  return -1;
}
