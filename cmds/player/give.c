/* The give command. Lets a player give an object to another.
   Mobydick, 7-16-94
*/

int do_command (string str) {

    object thing, target, env ;
    string name, item, short ;
    int res ;

    if (!str || sscanf(str,"%s to %s",item,name)!=2) {
        fail_msg ("Usage: give <item> to <player>\n") ;
	return 0 ;
    }
    thing = previous_object()->present(item) ;
    if (!thing) {
        fail_msg ("You don't have a "+item+" to give away.\n") ;
	return 0 ;
    }
    env = previous_object()->query_environment() ;
    target = env->present(name) ;
    if (!target || !target->query_user()) {
	fail_msg ("There is no one named "+capitalize(name)+" to give it to.\n") ;
	return 0 ;
    }
    res = thing->move(target) ;

    if (!res) {
	write (capitalize(name)+" cannot carry it.\n") ;
	return 1 ;
    }
    short = thing->query_short() ;
    write ("You give "+short+" to "+capitalize(name)+".\n") ;
    name = previous_object()->query_cap_name() ;
    target->catch_tell(name+" gives you "+short+".\n") ;
    say (name+" gives "+target->query_cap_name()+" "+short+".\n",
	({ target }) ) ;
    return 1 ;
}
