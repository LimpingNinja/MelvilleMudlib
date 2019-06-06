/*
   overrides.c
   Functions in the auto object that override kfuns, usually to add
   security on file-writing operations or other sensitive kfuns.
   These functions all need to be nomask so that they cannot be
   bypassed.
   Begun by Mobydick, 5-24-94.
*/

#include <config.h>
#include <options.h>
#include <type.h>

nomask int valid_write (string file) ;
nomask int valid_read (string file) ;

static nomask void write (string str) ;
static nomask object get_object (string name) ;

private static string creator ;        /* will be set in security.c */
private static string privileges ;     /* ditto */

/*
   save_object() checks for permissions to save. Users and players may
   save to the appropriate data file. Any object can save to a file
   which has the same name as the object but ends in .dat instead of .c.
   The mailer can save to /data/mail/foo.dat if its creator is foo, not
   otherwise. The mail daemon can save to /data/mail/foo.dat for any
   value of foo.
   Other save attempts are not permitted.
   Returns 1 if the save was permitted, 0 if not.
*/


static nomask int save_object (string file) {

    string s1,s2 ;
    string obname, prefix ;
    int clonenum ;

/* Separate the file name and the clone number. */
    sscanf (object_name(this_object()),"%s#%d",obname,clonenum) ;
    if (!obname) obname = object_name(this_object()) ;
/* Users can save to /data/user. */
    if (obname==USER) {
	if (sscanf(file,USER_SAVE_DIR+"%s.dat",prefix)==1 && prefix==creator) {
	    ::save_object(file) ;
	    return 1 ;
        }
        return 0 ;
    }
/* Players can save to /data/player. */
    if (obname==PLAYER) {
	if (sscanf(file,PLAYER_SAVE_DIR+"%s.dat",prefix)==1 && prefix==creator){
	    ::save_object(file) ;
	    return 1 ;
        }
        return 0 ;
    }
/* The mailer can save to /data/mail/foo.dat if foo is its creator. */
   if (obname==MAILER && file==MAIL_DIR+creator+MAIL_EXT) {
       ::save_object(file) ;
       return 1 ;
   }
/* The mailer daemon can always save. */
   if (obname==MAIL_D) {
       ::save_object(file) ;
       return 1 ;
   }
/* Any object can save to the a file with the same name as that object
   but ending with .dat instead of .c. */
    if (sscanf(file,"%s.dat",s1)==1 && s1==obname) {
	::save_object(file) ;
        return 1 ;
    }
/* For now, no other saves allowed. */
    return 0 ;
}

/* restore_object() checks to make sure you have read permission to any
   file you restore from. This is necessary to safeguard mail. */

static nomask int restore_object (string file) {
    if (!valid_read(file)) return 0 ;
    ::restore_object(file) ;
    return 1 ;
}

/* destruct_object() overrides the destruct efun. An object may only destroy
   itself.
*/

static nomask void destruct_object (object obj) {

    object env ;

    if (obj!=this_object()) return ;
/* If we have an environment, depart therefrom. */
/* having a problem, somehow, with this_object() being zero in the
   middle of an update. Can't find it, so let's just trap for it. */
    if (!this_object()) return ;
    env=this_object()->query_environment() ;
    if (env) env->release_object(this_object()) ;
    ::destruct_object(this_object()) ;
    return ;
}

/* write_file() calls valid_write() in the previous object to make
   sure the write attempt is legal.  */

static nomask varargs int write_file (string file, string str, int offset) {
    if (!valid_write(file)) return 0 ;
    return ::write_file(file,str,offset) ;
}

/* read_file() calls valid_read() in the previous object to make
   sure the write attempt is legal.  */

static nomask varargs string read_file (string file, int offset, int size) {
    if (!valid_read(file)) return 0 ;
    return ::read_file(file,offset,size) ;
}

/* remove_file calls valid_write() to check permission.  */

static nomask int remove_file (string file) {
    if (!valid_write(file)) return 0 ;
    return ::remove_file(file) ;
}

/* So does remove_dir. */

static nomask int remove_dir (string directory) {
    if (!valid_write(directory)) return 0 ;
    return ::remove_dir(directory) ;
}

/* What a coincidence! make_dir does too! */

static nomask int make_dir (string directory) {
    if (!valid_write(directory)) return 0 ;
    return ::make_dir(directory) ;
}

/* To no one's surprise, so does rename_file. */

static nomask int rename_file (string old, string new) {
    if (!valid_write(old)) return 0 ;
    if (!valid_write(new)) return 0 ;
    return ::rename_file(old,new) ;
}

/* This masks the clone_object kfun for backward compatibility, so
   that you can send files as well as objects if you want. */

static nomask object clone_object (mixed master) {
    if (typeof(master)==T_STRING) {
        master = get_object(master) ;
    }
    return ::clone_object(master) ;
}

/* This is here to impose limits on resource usage. A call_out
   can start a function call in any object, so the auto.c object
   needs to have an rlimits() call that will be invoked in every
   object when a call_out is made. The idea is to override the
   call_out kfun, redirecting the call_out to a limit_call_out
   function that imposes the rlimits() and then sends control
   to the originally called function.
   I thank harte@xs4all.nl (Erwin Harte) for this idea.

static nomask varargs int call_out (string func, int delay, mixed args) {
write ("func is "+func+" and delay is "+delay+"\n") ;
    return ::call_out("limit_call_out", delay, func, args) ;
}

static nomask varargs int limit_call_out(string func, mixed args) {
    get_object("/system/player#12")->catch_tell("lick me, "+func+"!\n") ;
    rlimits (MAX_DEPTH; MAX_TICKS) {
        return call_other(this_object(), func, args);
    }
}
*/
