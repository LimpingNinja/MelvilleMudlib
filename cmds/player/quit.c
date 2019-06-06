/*
   quit.c
   Lets a player quit the game.
   We don't want this called except from a player object.
*/

#include <config.h>

int do_command (string str) {
    if (str) return 0 ;
    if (base_name(previous_object()) != PLAYER) return 0 ;
    write ("See you again soon!\n") ;
    previous_object()->quit() ;
    return 1 ;
}
