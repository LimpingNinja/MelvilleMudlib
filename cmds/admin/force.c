/* The force command.
   Mobydick, 6-85-98.
   Lets an admin force another user to carry out a specified command.
   You can't force other admins, however; that's Unwise.
*/

#include <config.h>

int do_command (string str) {

    object forced_user, forced_player ;
    string who, command ;
    int i ;

    if (previous_object()->query_privileges()!="admin") return 1 ;
    if (!str || sscanf(str,"%s to %s",who,command)!=2) {
	fail_msg ("Usage: force <user> to <command>") ;
	return 0 ;
    }
    forced_user = USERS_D->find_user(who) ;
    if (!forced_user) {
	write ("There is no user named "+capitalize(who)+" logged in.\n") ;
	return 1 ;
    }
    forced_player = forced_user->query_player() ;
    if (!forced_player) {
	write (capitalize(who)+" has no body!\n") ;
	return 1 ;
    }
    if (forced_player==previous_object()) {
	write ("Self-forcing is not permitted.\n") ;
	return 1 ;
    }
    if (forced_player->query_privileges()=="admin") {
	write ("You may not force other admins.\n") ;
	return 1 ;
    }
    forced_player->catch_tell(capitalize(previous_object()->query_name())+
	" forces you to "+command+".\n") ;
    write ("You force "+capitalize(who)+" to: "+command+"\n") ;
    USERS_D->set_this_player(forced_player) ;
    forced_player->command(command) ;
    USERS_D->set_this_player(previous_object()) ;
    return 1 ;
}
