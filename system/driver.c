/*
   Driver.c
   The driver object. All driver calls except those that go into users
   come into this object.
   Begun by Mobydick, 5-19-94.
   This object started as a copy of the Dworkin 2.4.5 simulation driver
   object, except that the 2.4.5 preload in initialize() was removed
   and lines to preload user.c and player.c were added, and some small
   changes to privileges were made.
   Since that time it has undergone some more changes, but not many:
   the influence of Dworkin is still strongly marked upon it.
*/

#include <config.h>
#include <options.h>

/*
 * NAME:	initialize
 * DESCRIPTION:	called once at game startup
 * This function loads four objects that need to be loaded right
   away, and starts the memory-cleaning call_out chain.
 */
static void initialize() {
    send_message("DGD/Melville started at "+ctime(time())+".\n") ;
    send_message("Loading user.c\n") ;
    compile_object(USER) ;
    send_message ("Loading player.c\n") ;
    compile_object(PLAYER) ;
    send_message ("Loading the user daemon\n") ;
    compile_object(USERS_D) ;
    send_message ("Loading the start room\n") ;
    compile_object(START) ;
    send_message("Setting up ipc.\n");
    call_out("swapswap", SWAPTIME);
}

/*
 * NAME:	swapswap()
 * DESCRIPTION:	swap out all objects
 */
static void swapswap() {
    call_out("swapswap", SWAPTIME);
    swapout();
    return ;
}

/*
 * NAME:	restored()
 * DESCRIPTION:	re-initialize the system after a restore
   I want this to get all user objects (which are non-interactive
   if we just rebooted) and delete them and the bodies attached
   to them. I want this because it doesn't seem Right to have a
   lot of linkdead bodies lying around if you've just rebooted.
   Depending on the application you have in mind you might not
   want to do this, or you might want to do something else.
 */
static void restored() {

    int i ;
    object body ;
    object *usrs ;

    usrs = USERS_D->query_users() ;
    for (i=0;i<sizeof(usrs);i++) {
	if (body=usrs[i]->query_player()) {
	    catch(body->quit()) ;
	} else {
	    catch(usrs[i]->quit());
	}
    }
    send_message("\nState restored.\nSetting up ipc.\n");
    return ;
}

/*
 * NAME:	path_read()
 * DESCRIPTION:	handle an editor read path
 */

string path_read(string path) {

/* We don't alter the path at all: instead we rely on the editor getting
   the full path name. We just check for permissions. */

    if (!previous_object()->valid_read(path)) return 0 ;
    return path ;
}

/*
 * NAME:	path_write()
 * DESCRIPTION:	handle an editor write path
 */

string path_write(string path) {

/* Same as above: rely on the full path going to the editor. */

    if (!previous_object()->valid_write(path)) return 0 ;
    return path ;
}

/*
 * NAME:	call_object()
 * DESCRIPTION:	translate an object path
 */

object call_object(string path) {

    int i;
    object obj ;

    if (sscanf(path, "../%*s") != 0 || sscanf(path, "%*s/../%*s") != 0) {
	error("Illegal path");
    }
    if ((i=strlen(path)) >= 2 && path[i - 2 ..] == ".c") {
	path = path[0 .. i - 3];
    }
    obj = find_object(path) ;
    if (!obj) obj = compile_object(path) ;
    return obj;
}

/*
 * NAME:	inherit_program()
 * DESCRIPTION:	translate an inherit path
*/

object inherit_program(string file, string path) {

    int i ;
    object tmp ;
    
    if (sscanf(path, "../%*s") != 0 || sscanf(path, "%*s/../%*s") != 0) {
	error("Illegal path");
    }
    if ((i=strlen(path)) >= 2 && path[i - 2 ..] == ".c") {
	path = path[0 .. i - 3];
    }
    tmp = find_object(path) ;
    if (!tmp) tmp = compile_object(path) ;
    return tmp;
}


/*
 * NAME:	path_include()
 * DESCRIPTION:	translate an include path
 */
string path_include(string file, string path) {
    if (path[0] != '/') {
	return file + "/../" + path;
    }
    return path;
}

/*
 * NAME:	compile_object()
 * DESCRIPTION:	(not) used for virtual objects
 */
static object compile_object(string file) {
    return 0;
}

/*
 * NAME:	recompile()
 * DESCRIPTION:	(not) used to recompile objects
 */
static void recompile(object obj) {
    return ;
}

/*
 * NAME:	telnet_connect()
 * DESCRIPTION:	return a user object for a new telnet connection.
 *              The open() function will be called in the returned object.
 */
static object telnet_connect() {
    object user, player, tmp ;

    tmp =find_object(USER);
    if (!tmp) {
        tmp = compile_object(USER);
    }
    user = clone_object(tmp);
    tmp =find_object(PLAYER);
    if (!tmp) {
        tmp = compile_object(PLAYER);
    }
    player = clone_object(tmp);
    user->set_player(player);
    player->set_user(user);

    return user;
}

/* Binary connections are not yet supported. You could use one for
   lots of things; intermud services, FTP, a variety of things.
   But Melville doesn't. */

static object binary_connect() {
    return 0 ;
}

/*
 * NAME:	runtime_error()
 * DESCRIPTION:	log a runtime error
Dworkin's version of this logged the full trace to stderr, and dumped a
one-line description to the user's screen. I prefer the reverse: I'd like
the wizard in question to get all the information possible, and keep the
logs shorter. But you can choose what you want: the two strings long_err
and short_err store the full trace and the one-liner respectively. Store
them and send them where you will...
 */

static void runtime_error(string error, int caught, int ticks) {

    mixed **trace;
    string progname, objname, function, str;
    string long_err, short_err ;
    int i, sz, line, len;
    object player;

    if (caught) {
	return;
    }
    short_err = error + "\n" ;
    long_err = error + "\n" ;
    trace = call_trace();
    if ((sz=sizeof(trace) - 1) != 0) {
	for (i = 0; i < sz; i++) {
	    progname = trace[i][1];
	    function = trace[i][2];
/* I'm not sure what this is catching: bad object to call_other maybe? */
	    if (progname == AUTO && strlen(function) > 3) {
		switch (function[0 .. 2]) {
		case "bad":
		    progname = trace[i - 1][1];
		    function = trace[i - 1][2];
		case "_F_":
                case "_Q_":
		    continue ;
		default:
		    break;
		}
	    }

	    objname  = trace[i][0];
	    line     = trace[i][3];

	    if (line == 0) {
		str = "    ";
	    } else {
		str = "    " + line;
		str = str[strlen(str) - 4 ..];
	    }
	    str += " " + function + " ";
	    len = strlen(function);
	    if (len < 17) {
		str += "                 "[len ..];
	    }
	    str += " " + progname;
	    if (progname != objname) {
		len = strlen(progname);
		if (len < strlen(objname) && progname == objname[.. len - 1]) {
		    str += " (" + objname[len ..] + ")";
		} else {
		    str += " (" + objname + ")";
		}
	    }
	    long_err += str+"\n" ;
	}

	short_err += "Object: " + objname + ", program: " + progname + 
		", line " +line + "\n";

/* Send short_err to the driver log. */
        send_message(short_err) ;

/* If this player is a wizard, send longerr: if not, send the bogus message. */
	player = this_user() ;
	if (player!=0) {
	    player->catch_tell("You suddenly notice a strange distortion in the fabric of space.\n") ;
	}
	if (player != 0 && player->query_wizard()) {
	    player->catch_tell(long_err) ;
	}
    }
/* It better be loaded. It might be better to use get_object(), but
   that causes compile-time problems since auto isn't available yet.
   For now I'm leaving it this way... it can be worked around very
   easily if it turns out to be a problem. */
    find_object(USERS_D)->set_this_player(0) ;
}

/*
   compile_error() is called by the driver when an object fails
   to compile. It passes the name of the file, the line with the
   problem, and a (sometimes opaque) description of the problem.
   compile_error() writes it to the appropriate user and log file.
*/
void compile_error (string file, int line, string err) {

    string *path;
    string logfile ;

    path = explode(file, "/");
    if (path[0] == "users") {
        logfile = "/users/" + path[1] + "/log" ;
    }
    logfile = "/log/log";
    write_file(logfile,"In line "+line+" of "+file+" occurred:\n"+err+"\n") ;
    return ;
}

/*
 * NAME:         interrupt() ;
 * DESCRIPTION:  handle a crash or process kill
 * Log the event, get rid of the users, dump the state if you're into
   that sort of thing, and bail out.
*/

static void interrupt() {

    int i ;
    object *goons ;

    write_file (SHUTDOWN_LOG,MUD_NAME+" crashed at "+ctime(time())+"!\n") ;
    goons = USERS_D->query_users() ;
    for (i=0;i<sizeof(goons);i++) {
        goons[i]->catch_tell("Game driver crashing!\n") ;
        goons[i]->query_player()->quit() ;
    }
#ifdef DUMP_STATE_AT_SHUTDOWN
    dump_state() ;
#endif
    shutdown() ;
    return ;
}

/*
 * NAME:         remove_program() ;
 * DESCRIPTION:  Called when the master program of an object is
                 removed (that is, when the master object and all
		 of its clones have been destroyed)
*/

static void remove_program() {
/* This is here so the mudlib can take action when an object's code is
   removed from memory/disk, usually some kind of resource tracking
   housecleaning. I don't do that, so it does nothing. */
    return ;
}

/* compile_rlimits() and runtime_rlimits() are security that are
   invoked at compile time and runtime, respectively, whenever an
   object invokes rlimits to change the stack depth and tick limit
   used in code. Returning 1 means the change is legal, returning 0
   means it's not. If compile_rlimits returns 0, then runtime doesn't
   get checked.
   For the moment, I'm letting everything be legal cause it's easier.
   These are not static so that another object can call into the
   driver to see if something would be permitted. I don't see much
   use for that but there's no reason to stop it either.
*/

int compile_rlimits (string objname) {
    return 1 ;
}

int runtime_rlimits (object obj, int depth, int ticks) {
    return 1 ;
}

