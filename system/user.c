/*
  user.c
  The user object. Not to be confused with the user's body, which is
  player.c. The user object holds the remote connection, and directs
  incoming typing to the appropriate place.
  Begun by Mobydick, 5-19-94. About half of the code from Dworkin's
  user.c got used in this object.
*/

#include <config.h>
#include <options.h>

inherit "/system/user/login.c" ;

/* Internal variables we don't save. */

static int echo;		/* is input echoing turned on */
static object input_obj;	/* object for input_to */
static string input_func;	/* function for input_to */
static int editing;		/* are we editing? */
static int timestamp;		/* last time something was typed */

/* For security, user objects can be cloned only by the driver object. */

void create() {
    if (object_name(previous_object())!=DRIVER) {
        destruct_object(this_object()) ;
    }
}

/*
 * NAME:	set_player()
 * DESCRIPTION:	set the player object associated with this user
 */
void set_player(object obj) {
    if (object_name(previous_object()) == DRIVER) {
	player = obj;
    }
    return ;
}

/*
 * NAME:	query_player()
 * DESCRIPTION:	return the player object associated with this user
 */
object query_player() {
    return player;
}

/* query_wizard() returns 1 for wizards, zero for others. */

int query_wizard() {
    return wizard ;
}

/* The promote to wizard function does what you'd think. It can be
   called only from the makewiz command. It sets the wizard flag in
   the user and the player, resets the privileges, and does nothing
   else.
   Note that there is no demote function. If you need to demote a
   wizard, nuke him. When he re-creates he's a player. (Assuming
   you have autowiz off. If you have it on, it's impossible to dewiz
   anyone effectively anyway. */

void promote_to_wizard() {
    if (object_name(previous_object())!=MAKEWIZ_CMD) return ;
    wizard = 1 ;
    player->set_wizard(1) ;
    set_privileges() ;
    player->set_privileges() ;
}

static void set_name(string str) {
    name=str ;
}

/* query_name returns the player's name. */

string query_name() {
    return name ;
}

/* query_cap_name returns the player's name, capitalized. */

string query_cap_name() {
    return cap_name ;
}

/*
 * NAME:	open()
 * DESCRIPTION:	start a user connection for this object. Called
                automatically by the driver when a new connection
                is made.
 */
static void open() {
    timestamp = time();
    echo = 1;
    cat ("/doc/news/welcome") ;
    send_message("Enter the name of your character: ");
    input_to ("get_name") ;
    return ;
}

/*
 * NAME:	close()
 * DESCRIPTION:	close the user connection of this object. Automatically
                called by the driver when the user object is dested: no
	        need to call it yourself.
 */
static void close() {
    if (editing != 0) {
	player->rescue_edit_file();
    }
    send_message("Bye!\n") ;
}

/* Control over editing function. Can come only from the associated
   player object */

void set_in_edit (int foo) {
    if (previous_object()!=player) return ;
    editing = foo ;
}

int query_in_edit() {
    return editing ;
}

int query_in_input() {
    if (input_obj) return 1 ;
    return 0 ;
}

/* The destruct() function overrides the one defined in the auto object,
   making it static. User objects can only call dest internally: if you
   need to dest one, call quit() in it (though that requires permissions
   as well.)
   This protection does not apply to the master copy of the object,
   since you need to remove that to update.
*/

int destruct() {
    if (clone_num(this_object())) {
        if (previous_object()!=player) return 0 ;
    }
    CHANNEL_D->remove_user_from_all(this_object()) ;
    destruct_object(this_object()) ;
    return (this_object()==0) ;
  }

/* The quit() function is called from the player object when the
   player types the quit command. Save the user and destruct. */

void quit() {

    string str ;

/* If there's no associated player let's let anyone force us to quit.
   This might have Security Implications but I don't see any immediate
   problem. */
    if (player && previous_object()!=player) return ;
    last_on = time() ;
    str = query_ip_number(this_object()) ;
    if (str) last_ip = str ;
    save_object(USER_SAVE_DIR+name+USER_SAVE_EXT) ;
    USERS_D->remove_user(this_object(),name) ;
    log_file(USAGE_LOG,cap_name+" quit from "+query_ip_number(this_object())+
	     " at "+ctime(time())+"\n") ;
    destruct() ;
}

/* Write the player's prompt to him. Called by the user object when
   needed for the next command. */

void write_prompt() {
    send_message ("> ") ;
  }

/* Write the edit-time prompt. */

void write_edit_prompt() {
    if (query_editor(player)=="insert") {
        send_message("*\b") ;
    } else {
        send_message(": ") ;
    }
}

/*
 * NAME:	catch_tell()
 * DESCRIPTION:	catch a message
 */
void catch_tell(string str) {

    if (str == 0) {
	return;
    }
    send_message(str);
}

/*
 * NAME:	receive_message()
 * DESCRIPTION:	receive user input. Any string sent by the user will
                be passed to this function by the driver.
 */
static void receive_message(string str) {

    string func ;
    object obj ;
    int i ;

/* Invoke appropriate rlimits... */

rlimits (MAX_DEPTH; MAX_TICKS) {

/* Set the timestamp, so we can know when the last command was issued. */
    timestamp = time();

/* Set this_player(). */
    get_object(USERS_D)->set_this_player(player) ;

/* Do we need to turn on the echo? If so, send a blank line. */
    if (echo == 0) {
	send_message(1);
	echo=1 ;
        send_message("\n") ;
    }
/* If input_obj is defined, then there is a pending input_to call for
   this user, and we need to pass the string back to the responsible
   function. */
    if (input_obj) {
/* Clear the input_obj now that we've handled the call.
   For some reason statements issued after the call_other don't have
   any effect. Probably a DGD bug. Meanwhile we kludge it so we can
   clear input_obj before the call. */
        obj = input_obj ;
	func = input_func ;
	input_obj=0 ;
	input_func=0 ;
        call_other(obj,func,str) ;
	/* If we're not in input_to or the editor, send a prompt. */
	if (!input_obj && !editing) write_prompt() ;
	get_object(USERS_D)->set_this_player(0) ;
	return ;
    }
/* Are we in the editor? */
    if (editing) {
	if (strlen(str)>0 && str[0]=='!') {
	    i = strlen(str) ;
	    str = str[1..i-1] ;
	    player->command(str) ;
	    get_object(USERS_D)->set_this_player(0) ;
	    return ;
        }
        player->edit_command(str) ;
	if (editing) write_edit_prompt() ; else write_prompt() ;
	get_object(USERS_D)->set_this_player(0) ;
	return ;
    }
/* Treat it as a user command, then.
   Did he send a blank line? If so, kick back a prompt. */
   if (!str || strlen(str)==0) {
       write_prompt() ;
	get_object(USERS_D)->set_this_player(0) ;
       return ;
   }

    player->command(str) ;
/* If we didn't enter input_to or the editor, send a prompt for the
    next command (unless we just quit). */
    if (!input_obj && !editing && str!="quit") write_prompt() ;
    if (editing) write_edit_prompt() ;
    get_object(USERS_D)->set_this_player(0) ;
    return ;
  }
}

/*
 * NAME:	set_input_to()
 * DESCRIPTION:	redirect user input to a function
 */
int set_input_to(object obj, string func, int flag) {
    if (input_obj == 0) {
	input_obj = obj;
	input_func = func;
	if (flag) {
	    send_message(echo = 0);
	}
	return 1;
    }
    return 0;
}

/*
 * NAME:	query_idle()
 * DESCRIPTION:	return idle time of this user
 */
int query_idle() {
    return time() - timestamp;
}

/* The linkdead function checks the user's IP. If there is none, he's
   linkdead and we return 1. If there is one, we return 0. */

int linkdead() {
    if (query_ip_number(this_object())) return 0 ;
    return 1 ;
}
