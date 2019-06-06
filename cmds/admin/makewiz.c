/* The makewiz command.
   Promotes a user to wizard, by making a privileged call into the user
   object. Admins only, please. */

#include <config.h>

int do_command (string str) {

    object new_wiz ;

    if (previous_object()->query_privileges()!="admin") return 1 ;
    if (!str) {
	write ("Usage: makewiz <name of new wizard>\n") ;
	return 1 ;
    }
    new_wiz = USERS_D->find_user(str) ;
    if (!new_wiz) {
	write (capitalize(str)+" must be logged in to be promoted.\n") ;
	return 1 ;
    }
    new_wiz->promote_to_wizard() ;
    write ("You have promoted "+capitalize(str)+" to wizard.\n") ;
    new_wiz->catch_tell("You have just been promoted to wizard! Congratulations!\n") ;
    return 1 ;
}
