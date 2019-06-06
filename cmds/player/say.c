/* The say command.
   Broadcast a message to every living object in the same room.
   Written by Mobydick, 5-27-94.
*/

int do_command (string str) {
    if (!str) {
        write ("You mumble.\n") ;
	say (previous_object()->query_cap_name()+" mumbles.\n") ;
        return 1 ;
    }
    write ("You say: "+str+"\n") ;
    say (previous_object()->query_cap_name()+" says: "+str+"\n") ;
    return 1 ;
}
