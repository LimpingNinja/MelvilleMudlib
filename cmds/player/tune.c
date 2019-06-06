/* The tune command. For channels. See /system/channel_d.c for details
   Mobydick, 6-5-98
*/
#include <options.h>
#include <config.h>

int do_command (string channel) {

    object user ;

    if (!channel) {
	fail_msg("Usage: tune <channel>\n") ;
	return 0 ;
    }
    if (member_array(channel, CHANNELS)==-1) {
	write ("There is no such channel as "+channel+".\n") ;
	return 1 ;
    }
    if (member_array(channel, WIZ_CHANNELS)>-1 &&
               !this_user()->query_wizard()) {
	write ("There is no such channel as "+channel+".\n") ;
	return 1 ;
    }
    user = this_player()->query_user() ;
    if (CHANNEL_D->user_listening_to_channel(user, channel)) {
	CHANNEL_D->remove_user_from_channel(user, channel) ;
	write ("Tuning out "+channel+".\n") ;
    } else {
	CHANNEL_D->add_user_to_channel(user, channel) ;
	write ("Tuning in "+channel+".\n") ;
    }
    return 1 ;
}
