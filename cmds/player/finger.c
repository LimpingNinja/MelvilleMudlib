/*
   finger.c
   The finger command, tells you when a user was last on, his real
   name and email address, and so forth.
   It does _not_ restore the user object because the user object
   can be cloned only by the driver object (for security reasons).
   Instead we read the .o file by hand.
   Written by Mobydick, 5-28-94.
*/

int do_command (string str) {

    int i,bogon ;
    string s1,s2 ;
    string *user_file ;
    mapping user_data ;

    if (!str) {
        fail_msg ("Not supported.\n") ;
        return 0 ;
    }
    if (file_exists("/data/user/"+str+".dat")<1) {
        fail_msg ("There is no player named "+capitalize(str)+".\n") ;
	return 0 ;
    }
    user_file = explode(read_file("/data/user/"+str+".dat"),"\n") ;
    user_data = ([ ]) ;
    for (i=0;i<sizeof(user_file);i++) {
        if (sscanf(user_file[i],"%s %d",s1,bogon)==2) {
	   user_data[s1]=bogon ;
        } else {
            if (sscanf(user_file[i],"%s %s",s1,s2)==2) {
	        user_data[s1]=s2 ;
	    }
	}
    }
    /* The formatting is kind of primitive since I haven't written
       any sort of printf() yet. */
    write ("User name:   "+user_data["cap_name"]+"\n"+
	   "Real name:   "+user_data["real_name"]+"\n"+
	   "Email:       "+user_data["email"]+"\n"+
	   "Last on:     "+ctime(user_data["last_on"])+"\n"+
	   "From:        "+user_data["last_ip"]+"\n") ;
    return 1;
}
