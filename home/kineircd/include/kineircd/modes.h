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


// Server mode change routines
extern bool toggleServermodeAUTOTBS(bool setting, Server *server, String *param);
extern bool toggleServermodeHIDDEN(bool setting, Server *server, String *param);
extern bool toggleServermodeMODELOCK(bool setting, Server *server, String *param);
extern bool toggleServermodeNOOP(bool setting, Server *server, String *param);
extern bool toggleServermodeREMOTEMOTD(bool setting, Server *server, String *param);
extern bool toggleServermodeSHORTMOTD(bool setting, Server *server, String *param);
extern bool toggleServermodeUSERSFULL(bool setting, Server *server, String *param);

#endif
