/* rmdir.c
   Remove a directory.
   Mobydick, 7-10-94.
*/

int do_command (string str) {

    string dir ;

    if (!str || strlen(str)==0) {
        fail_msg ("Usage: rmdir <directory>\n") ;
	return 0 ;
    }
    if (!absolute_path(str)) {
        dir = previous_object()->query_cwd() ;
	str = dir + str ;
    }
    str = resolve_path(str) ;
    if (file_exists(str)>-1) {
        write ("There is no directory by that name.\n") ;
        return 1 ;
    }
    if (!previous_object()->valid_write(str)) {
        write ("Permission denied: "+str+"\n") ;
	return 1 ;
    }
    if (remove_dir(str)) {
        write ("Directory removed: "+str+"\n") ;
	return 1 ;
    }
    write ("Could not remove directory: "+str+"\n") ;
    return 1 ;
}

