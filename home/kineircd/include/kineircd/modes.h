/* modes.h
 * Flags :) Modes :) Whatever you wanna call them :)
 */

#ifndef __MODES_H_
# define __MODES_H_

# include "str.h"

/* This is the type of the modes - big enough for the modes to fit!
 * Warning: Changing these MAY EFFECT SERVER PROTOCOL!!
 * These are kinda temporary too, btw. Use User::modes_t/Channel::modes_t etc
 */
typedef unsigned long long TYPE_USERMODES;
typedef unsigned long long TYPE_CHANMODES;
typedef unsigned short TYPE_CHANMEMMODES;
typedef unsigned long TYPE_SERVERMODES;

/*
 * Once these flags are set, do not change them - they may effect protocol
 * output etc.
 */

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
