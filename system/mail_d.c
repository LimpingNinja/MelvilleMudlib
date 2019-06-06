/* The mailer daemon.
   There has to be some object in the mail system that can write to
   any mail spool so that when user A sends a message to users B, C,
   and D, the contents can be added to their mail spools. This daemon
   does that.
   Mobydick, 8-2-94.
*/

#include <config.h>
#include <options.h>

mixed *mail ;

/* This object has only two functions.
   The first function, add_mail_to_spool(), adds a mail message
   to a given user's queue. It restores that user's mail spool,
   adds the note, re-saves the spool, and that's it.
   It gets called from the mailer, and can be called only by a clone
   of mailer which has the same creator as the mail's from field.
*/

void add_mail_to_spool (string recpt, mapping message) {

    string sender ;
    int msgs ;

    if (base_name(previous_object())!=MAILER) return ; 
    sender = capitalize(previous_object()->query_creator()) ;
    if (sender!=message["from"]) return ;
/* We could check that the recipient was actually on the to field of
   the message, but it seems unnecessary right now. */
    restore_object(MAIL_DIR+recpt+MAIL_EXT) ;
    if (!mail) mail = ({ }) ;
    msgs = sizeof(mail) ;
    mail += ({ message }) ;
    save_object(MAIL_DIR+recpt+MAIL_EXT) ;
    mail = ({ }) ;
    return ;
}

/* The second function restores a user's mail spool, and returns
   the number of unread messages. Handy at login time. It should
   probably return an array of ints that also includes the number
   of total messages, but it doesn't. Finger could use that.
*/

int count_unread_mails (string str) {

    int i, unr ;

    restore_object(MAIL_DIR+str+MAIL_EXT) ;
    if (!mail || sizeof(mail)==0) return 0 ;
    unr = 0 ;
    for (i=0;i<sizeof(mail);i++) {
        if (mail[i]["unread"]) unr++ ;
    }
    return unr ;
}
