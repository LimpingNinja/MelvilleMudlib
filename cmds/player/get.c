/* The get command. Lets a player pick up an object.
   "get all" is not yet supported. Getting items from containers should
   probably be defined in container.c and not here.
   Mobydick, 7-15-94
*/

int do_command (string str) {

    object thing, env ;
    int res ;

    if (!str) {
        fail_msg ("Usage: get <item>\n") ;
	return 0 ;
    }
    env = previous_object()->query_environment() ;
    thing = env->present(str) ;
    if (!thing) {
        fail_msg ("I don't see a "+str+" here.\n") ;
	return 0 ;
    }

/* Check to see if the item is willing to be picked up. */
    if (thing->prevent_get()) {
	write ("You cannot pick that up.\n") ;
	return 1;
    }
    res = thing->move(previous_object()) ;
    if (!res) {
        write ("You can't pick it up.\n") ;
	return 1 ;
    }
    write ("Taken.\n") ;
    say (previous_object()->query_cap_name()+" takes "+thing->query_short()+
	".\n") ;
    return 1 ;
}
