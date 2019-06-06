/* The clone command. Take a filename, clone a copy of it, and move it
   to the cloning wizard's inventory.
   Written by Mobydick, 7-13-94
*/

int do_command (string file) {

    object ob ;
    int res ;

    if (!file) {
	file = this_player()->query_cwf() ;
	if (!file) {
            fail_msg ("Usage: clone <filename>\n") ;
	    return 0 ;
	}
    }
    if (!absolute_path(file)) file = previous_object()->query_cwd()+file ;
    file = resolve_path(file) ;
    if (file[strlen(file)-2..strlen(file)-1]!=".c") file += ".c" ;
    if (file_exists(file)<1) {
        write ("No such file: "+file+"\n") ;
	return 1 ;
    }
/* First, ob is the master object (get_object() loads it if that is
   necessary), then ob becomes the new clone.
*/
    ob = get_object(file) ;
    ob = clone_object(ob) ;
    if (!ob) {
        write ("Failed to clone "+file+"\n") ;
	return 1 ;
    }
    res = ob->move(previous_object()) ;
    if (res==0) {
        write ("Could not move object to your inventory.\n") ;
	res = ob->move(previous_object()->query_environment()) ;
	if (res==0) {
	    write ("Could not move it to your environment. Aborting clone.\n");
	    return 1 ;
	}
	write (file+" cloned to your environment.\n") ;
	say (previous_object()->query_cap_name()+" creates "+ob->query_short()+
	      ".\n") ;
	return 1 ;
    }
    write (file+" cloned.\n") ;
    say (previous_object()->query_cap_name()+" creates "+ob->query_short()+
	   ".\n") ;
    return 1 ;
}
