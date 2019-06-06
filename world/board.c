/* The start room bulletin board. */

#include <config.h>

inherit BOARD ;

void create() {
    set_short("Start room board") ;
    set_long (
"This board lets you 'post' notes which other wizards can then 'read'. You\n"+
"can also 'remove' any notes you wrote. The board can store up to 30 notes.\n");
    move("/world/start") ;
    ::create() ;
}
