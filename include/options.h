/* User-configurable options for various things. */

/* The name of the mud :)  */
#define MUD_NAME "Melville"

/* Time interval in seconds at which swapout() shall be called. */
#define SWAPTIME 900

/* The maximum number of lines that will be displayed by cat. */
#define MAX_CAT_LINES 100

/* If this is 1, new users are wizzed: if it's 0, they aren't. */
#define AUTO_WIZ 1

/* The extensions used on player and user save files. */
#define USER_SAVE_EXT ".dat"
#define PLAYER_SAVE_EXT ".dat"

/* The extension used on mail spools. */
#define MAIL_EXT ".mail"

/* The error message given to players when their commands don't work. */
#define ERROR_MSG "What?\n"

/* The length at which messages sent to the user by catch_tell will be
   wrapped. Messages sent directly via send_message() are not wrapped.
   No messages are wrapped if this is undefined. */
#define LINE_WRAP 78

/* Maximum depth of the stack (function calls in any one execution
   thread) and maximum tick cost of execution. */

#define MAX_DEPTH 20
#define MAX_TICKS 100000

/* The size of a player's history stack. */

#define HISTORY_SIZE 20

/* A list of all currently valid channels. */
#define CHANNELS ({ "gossip", "wiz" })

/* A list of channels which are wizard_only. */
#define WIZ_CHANNELS ({ "wiz" })

/* If it makes you feel better, you can define this to sizeof(CHANNELS)
   and then you don't have to update it. */
#define NUM_CHANNELS 2

/* Define this if you want state to be saved at shutdown, and
  undefine it if you don't want that. */
#define DUMP_STATE_AT_SHUTDOWN 1

/* Define this if you want users to enter the game at the point where
   they quit it (this would seem useful for continuous games). Do not
    defined it if you don't want that. */
#define START_AT_QUIT_LOCATION 1
