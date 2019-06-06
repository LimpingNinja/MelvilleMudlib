/*
   void.c
   The void. Used as a temporary place to stuff objects if their own
   environment is temporarily not available (for instance, being updated).
   Mobydick, 5-28-94. 
*/

#include <config.h>

inherit ROOM ;

void create() {
    set_short ("In a disturbing void") ;
    set_long (
"There is nothingness all around you and you cannot stand it!\n") ;
    set_exits ( ([ "start" : "/world/start" ]) ) ;
}
