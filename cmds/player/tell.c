/* The tell command. Used to send a message to another player. */

#include <config.h>

int do_command (string str) {

    string name, message ;
    object target ;

    if (sscanf(str,"%s %s",name,message)!=2) {
	fail_msg ("Usage: tell <name> <message>\n") ;
	return 0 ;
    }
    target = USERS_D->find_user(name) ;
    if (!target) {
	fail_msg (capitalize(name)+" is not logged on right now.\n") ;
	return 0 ;
    }
    target = target->query_player() ;
    if (target==previous_object()) {
	fail_msg ("You can't talk to yourself.\n") ;
	return 0 ;
    }
    write ("You tell "+capitalize(name)+": "+message+"\n") ;
    name = previous_object()->query_name() ;
    target->catch_tell(capitalize(name)+" tells you: "+message+"\n") ;
    return 1 ;
}
