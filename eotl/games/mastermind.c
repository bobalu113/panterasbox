inherit ObjectCode;
 
string number;
object guy;
mapping hist;
 
void extra_create()
{
  set("short", "a mastermind game" );
  set("long", "This is a number game, a lot like Mastermind.  It is brown.\n"
       "Either \"mmstart <digits>\" or \"mmguess <number>\""
       "\n\nOhyeah, you can \"mmguess /hist\" now.");
  set("id", ({ "mastermind", "mm", "mmgame", "game", "number game" }));
  set("gettable", 1);
  set("droppable", 1);
  set("value", 0);
  return;
}
 
void extra_init()
{
  add_action("do_start", "mmstart");
  add_action("do_guess", "mmguess");
  return;
}
 
status do_start(string str);

void restart(string str, string extra)
{
  if(str[0]=='y' || str[0]=='Y')
  {
    guy = 0;
    do_start(extra);
  }
  return;
}
 
 
do_start(string str)
{
  int digits;
  if(!str || str=="")
  {
    notify_fail("Start what?");
    return 0;
  }
  if((digits = to_int(str)) < 1 || digits > 9 )
  {
    notify_fail("That is not a valid number of digits.\n");
    return 0;
  }
  if(guy)
  {
    if(THISP!=guy)
    {
      notify_fail(sprintf("%s is playing the game right now.  Wait till %s's "
        "done.\n", guy->query_name(), subjective(guy)));
      return 0;
    }
    write("You already are playing the game.  Start over? [y/n]  ");
    input_to("restart", 0, str);
    return 1;
  }
  guy = THISP;
  number = sprintf("%*'0'd", digits, 
             random(to_int(exp(digits*log(to_float(10))))));
  hist = ([ ]);
  tell_room(ENV(THISP), PNAME + " starts the number game.\n", ({ THISP }));
  write("You start the number game.\n");
  return 1;
} 
 
void do_hist()
{
  string *guesses, out;
  int i;
  guesses = m_indices(hist);
  out = sprintf("%*-s   %19|s    %17|s\n", MAX(strlen(number), 5), 
             "Guess", "Incorrect Positions", "Correct Positions");
  for(i=0;i<sizeof(guesses);i++)
    out += sprintf("%*-s   %19|d   %17|d\n", MAX(strlen(number), 5),
             guesses[i], hist[guesses[i], 0], hist[guesses[i], 1]);
  THISP->more_string(out);
  return;
}

do_guess(string str)
{
  int count, pos, i, tmp;
  string copy;
  if(!guy)
  {
    notify_fail("You must <start \"digits\"> first.\n");
    return 0;
  }
  if(THISP!=guy)
  {
    notify_fail(sprintf("%s is playing the game right now.  Wait till %s's "
      "done.\n", guy->query_name(), subjective(guy)));
    return 0;
  } 
  if(!str || str=="")
  {
    notify_fail("guess <number>\n");
    return 0;
  }
  if(str == "/hist")
  {
    do_hist();
    return 1;
  }
  if(!to_int(str))
  {
    notify_fail("That is not a number.\n");
    return 1;
  }
  if(strlen(str)!=strlen(number))
  {
    printf("The number must be %d digits long.\n", strlen(number));
    return 1;
  }
  if(str==number)
  {
    printf("You win!\nThat took you %d guesses.\n", sizeof(hist));
    guy = 0; hist = ([ ]); number = "";
    return 1;
  }
  for(i=0,copy = number; i<strlen(str) ;i++)
  {
    if(str[i]==number[i]) 
      pos++;
    if((tmp=searcha(to_array(copy), str[i]))!=-1)
    {
      count++;
      copy = deletea(copy, tmp);
    }
  }
  count -= pos;
  hist += ([ str : count; pos ]);
  printf("Incorrect Positions: %d\nCorrect Positions: %d\n", count, pos);
  return 1;
}
