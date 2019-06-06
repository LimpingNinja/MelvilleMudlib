/* A command to call the status() kfun and print the results.
   Similar to a command in the 2.4.5 lib but the code is all my own.
   Mobydick, 8-1-94.
*/

#include <status.h>

string convert_time (int i) {

    int weeks, days, hours, minutes, seconds ;
    string val ;

    weeks = i/604800 ;
    i = i%604800 ;
    days = i/86400 ;
    i = i%86400 ;
    hours = i/3600 ;
    i = i%3600 ;
    minutes = i/60 ;
    seconds = i%60 ;
    val = "" ;
    if (weeks) val += weeks+" weeks, " ;
    if (weeks || days) val += days+" days, " ;
    if (weeks || days || hours) val += hours+" hours, " ;
    if (weeks || days || hours || minutes) {
	if (weeks || days || hours) val += minutes+" minutes, and " ;
	else val += minutes+" minutes and " ;
    }
    val += seconds+" seconds." ;
    return val ;
}

int do_command(string str) {

    mixed *stats ;
    object obj ;

    if (!str) {
        stats = status() ;
        write ("Driver version:     "+stats[ST_VERSION]+"\n") ; 
        write ("Started at:         "+ctime(stats[ST_STARTTIME])+"\n") ;
        write ("Boot time:          "+ctime(stats[ST_BOOTTIME])+"\n") ;
	write ("Uptime:             "+convert_time(stats[ST_UPTIME])+"\n") ;
        write ("Swap size:          "+stats[ST_SWAPSIZE]+"\n") ;
        write ("Swap space used:    "+stats[ST_SWAPUSED]+"\n") ;
        write ("Swap rate:          "+stats[ST_SWAPRATE1]+"\n") ;
        write ("Static memory:      "+stats[ST_SMEMSIZE]+"\n") ;
        write ("Static mem used:    "+stats[ST_SMEMUSED]+"\n") ;
        write ("Dynamic memory:     "+stats[ST_DMEMSIZE]+"\n") ;
        write ("Dynamic mem used:   "+stats[ST_DMEMUSED]+"\n") ;
        write ("Object table size:  "+stats[ST_OTABSIZE]+"\n") ;
        write ("Objects loaded:     "+stats[ST_NOBJECTS]+"\n") ;
        write ("Callout table size: "+stats[ST_COTABSIZE]+"\n") ;
        write ("Short callouts:     "+stats[ST_NCOSHORT]+"\n") ;
        write ("Long callouts:      "+stats[ST_NCOLONG]+"\n") ;
        write ("User table size:    "+stats[ST_UTABSIZE]+"\n") ;
        write ("Editor table size:  "+stats[ST_ETABSIZE]+"\n") ;
        return 1 ;
    }
    obj = find_object(str) ;
    if (!obj) {
        write ("No such object as "+str+"\n") ;
	return 1 ;
    }
    stats = status(obj) ;
    write ("Compiled at:      "+ctime(stats[O_COMPILETIME])+"\n") ;
    write ("Program size:     "+stats[O_PROGSIZE]+"\n") ;
    write ("Data size:        "+stats[O_DATASIZE]+"\n") ;
    write ("Sectors used:     "+stats[O_NSECTORS]+"\n") ;
    write ("Callouts pending: "+sizeof(stats[O_CALLOUTS])+"\n") ;
    return 1 ;
}
