/*
   start.c
   The start room.
   Mobydick, 5-25-94. We'll see how this goes.
*/

#include <config.h>

inherit ROOM ;

void create() {
    object squid ;
    set_short ("The start room") ;
    set_long (
"You are in a tiny cube with blank walls, and no doors or windows.\n"
        );
    set_exits ( ([ "south" : "/world/ocean" ]) ) ;
    squid = clone_object("/obj/squid") ;
    squid->move(this_object()) ;
/* Load the board object. Because it's not a clone, DGD does not call
   create() in it automatically, and so we need to call create in it
   ourselves. If create() gets called twice no harm will be done. */
    get_object("/world/board")->create() ;
}
