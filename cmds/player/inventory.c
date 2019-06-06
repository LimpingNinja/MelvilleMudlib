/* The inventory command. Show a player the short of everything in his
   possession. 
   Mobydick, 7-14-94.
*/

int do_command() {

    object *stuff ;
    string short ;
    int i ;

    stuff = previous_object()->query_inventory() ;
    if (!stuff || sizeof(stuff)==0) {
        write ("You are empty-handed.\n") ;
	return 1 ;
    }
    write ("You are carrying:\n") ;
    for (i=0;i<sizeof(stuff);i++) {
        short = stuff[i]->query_short() ;
/* If it has no short it shouldn't show up in your inventory list. */
	if (!short || strlen(short)==0) continue ;
	write ("  "+capitalize(short)+"\n") ;
    }
    return 1 ;
}
