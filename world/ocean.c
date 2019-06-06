/*
   ocean.c
   The ocean room. Used for testing movement.
   Mobydick, 5-28-94. 
*/

#include <config.h>

inherit ROOM ;

void create() {
    set_short ("Swimming in the ocean") ;
    set_long (
"You are swimming deep below the ocean waves, among the fish and the whales.\n"
        );
    set_exits ( ([ "north" : "/world/start" ]) ) ;
}
