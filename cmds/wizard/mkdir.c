/* mkdir.c
   Make a directory.
   Mobydick, 7-10-94.
*/

int do_command (string str) {

    string dir ;

    if (!str || strlen(str)==0) {
        fail_msg ("Usage: mkdir <directory>\n") ;
	return 0 ;
    }
    if (!absolute_path(str)) {
        dir = previous_object()->query_cwd() ;
	str = dir + str ;
    }
/* Clear out any ., .., or ~ embedded in the string. */
    str = resolve_path(str) ;
    if (file_exists(str)) {
        write ("There is already a file or directory by that name.\n") ;
        return 1 ;
    }
    if (!previous_object()->valid_write(str)) {
        write ("Permission denied: "+str+"\n") ;
	return 1 ;
    }
    if (make_dir(str)) {
        write ("Directory made: "+str+"\n") ;
	return 1 ;
    }
    write ("Could not create directory: "+str+"\n") ;
    return 1 ;
}

