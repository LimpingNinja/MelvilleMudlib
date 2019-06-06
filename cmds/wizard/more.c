/* The more command.
   Still rather primitive.
   Begun by Mobydick, 6-18-94.
*/

int do_command (string str) {
    if (!str) return 0 ;
    if (!absolute_path(str)) { 
	str = previous_object()->query_cwd() + str ;
    }
/* Clear out any ., .., or ~ in the string. */
    str = resolve_path(str) ;
    if (file_exists(str)<1) {
	fail_msg ("No such file: "+str+"\n") ;
	return 0 ;
    }
    if (!previous_object()->valid_read(str)) {
	write ("Permission denied to read "+str+"\n") ;
	return 1 ;
    }
    write (":: "+str+" ::\n") ;
    previous_object()->more_file(str) ;
    return 1 ;
}
