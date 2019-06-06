/* Print the cwd of this user back to him. */

int do_command() {

    string pwd, pwf ;

    pwd = previous_object()->query_cwd() ;
    if (pwd!="/") pwd = "/" + pwd ;
    write ("Current working directory: "+pwd+"\n") ;
    pwf = previous_object()->query_cwf() ;
    write ("Current working file: "+pwf+"\n") ;
    return 1 ;
}
