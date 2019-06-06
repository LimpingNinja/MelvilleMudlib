/* This demonstrates heartbeats. */

#include <config.h>

inherit OBJECT ;

int i ;

void create() {
    set_short ("a clock") ;
    set_long ("An annoying little clock.\n") ;
    set_id(({"clock"})) ;
    if (!clone_num(this_object())) return ;
    set_heart_beat(5) ;
}

void heart_beat() {

    object foo ;

    foo = query_environment() ;
    set_heart_beat(2) ;
    i = !i ;
    if (i) foo->catch_tell("tick\n") ; else foo->catch_tell("tock\n") ;
}

