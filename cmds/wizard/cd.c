/* The change directory command.
   Mobydick started on it, 6-3-94. */

int do_command (string dir) {

    if (!dir) {
        dir = "/users/"+previous_object()->query_name() ;
        if (file_exists(dir)!=-1) {
	    dir = "/" ;
	}
    } else {
	if (!absolute_path(dir)) {
	    dir = previous_object()->query_cwd()+dir+"/" ;
	}
    }
    dir = resolve_path(dir) ;
    if (!dir) {
        write ("Illegal path.\n") ;
	return 1 ;
    }
    if (previous_object()->set_cwd(dir)) {
        write ("Current working directory: /"+dir+"\n") ;
        return 1 ;
    }
    write ("No such directory: "+dir+"\n") ;
    return 1 ;
}
