/* src/include/modes.h
 * Flags :)
 */

#ifndef __MODES_H_
# define __MODES_H_

# include "str.h"

/* This is the type of the modes - big enough for the modes to fit!
 * Warning: Changing these MAY EFFECT SERVER PROTOCOL!!
 * BTW, you may be wondering why these are not typedefs. Answer: Some compilers
 * HATE long long's (64-bit numbers) as typedefs, GCC included. :(
 */
typedef unsigned long long TYPE_USERMODES;
typedef unsigned long long TYPE_CHANMODES;
typedef unsigned short TYPE_CHANMEMMODES;
typedef unsigned long long TYPE_SERVERMODES;

/*
 * Once these flags are set, do not change them - they may effect protocol
 * output etc.
 */

/* Channel flags
 * Note: Flags not mentioned here are most likely flags with a parameter,
 *       so check the mode tables
 */
# define CHANMODE_INVITE	0x0000000000000001 // +i (invite only)
# define CHANMODE_MODERATED	0x0000000000000002 // +m (moderated)
# define CHANMODE_NOOUTSIDEMSG	0x0000000000000004 // +n (no extern PRIVMSGs)
# define CHANMODE_TOPICLOCK	0x0000000000000008 // +t (topic locked for opers)
# define CHANMODE_PRIVATE	0x0000000000000010 // +p (private)
# define CHANMODE_SECRET	0x0000000000000020 // +s (secret)
# define CHANMODE_QUIET		0x0000000000000040 // +q (server msgs not sent)
# define CHANMODE_ANONYMOUS	0x0000000000000080 // +a (masks changed to anonymous!anonymous@anonymous)
# define CHANMODE_EVENT		0x0000000000000100 // +E (no nick changes allowed)
# define CHANMODE_REOP		0x0000000000000200 // +r (re-op mode)
# define CHANMODE_REGNICKSONLY	0x0000000000000400 // +R (registered nicks only)


// Channel member flags
# define CHANMEMMODE_CREATOR	0x0001		// +O (channel creator)
# define CHANMEMMODE_OPPED	0x0002		// +o (channel operator)
# define CHANMEMMODE_VOICED	0x0004		// +v (voiced)
# define CHANMEMMODE_HALFOPPED	0x0008		// +h (half opped)


// User flags -- note the first byte is set up on purpose (see RFC2812)
# define USERMODE_WALLOPS	0x0000000000000004 // +w (wallops)
# define USERMODE_INVISIBLE	0x0000000000000008 // +i (invisible)
# define USERMODE_NONKICKABLE	0x0000000000000100 // +k (non-kick/deop/killable)
# define USERMODE_DEAF		0x0000000000000200 // +d (deaf)
# define USERMODE_SECURE	0x0000000000000400 // +S (secure connection)
# define USERMODE_VWORLD	0x0000000000000800 // +v (virtual world)
# define USERMODE_LOCALOPER	0x0000000000001000 // +o (local operator)
# define USERMODE_GLOBALOPER	0x0000000000002000 // +O (global operator)
# define USERMODE_HELPER	0x0000000000004000 // +h (AustNet helper)
# define USERMODE_RESTRICTED	0x0000000000008000 // +r (restricted connection)
# define USERMODE_IDENTIFIED	0x0000000000010000 // +I (services identified)
# define USERMODE_GOT_IDENTD	0x0000000000020000 // +n (received identd reply)
# define USERMODE_REGNICKSMSG	0x0000000000040000 // +R (msgs from reg nicks)
# define USERMODE_IGNORING	0x0000000000080000 // +g (ignoring)


// Server flags (note flags that use paramters are not listed here)
# define SERVERMODE_HIDDEN	0x0000000000000001 // +h (hidden server)
# define SERVERMODE_NOOP	0x0000000000000002 // +n (NOOP server)
# define SERVERMODE_AUTOTBS	0x0000000000000004 // +a (automatic TBSing)
# define SERVERMODE_REMOTEMOTD	0x0000000000000008 // +M (allow remote /motd)
# define SERVERMODE_SHORTMOTD	0x0000000000000010 // +m (short motd on connection)
# define SERVERMODE_MODELOCK	0x0000000000000020 // +L (lock server mode toggles)
# define SERVERMODE_USERSFULL	0x0000000000000040 // +F (full, no more users)


// Finally get around to including these files
# include "handler.h"
# include "user.h"
# include "channel.h"
# include "server.h"


// Channel types we support for the ISUPPORT line.
# define CHANNEL_TYPES			"&#+!"


// Channel mode toggler macro

// Pointers to our flag lists
struct channelModeTableStruct {
   char letter;				// Letter of this mode
   TYPE_CHANMODES flag;			// Flag used in bit-wise operations
   bool hasParamOn;			// Requires a parameter to set?
   bool hasParamOff;			// Requires a parameter to unset?
   bool userToggle;			// Can users toggle this mode?
   bool (*toggler)(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);	// Function to toggle this mode
};

// Channel mode stuff
extern const char *channelModeStr;
extern const char *channelModeParamStr;
extern const char *channelPrefixes;
extern channelModeTableStruct channelModeTable[];
extern String makeChannelModes(Channel *);	// Make a + channel mode string

// Channel mode change routines
extern bool toggleChanmodeANONYMOUS(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeBAN(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeBANEXCEPT(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeCHANCREATOR(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeCHANOPER(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeDENY(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeEVENT(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeHALFOPER(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeINVITE(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeINVITEALLOW(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeKEY(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeLIMIT(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeMODERATED(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeNOOUTSIDEMSG(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodePRIVATE(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeQUIET(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeREGNICKSONLY(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeREOP(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeSECRET(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeTOPICLOCK(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);
extern bool toggleChanmodeVOICE(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param);


// User mode toggler macro

// User mode table structure
struct userModeTableStruct {
   char letter;                 	// Letter of this mode
   TYPE_USERMODES flag;          	// Flag used in bit-wise operations
   bool hasParamOn;			// Requires a parameter to set?
   bool hasParamOff;			// Requires a parameter to unset?
   bool userToggle;			// Can users toggle this mode?
   bool (*toggler)(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param);	// Function to toggle this mode
};

// User mode stuff
extern const char *userModeStr;
extern const char *userModeParamStr;
extern userModeTableStruct userModeTable[];
extern String makeUserModes(TYPE_USERMODES);	// Make a + user mode string
extern String makeUserModes(User *);		// Above, but includes params

// User mode change routines
extern bool toggleUsermodeDEAF(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param);
extern bool toggleUsermodeGOT_IDENTD(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param);
extern bool toggleUsermodeHELPER(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param);
extern bool toggleUsermodeIDENTIFIED(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param);
extern bool toggleUsermodeIGNORING(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param);
extern bool toggleUsermodeINVISIBLE(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param);
extern bool toggleUsermodeNONKICKABLE(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param);
extern bool toggleUsermodeOPER(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param);
extern bool toggleUsermodeREGNICKSMSG(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param);
extern bool toggleUsermodeRESTRICTED(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param);
extern bool toggleUsermodeSECURE(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param);
extern bool toggleUsermodeSERVNOTICES(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param);
extern bool toggleUsermodeVWORLD(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param);
extern bool toggleUsermodeWALLOPS(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param);


// Server mode toggler macro

// Server mode table structure
struct serverModeTableStruct {
   char letter;				// Letter of this mode
   TYPE_SERVERMODES flag;		// Flag used in bit-wise operations
   bool hasParamOn;			// Requires a parameter to set?
   bool hasParamOff;			// Requires a parameter to unset?
   bool operToggle;			// Can this mode be toggled by opers?
   bool (*toggler)(bool setting, Server *server, String *param);	// Function to toggle this mode
};

// Server mode stuff
extern const char *serverModeStr;
extern const char *serverModeParamStr;
extern serverModeTableStruct serverModeTable[];
extern String makeServerModes(Server *);	// Make a + server mode string

// Server mode change routines
extern bool toggleServermodeAUTOTBS(bool setting, Server *server, String *param);
extern bool toggleServermodeHIDDEN(bool setting, Server *server, String *param);
extern bool toggleServermodeMODELOCK(bool setting, Server *server, String *param);
extern bool toggleServermodeNOOP(bool setting, Server *server, String *param);
extern bool toggleServermodeREMOTEMOTD(bool setting, Server *server, String *param);
extern bool toggleServermodeSHORTMOTD(bool setting, Server *server, String *param);
extern bool toggleServermodeUSERSFULL(bool setting, Server *server, String *param);

#endif
