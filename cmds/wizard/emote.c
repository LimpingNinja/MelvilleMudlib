/* The emote command.
   Mobydick, 8-14-94.
   Lets any wizard write any string beginning with his capital name.
   At the moment it also prepends -> to the string so that there can
   be no confusion between emotes and other system messages. You might
   want to turn that off if you don't like it. */


int do_command (string str) {
    say ("-> "+previous_object()->query_cap_name()+" "+str+"\n") ;
    write ("You emote: "+previous_object()->query_cap_name()+" "+str+"\n") ;
    return 1 ;
}
