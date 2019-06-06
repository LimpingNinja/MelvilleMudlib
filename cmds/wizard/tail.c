/* look at the end of a file. */
/* Moby, 7-21-94 */

#define TAIL_LINES 20

int do_command (string str) {

    string *lines ;
    int i, start ;

    if (!str) {
	fail_msg ("Usage: tail <filename>\n") ;
	return 0;
    }
    if (!absolute_path(str)) {
	str = previous_object()->query_cwd()+str ;
    }
/* Clear out any ., .., or ~ in the string. */
    str = resolve_path(str) ;
    if (file_exists(str)<1) {
	write ("There is no such file.\n") ;
	return 1 ;
    }
    if (!previous_object()->valid_read(str)) {
	write ("You don't have permission to read that file.\n") ;
        return 1 ;
    }
    lines = explode(read_file(str),"\n") ;
    if (sizeof(lines)<TAIL_LINES) {
	start = 0 ;
    } else {
	start = sizeof(lines)-TAIL_LINES ;
    }
    write ( ":: "+str+" ::\n") ;
    for (i=start;i<sizeof(lines);i++) {
	write (lines[i]+"\n") ;
    }
    return 1 ;
}
