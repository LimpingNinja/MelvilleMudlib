/* Show the privileges and creator of a named object. */

int do_command (string str) {

    string creator ;
    string priv ;
    object foo, env ;

    if (!str) {
	write ("Usage: priv <object>\n") ;
	return 1 ;
    }
    env = previous_object()->query_environment() ;
    foo = env->present(str) ;
    if (!foo) foo = previous_object()->present(str) ;
    if (!foo) foo = get_object(str) ;
    if (!foo) {
	write ("Could not find "+str+"\n") ;
	return 1 ;
    }
    if (foo) str = object_name(foo) ;
    creator = call_other(str,"query_creator") ;
    priv = call_other(str,"query_privileges") ;
    write ("The creator is "+creator+". ") ;
    write ("The privileges are "+priv+".\n") ;
    return 1 ;
  }
