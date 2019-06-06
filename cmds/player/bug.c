/* The bug command. 
   Lets a player (or wizard) report a problem that needs wizardly or
   adminly attention. It logs the location and time of the message as
   well as any text the user types.
   Mobydick, 7-11-94.
   The idea is due to Pallando@TMI-2's similar command, which he may
   have gotten from somewhere else.
*/

int do_command() {
	write ("Begin entering your bug report. Be as specific as you can\n"+
	"be about the problem and its solution. \".\" ends, \"~a\" aborts.\n") ;
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
    write_file ("/log/BUGS",str) ;
    write ("Your report has been saved.\n") ;
    return ;
}
