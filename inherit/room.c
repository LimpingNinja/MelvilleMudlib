/*
   room.c
   Any object which is meant to contain players should inherit room.c.
   It defines exits for rooms which players can move through. It also
   has an altered short and long that shows those exits, and the long
   shows the contents of the room as well. There is also a room_tell()
   lfun which you can call to send a message to all players in that 
   room.
   Begun by Mobydick, 5-25-94.
*/

#include <config.h>

inherit CONTAINER ;

mapping exits ;

int catch_tell (string str) ;
  
void set_exits (mapping ex) {
    exits = ex ;
}

void add_exit (string direction, string file) {
    exits += ([ direction : file ]) ;
}

mapping query_exits() {
    return copy(exits) ;
}

string query_exit (string direction) {
    return exits[direction] ;
}

/* You may not pick up your surroundings. Basic theorem in topology. */

int prevent_get() {
    return 1 ;
}

varargs string query_long (int brief) {

    string value, list, *all_but_last, *frog ;
    object *inventory ;
    int i ;

/* Start with whatever desc the room coder supplied. Give the short
   or the long desc, according to the brief argument. */
    if (brief) value = short_desc+"\n" ; else value = long_desc ;
/* Add in any exits there may be. */
    if (!exits || map_sizeof(exits)==0) {
        value += "There are no obvious exits.\n" ;
    } else {
        if (map_sizeof(exits)==1) {
	    list = map_indices(exits)[0] ;
	    value += "The only obvious exit is "+list+".\n" ;
        } else {
	    if (map_sizeof(exits)==2) {
	        list = map_indices(exits)[0]+" and "+map_indices(exits)[1] ;
	    } else {
	      frog = map_indices(exits) ;
	      all_but_last = frog[0..sizeof(frog)-2] ;
	      list = implode(all_but_last,", ") ;
	      list += ", and "+frog[sizeof(frog)-1] ;
	    }
	    value += "Obvious exits are "+list+".\n" ;
        }
    }
/* List the contents of the room. */
    inventory = query_inventory() ;
    if (inventory && sizeof(inventory)>0) {
        for (i=0;i<sizeof(inventory);i++) {
       /* Make sure there isn't an empty item in the inventory somehow. */
	    if (!inventory[i]) continue ;
       /* Don't include our own body in the list. */
	    if (inventory[i]->query_user()==this_user()) continue ;
	    list = inventory[i]->query_short() ;
	    if (list) {
	        value += "   "+capitalize(list)+"\n" ;
            }
        }
    }
    return value ;
}

/* room_tell, analagous to catch_tell, catches a message and passes
   it on to every living object in the room except excluded ones. */

varargs void room_tell (string str, object *excluded) {

    int i ;
    object *inventory ;

    if (!catch_tell(str)) return ;
    inventory = query_inventory() ;
    if (!inventory || sizeof(inventory)==0) return ;
    for (i=0;i<sizeof(inventory);i++) {
        if (!inventory[i]->query_living()) continue ;
        if (excluded && member_array(inventory[i],excluded)>-1) continue ;
        inventory[i]->catch_tell(str) ;
    }
}

/* catch_tell is called from say() and from room_tell() above. It can
   return 0 if it wants to prevent the message from being echoed to the
   room. */

int catch_tell (string str) {
    return 1 ;
}
