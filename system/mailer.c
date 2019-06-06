/* A primitive mailer object.
   You can send mail, read it, store it, delete it, that kind of thing.
   Mobydick, 7-27-94
   The design of this mailer has been very heavily influenced by Huthar's
   intermud mailer from TMI-2.
*/

#include <config.h>
#include <options.h>

mixed *mail ;
static int current_m ;
static mapping message ;

/* General utility functions */
static void print_mail_info() ;
static void write_mail_prompt() ;
static void set_current_message() ;

/* Functions that handle each mailer command. */
static void send_new_mail (string str) ;
static void read_mail (string str) ;
static void show_headers() ;
static void reply_to_mail (string str) ;
static void delete_mail (string str) ;
static void undelete_mail (string str) ;
static void show_mail_help() ;
static void exit_mailer() ;

/* It is necessary to create this object in such a way that only
   the player who owns it can use it. The creator string will be
   the name of the player who cloned the mailer, except for the
   master object. The create() function will prevent the master
   mailer from loading a data file, and that's the only restore
   call in the code. If you add more restore calls, then you need
   to safeguard them in the same way.
*/

void create() {

    string spool ;
    int i ;

    if (!clone_num(this_object())) return ;
    spool = MAIL_DIR+query_creator()+MAIL_EXT ;
    if (file_exists(spool)==1) {
        restore_object(spool) ;
	set_current_message() ;
    } else {
        mail = ({ }) ;
	current_m = -1 ;
    }
}

void enter_mail (string str) {
    if (object_name(previous_object())!=MAIL_CMD) return ;
    if (str) {
        send_new_mail(str) ;
	return ;
    }
    print_mail_info() ;
    write_mail_prompt() ;
    input_to ("get_mail_command") ;
}

/* You could use this to customize the prompt. */

static void write_mail_prompt() {
    write ("mail> ") ;
}

/* Set the current message number correctly. */

static void set_current_message() {

    int i ;

    for (i=0;i<sizeof(mail);i++) {
	if (mail[i]["unread"]) {
	    current_m = i ;
	    return ;
	}
    }
    current_m = sizeof(mail)-1 ;
}

/* This function is called to display a short list of all the mail
   messages in the current spool. It's called when the mailer starts
   up: there will also be a command to display it at some point.
*/

static void print_mail_info() {

    int i ;
    string line ;

    if (!mail || sizeof(mail)==0) {
        write ("No messages\n") ;
	return ;
    }
    write ("Message  From            Subject\n") ;
    for (i=0;i<sizeof(mail);i++) {
	if (i==current_m) {
	    line = " >" ;
	} else {
	    line = "  " ;
	}
        line += pad((i+1)+"",3) ;
	if (mail[i]["unread"]) line+="U" ;
	if (mail[i]["deleted"]) line+="D" ;
        line = pad(line, 9) ;
	line += pad(mail[i]["from"],16) ;
	line += mail[i]["subject"]+"\n" ;
	write(line) ;
    }
}

/* This is the whopping big function which the input_to() calls go to.
   It parses a line from the user and takes appropriate action. */

int get_mail_command (string str) {

    string cmd, arg ;
    int num ;

/* Some security so only the player can call this. */
    if (object_name(previous_object())!=MAIL_CMD &&
	previous_object()->query_creator()!=query_creator()) return 1 ;
/* If the user typed a number, read that message number. */
    if (sscanf(str,"%d",num)==1) {
	read_mail(str) ;
	return 1 ;
    }
/* If not, see if he sent a command and an argument, or just a command. */
    sscanf(str,"%s %s",cmd,arg) ;
    if (!cmd) {
	cmd=str ;
	arg="" ;
    }
/* Based on that command, handle it appropriately. */
/* Handle the case of a blank line feed. */
    if (!str || str=="") {
/* Do we have no mail? Do nothing. */
	if (sizeof(mail)==0) {
	    write ("You have no mail.\n") ;
	    write_mail_prompt() ;
	    input_to ("get_mail_command") ;
	    return 1 ;
	}
/* If the current message has been read and is not the last, increment
   the current message. */
	if (!mail[current_m]["unread"] && current_m<sizeof(mail)-1) {
	    current_m++ ;
	}
/* Read whatever message we're now pointing at. */
	read_mail("") ;
	return 1 ;
    }
    if (cmd[0]=='h') {
	show_headers() ;
	return 1 ;
    }
    if (cmd[0]=='m') {
        send_new_mail(arg) ;
	return 1 ;
    }
    if (cmd[0]=='d') {
	delete_mail(arg) ;
	return 1 ;
    }
    if (cmd[0]=='u') {
	undelete_mail(arg) ;
	return 1 ;
    }
    if (cmd[0]=='r') {
        reply_to_mail(arg) ;
	return 1 ;
    }
    if (cmd[0]=='?') {
        show_mail_help() ;
	return 1 ;
    }
    if (str[0]=='q') {
	exit_mailer() ;
	return 1 ; 
    }
    write ("Invalid mail command.\n") ;
    write_mail_prompt() ;
    input_to ("get_mail_command") ;
    return 1 ;
}

static void send_new_mail (string str) {
    message = ([ ]) ;
    if (!str || str=="") {
        write ("Send mail to: ") ;
	input_to("get_to") ;
	return ;
    }
    str = lowercase(str) ;
    message["to"] = capitalize(str) ;
    write ("Subject: ") ;
    input_to("get_subject") ;
}

void get_to (string str) {
    if (previous_object()->query_creator()!=query_creator()) return ;
    message["to"] = capitalize(str) ;
    write ("Subject: ") ;
    input_to("get_subject") ;
}

void get_subject (string str) {
    if (previous_object()->query_creator()!=query_creator()) return ;
    message["subject"] = str ;
    write ("Begin editing your message. ~a aborts, . ends the message.\n") ;
    write ("====================================================\n") ;
    this_player()->begin_edit("finish_send") ;
    return ;
}

void finish_send (string *lines) {

    string *recpts ;
    int i, count ;

    if (previous_object()->query_creator()!=query_creator()) return ;
    message["text"] = implode(lines,"\n") ;
    message["unread"] = 1 ;
    message["deleted"] = 0 ;
    message["time"] = time() ;
    message["from"] = capitalize(previous_object()->query_creator()) ;
    count = 0 ;

/* Break up the "to" field into a list of recipients and send it to each. */
    recpts = explode(message["to"]," ") ;
    for (i=0;i<sizeof(recpts);i++) {

/* Does the target exist? */
       recpts[i] = lowercase(recpts[i]) ;
       if (!file_exists("/data/player/"+recpts[i]+".dat")) {
           write ("No such user: "+recpts[i]+"\n") ;
       } else {

/* If this message is to ourselves, then we can just add it to the
   spool in memory. Otherwise we call to the daemon to add it to the
   recipient's spool. */
	    if (recpts[i]==lowercase(message["from"])) {
                mail += ({ message }) ;
		set_current_message() ;
            } else {
	        MAIL_D->add_mail_to_spool(recpts[i],message) ;
            }
	    count++ ;
        }
    }
    switch (count) {
    case 0: { write ("Message not sent.\n") ; break ; }
    case 1: { write ("Message sent.\n") ; break ; }
    default: { write ("Message send to "+count+" users.\n") ; break ; }
    }
    write_mail_prompt() ;
    input_to ("get_mail_command") ;
    return ;
}

static void reply_to_mail (string str) {

    int num ;

    if (str && str!="") {
        if (sscanf(str,"%d",num)!=1) {
	    write ("Reply to a given message number.\n") ;
	    write_mail_prompt() ;
	    input_to("get_mail_command") ;
	    return ;
        }
    } else {
        num = current_m ;
    }
    message = ([ ]) ;
    message["to"] = mail[num]["from"] ;
    if (strlen(mail[num]["subject"])>3 && mail[num]["subject"][0..2]=="Re:") {
        message["subject"] = mail[num]["subject"] ;
    } else {
        message["subject"] = "Re: "+mail[num]["subject"] ;
    }
    get_subject(message["subject"]) ;
}

static void read_mail (string str) {

    int num ;
    string *text ;

    if (!str || str=="") {
	num = current_m ;
    } else {
	sscanf(str,"%d",num) ;
	num -- ;
    }
    if (num<0 || num>sizeof(mail)) {
	write ("Can't read that message number.\n") ;
	write_mail_prompt() ;
	input_to ("get_mail_command") ;
	return ;
    }
    write ("Message #"+(num+1)+"\n") ;
    write ("To: "+mail[num]["to"]+"\n") ;
    write ("From: "+mail[num]["from"]+"\n") ;
    write ("Time: "+ctime(mail[num]["time"])+"\n") ;
    write ("Subject: "+mail[num]["subject"]+"\n") ;
    write ("------------------------------------------------------------\n") ;
    text = explode(mail[num]["text"],"\n") ;
    this_player()->more_strings(text) ;
    write ("\n") ;
    current_m = num ;
    mail[num]["unread"]=0 ;
    write_mail_prompt() ;
    input_to ("get_mail_command") ;
    return ;
}

static void delete_mail (string str) {

    int num ;

    if (!str || str=="") {
        num = current_m ;
    } else {
        sscanf(str,"%d",num) ;
        num -- ;
    }
    if (num<0 || num>sizeof(mail)) {
	write ("Can't delete that message.\n") ;
	write_mail_prompt() ;
        input_to ("get_mail_command") ;
        return ;
    }
    if (mail[num]["deleted"]) {
	write ("Message "+num+" already deleted.\n") ;
	write_mail_prompt() ;
        input_to ("get_mail_command") ;
        return ;
    }
    mail[num]["deleted"] = 1 ;
    write ("Message "+(num+1)+" marked for deletion.\n") ;
    write_mail_prompt() ;
    input_to ("get_mail_command") ;
    return ;
}

static void undelete_mail (string str) {

    int num ;

    if (!str || str=="") {
        num = current_m ;
    } else {
        sscanf(str,"%d",num) ;
        num -- ;
    }
    if (num<0 || num>sizeof(mail)) {
	write ("That message isn't deleted.\n") ;
        write_mail_prompt() ;
        input_to ("get_mail_command") ;
        return ;
    }
    if (!mail[num]["deleted"]) {
        write ("Message "+num+" not deleted.\n") ;
        write_mail_prompt() ;
        input_to ("get_mail_command") ;
        return ;
    }
    mail[num]["deleted"] = 0 ;
    write ("Message "+(num+1)+" undeleted.\n") ;
    write_mail_prompt() ;
    input_to ("get_mail_command") ;
    return ;
}

static void show_headers() {
    print_mail_info() ;
    write_mail_prompt() ;
    input_to ("get_mail_command") ;
}

static void show_mail_help() {
    write (
"Mail commands are: # to read message, m to send mail, r to reply,\n"+
"d to delete, u to undelete, h to show headers, q to quit, ? for help.\n") ;
    write_mail_prompt() ;
    input_to ("get_mail_command") ;
}

static void exit_mailer() {

    int i,saved,purged ;

    i=0 ;
    saved = 0 ;
    purged = 0 ;
    while (i<sizeof(mail)) {
	if (mail[i]["deleted"]) {
	    mail -= ({ mail[i] }) ;
	    purged++ ;
	} else {
	    i++ ;
	   saved++ ;
	}
    }
    save_object(MAIL_DIR+query_creator()+MAIL_EXT) ;
    write ("Deleted "+purged+" messages and kept "+saved+".\n") ;
    destruct_object(this_object()) ;
}
