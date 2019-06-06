/* The users daemon.
   Keeps track of logged in users, and defines a couple functions
   to access them.
   Begun by Mobydick, 6-3-94
   Note that if you update this during runtime, you'll blow away
   the users list. This is Not Good. I recommend that you not do
   that. If you need to update this, either shut down, or if you're
   the only person on, log out and back in after you update.
   Ultimately this should somehow save its data across destruction...
*/

#include <config.h>

mapping users ;
object this_player ;

/* Initialize the list. */

void create() {
    users = ([ ]) ;
}

/* Add a new user. Can be called only from user.c. */

void add_new_user(object ob, string name) {
    if (base_name(previous_object())!=USER) return ;
    users += ([ name : ob ]) ;
}

/* Delete a user. Same caveat. */

void remove_user(object ob, string name) {
    if (base_name(previous_object())!=USER) return ;
    users[name] = 0 ;
}

/* Query all user objects. This one needs a copy() added. */

object *query_users() {
    return copy(map_values(users)) ;
}

/* And all names. Same caveat. */

string *query_user_names() {
    return copy(map_indices(users)) ;
}

/* Find a player with a given name. */

object find_user(string name) {
    if (!users) users = ([ ]) ;
    if (member_array(name,map_indices(users))==-1) return 0 ;
    return users[name] ;
}

/* This keeps track of this_player(). Security is really, really
   important here. At the moment I have neither a force command
   nor a way to force monsters to invoke commands, so there is
   only two ways to set this_player(), which is for a player to
   issue a command. this_player() is also set to 0 when the
   command finishes running. Therefore calls can only come from
   the user object, and other access is prevented. If you decide
   to allow a player to force a monster to issue a command, your
   monster code needs permission to set this_player to the monster;
   if you want a force command, that needs permission to set it
   to the forced player. If you want this_player() to persist
   across call_outs you will need to let the auto program do it
   too. Have fun :)
*/

void set_this_player (object ob) {
/*
    write (object_name(previous_object())+" is ") ;
    if (ob) {
        write ("Trying to set this_player to "+object_name(ob)+".\n") ;
    } else {
       write ("Trying to set this_player to 0.\n") ;
     }
*/
    if (base_name(previous_object())!=USER &&
	base_name(previous_object())!=FORCE_CMD &&
	base_name(previous_object())!=DRIVER) return ;
/*
    if (ob) {
        write ("Setting this_player to "+object_name(ob)) ;
      } else {
	write ("Setting this_player to 0.\n") ;
      }
*/
    this_player = ob ;
    return ;
}

/* This lets you query it. The only thing that really needs to call
   this is the this_player() function in the auto object, but hey,
   if someone else wants to call it, that's fine too. Objects are
   pointers so we need to return a copy.
*/

object query_this_player() {
    return copy(this_player) ;
}
