/*
   object.c
   An inheritable which defines the functions needed by all objects
   which play a part in the MUD environment. It handles descriptions,
   environment handling, moving, and so forth.
   Any object which might be handled by a player or monster, or appears
   in a room on the MUD, or will be moved from one place to another, or
   defines a player command, or any such thing, should inherit this.
*/

#include <type.h>

static string short_desc ;  /* description of the object in inventories */
static string long_desc ;   /* description of the object when looked at */
static object environment ;         /* object that contains this object */
static string *id ;         /* an array of string that name this object */
static mapping commands ;       /* command words defined by this object */

void set_short (string str) {
    short_desc = str ;
    return ;
}

string query_short() {
    return short_desc ;
}

void set_long (string str) {
    long_desc = str ;
    return ;
}

string query_long() {
    return long_desc ;
}

/* query_environment will return the object's environment. */

object query_environment() {
    return environment ;
}

void set_id (string *newid) {
    id = newid ;
}

string *query_id() {
    return id ;
}

int id (string str) {
    if (!id || sizeof(id)==0) return 0 ;
    if (member_array(str,id)==-1) return 0 ;
    return 1 ;
}

/* Move will try to move an object from its current environment to
   a new one. Both the current env and the new one get a chance to
   block the move. Return of 1 indicates a successful move, return
   of 0 indicates a failure.
   Warning: the object can lost in space if the old env releases it,
   the new one won't take it, and the old one won't take it back. */

int move (mixed dest) {
    /* If dest is a string, convert it to an object. If it's not
       defined, bail out.  */
    if (!dest) return 0 ;
    if (typeof(dest)==T_STRING) {
        /* Load the dest if needed, then convert it to an object. */
        dest=get_object(dest) ;
    }
    if (!dest) return 0 ;
  
    /* Is our current environment willing to let us go? */
    if (environment) {
        if (!environment->release_object(this_object())) return 0 ;
    }

    /* Is the new environment willing to take us?  */
    if (!dest->receive_object(this_object())) {

    /* Didn't take us, so try to go back to the old environment. */
        if (environment) {
            if (!environment->receive_object(this_object())) {
        /* Now we're lost in space: no one wants us.
	 Abigail suggested setting env to 0, I agreed. */
	        environment = 0 ;
	    }
	}
        return 0 ;
    }
    /* The move succeeded, so set the new environment. */
    environment = dest ;
    return 1 ;
}

void add_command (string word, string function) {
    if (!commands) commands = ([ ]) ;
    commands += ([ word : function ]) ;
    return ;
}

varargs int perform_action (string verb, string arg) {
    if (!commands || !commands[verb]) return 0 ;
    if (arg) return call_other(this_object(),commands[verb],arg) ;
    return call_other(this_object(),commands[verb]) ;
}
