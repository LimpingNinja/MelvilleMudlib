/* The task command. 
   Lets a wizard report something that needs to be created or otherwise
   attended to. It logs the location and time of the message as
   well as any text the user types. Mostly intended as an organizational
   device. Players cannot use this but can use the bug command.
   Mobydick, 7-11-94.
   The idea is due to Pallando@TMI-2's similar commands, which he may
   have gotten from somewhere else.
*/

int do_command() {
	write ("Begin entering your task report. Be as specific as you can\n"+
      "be about what needs to be coded and how. \".\" ends, \"~a\" aborts.\n");
	write ("----------------------------------------------------------\n");
	previous_object()->begin_edit("finish_report") ;
	return 1 ;
}

void finish_report (string *report) {

	string str ;

    if (!report || sizeof(report)==0) {
	write ("Bug report aborted.\n") ;
	return ;
    }
    str = previous_object()->query_cap_name()+" at "+ctime(time())+" in "+
	  object_name(previous_object()->query_environment()) ;
    report = ({ str }) + report ;
    str = implode(report,"\n") + "\n" ;
    write_file ("/log/TASKS",str) ;
    write ("Your report has been saved.\n") ;
    return ;
}
