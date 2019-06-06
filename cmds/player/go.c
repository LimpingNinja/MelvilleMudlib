/* The go command.
   "go direction" tries to take you through the direction exit.
   "n" is aliased to "go north", and so on for the four cardinal
   directions.
   Mobydick, 5-28-94
*/

int do_command (string str) {

    mapping exits ;
    int result ;

    if (!str) return 0 ;
    exits = previous_object()->query_environment()->query_exits() ;
    if (!exits || !exits[str]) {
        fail_msg ("You can't go that way from here.\n") ;
	return 0 ;
    }
    result = previous_object()->move(exits[str],str) ;
    if (!result) {
        write ("You remain where you are.\n") ;
	return 1 ;
    }
    return 1 ;
}
