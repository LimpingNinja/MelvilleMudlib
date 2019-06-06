/* The remove file command.
   The kfun wrapper is based on previous_object() security so it is
   necessary to check permissions of the caller before allowing the
   call. */

int do_command(string file) {

    int i ;

    if (!file) {
        fail_msg ("Usage: rm <filename>.\n") ;
        return 0 ;
    }
    if (!absolute_path(file)) file = previous_object()->query_cwd()+file ;
    file = resolve_path(file) ;
    if (file_exists(file)<1) {
        write ("No such file: "+file+".\n") ;
	return 1 ;
    }
    i = previous_object()->valid_write(file) ;
    if (!i) {
        write ("Permission denied to remove "+file+"\n") ;
        return 1 ;
    }
    if (!remove_file(file)) {
        write ("Failed to remove "+file+"\n") ;
    } else {
	write ("File removed: "+file+"\n") ;
    }
    return 1 ;
}
