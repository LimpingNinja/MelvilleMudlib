/* The shutdown command.
   Mobydick, 5-26-94.
   Moby secured it to admins and had it force users to quit. 7-9-94.
*/

#include <config.h>
#include <options.h>

int do_command (string str) {

    object *goons ;
    object body ;
    int i ;

    if (previous_object()->query_privileges()!="admin") return 1 ;
    if (!str) {
        write ("You must supply a shutdown reason.\n") ;
	return 1 ;
    }

    write ("Yes, my master.\n") ;
    log_file (SHUTDOWN_LOG,this_user()->query_cap_name()+" shut down the "+
	      "game at "+ctime(time())+" because:\n    "+str+"\n") ;
    goons = USERS_D->query_users() ;
    for (i=0;i<sizeof(goons);i++) {
	goons[i]->catch_tell("Shutting down!\n") ;
	if (body=goons[i]->query_player()) {
	    catch(body->quit()) ;
	} else {
	    catch(goons[i]->quit()) ;
	}
    }
#ifdef DUMP_STATE_AT_SHUTDOWN
    dump_state() ;
#endif
    shutdown() ;
    return 1 ;
}
