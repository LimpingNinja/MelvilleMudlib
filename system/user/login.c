/*
  The various functions that handle creation of new characters and
  logins by old characters. Inherited by user.c.
  Begun by Mobydick, 5-20-94.
*/

#include <config.h>
#include <options.h>

string password ;
string cap_name ;
string real_name ;
string last_ip ;
int last_on ;
string email ;
int wizard ;
static object player ;
static string name ;
static int attempts ;

static void create_character (string str) ;
static void get_new_password() ;
static void set_name (string str) ;
string query_name() ;
void accept_connect() ;

/* get_name() is called from open() when a new connection is made.    */

static void get_name (string str) {
    if (!str || strlen(str)<3) {
        send_message("Sorry, your name must have at least three letters.\n") ;
	send_message("Enter the name of your character: ") ;
	input_to ("get_name") ;
        return ;
    }
    if (strlen(str)>10) {
        send_message("Sorry, your name can have at most ten letters.\n") ;
        send_message("Enter the name of your character: ") ;
        input_to ("get_name") ;
	return ;
    }
    str = lowercase(str) ;
    set_name(str) ;
    cap_name = capitalize(str) ;
    if (file_exists(USER_SAVE_DIR+str+USER_SAVE_EXT)>0) {
        /* Character already exists */
	send_message("Please enter your password: ") ;
	restore_object(USER_SAVE_DIR+str+USER_SAVE_EXT) ;
	attempts=1 ;
        input_to ("check_password",1) ;
    } else {
        /* New user */
        create_character(str) ;
    }
    return ;
}

static void check_password (string str, int tries) {

    string crypted ;
    object existing ;
    crypted = crypt(str,"MD") ;
    if (password!=crypted) {
        send_message("Sorry, that password is incorrect.\n") ;
        if (attempts==3) {
	    write ("You have taken too many tries.\n") ;
	    destruct() ;
	    return ;
	}
	attempts += 1 ;
	send_message("Please enter your password: ") ;
	input_to("check_password",1) ;
        return ;
    }
    write ("\n") ;
/* We don't permit multiple logins, but if there's a netdead version of
   you you can force it to quit. */
    existing = USERS_D->find_user(name) ;
    if (existing) {
	if (existing->linkdead()) { 
	    write ("You are already logged on, but linkdead. Force "+
		       "existing copy to quit? ") ;
	    input_to ("force_off") ;
	    return ;
	}
	write ("You are already logged on: multiple logins aren't allowed.\n");
	destruct() ;
	return ;
    }
    accept_connect() ;
}

/* If there is a linkdead version logged on, we input_to() down to here to
   let the user force the linkdead copy off. */

static void force_off (string str) {

    if (str[0]!='y') {
	write ("Then you cannot log in. Come back another time.\n") ;
	destruct() ;
	return ;
    }
    USERS_D->find_user(name)->query_player()->quit() ;
    accept_connect() ;
}

/* Now that we have a valid password, reset the privileges and creator
   to match the verified identity of the user. 
   This is also called from the new character create routine, which is
   why it's a separate function. */

void accept_connect() {

    int new_mail ;

    if (previous_object()!=this_object() && previous_object()!=player) return ;
/* cat the news file while we still have login privs */
    cat("/doc/news/news") ;
/* Set the player's name. */
    player->set_name(name) ;
/* Set the wizard bit in the player, if need be. */
    if (wizard) player->set_wizard(1) ;
    set_creator() ;
    set_privileges() ;
    player->set_creator() ;
    player->set_privileges() ;
/* If they've been on before, say when. If not, then we should save their
   user data file. */
    if (last_on) {
        send_message("Hello, "+cap_name+"! You were last on from "+last_ip+
		 " at "+ctime(last_on)+"\n") ;
    } else {
        save_object(USER_SAVE_DIR+name+USER_SAVE_EXT) ;
    }
    last_on = time() ;
    new_mail = MAIL_D->count_unread_mails(name) ;
    if (new_mail) {
	write ("You have "+new_mail+" unread mail messages.\n") ;
    }
    write ("\n") ;
    USERS_D->add_new_user(this_object(),name) ;
    log_file(USAGE_LOG,cap_name+" entered from "+
	     query_ip_number(this_object())+" at "+ctime(time())+"\n") ;
    player->enter_world() ;
    player->write_prompt() ;
    return ;
}

static void create_character (string str) {
    send_message (capitalize(str)+" will be a new character. Is that the "+
		  "name you want to use? ") ;
    input_to ("check_new_name") ;
}

static void check_new_name (string str) {
    if (str[0]=='y') {
	get_new_password() ;
	return ;
    }
    write ("OK, what name would you like, then? ") ;

    input_to ("get_name") ;
    return ;
}

static void get_new_password() {
    send_message ("Please enter a password for your character: ") ;
    input_to ("get_password", 1) ;
    return ;
}

static void get_password (string pass) {
    if (!pass) {
        send_message ("You cannot use a blank string as a password.\n") ;
	return ;
    }
    password = crypt(pass,"MD") ;
    send_message ("Enter it again: ") ;
    input_to ("get_pass2", 1) ;
    return ;
}

static void get_pass2 (string pass) {
    if (!pass || crypt(pass,"MD")!=password) {
        send_message ("The passwords must match.\n") ;
	send_message ("Please enter a password for your character: ") ;
	input_to ("get_password", 1) ;
	return ;
    }
    send_message ("What is your real name? ") ;
    input_to("get_real_name") ;
    return ;
}

static void get_real_name (string str) {
    if (!str) {
        send_message ("What is your real name? ") ;
	input_to("get_real_name") ;
	return ;
    }
    real_name = str ;
    send_message ("What is your email address? ") ;
    input_to("get_email") ;
}

static void get_email (string str) {

    string s1,s2 ;

    if (!str || sscanf(str,"%s@%s",s1,s2) != 2) {
        send_message ("Your email address must be in the form: userid@host.\n") ;
	send_message ("What is your email address? ") ;
	input_to ("get_email") ;
	return ;
    }
    email=str ;
    last_ip = query_ip_number(this_object()) ;
/* Should we make him a wizard?  */
    if (AUTO_WIZ) {
        wizard = 1 ;
        send_message ("You are being granted wizard status.\n") ;
    }
    save_object(USER_SAVE_DIR+query_name()+USER_SAVE_EXT) ;
    log_file(CREATE_LOG,cap_name+" created from "
	     +query_ip_number(this_object())+" at "+ctime(time())+"\n") ;
    player->create_new_body() ;
}
