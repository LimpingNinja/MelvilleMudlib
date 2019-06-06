/* The disconnect command: used to force a logged-on user to disconnect. */
/* Mobydick, 11-23-94. */

#include <config.h>

int do_command (string str) {

    object turkey ;

    if (previous_object()->query_privileges()!="admin") return 1 ;
    turkey = USERS_D->find_user(str) ;
    if (!turkey) {
	write ("There is no such user logged on.\n") ;
	return 1 ;
    }
    turkey->catch_tell("You have been forced to quit.\n") ;
    turkey = turkey->query_player() ;
    turkey->quit() ;
    write ("You have disconnected "+capitalize(str)+".\n") ;
    return 1 ;
}
