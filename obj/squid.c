/* A test object, near and dear to the hearts of whales. */

#include <config.h>

inherit OBJECT ;

void create() {
    set_short ("a squid") ;
    add_command("eat", "eat") ;
    set_long ("A real tasty squid!\n") ;
    set_id ( ({ "squid" }) ) ;
}

int eat (string str) {
	if (str!="squid") return 0 ;
	write ("You eat the squid!\n") ;
	say (this_player()->query_cap_name()+" eats a squid!\n") ;
	destruct() ;
	return 1 ;
}
