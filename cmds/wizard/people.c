/* A quaint little people command.
   mobydick, 7-10-94. */

#include <config.h>

int do_command() {

    string name, room, idlestr, ip, rank ;
    int idle, i ;
    object *users ;

    users = USERS_D->query_users() ;
    write ("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    write ("Name            Rank     Idle    IP             Location\n") ;
    write ("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    for (i=0;i<sizeof(users);i++) {
	name = users[i]->query_cap_name() ;
	rank = capitalize(users[i]->query_privileges()) ;
	rank = pad(rank,9) ;
	if (users[i]->query_in_edit()) name = "["+name+"]" ;
	name = pad(name,16) ;
	idle = users[i]->query_idle()/60 ;
	if (idle) idlestr = idle+"m" ; else idlestr ="" ;
	idlestr = pad(idlestr,8) ;
        ip = query_ip_number(users[i]) ;
	if (!ip) ip = "<netdead>" ;
	ip = pad(ip,15) ;
	room = object_name(users[i]->query_player()->query_environment()) ;
	write (name+rank+idlestr+ip+room+"\n") ;
    }
    write ("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    return 1 ;
}
