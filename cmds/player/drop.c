/* The drop command. Lets a player pick up an object.
   "drop all" is not yet supported. Putting items in containers should
   probably be defined in container.c and not here.
   Mobydick, 7-15-94
*/

int do_command (string str) {

    object thing ;
    int res ;


    if (!str) {
        fail_msg ("Usage: drop <item>\n") ;
	return 0 ;
    }
    thing = previous_object()->present(str) ;
    if (!thing) {
        fail_msg ("You don't possess a "+str+".\n") ;
	return 0 ;
    }
    res = thing->move(previous_object()->query_environment()) ;
    if (!res) {
        write ("You can't bring yourself to drop it.\n") ;
	return 1 ;
    }
    write ("Dropped.\n") ;
    say (previous_object()->query_cap_name()+" drops "+thing->query_short()+
	".\n") ;
    return 1 ;
}
