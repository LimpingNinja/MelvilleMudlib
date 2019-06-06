/* The copy file command.
   The copy_file auto function is based on previous_object() security so
   it is necessary to check permissions of the caller before allowing the
   call. */

int do_command(string file) {

    int i ;
    string old, new, filename ;
    string *tmp ;

    if (!file || sscanf(file,"%s %s",old,new)!=2) {
        fail_msg ("Usage: cp <old filename> <new filename>\n") ;
        return 0 ;
    }
    if (!absolute_path(old)) old = previous_object()->query_cwd()+old ;
    old = resolve_path(old) ;
    if (!file_exists(old)) {
        write ("No such file or directory: "+old+".\n") ;
	return 1 ;
    }
/* Break out the actual file name from the whole path. We may need it below. */
    tmp = explode(old,"/") ;
    filename = tmp[sizeof(tmp)-1] ;
    if (!absolute_path(new)) new = previous_object()->query_cwd()+new ;
    new = resolve_path(new) ;
/* If new is a directory, add the filename to the end. Thus "mv foo.c /cmds"
   will move foo.c to /cmds/foo.c. */
    if (file_exists(new)==-1) new = new + "/" + filename ;
    if (file_exists(new)) {
        write ("Cannot overwrite existing file: "+new+"\n") ;
        return 1 ;
    }
    i = previous_object()->valid_read(old) ;
    if (!i) {
        write ("Permission denied to read "+old+"\n") ;
        return 1 ;
    }
    i = previous_object()->valid_write(new) ;
    if (!i) {
        write ("Permission denied to create "+new+"\n") ;
        return 1 ;
    }
    if (!copy_file(old,new)) {
        write ("Failed to copy "+old+" to "+new+"\n") ;
    } else {
        write (old+" copied to "+new+"\n") ;
    }
    return 1 ;
}
