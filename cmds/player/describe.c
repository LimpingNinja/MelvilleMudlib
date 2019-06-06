/* The describe command.
   Lets a player set a self-description in his long desc.
   Mobydick, 7-26-94.
   Ancient MUD idea. Not mine originally by a long shot.
*/

int do_command() {
    write ("Enter the description you'd like to have, ending with \".\" or "+
	   "aborting with ~a.\n") ;
    write ("-------------------------------------------------------------\n");
    previous_object()->begin_edit("apply_describe") ;
    return 1 ;
}

void apply_describe (string *desc) {

    string str ;

    if (!desc || sizeof(desc)==0) {
        write ("Description aborted.\n") ;
	return ;
    }
    str = implode(desc,"\n") ;
    previous_object()->set_describe(str) ;
    write ("Your description has been changed. Look at yourself to see it.\n");
    return ;
}
