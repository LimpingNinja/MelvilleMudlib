/*
   config.h
   Various defines for commonly referenced objects.
*/

/* The system objects */
#define DRIVER "/system/driver"
#define PLAYER "/system/player"
#define USER "/system/user"
#define AUTO "/system/auto"

/* Inheritable objects */
#define OBJECT "/inherit/object"
#define CONTAINER "/inherit/container"
#define ROOM "/inherit/room"
#define BOARD "/inherit/board"

/* Critical locations in the world */
#define START "/world/start"
#define VOID "/world/void"

/* The mailer */
#define MAILER "/system/mailer"

/* Privileged commands */
#define SHUTDOWN_CMD "/cmds/admin/shutdown"
#define QUIT_CMD "/cmds/player/quit"
#define CD_CMD "/cmds/wizard/cd"
#define ED_CMD "/cmds/wizard/ed"
#define UPDATE_CMD "/cmds/wizard/update"
#define DESCRIBE_CMD "/cmds/player/describe"
#define DISCONNECT_CMD "/cmds/admin/disconnect"
#define FORCE_CMD "/cmds/admin/force"
#define HISTORY_CMD "/cmds/player/history"
#define MAIL_CMD "/cmds/player/mail"
#define MAKEWIZ_CMD "/cmds/admin/makewiz"

/* Daemons */
#define USERS_D "/system/users_d"
#define SOUL_D "/system/soul_d"
#define MAIL_D "/system/mail_d"
#define CHANNEL_D "/system/channel_d"

/* Various logs */
#define USAGE_LOG "/log/usage"
#define CREATE_LOG "/log/creation"
#define SHUTDOWN_LOG "/log/shutdown"

/* Key directories. Should always end with a /. */
#define USER_SAVE_DIR "/data/user/"
#define PLAYER_SAVE_DIR "/data/player/"
#define MAIL_DIR "/data/mail/"
#define HELP_DIR "/doc/help/"
