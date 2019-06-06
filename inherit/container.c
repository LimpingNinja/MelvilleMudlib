/*
   container.c
   This file is to be inherited by any object that is capable of
   holding other objects. It handles inventory_related business,
   which is defined here rather than in the auto object.
   Do not confuse this container with a bag. Bags need to inherit this,
   but player.c and room.c (which also contain objects) also inherit
   this. There is no support for opening, closing, or locking these
   containers, since many containers do not desire these options.
   You may wish to write an inheritable bag.c if you plan on having
   many bags (or chests or other such) in your MUD.
   Begun by Mobydick, 5-25-94
*/

#include <config.h>
#include <options.h>

inherit OBJECT ;

private static object *inventory ;

/* receive_object() is called when an object attempts to move into
   the inventory of this object. A return of 1 indicates success:
   return of 0 indicates failure. */

int receive_object (object ob) {
    if (!inventory) inventory = ({ }) ;
    inventory += ({ ob }) ;
    return 1 ;
}

/* release_object() is called when an object attemps to depart from
   the inventory of this object. Return 1 permits the object to leave;
   return 0 prevents it. */

int release_object (object ob) {
   /* Don't let it go if it's not here in the first place. */
   if (!inventory || member_array(ob,inventory)==-1) return 0 ;
   inventory -= ({ ob }) ;
   return 1 ;
}

/* query_inventory() returns the inventory of this object. Right now
   I'm passing the array which is the Wrong Thing. I need to alter
   this to return a copy. */

object *query_inventory() {
    return copy(inventory) ;
}

/* The present function takes a string and returns the first object in
   the inventory of the object which takes that string as id.
   If the string passed is "me", then the body of this_user() will be
   returned if in the container. If the string is "here", then the
   container will be returned. There are no other magic words.
   Does not, as yet, grok the concept of "sword 2".
   It is more object-oriented to have this in container, where it can
   be overriden by a given container if that is desired, than to have
   it as a kfun which was the old LPC way.
*/

object present (string id) {

    int i ;

    if (!inventory || sizeof(inventory)==0) return 0 ;
    if (id=="me") {
        if (member_array(this_player(),inventory)>-1) {
            return this_player() ;
        }
        return 0 ;
    }
/* I'm not sure this is really correct, but it makes "look at here" work. */
    if (id=="here") return this_object() ;
    for (i=0;i<sizeof(inventory);i++) {
        if (inventory[i]->id(id)) return inventory[i] ;
    }
    return 0 ;
}

