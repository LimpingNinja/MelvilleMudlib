/*
 * This file defines the fields of the array returned by the
 * status() kfun.  It is automatically generated by DGD on startup.
 */

# define ST_VERSION	0	/* driver version */
# define ST_STARTTIME	1	/* system start time */
# define ST_BOOTTIME	2	/* system reboot time */
# define ST_UPTIME	3	/* system virtual uptime */
# define ST_SWAPSIZE	4	/* # sectors on swap device */
# define ST_SWAPUSED	5	/* # sectors in use */
# define ST_SECTORSIZE	6	/* size of swap sector */
# define ST_SWAPRATE1	7	/* # objects swapped out last minute */
# define ST_SWAPRATE5	8	/* # objects swapped out last five minutes */
# define ST_SMEMSIZE	9	/* static memory allocated */
# define ST_SMEMUSED	10	/* static memory in use */
# define ST_DMEMSIZE	11	/* dynamic memory allocated */
# define ST_DMEMUSED	12	/* dynamic memory in use */
# define ST_OTABSIZE	13	/* object table size */
# define ST_NOBJECTS	14	/* # objects in use */
# define ST_COTABSIZE	15	/* callouts table size */
# define ST_NCOSHORT	16	/* # short-term callouts */
# define ST_NCOLONG	17	/* # long-term & millisecond callouts */
# define ST_UTABSIZE	18	/* user table size */
# define ST_ETABSIZE	19	/* editor table size */
# define ST_STRSIZE	20	/* max string size */
# define ST_ARRAYSIZE	21	/* max array/mapping size */
# define ST_STACKDEPTH	22	/* remaining stack depth */
# define ST_TICKS	23	/* remaining ticks */
# define ST_PRECOMPILED	24	/* precompiled objects */

# define O_COMPILETIME	0	/* time of compilation */
# define O_PROGSIZE	1	/* program size of object */
# define O_DATASIZE	2	/* # variables in object */
# define O_NSECTORS	3	/* # sectors used by object */
# define O_CALLOUTS	4	/* callouts in object */
# define O_INDEX	5	/* unique ID for master object */

# define CO_HANDLE	0	/* callout handle */
# define CO_FUNCTION	1	/* function name */
# define CO_DELAY	2	/* delay */
# define CO_FIRSTXARG	3	/* first extra argument */
