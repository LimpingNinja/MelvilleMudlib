/*
   auto.c
   This object is automatically inherited by all other objects. It can
   be used to store interal data on objects, and it can also be used to
   define functions that all objects can access. Thus it can be used to
   make simul_efuns, particularly to mask sensitive efuns (eg, write_file)
   for security. See the DGD docs for more information.
   Not all the functions that are in this object necessarily need
   to be here; especially the heart beat ones, but also you might be
   be able to move the file handling and string mangling functions
   out and inherit them separately if you want to. It will improve
   performance (perhaps only slightly) to do so; it will also cause
   some confusion when you forget that you need to inherit those
   things :) By me it's not worth it, but you could do it if you
   thought it was, or if you wanted a slightly more C-like
   programming environment. But I don't like that environment :)
   Begun by Mobydick, 5-19-94
*/

/* config.h and options.h are included by overrides.c.
   admin.h is included by security.c */

/* You cannot inherit into the auto object, so these are included. */

#include "/system/auto/overrides.c"
#include "/system/auto/security.c"

/* Variables used in the heart_beat functions. */
private int heart_beat_time ;
private int heart_beat_handle ;

static nomask void write (string str) ;
static nomask object this_player() ;
static nomask int member_array (mixed elt, mixed *arr) ;

/* create() is called from init_object(), which is called when an object
   is created by the driver. It is here only so that we won't have
   problems with it being undefined. Most things will redefine it.
*/

void create() {
    return ;
}

/* init_object() is called by the driver when an object is loaded. It
   sets up the privileges and creator and calls create() in the object.*/

nomask void init_object() {
    set_privileges() ;
    set_creator() ;
    create() ;
}

/* input_to() makes a call into the user object. The next string
   passed by the user will be sent to this object to the named
   function, and will not be treated as a command. 
   If flag is not zero, the string will _not_ be echoed back to the
   user's terminal. Useful for password entry.
   It is static so one cannot call_other input_to(). Maybe we ought
   to permit such calls? It seems Dangerous to do so but I can imagine
   uses for it. But I think it's best not to unless you need to. */

varargs static nomask void input_to (string func,int flag) {

    object ob ;

/* The call should go to the user associated with this_player().
   If there isn't one, probably the best bet is the current user. */

    if (this_player()) ob = this_player()->query_user() ;
    if (!ob) ob=this_user() ;

    ob->set_input_to(this_object(),func,flag) ;
    return ;
}

/* This function capitalizes the first letter of a string. Handy.
   Taken from Dworkin's auto object. */

static nomask string capitalize(string str) {
    if (!str) return str ;
    if (str[0] >= 'a' && str[0] <= 'z') {
        str[0] -= 'a' - 'A';
    }
    return str;
}

/* This one lowercases all caps in the string. Also taken from Dworkin. */

static nomask string lowercase(string str) {

    int i ;

    for (i=0;i<strlen(str);i++) {
        if (str[i]>='A' && str[i]<='Z') str[i] += 'a' - 'A' ;
    }
    return str ;
}

/* This returns 1 if a file exists, -1 if a directory with that name
   exists, and 0 if no file exists. */

static nomask int file_exists (string str) {

    mixed *val ;
    int *sizes ;

    val=get_dir(str) ;
    sizes = val[1] ;
/* If there's no names at all, then the file doesn't exist. */
    if (!sizes || sizeof(sizes)==0) return 0 ;
/* It's not clear to me what the behavior should be in the case of
   more than one file returning (ie, if a wildcard is passed). For
   now, I'm going to return information for the first file in the
   list. Don't pass wildcards to this function, eh?  ;)    */
    if (sizes[0]==-2) return -1 ;
    return 1 ;
}

/* This one takes an object and returns the file name part of the
   object name. TMI-2 has the same thing. */

static nomask string base_name(object ob) {

    string str,obname ;
    int clone ;

    if (!ob) ob=this_object() ;
    obname = object_name(ob) ;
    if (sscanf(obname,"%s#%d",str,clone)==2) return str ;
    return obname ;
}

/* This one returns the clone number. Dunno if anyone has this but
   it seems like an obvious thing to have. If the object is a master
    copy, it returns 0. */

static nomask int clone_num(object ob) {

    string str ;
    int clone ;

    if (!ob) ob=this_object() ;
/* I'm getting a very weird bug where this_object() is returning 0.
   Surely if this_object() is 0, clone_num is 0 also?
*/
   if (!ob) return 0 ;
    if (sscanf(object_name(ob),"%s#%d",str,clone)==2) {
        return clone ;
    }
    return 0 ;
}

/* This one either finds or loads an object and returns it. It is
   basically a version of the kfun find_object() that Knows What To
   Do if it doesn't find the object. I don't want to override
   find_object() because there might be times when you want to know
   whether an object is or is not loaded and find_object() is useful
   for that.
*/

static nomask object get_object (string name) {

   object obj ;

/* If name has a trailing .c, let's get rid of it. */
    if (strlen(name)>2 && name[strlen(name)-2..strlen(name)-1]==".c") {
	name = name[0..strlen(name)-3] ;
    }
   obj = find_object(name) ;
   if (!obj) obj = compile_object(name) ;
   return obj ;
}

/* Send a message to this_player() if there is one, and this_user()
   if there isn't one. If there's no this_user() either, toss it
   over to this_object() and hope for the best. This means write()
   and say() can continue to be used inside the user object during
   call_outs (when this_user() is 0 because DGD handles it that way.)
   If you need to send messages in call_out'd functions, you can pass
   the player to the function and use catch_tell(). */

static nomask void write (string str) {
    if (this_player()) {
	this_player()->catch_tell(str) ;
	return ;
    }
/* If there's no working player, pop it to the user who started the
   call. That might be important in login, for instance. */
    if (this_user()) {
        this_user()->catch_tell(str) ;
	return ;
    }
/* No user either? Then pop it to this_object(). Probably we're
   in a call_out, and sending it to this_object() is better than
   letting it go nowhere. */
   this_object()->catch_tell(str) ;
}

/* Send a message to all players in the same room as the current one.
   Note that it calls catch_tell() in the room, to pass the string along
   to the room object. This might be handy sometimes. Right now the room
   can return 0 to prevent the say if it wants to. You might want to let
   the room alter the say and return a new value if you think that'd be
   cool.
*/

static nomask varargs void say (string str, object *excluded) {

    object ob, env ;
    object *contents ;
    int i ;

/* Find the environment of the current user. */

    ob = this_player() ;
    if (!ob) ob=this_object() ;
    if (!ob) return ;
    env = ob->query_environment() ;
/* Make sure we're not in the void. */
    if (!env) return ;
/* Notify the room of what's being said. If the room returns 0 we
   abort the say attempt. */
    i = env->catch_tell(str) ;
    if (!i) return ;
/* Get every object in the room, and if it's living, call catch_tell. */
    contents = env->query_inventory() ;
    if (!contents || sizeof(contents)==0) return ; /* should be impossible */
    for (i=0;i<sizeof(contents);i++) {
        if (!contents[i]->query_living()) continue ;
	if (contents[i]==ob) continue ;
	if (excluded && member_array(contents[i],excluded)>-1) continue ;
	contents[i]->catch_tell(str) ;
    }
}

/* Dump a file to the user, line by line. Don't exceed MAX_CAT_LINES. */

static nomask int cat (string file) {

    int i ;
    string *lines ;

    i=file_exists(file) ;
    if (i==0) {
        write ("No such file.\n") ;
	return 0 ;
    }
    if (i==-1) {
        write ("That file is a directory.\n") ;
        return 0 ;
    }
    lines = explode(read_file(file),"\n") ;
    if (sizeof(lines)>MAX_CAT_LINES) lines = lines[0..MAX_CAT_LINES-1] ;
    for (i=0;i<sizeof(lines);i++) {
        write (lines[i]+"\n") ;
    }
    return 1 ;
}

/*
 * NAME:        member_array()
 * DESCRIPTION: return the index of the element in the array, or -1
 * Taken from Dworkin's auto object.
 */
static nomask int member_array(mixed elt, mixed *arr) {

    int i, sz;

    sz = sizeof(arr) ;
    for (i=0;i<sz;i++) {
        if (arr[i] == elt) {
            return i;
        }
    }
    return -1;
}

/* Return the appropriate article for a string. */

static nomask string article (string str) {
    if (member_array(str[0],({ 'a', 'e', 'i', 'o', 'u' }))==-1) return "a" ;
    return "an" ;
}

/* The all-important destruct call. You can call this from any object.
   It will decide whether it wants to be destructed or not. For most
   objects it will acquiese, but you can override this if you want
   other behavior. */

int destruct() {
    destruct_object(this_object()) ;
    return (this_object()==0) ;
}

/* The infamous resolve_path(). Used to eliminate any . and .. from
   a path before applying it. */

static nomask string resolve_path (string path) {

    int i,j ;
    string *dirs ;

    if (!path) return "" ;
    dirs = explode(path,"/") ;
/* First, remove any . from the array. */
    dirs -= ({ "." }) ;
/* Next, replace all occurences of ~ with the directory of this_user().
   It may sometimes be that this_user() is not right but we'll cross
   this bridge when we come to it.
   Also replace ~foo with the directory of foo.
*/
    for (i=0;i<sizeof(dirs);i++) {
	if (!dirs[i] || dirs[i]=="") continue ;
	if (dirs[i][0]=='~') {
	    if (strlen(dirs[i])==1) {
		dirs[i] = "users/"+this_user()->query_name() ;
	    } else {
		dirs[i] = "users/"+dirs[i][1..] ;
	    }
	}
    }
/* Now remove any .. and the preceding element. */
    i = member_array("..",dirs) ;
    while (i>-1) {
/* Can't start with a ..  */
        if (i==0) return "" ;
        j = sizeof(dirs) ;
/* Piece it back together, depending on if we remove the first two
   elements, the last two, or two from the middle. */
	if (i==1) {
	    if (j==2) return "" ;
	    dirs = dirs[i+1 .. j-1] ;
        } else {
	    if (i==j-1) {
	        dirs = dirs[0 .. i-2] ;
            } else {
	        dirs = dirs[0 .. i-2] + dirs[i+1 .. j-1] ;
	    }
	}
/* Look for another .. in the path. */
	i = member_array("..",dirs) ;
    }
    path = implode(dirs,"/") ;
    return path ;
}

/* absolute_path() returns 1 if the string is an absolute path (ie, it
   begins with ~ or /) and 0 if not. This is done in so many different
   commands that it seemed wisest to just put it in the auto object so
   you didn't have to change it in 30 places. The null string returns 0. */

static nomask int absolute_path (string str) {
    if (!str || str=="") return 0 ;
    if (str[0]=='/' || str[0]=='~') return 1 ;
    return 0 ;
}

/* log_file() is a front-end for write_file() but it has some special
   privileges that permit objects to write to /log when they don't
   ordinarily have permission to do so. See valid_write() for details.
*/

static nomask int log_file (string file, string str) {
    if (!valid_write(file)) return 0 ;
    return ::write_file(file,str) ;
}

/* pad takes a mixed arg, converts to string, and pads it to the desired
   length. Left-justify is the default: if the justify arg is 1, it will
   be right-justified. */

static nomask varargs string pad (string str, int size, int justify) {

    int i,len ;

    len = strlen(str) ;
    if (len>=size) return str[0..size-1] ;
    len = size-len ;
    for (i=0;i<len;i++) {
	if (justify) {
	    str = " "+str ;
	} else {
	    str += " " ;
	}
    }
    return str ;
}

/* A simple file-copying function. Read the file, write it to a new
   name. It will choke if the file has more than MAX_ARRAY_SIZE lines:
   in that case it should (but does not) break the file up into groups
   of MAX_ARRAY_SIZE lines and do it piece by piece. The extension is
   left as an exercise for the student. :P
   This implementation will not permit you to copy over an existing file,
   because I've wiped out too much stuff myself that way.  */

static nomask int copy_file (string oldfile, string newfile) {

    string contents ;

    if (file_exists(oldfile)<1) return 0 ;
    if (file_exists(newfile)) return 0 ;
    if (!valid_read(oldfile)) return 0 ;
    if (!valid_write(newfile)) return 0 ;
    contents = read_file(oldfile) ;
    if (!contents || strlen(contents)==0) return 0 ;
    return write_file (newfile,contents) ;
}

/* The previous_function() function. It reports the name of the
   function in previous_object() that called the function which
   calls previous_function(). (Read that a couple times until it
   makes sense.)
   It's just a front end that extracts the particular information
   from the call_trace() kfun.
*/

static nomask string previous_function() {

    mixed *trace ;
    mixed *elem ;
    int size ;

    trace = call_trace() ;
    size = sizeof(trace) ;
/* The last element is this function. The second to last element is
   the function that requested previous_function(). The third to
   last element is the function before that: that's the one we want. */   
   if (size<3) return 0 ;
   elem = trace[size-3] ;
/* Element 2 is the function name. */
   return elem[2] ;
}

/* The this_player() function returns the working player body. This
   is tracked in the users daemon. It will normally be the player
   body associated with this_user(). If you used a force command
   then it'll be the body of the player you forced. Security is
   provided in the users daemon; see that file for details.
*/

static nomask object this_player() {
    return get_object(USERS_D)->query_this_player() ;
}

/* The fail_msg() autofun calls the set_fail_msg() in this_player().
   This allows you to set a failure message in the player to be printed
   instead of "What?" when a command doesn't recognize the input it was
   given.
   It is equivalent, and maybe better, to just call the function in the
   player.c object directly. It would be more properly object oriented
   to do so, it would be faster, and it would make for one less autofun.
   However, it is also much more typing, and I want it to be very easy
   for wizards to use this in their own add_commands, so I don't want
   people to have to type this_player()-> all the time. Also, doing it
   this way is analagous to the way MudOS handled it (the notify_fail()
   efun) and so I think people are more used to this.
*/

static nomask void fail_msg (string str) {
    this_player()->set_fail_msg(str) ;
}

/* The heartbeat function. There is really only one function, which is
   set_heart_beat(). It takes an integer arg. Setting it to zero turns
   off the heartbeat: setting it to an integer N provides a call_out to
   heart_beat() every N seconds.
   If you change the heartbeat time while the heartbeat is on, then the
   current callout will be executed and the time change will take effect
   for the following call. This is done because it's simplest and not
   for any deep philosophical reason.
   There is a second function, do_heart_beat(), which does the callout
   to heart_beat() and which should be left alone. */

static void set_heart_beat (int time) {

/* If we're turning it off, handle that case. */
    if (time==0) {
        heart_beat_time=0 ;
	remove_call_out (heart_beat_handle) ;
        return ;
    }

/* If it's not on now, then start it. */
    if (heart_beat_time==0) {
        heart_beat_handle=call_out("do_heart_beat",time) ;
    }

/* Set the new heartbeat time. */
    heart_beat_time = time ;
    return ;
}

/* This is called out. It sets up the next call to heart beat,
   then makes the call.
   It makes the heartbeat call before it sets up the next one.
   This way, if there's a runtime error in the heartbeat, the
   heartbeat won't be called again. Some fancy footwork has to
   be done to get things to Work Out Right. 
   It can only be called by the driver or within the same object.
   You probably -don't- want to call it within the same object -
   go through set_heart_beat - but if for some reason you need
   to, fine. It can't be static because the driver has to be
   able to reach it with call_out.
*/

nomask void do_heart_beat() {

    string err ;

    if (previous_object() && previous_object()!=this_object()) return ;
    err = catch(this_object()->heart_beat()) ;
    if (err) {
	heart_beat_time=0 ;
	return ;
    }
    heart_beat_handle = call_out("do_heart_beat",heart_beat_time) ;
}
