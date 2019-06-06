/* The bulletin board inheritable.
   Post notes, read notes, remove notes.
   Should be inhereited by a specific board with a unique file name: the
   name of the note save file is based on the file name of that object.
   The inheriting object should set a long and a short normally, and
   move itself to an interesting location. It should also call ::create
   to invoke the create() function in this file. It need do nothing else: 
   this inheritable handles the actual note mechanics.
   There is an example bboard object, /world/board.c, which inherits
   this object and which you should examine.
   Does not, as yet, archive notes, or lock them or any of that.
   Mobydick started this, 7-17-94. Brasil!
   I know not who wrote the first bulletin board object, but I got
   the idea from TMI and I know that Zak was connected with that
   object, at least to the extent of adding to it.
*/

#define MAX_NOTES 30
#define ARCHIVE_KEEP 10          /* # of notes kept at archive time */

#include <config.h>

inherit OBJECT ;

static void archive_board() ;

/* Each note is a mapping, with four fields: name of the poster, time
   of the posting, subject of the post, and the contents. MAX_POSTS of
   them are permitted at once: after that the board has to be archived.
*/

mixed *notes ;
int lastnum, in_post ;

/* Here for a bugfix. */

int move (string dest) {
    return ::move(dest) ;
}

/* This just returns the name of the save file so it's in one place
   instead of being separately calculated everywhere. In the case
   of /inherit/board (the inheritable) we return a nonsense string so
   that the inheritable will not produce a save file. */

string save_file_name() {

    string tmp1, tmp2 ;

    tmp1 = object_name(this_object()) ;
    if (tmp1=="/inherit/board") return "please.don't.save.me" ;
    return tmp1 + ".dat" ;
}

void create() {

    int i ;

    set_id( ({ "board" }) ) ;
/* If there's a save file, we need to restore it: if not, we need to
   initialize a blank data structure. */
    if (file_exists(save_file_name())==1) {
	restore_object(save_file_name()) ;
    } else {
	notes = allocate(MAX_NOTES) ;
	for (i=0;i<MAX_NOTES;i++) {
	    notes[i] = ([ ]) ;
	}
	lastnum = 0 ;
    }
    add_command("post", "post_note") ;
    add_command("read", "read_note") ;
    add_command("remove", "remove_note") ;
}

/* Don't really want people carrying these around. */

int prevent_get() {
    return 1 ;
}

int post_note (string str) {

    if (!str) str = "(no subject)" ;
    write ("Enter the text of your note. \".\" ends, \"~a\" aborts.\n"+
        "---------------------------------------------------------------\n");
    this_player()->begin_edit("finish_post",str) ;
    return 1 ;
}

void finish_post (string *foo, string subj) {
    if (!foo || sizeof(foo)==0) {
        write ("Posting aborted.\n") ;
	return ;
    }
    if (lastnum==30) archive_board() ; /* changes lastnum to 10 */
    notes[lastnum]["subject"] = subj ;
    notes[lastnum]["text"] = implode(foo,"\n") ;
    notes[lastnum]["time"] = ctime(time()) ;
    notes[lastnum]["poster"] = previous_object()->query_cap_name() ;
    write ("Posted.\n") ;
    lastnum += 1 ;
/* Would not want to lose a post if the game crashed before this was saved. */
    save_object(save_file_name()) ;
    return ;
}

int read_note (string str) {

    int readnum ;

    if (lastnum==0) {
	write ("There are no notes on the board to be read.\n") ;
	return 1 ;
    }
    sscanf(str,"%d",readnum) ;
    if (!readnum || readnum<1 || readnum>lastnum) {
        write ("Read a note from 1 to "+lastnum+".\n") ;
	return 1 ;
    }
    readnum -- ;
    write ("Note "+(readnum+1)+" written by "+notes[readnum]["poster"]+" at "+
	   notes[readnum]["time"]+"\nSubject: "+notes[readnum]["subject"]+
	   "\n") ;
    write("-----------------------------------------------------------\n") ;
    write (notes[readnum]["text"]+"\n") ;
    return 1 ;
}

int remove_note (string str) {

    int remvnum,i ;

    if (lastnum==0) {
	write ("There are no notes on the board to be removed.\n") ;
	return 1 ;
    }
    sscanf(str,"%d",remvnum) ;
    if (!remvnum || remvnum<1 || remvnum>lastnum) {
        write ("Remove a note from 1 to "+lastnum+".\n") ;
	return 1 ;
    }
/* Admins can remove a note, as can the person who posted it. No one else. */
    if (this_user()->query_privileges()!="admin" &&
        capitalize(this_user()->query_creator())!=notes[remvnum]["poster"]) {
        write ("You don't have permission to remove that note.\n") ;
	return 1 ;
    }
/* Ok, we have permission, so remove the note and compress the stack. */
    for (i=remvnum;i<lastnum;i++) {
        notes[i-1] = notes[i] ;
    }
    notes[lastnum-1]= ([ ]) ;
    lastnum -- ;
    write ("Note "+remvnum+" removed.\n") ;
/* Save the new state of the board in case of crash. */
    save_object(save_file_name()) ;
    return 1 ;
}

string query_short() {
    return short_desc + " ("+lastnum+" notes)" ;
}

string query_long() {

    int i ;
    string str, numstr, namestr ;

    str = long_desc ;
    if (lastnum==0) {
	return str+"There are no notes on the board to be read.\n" ;
    }
    str += "The board contains the following notes:\n" ;
    for (i=0;i<lastnum;i++) {
        numstr = pad(i+1+"",3) ;
	namestr = pad(notes[i]["poster"],15) ;
	str += numstr+namestr+notes[i]["subject"]+"\n" ;
    }
    return str ;
}

/* archive_board is invoked when the number of notes on the board reaches
   30 and someone posts a 31st note. It copies notes 21-30 into positions
   1-10 and blanks out notes 11-30. 
   At the moment, the other 20 notes are completely lost. You may want
   to add an archiving system of some sort: but I don't. :)
   Because I don't post much, this has not been thoroughly tested. */

static void archive_board() {

    int i ;

    for (i=0;i<ARCHIVE_KEEP;i++) {
        notes[i] = notes[i+MAX_NOTES-ARCHIVE_KEEP] ;
    }
    for (i=ARCHIVE_KEEP;i<MAX_NOTES;i++) {
        notes[i] = ([ ]) ;
    }
/* Have to change the pointer to the last note now. */
    lastnum = ARCHIVE_KEEP-1 ;
    return ;
}

/* Any time this is destructed we want to save its contents. */

int destruct() {
    save_object(save_file_name()) ;
    return ::destruct() ;
}
