/* The nuke command. */
/* Erase a user's save file, player save file, and mail spool.
   Only do this when he's not logged on. Force him to quit first if you
   have to. The reason is, if you nuke him while he's on, his save file
   will be re-created when he quits.
*/

#include <config.h>
#include <options.h>
#include <admin.h>

int do_command (string str) {
    if (previous_object()->query_privileges()!="admin") return 1 ;
    if (USERS_D->find_user(str)) {
	write (capitalize(str)+" is logged on and can't be nuked.\n") ;
	return 1 ;
    }
    if (member_array(str,ADMIN)>-1) {
	write ("You cannot nuke an admin.\n") ;
	return 1 ;
    }
    remove_file(USER_SAVE_DIR+str+USER_SAVE_EXT) ;
    remove_file(PLAYER_SAVE_DIR+str+PLAYER_SAVE_EXT) ;
    remove_file(MAIL_DIR+str+MAIL_EXT) ;
    write (capitalize(str)+" has been nuked.\n") ;
    return 1 ;
}
