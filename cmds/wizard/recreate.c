/* The update command. Find the named object, dest it, then call_other
   to reload it.
   Mobydick, 6-3-94, getting tired of shutting down all the time. 
*/

#include <config.h>

int do_command (string str) {

    int len, i ;
    object obj ;
    object *stuff ;
    string bigname, where ;

    if (!str) {
        str = this_player()->query_cwf() ;
	if (!str) {
	    fail_msg ("Usage: update <filename>\n") ;
	    return 0 ;
	}
    }
/* This operation requires a version of the string without the .c
   (for finding the already-existing object, if there is one) and
   one with the .c (for checking the existence of the file). str is
   without the .c and bigname is with the .c. Since the user could
   have passed either of the two, and we want to handle it gracefully
   either way, first we remove .c from str if it's there, then we
   add .c to it to produce bigname.
*/
    if (strlen(str)>2 && str[strlen(str)-2..strlen(str)-1]==".c") {
        str = str[0..strlen(str)-3] ;
    }
    if (!absolute_path(str)) str = previous_object()->query_cwd()+str ;
    str = resolve_path(str) ;
    bigname = str ;
    len = strlen(str) ;
    if (bigname[len-2..len-1]!=".c") bigname += ".c" ;
    if (file_exists(bigname)!=1) {
        write ("No such file: "+bigname+"\n") ;
	return 1 ;
    }
    this_player()->set_cwf(bigname) ;

/* Before we do anything, we need to stash the contents elsewhere. */
    obj = find_object(str) ;
    if (obj) {
      stuff = obj->query_inventory() ;
      if (str==VOID) {
	where = START ;
      } else {
	where = VOID ;
      }
      if (stuff && sizeof(stuff)>0) {
	for (i=0;i<sizeof(stuff);i++) {
	    stuff[i]->move(where) ;
	}
      }
    }

    obj->destruct() ;
    if (obj) {
      write ("Destruction failed. Recreation aborted!\n") ;
      return 1 ;
    }
    compile_object(str) ;

/* Now we put everything back. We can't do this until the update is
   finished, so we use a call_out(0) to get around it.
*/
    if (stuff && sizeof(stuff)>0) {
        call_out ("replace_stuff",0,stuff,str) ;
    }

    write ("Updated and loaded: "+str+"\n") ;
    return 1 ;
}

void replace_stuff (object *stuff, string str) {

    int i ;

   for (i=0;i<sizeof(stuff);i++) {
	stuff[i]->move(str) ;
    }
}
