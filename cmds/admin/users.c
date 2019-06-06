/* Object names of the current users and players. */

#include <config.h>

int do_command() {

    object *goons ;
    object body ;
    int i ;

    if (previous_object()->query_privileges()!="admin") return 1 ;
    goons = USERS_D->query_users() ;
    for (i=0;i<sizeof(goons);i++) {
	body = goons[i]->query_player() ;
	write (goons[i]->query_name()+" "+object_name(goons[i])) ;
	if (body) {
	    write (" "+object_name(body)+"\n") ;
	} else {
	    write (" No player object.\n") ;
	}
    }
	return 1 ;
}
