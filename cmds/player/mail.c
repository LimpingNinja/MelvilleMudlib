/* The mail command.
   This simply clones a mailer object for the user and initializes the
   mailer. That sends input to the mailer until the user quits from mail.
   Note that since this is in /cmds/player, any player can use it at
   any time. You may want to move it to /cmds/wizard and add a post
   office for players. If you do that, then change the MAIL_CMD define
   in config.h also.
   Mobydick, 8-1-94
*/

#include <config.h>

int do_command (string str) {

    object mailer_ob ;

    mailer_ob = clone_object(MAILER) ;
    mailer_ob->enter_mail(str) ;
    return 1 ;
}
