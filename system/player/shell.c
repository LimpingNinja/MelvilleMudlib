/*
   shell.c
   Handles aliases in a not-very-sexy but functional way.
   Might install something better later: I might not.
   It also permits !! and ^^substitution. This would also be a good
   place to put a full history queue at some time.
*/

#include <config.h>
#include <options.h>

static string *last_commands ;
static int last_command ;
static int num_commands ;

/* This is called from create() in player.c, and preps the user's
   history stuff. */

static void initialize_shell() {
    num_commands=0 ;
    last_command=HISTORY_SIZE-1 ;
    last_commands = allocate(HISTORY_SIZE) ;
  }

/* Called by command() in player.c. Very primitive right at the
   moment, and scheduled to be upgraded RSN.
   Should process_history (see below) be called first, or second? */

static string process_alias (string command) {
    if (command=="l") command="look" ;
    if (command=="n") command="go north" ;
    if (command=="s") command="go south" ;
    if (command=="e") command="go east" ;
    if (command=="w") command="go west" ;
    if (command=="i") command="inventory" ;
    if (strlen(command)>2 && command[0..2]=="exa")
              command="look at"+command[3..] ;
    return command ;
}

/* Also called from command() in player.c. */

static string process_history (string command) {

    string old,replace,first,last ;
    int prevnum, prevloc, i, j ;

/* First check for "!!" which repeats the last command, and appends
   anything else that was typed to it. */

    if (strlen(command)>1 && command[0..1]=="!!") {
	if (strlen(command)==2) {
	    command = last_commands[last_command] ;
	} else {
	    command = last_commands[last_command]+command[2..] ;
	}
	write (command+"\n") ;
	return command ;
    }

/* Next check for "!num" which repeats command num if we can still
   access it in the history mapping. */
   if (sscanf(command,"!%d",prevnum)==1) {
       if (prevnum<num_commands-HISTORY_SIZE) {
	   write ("History only extends back to "+(num_commands-HISTORY_SIZE+2)+
		  ".\n") ;
	   return "" ;
	 }
       prevloc = last_command - (num_commands-prevnum) ;
       if (prevloc<0) prevloc += HISTORY_SIZE ;
       return last_commands[prevloc] ;
     }

/* Now check for "!foo" which repeats the last command beginning with foo.*/
   if (sscanf(command,"!%s",old)==1) {
/* Start looking at the last command and go backwards. When we reach -1,
   return to the top of the list. When we get to last_command+1, we are
   done. */
/* This fails if last_command happens to be HISTORY_SIZE-1 (because
   in that case we'll never get it equal to HISTORY_SIZE.) In that
   case we start at -1 (which is then immediately rolled over to
   HISTORY_SIZE-1) and stop when we reach -1+1 = 0. */
       j = last_command ;
       if (j==HISTORY_SIZE-1) j=-1 ;
       for (i=j;i!=j+1;i--) {
	   if (i==-1) i=HISTORY_SIZE-1 ;
	   if (strlen(last_commands[i])>=strlen(old)) {
	       if (last_commands[i][0..strlen(old)-1]==old) {
		   return last_commands[i] ;
	       }
	   }
       }
   }

/* Second, check for ^foo^bar, which takes the last command and
   replaces the first occurence of "foo" with "bar" and re-issues
   it. EG. if last command was "foofoo", then "^foo^bar" will
   send the command "barfoo". */

    if (sscanf(command,"^%s^%s",old,replace)==2) {
        if (sscanf(last_commands[last_command],"%s"+old+"%s",first,last)!=2) {
	    write ("Subsitution failed.\n") ;
	    return "" ;
        }
        command = first+replace+last ;
        write (command+"\n") ;
        return command ;
    }

/* Then just send back whatever was sent. */
    return command ;
}

static void increment_history (string str) {

    int oldest ;

/* First, increment the number of commands issued. */
    num_commands++ ;

/* Second, replace the oldest command with the current one. */
    oldest = last_command+1 ;
    if (oldest==HISTORY_SIZE) oldest=0 ;
    last_commands[oldest]=str ;

/* Third, update the last_command pointer. */
    last_command++ ;
    if (last_command==HISTORY_SIZE) last_command=0 ;

    return ;
}

/* Some querying functions that only the history command is allowed
   to call. Privacy, you know. */

int query_last_command() {
    if (object_name(previous_object())!=HISTORY_CMD) return 0 ;
    return last_command ; 
}

string *query_commands() {
    if (object_name(previous_object())!=HISTORY_CMD) return ({ }) ;
    return last_commands ;
}

int query_num_commands() {
    return num_commands ;
}
