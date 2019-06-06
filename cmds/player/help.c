/* Ye olde help command. It takes a string arg and mores a file in
   /doc/help with that name to the user. If the user passes no arg,
   then /doc/help/introduction is sent.
   Mobydick, 8-6-94.
*/

#include <config.h>

int do_command (string str) {

    if (!str || str=="") str="introduction" ;
    str = HELP_DIR + str ;
    if (!file_exists(str)) {
        fail_msg ("There is no help available on that topic.\n") ;
	return 0 ;
    }
    previous_object()->more_file(str) ;
    return 1 ;
}
