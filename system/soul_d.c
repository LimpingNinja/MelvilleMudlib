/* The soul daemon.
   Any input the users types that doesn't match a valid command is passed
   here. If we can find a matching soul command, we use it. If not, we
   return 0. Each soul command has three lines in the soul data file,
   which is /data/system/soul.data. The first is the command word. The
   second is the message to the command giver. The third is the message
   to anyone else in the room. $N will be replaced by the name of the command
   giver, and any additional input will be appended to the messages.
   It is assumed that the last character of the soul message is
   punctuation, so the "appended" input goes before the last character.
   This object is a greatly simplified version of Truilkan's soul
   daemon at TMI (and other libs).
*/

mapping own_message ;
mapping other_message ;

void create() {

    int i, num;
    string *msgs ;
    string cmd ;

    msgs = explode(read_file("/data/system/soul.data"),"\n") ;
    num = sizeof(msgs)/3 ;
    own_message = ([ ]) ;
    other_message = ([ ]) ;
    for (i=0;i<num;i++) {
	cmd = msgs[i*3] ;
	own_message[cmd] = msgs[i*3+1] ;
	other_message[cmd] = msgs[i*3+2] ;
    }
    return ;
}

/* This function is called from player.c to try for a soul command. */

int do_soul (string str) {

    string cmd, message, own, punc ;
    string p1, p2 ;
    int len ;
    string name, replaced ;
    object room ;


    if (sscanf(str,"%s %s",cmd,message)!=2) cmd=str ;
    if (!own_message[cmd]) return 0 ;
/* This strips off the last character (punctuation) */
    len = strlen(own_message[cmd]) ;
    own = own_message[cmd][0..len-2] ;
    punc = own_message[cmd][len-1..len-1] ;
    name = capitalize(previous_object()->query_name()) ;
    sscanf(other_message[cmd],"%s$N%s",p1,p2) ;
/* Get the punctuation off p2. */
    len = strlen(p2) ;
    punc = p2[len-1..len-1] ;
    p2 = p2[0..len-2] ;
    replaced = p1+name+p2 ;
    room = previous_object()->query_environment() ;
    if (message) {
	write (own+" "+message+punc+"\n") ;
	say (replaced+" "+message+punc+"\n") ;
	return 1 ;
    }
    write (own_message[cmd]+"\n") ;
    say (replaced+punc+"\n") ;
    return 1 ;
}
