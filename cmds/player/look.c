/*
   look.c
   The basic look command.
   Written by Mobydick, 5-26-94.
*/

int do_command (string str) {

    string arg ;
    object env, thing ;

    /* Can we see? */
    if (!previous_object()->query_vision()) {
        write ("It is dark and you can't see a thing.\n") ;
	return 1 ;
    }

/* If no argument was passed, then we're looking in our room. */
    if (!str) {
        env = previous_object()->query_environment() ;
        if (!env) {
	    write ("You are in the void and can see nothing.\n") ;
	    return 1 ;
        }
        write (env->query_long()) ;
        return 1 ;
    }

/* Was an argument passed? If so, find an object that matches that
   string and return its long description. */
    if (sscanf(str,"at %s",arg)==1) {
	env = previous_object()->query_environment() ;
        thing = env->present(arg) ;
	if (thing) {
	    write (thing->query_long()) ;
	    return 1 ;
	}
	thing = previous_object()->present(arg) ;
	if (thing) {
	    write (thing->query_long()) ;
	    return 1 ;
	}
        write ("I don't see "+article(arg)+" "+arg+" here.\n") ;
        return 1 ;
    }

/* Give up. */
    return 0 ;
}
