/* Show the files names of all objects in the wizard's environment.
   A temporary command unless I expand it later. */

int do_command (string str) {

    object env ;
    object *contents ;
    int i ;

    env = previous_object()->query_environment() ;
    if (!env) {
        write ("Can't do that in the void.\n") ;
	return 1 ;
    }
    contents = env->query_inventory() ;
    /* There has to be contents since the player is there. */
    for (i=0;i<sizeof(contents);i++) {
        write (object_name(contents[i])+"\n") ;
    }
    return 1 ;
}
