/*
   creation.c
   This handles the details of creating a body data file for a new
   player.
   Begun by Mobydick, 5-24-94.
*/

#include <config.h>
#include <options.h>

string gender ;
static object user ;

void enter_world() ;

/* create_new_body() is called from user.c when a new player is created. */

void create_new_body() {
/*    if (previous_object()->base_name()!=USER) return ; */
    write("Are you male, female, or neither? ") ;
    input_to ("get_gender") ;
}

void get_gender (string str) {
    if (str=="m") str="male" ;
    if (str=="f") str="female" ;
    if (str=="n") str="neuter" ;
    if (str=="neither") str="neuter" ;
    if (str!="male" && str!="female" && str!="neuter") {
        write ("You must be male, female, or neither.\n") ;
	write ("Are you male, female, or neither? ") ;
	input_to ("get_gender") ;
        return ;
    }
    gender = str ;
    cat ("/doc/news/new_player") ;
    user->accept_connect() ;
    save_object(PLAYER_SAVE_DIR+user->query_name()+PLAYER_SAVE_EXT) ;
}
