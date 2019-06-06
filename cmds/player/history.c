/* A history command. New in Melville 0.9.
   Mobydick, 5-31-98
*/

#include <options.h>

int do_command (string str) {

    int i, last, num ;
    string *commands ;

    commands = allocate(HISTORY_SIZE) ;
    commands = this_player()->query_commands() ;
    last = this_player()->query_last_command() ;
    num = this_player()->query_num_commands() ;
/* Start counting with last, whatever that is, and go down to 0. When
   you reach -1, go back to the top and keep counting down until you
   reach last+1. */
/* This fails if last is HISTORY_SIZE-1; in that case, set last to -1
   and the algorithm succeeds. */
    if (last==HISTORY_SIZE-1) last=-1 ;
    for (i=last;i!=last+1;i--) {
	if (i==-1) i=HISTORY_SIZE-1 ;
	write (num+" "+commands[i]+"\n") ;
	num-- ;
	if (num==0) return 1 ; 
    }
    return 1 ;
}
