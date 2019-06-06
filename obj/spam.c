/* A test object, far from and displeasing to the hearts of whales. */

#include <config.h>

inherit CONTAINER ;

void create() {
    set_short ("a can of Spam") ;
    set_long ("A very nasty and repulsive object.\nYou think you may barf.\n");
    set_id ( ({ "spam", "can", "can of spam" }) ) ;
}
