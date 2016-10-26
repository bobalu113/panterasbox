int forced;

void
create()
{
  enable_commands();
  return;
}

void force_id( int what ) { forced = what; }

void stuff_id_check( int out )
{
  out = forced;
  return;
}
