/* The rename file command.
   The kfun wrapper is based on previous_object() security so it is
   necessary to check permissions of the caller before allowing the
   call. */

int do_command(string file) {

    int i ;
    string old, new ;
    string *tmp ;
    string filename ;

    if (!file || sscanf(file,"%s %s",old,new)!=2) {
        fail_msg ("Usage: mv <old filename> <new filename>\n") ;
        return 0 ;
    }
    if (!absolute_path(old)) old = previous_object()->query_cwd()+old ;
/* Clear out any ., .., or ~ in the string. */
    old = resolve_path(old) ;
    if (!file_exists(old)) {
        write ("No such file or directory: "+old+".\n") ;
	return 1 ;
    }
/* Break the old file name apart by / and save the last bit, which is
   the actual name of the file. We may need this below.
*/
    tmp = explode(old,"/") ;
    filename = tmp[sizeof(tmp)-1] ;

/* Fix up the new path. */
    if (!absolute_path(new)) new = previous_object()->query_cwd()+new ;
    new = resolve_path(new) ;
/* If the new location is a directory, then add the old file name onto
   the end of it. Thus: "mv foo.c /cmds" will move foo.c to /cmds/foo.c.
*/
    if (file_exists(new)==-1) new = new+"/"+filename ;
    if (file_exists(new)) {
        write ("Cannot overwrite existing file: "+new+"\n") ;
        return 1 ;
    }
    i = previous_object()->valid_write(old) ;
    if (!i) {
        write ("Permission denied to remove "+old+"\n") ;
        return 1 ;
    }
    i = previous_object()->valid_write(new) ;
    if (!i) {
        write ("Permission denied to create "+new+"\n") ;
        return 1 ;
    }
    if (!rename_file(old,new)) {
        write ("Failed to move "+old+" to "+new+"\n") ;
    } else {
        write (old+" moved to "+new+"\n") ;
    }
    return 1 ;
}
