/* The brief command.
   Lets the player choose whether he sees the short or long description
   of a room when he moves into it.
   Mobydick, 5-28-94
*/

int do_command (string str) {

    int current ;

    current = previous_object()->query_brief() ;
    current = !current ;
    if (current) {
        write ("Brief mode.\n") ;
    } else {
        write ("Verbose mode.\n") ;
    }
    previous_object()->set_brief(current) ;
    return 1 ;
}
