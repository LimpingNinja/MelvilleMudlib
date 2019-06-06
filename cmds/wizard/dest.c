/* The dest command. Take a string arg, find an object in the wizard's
   inventory or environment that has that ID, and destruct it.
   Written by Mobydick, 7-15-94
*/

int do_command (string str) {

    object ob, env ;
    int res ;
    string short ;

    if (!str) {
        fail_msg ("Usage: dest <filename>\n") ;
	return 0 ;
    }
    ob = previous_object()->present(str) ;
    if (!ob) { 
        env = previous_object()->query_environment() ;
        ob = env->present(str) ;
    }
    if (!ob) {
        write ("Failed to find object: "+str+"\n") ;
	return 1 ;
    }
/* Gotta get the short before we destruct it. */
    short = ob->query_short() ;
    res = ob->destruct(previous_object()) ;
    if (res==0) {
        write ("Could not destruct that object.\n") ;
	return 1 ;
    }
    write ("You destroy "+short+".\n") ;
    say (previous_object()->query_cap_name()+" destroys "+short+".\n") ;
    return 1 ;
}
