/* edit.c
   A simple input_to based editor.
   Can be called by mailers, bulletin boards, and other objects which
   allow players to write files.
   You fire it up by calling begin_edit, passing it the name of a 
   function to call when the edit finishes. The return call must go
   to the same object that started the edit, for security. You can
   also pass a second argument which will be carried along and sent
   back to the return call. The bboard, for example, uses this to
   store the subject of the note.
   Mobydick, 7-10-94.
   Should eventually add a hook for wizards to enter the real editor.
*/

static string *file ;
static object return_ob ;
static string return_func ;
static int edit_busy ;
static mixed argument ;

object query_user() ;
static void finish_edit () ;

varargs int begin_edit(string func, mixed arg) {
    if (edit_busy) return 0 ;
    if (!func) return 0 ;
    argument = arg ;
    edit_busy = 1 ;
    file = ({ }) ;
/* For security reasons, the return call must go to the calling object. */
    return_ob = previous_object() ;
    return_func = func ;
    write ("*\b") ;
    input_to ("enter_line") ;
    return 1 ;
}

void enter_line (string str) {
    if (previous_object()!=query_user()) return ;
    if (str==".") {
        finish_edit() ;
	return ;
    }
    if (str=="~a") {
        file = ({ }) ;
	finish_edit() ;
	return ;
    }
    file += ({ str }) ;
    write ("*\b") ;
    input_to ("enter_line") ;
}

static void finish_edit() {
    edit_busy=0 ;
    if (argument) {
        call_other (return_ob,return_func,file,argument) ;
    } else {
        call_other(return_ob,return_func,file) ;
    }
    return ;
}
