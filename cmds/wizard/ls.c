/* The all-important ls command.
   Mobydick, 5-29-94.
*/

int do_command (string str) {

    string directory, workdir ;
    mixed *files ;
    int i, size ;

    if (!str) {
        directory = previous_object()->query_cwd() ;
    } else {
	if (absolute_path(str)) {
	    directory = str ;
        } else {
	    directory = previous_object()->query_cwd() + "/" + str ;
	}
	directory = resolve_path(directory) ;
    }
    if (directory=="") directory = "/" ;
    i = file_exists(directory) ;
    if (!i || i==1) {
        write ("No such directory: "+directory+"\n") ;
	return 1 ;
    }
    if (!previous_object()->valid_read(directory)) {
	write ("Permission denied to ls "+directory+"\n") ;
	return 1 ;
    }
    i = strlen(directory)-1 ;
    workdir = directory ;
    if (directory[i]=='/') workdir += "*" ; else workdir += "/*" ;
    files = get_dir(workdir) ;
    write (sizeof(files[0])+" files:\n") ;
    for (i=0;i<sizeof(files[0]);i++) {
        if (files[1][i]==0)
 	    size = 0 ;
        else if (files[1][i]==-2) 
	    size = 1 ;
        else size = (files[1][i]+999)/1000 ;
        str = size + " "+files[0][i] ;
        if (files[1][i]==-2) str += "/" ;
	while (strlen(str)<16) str += " " ;
        write (str) ;
	if (i % 4 == 3) write ("\n") ;
    }
    if (sizeof(files[ 0]) % 4 >0) write ("\n") ;
    return 1 ;
}
