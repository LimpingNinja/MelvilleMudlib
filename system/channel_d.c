/* channel_d.c
   A very simple channel daemon. The implementation is this: the
   command /cmds/[player|wizard|admin]/[name] sends a message to
   the daemon requesting that the message it passes be sent to all
   tuned-in users. The tune command makes calls in to maintain the
   list of tuned-in users, and the show command displays it. The
   disadvantage of this method is that you have to have one command
   for each channel you want to implement. The alternative is to
   have the command processing try to implement any input it does
   not recognize as a channel; this is more elegant in some ways
   but makes the code rather harder to follow. I've chosen to do
   it with the one-command-per-channel method to keep it simple. The
   advantage to this system is that it doesn't need to maintain
   security over each channel; the security on command access
   (which already exists) does that. It's also the most natural
   way to code it IMHO.
   Mobydick, 6-2-98
*/

#include <config.h>
#include <options.h>


/* This mapping keeps track of each channel and who's listening.
   I put the user ob in, rather than the player ob, so that you
   can switch bodies and not lose your channel settings. Also
   if you go linkdead you will drop out naturally. You could
   do it the other way if you wanted to, though.
*/

mapping channel_users ;

/* Return codes for add_user_to_channel and remove_user_from_channel:
    0 : no such channel
   -1 : not a user object
   -2 : already tuned in/not tuned in
    1 : success
*/

int add_user_to_channel (object user, string channel) {
    if (member_array(channel, CHANNELS)==-1) return 0 ;
/* Users only! */
    if (base_name(user)!=USER) return -1 ;
/* Are you already listening? */
    if (member_array(user,channel_users[channel])!=-1) return -2 ;
/* OK, add it. */
    channel_users[channel] += ({ user }) ;
    return 1 ;
}

int remove_user_from_channel (object user, string channel) {
    if (member_array(channel, CHANNELS)==-1) return 0 ;
    if (base_name(user)!=USER) return -1 ;
    if (member_array(user,channel_users[channel])==-1) return -2 ;
    channel_users[channel] -= ({ user }) ;
    return 1 ;
}

/* This is invoked by the player object to get someone out of all
   channels when they quit. */

int remove_user_from_all (object user) {

    int i ;

    if (base_name(user)!=USER) return -1 ;
    for (i=0;i<sizeof(CHANNELS);i++) {
	channel_users[CHANNELS[i]] -= ({ user }) ;
    }
    return 1 ;
}

object *listening_to_channel (string channel) {
    if (member_array(channel, CHANNELS)==-1) return ({ }) ;
    return channel_users[channel] ;
}

/* Return codes for user_listening_to_channel:
    0: not listening
    1: listening
   -1: no such channel
*/

int user_listening_to_channel (object user, string channel) {
    if (member_array(channel, CHANNELS)==-1) return 0 ;
    if (member_array(user,channel_users[channel])==-1) return 0 ;
    return 1 ;
}

/* Return codes for send_channel message:
    0: you're not tuned in
   -1: no such channel
    1: success
*/

int send_channel_message (object user, string channel, string message) {

    int i ;

    if (member_array(channel, CHANNELS)==-1) return -1 ;
    if (member_array(user,channel_users[channel])==-1) return -2 ;
    for (i=0;i<sizeof(channel_users[channel]);i++) {
/* Might be 0 somehow if a user is gone and not removed from the list. */
       if (!channel_users[channel][i]) continue ;
/* Don't send to yourself */
       if (channel_users[channel][i]==user) continue ;
       channel_users[channel][i]->catch_tell("["+channel+"] "+
	    capitalize(user->query_name())+": "+message+"\n") ;
    }
    write ("You "+channel+": "+message+"\n") ;
    return 1 ;
}

void create() {

    int i ;

    channel_users = ([ ]) ;
    for (i=0;i<NUM_CHANNELS;i++) {
	channel_users[CHANNELS[i]]=({ }) ;
    }
    return ;
}

/* When the object is removed, tune everyone out. */

int destruct() {

    int i, j ;

    for (i=0;i<sizeof(CHANNELS);i++) {
	for (j=0;j<sizeof(channel_users[CHANNELS[i]]);j++) {
	    channel_users[CHANNELS[i]][j]->catch_tell(
		"Tuning you out of "+CHANNELS[i]+" channel!\n") ;
	}
    }
    return ::destruct() ;
}
