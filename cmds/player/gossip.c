/* wiz.c
   A command to provide a wizard channel.
   For details, take a look at the channel daemon, /system/channel_d.c
   Mobydick, 6-9-98
*/

#include <config.h>

int do_command (string str) {

    object sender ;

    sender = this_player()->query_user() ;
    if (!sender) {
	fail_msg ("non-interactives can't send.\n") ;
	return 0 ;
    }
    if (CHANNEL_D->user_listening_to_channel(sender, "gossip")!=1) {
	write ("You're not tuned into that channel.\n") ;
	return 1 ;
    }
    CHANNEL_D->send_channel_message(sender, "gossip", str) ;
    return 1 ;
}
