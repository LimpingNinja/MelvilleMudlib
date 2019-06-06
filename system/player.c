/*
   player.c
   The body of the player. It handles all mudlib related actions such
   as command processing, descriptions, and so forth. Not to be confused
   with user.c, which holds the player's connection and handles incoming
   and outgoing messages.
   Begun by Mobydick, 5-19-94.
*/

#include <config.h>
#include <options.h>
#include <type.h>

inherit CONTAINER ;
inherit "/system/player/creation.c" ;
inherit "/system/player/shell.c" ;
inherit "/system/player/edit.c" ;

string name ;
string cap_name ;
string describe ;     /* so the player can describe himself */
int wizard ;
int brief ;
string cwd ;
string cwf ;
string quit_location ;
static string *more_file ;   /* used when moring a file. */
static int more_line ;
static string fail_msg ;      /* Set when a command fails to work. */


varargs int move (mixed dest, string direction, int silent) ;
static void initialize_shell() ;
static void increment_history (string str) ;
string query_subjective() ;

/* At the moment, all create() needs to do is to initialize the
   shell stuff. */

void create() {
    initialize_shell() ;
}

void set_user(object newuser) {
    if (object_name(previous_object())!=DRIVER) return ;
    user = newuser ;
}

object query_user() {
    return user ;
}

/* It's Bad if people can carry each other. */

int prevent_get() {
    return 1 ;
}

/* Set the player's wizard bit. This can only be done by the associated
   user object. */

void set_wizard (int val) {
    if (previous_object()!=user) return ;
    wizard = val ;
}

/* Query the wizard bit. */

int query_wizard() {
    return wizard ;
}

/* The query_living() function returns 1 to mark this object as living. Note
   that user objects are _not_ living. The living function mostly serves
   to mark objects as those that define catch_tell() and should thus be
   send messages.
   It returns 0 if the player is linkdead.
*/

int query_living() {
    if (user->linkdead()) return 0 ;
    return 1 ;
}

/* This is called after the user associated with this player has entered
   the correct password. It moves his body to the start room and does
   other entry tasks. */

void enter_world() {

    string entry_point ;

    /* We only want the user object to call this. */
    if (previous_object()!=user) return ;
/* Restore the data from the file. */
    restore_object(PLAYER_SAVE_DIR+name+PLAYER_SAVE_EXT) ;
    set_id ( ({ name }) ) ;
#ifdef START_AT_QUIT_LOCATION
    entry_point = quit_location ;
    if (!entry_point) entry_point=START ;
#else
    entry_point=START ;
#endif
    move(entry_point,"",1) ;
    START->room_tell(cap_name+" enters the game.\n", ({ this_object() }) ) ;
}

/* This is here as a quasi-hack. Something similar to it will be kept. */
/* direction is a string indicated the direction of the move, which is
   printed to the old room. If it's not passed, we assume a teleport,
   and print a different set of messages entirely. silent is an int
   which can be used to suppress printing the messages. */

varargs int move (mixed dest, string direction, int silent) {

    object old_env ;
    int res ;

    if (!dest) return 0 ;
    old_env = query_environment() ;
    if (typeof(dest)==T_STRING) {
        dest=get_object(dest) ;
    }
    res = ::move(dest) ;
    if (res) {
	/* Let the player take a peek at his new environment */
        write(environment->query_long(brief)) ;

	if (silent) return res ;

        if (direction && direction!="") {
	    if (old_env) old_env->room_tell(cap_name +
			   " leaves "+direction+".\n") ;
	    dest->room_tell(cap_name+" enters.\n", ({ this_object() }) ) ;
        } else {
	    if (old_env) old_env->room_tell(cap_name+ 
		          " vanishes into the shadows.\n") ;
            dest->room_tell(cap_name+" appears from the shadows.\n",
			    ({ this_object() }) ) ;
	}
    }
    return res ;
}

void set_brief (int val) {
    brief = val ;
}

int query_brief() {
    return brief ;
}

/* command() is called by the user object any time the player sends
   a string that is neither an input_to nor an editor. It finds the
   appropriate command bin and calls it. */

void command (string str) {

    string verb, arg, myprivs ;
    int i, result ;
    object *obs ;

/* Only the associated user object and the force command call this. */
    if (previous_object()!=user && object_name(previous_object())!=FORCE_CMD)
		return ;

/* Clear the failure message. */
    fail_msg = "" ;
   
/* If they sent nothing, toss back a prompt. */
    if (!str || str=="") {
        user->write_prompt() ;
	return ;
      }

/* Do the history and alias processing, such as we have it, and store
   the command. */
    str = process_history(str) ;
    if (str=="") return ;
    str = process_alias(str) ;
    increment_history(str) ;

/* Separate the first word from later words. */
    if (sscanf(str,"%s %s",verb,arg)!=2) verb=str ;

/* First, see if we can match this up to a player command. */
    if (file_exists("/cmds/player/"+verb+".c")==1) {
        if (arg) {
            result = call_other("/cmds/player/"+verb,"do_command",arg) ;
        } else {
	    result = call_other("/cmds/player/"+verb,"do_command") ;
        }
    }
    if (result) return ;

/* If we got no result that way, then try a wizard, if the player is
   in fact a wizard. */
    myprivs = query_privileges() ;
    if (myprivs=="wizard" || myprivs=="admin") {
        if (file_exists("/cmds/wizard/"+verb+".c")==1) {
            if (arg) {
                result = call_other("/cmds/wizard/"+verb,"do_command",arg) ;
	    } else {
                result = call_other("/cmds/wizard/"+verb,"do_command") ;
	    }
	}
    }
    if (result) return ;

/* If still no result, try admin commands for admins. */
    if (myprivs=="admin") {
        if (file_exists("/cmds/admin/"+verb+".c")==1) {
            if (arg) {
                result = call_other("/cmds/admin/"+verb,"do_command",arg) ;
	    } else {
                result = call_other("/cmds/admin/"+verb,"do_command") ;
	    }
	}
    }
    if (result) return ;

/* We have failed to match to any of the three command bins. Next,
   we see if the room defined an action for us. */

    if (environment) {
	result = environment->perform_action(verb,arg) ;
    }
    if (result) return ;

/* Any object in our inventory? */

    obs = query_inventory() ;
    if (obs && sizeof(obs)>0) {
	for (i=0;i<sizeof(obs);i++) {
	    result = obs[i]->perform_action(verb,arg) ;
	    if (result) return ;
	}
    }

/* In the inventory of our environment? */
    if (environment) {
	obs = environment->query_inventory() ;
	if (obs) {
	    for (i=0;i<sizeof(obs);i++) {
		result = obs[i]->perform_action(verb,arg) ;
	   	if (result) return ;
	    }
	}
    }

/* No object defines this command. Try the soul daemon. */
     
    result = SOUL_D->do_soul(str) ;

/* If that didn't work, we're hosed. */
    if (!result) {
	if (fail_msg && fail_msg!="") {
	    write (fail_msg) ;
	} else {
	    write (ERROR_MSG) ;
	}
    }
    return ;
}

/* This handles editor commands */

void edit_command (string str) {
    if (previous_object()!=user) return ;
    write (editor(str)) ;
/* Did we quit from the editor? If so, notify the user. */
    if (!query_editor(this_object())) {
        user->set_in_edit(0) ;
    }
}

/* This lets the player begin an edit session. */

varargs int edit_file (string filenm) {

   string myprivs ;

   myprivs = query_privileges() ;
/* Only the ed command gets to call into here. */
    if (object_name(previous_object())!=ED_CMD) return 0 ;
/* Non-wizards should not have access to the ed command. But just in
   case they do, we have a fail-safe here as well. */
    if (myprivs!="wizard" && myprivs!="admin") return 0 ;
/* Can't be in more than one session at once. */
    if (user->query_in_edit()) return 0 ;

/* OK, grudgingly we let the guy edit. */
    user->set_in_edit(1) ;
    if (!filenm) {
        write(editor()) ;
    } else {
        write (editor("e /"+filenm)) ;
	editor("1") ;           /* needed to start at top of the file. */
    }
}

/* Sometimes the driver calls this, and we want to pass the message
   back to the user. */

void receive_message (string str) {
    user->catch_tell(str) ;
}

/* This overrides the destruct() function in the auto object. Player
   objects can be dested only by internal request, not by any other
   call. If you want to dest one, call quit(): but you need the right
   privs to do that too :)
   Well, actually, let's make it easier to dest the master copy.
   That way the master object can be dested by the update command.
*/

int destruct() {
    if (clone_num(this_object())) {
	if (previous_object()!=this_object()) return 0 ;
    }
    destruct_object(this_object()) ;
    return (this_object()==0) ;
}

/* The quit() function saves the user, destructs the body, and requests
   the user object to destruct itself. It can be called only from the
   quit command, the shutdown command, or a user.c clone in login. */
/* Or now, from the disconnect command as well. */

void quit() {

    string prev_name, prev_priv ;
    int i ;
    string str ;
    object *inventory ;

    prev_name = base_name(previous_object()) ;
    prev_priv = previous_object()->query_privileges() ;
/* Quit can be called by three objects: the quit command (when the player
   types "quit"), the shutdown command (to force everyone off when we shut
   down) or from a user object in login (to reconnect after net-death). */
    if (prev_name!=QUIT_CMD && prev_name!=DISCONNECT_CMD &&
       (prev_name!=USER || prev_priv!="login") &&
       prev_name!=SHUTDOWN_CMD) return ;
    if (query_environment()) {
        quit_location = base_name(query_environment()) ;
    } else {
        quit_location = 0 ;
    }
    save_object(PLAYER_SAVE_DIR+user->query_name()+PLAYER_SAVE_EXT) ;
    inventory = query_inventory() ;
    if (inventory && sizeof(inventory)>0) {
	write ("You drop all your possessions.\n") ;
	for (i=0;i<sizeof(inventory);i++) {
/* Have to get the short now because we'll be dropping the item, and
   losing the pointer to it. */
	    str = inventory[i]->query_short() ;
	    if (inventory[i]->move(query_environment())) {
		say (cap_name+" drops "+str+".\n") ;
	    }
	}
    }
    say (cap_name+" quits the game.\n") ;
    user->quit() ;
    destruct_object(this_object()) ;
}

/* query_vision should return 1 if the player can see, and 0 if he
   can't. It should depend on ambient light, infravision, that kind
   of thing. */

int query_vision() {
    return 1 ;
}

/* query_short just returns the cap_name of this player. */

string query_short() {

    string ret ;

    ret = cap_name ;
    if (user->linkdead()) ret += " [linkdead]" ;
    return ret ;
}

/* query_long isn't finished yet. */

string query_long() {

    string ret ;
    int i ;
    object *inventory ;

    ret = capitalize(query_subjective())+" is the player "+cap_name+".\n" ;
    if (describe) ret += describe + "\n" ;
    inventory = query_inventory() ;
    if (!inventory || sizeof(inventory)==0) {
        ret += capitalize(query_subjective())+" is empty-handed.\n" ;
    } else {
        ret += capitalize(query_subjective())+" is carrying:\n" ;
	for (i=0;i<sizeof(inventory);i++) {
	    ret += "  "+inventory[i]->query_short()+"\n" ;
        }
    }
    return ret ;
}

void set_name (string str) {
    if (previous_object()!=user) return ;
    name = str ;
    cap_name = capitalize(str) ;
    return ;
}

string query_name() {
    return name ;
}

string query_cap_name() {
    return cap_name ;
}

/* For the current working directory. Not super-secure. The convention
   is to store the working directory with a / on the end, and return it
   that way. For some printing applications you might want to remove
   the trailing /.
   Can only be called by the cd.c command.
 */

int set_cwd (string str) {

    int len ;

    if (object_name(previous_object())!=CD_CMD) return 0 ;
    if (file_exists(str)!=-1) return 0 ;
    len = strlen(str) ;
    if (len==0) {
	str = "/" ;
    } else {
	if (str[len-1]!='/') str += "/" ;
    }
    cwd = str ;
    return 1 ;
}    

string query_cwd() {
    if (!cwd) return "/" ;
    return cwd ;
}

/* Similar functions for the current working file. Only the ed and
   update commands can set this; you may want to expand the list or
   remove the security altogether if you're willing.
*/

int set_cwf (string str) {

    if (object_name(previous_object())!=ED_CMD &&
	object_name(previous_object())!=UPDATE_CMD) return 0 ;
    if (str[0..0]!="/") str = "/"+str ;
    if (file_exists(str)==-1) return 0 ;
    cwf = str ;
    return 1 ;
}

string query_cwf() {
    if (!cwf) return 0 ;
    return cwf ;
}

/* catch_tell just passes the message back to the user object. */

void catch_tell (string str) {
    user->catch_tell(str) ;
}

/* A primitive file morer. */

varargs void more_file (mixed arg) {

    int i,limit ;

    if (typeof(arg)==T_STRING) {
        more_file = explode(read_file(arg),"\n")  ;
	more_line=0 ;
    }
    if (typeof(arg)==T_INT && arg>0) {
        more_line = arg ;
    }
    if (more_line>=sizeof(more_file)) return ;
    limit = more_line+20 ;
    if (limit>sizeof(more_file)) limit=sizeof(more_file) ;
    for (i=more_line;i<limit;i++) {
        write (more_file[i]+"\n") ;
    }
    if (limit==sizeof(more_file)) {
        more_line=0 ;
	more_file = ({ }) ;
	return ;
    }
    more_line += 20 ;
    write ("More:") ;
    input_to ("continue_more") ;
}

void continue_more (string str) {
/* Only want the user object to call this. */
    if (previous_object()!=user) return ;
/* You could parse the arg if you wanted. I'm just catching quit now. */
    if (str=="q") {
/*
        user->write_prompt() ;
*/
	more_line = 0 ;
	more_file = ({ }) ;
        return ;
    }
    more_file() ;
}

/* This lets you access the morer by passing a set of line args instead
   of a file name if you want to do that. */

void more_strings (string *file) {
    more_file = file ;
    more_line=0 ;
    more_file(0) ;
}

/* A bunch of functions to return the various pronouns and possessives
   that are handy to have.
   This is put into the player for two reasons:
   a) the gender is stored there, so we don't have to pass it around;
   b) it is more properly OO to have it there since it isn't defined
      except for players (or other livings).
   Mobydick, 7-21-94.
*/

string query_subjective() {
    switch (gender) {
        case "male" : return "he" ;
        case "female" : return "she" ;
        default: return "it" ;
    }
}

string query_objective() {
    switch (gender) {
        case "male" : return "him" ;
        case "female" : return "her" ;
        default: return "it" ;
    }
}

string query_possessive() {
    switch (gender) {
        case "male" : return "his" ;
        case "female" : return "hers" ;
        default: return "its" ;
    }
}

/* Allow the player to set his describe variable so he can tell the
   world what his character is like. Only can be called from the
   appropriate command. */

void set_describe (string str) {
    if (object_name(previous_object())!=DESCRIBE_CMD) return ;
    describe = str ;
}

string query_describe() {
    return describe ;
}

/* Set the failure message that will be printed when a user types a
   string that doesn't work with a given command. It's used so that
   if there's two objects defining "foo" as a command, then the first
   object does not print its failure message until after the second
   object also rejects (or worse, accepts) the command.
   Does what notify_fail() did under MudOS.
*/

void set_fail_msg (string str) {
    fail_msg = str ;
}
