/* kineircdconf.h
 * $Id$
 * Compile-time configuration settings
 * 
 * This file needs major organisational work :) 
 * 
 * Copyright (c) 2001,2002 KineIRCd Development Team
 * (See DEV-TEAM file for details)
 *
 * This file is a part of KineIRCd.
 * 
 * KineIRCd is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * KineIRCd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with KineIRCd; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _INCLUDE_AUTOCONF_H_
# define _INCLUDE_AUTOCONF_H_


/******************************************************************************
*                                                                             *
*  DO NOT TOUCH ANYTHING BELOW THIS POINT UNLESS YOU KNOW WHAT YOU'RE DOING!  *
*                                                                             *
******************************************************************************/


// THESE SHOULD BE REMOVED ASAP!! :( (see register.cpp)
#define ALLOW_CLIENT_CONNECTIONS
#define ALLOW_SERVER_CONNECTIONS
#define ALLOW_SERVICE_CONNECTIONS
#define MAXLEN_REALNAME 30


/* Defaults for the Config class :) Change at will, but be careful.
 * It is HIGHLY RECOMMENDED you just edit your configuration file.
 */

/* This is the environment variable we may need to check for a config file */
/* Define this if you want the LANGUAGE command to accept languages that are 
 * not defined/loaded by this server. This is wise to cut down abuse..
 * (eg. x-dickheads doesn't exist and probably never will :)
 */
# define ACCEPT_UNKNOWN_LANGS
  
  
# define CONFIG_FILE_ENV_VAR			"KINEIRCD_CONF"

/* Define these with an amount in seconds for each flood-lock timeout,
 * or undefine to disable the flood-lock.
 */
# define FLOODLOCK_ACCEPT_MSG 120
# define FLOODLOCK_AWAY_REPLY 60
# define FLOODLOCK_AWAY_SET 45
# define FLOODLOCK_KNOCK_MSG 300 

/* # define FORCE_MOTD */
  
/* Define this if you want to allow IRC operators to use the WALLOPS command.
 * Usually it's ok to allow this, but some networks do not like this very much
 * as it is often abused. Many clients/scripts turn +w on automatically like
 * BitchX to be 'cool' and 'hip' with the 'oper scene', when in actual fact
 * these days most of the network server notices, global operator messages etc
 * are shuffled via local server notices and global notices.
 */
/* # define ALLOW_OPER_WALLOPS */


/* Define this if you want strict channel names to be allowed. This means
 * channel names are not allowed to have any control characters in them, nor
 * the character 0xA0, which is a blank character in the windows character set.
 * The result of this is nobody can fake 'official' channels without it
 * looking like an obvious fake.
 */
/* # define STRICT_CHANNEL_NAMES */


/* When this is defined, idle information will not be presented unless the
 * idle time is greater than or equal to the given time (in seconds)
 */
# define TIME_TO_BE_IDLE 30
  
 
/* Define this if you want connections to be strict. This means someone who
 * connects as 'SERVER' first, then changes their mind to 'USER' will be
 * booted off with a "cannot re-register" message. This can minimise damage
 * caused by some attacks which exploit the fact that the server is so
 * braindead and flood the connection with registration requests.
 */
# define STRICT_REGISTRATIONS


/* Define this if you want helpers and operators to be able to 'see' user
 * modes. This means a helper/operator can /mode nickname to receive the modes
 * a user has currently set, but the helper/operator CANNOT change the modes
 * for the user.
 */
# define HELPERS_CAN_SEE_USER_MODES

  
/* Define this if your operators are paranoid and cannot live their lives
 * without knowing who is doing a /WHOIS on them. This only works locally,
 * but is enough to satisfy those paranoid +O's by telling them when someone
 * does a /WHOIS that will contain their idle time.
 */
 # define NOTIFY_PARANOID_OPERS_ON_WHOIS


/* Socket listen maximum backlog.. */
# define LISTEN_MAXQUEUE 25


/* Stuff that will be in the Config Class, just not yet. */
# define DEFAULT_CONFIG_SERVER_DESCRIPTION	"Mystery unconfigured server"
# define DEFAULT_CONFIG_SERVER_MAX_TTL		64 /* 64 is the maximum */
# define DEFAULT_CONFIG_WHOWAS_DECAY		3600 /* 10 minutes */
# define DEFAULT_CONFIG_WHOWAS_MAX_ENTRIES	50
# define DEFAULT_CONFIG_MOTD_FILE		"@MOTD_FILE@"

/* Stuff that should be in the Config class. */
# define MAX_WHO_LIST_LINES			20 /* opers are not constricted by this. */
# define ANONYMOUS_IDENT_MASK		"anonymous!anonymous@anonymous" /* for +a channels */

/* Stuff that COULD/SHOULD be in the config file, but can break things.
 * (maybe some of these could be for tuning sub-classes in the config file)
 */
# define MAXLEN_SERVERNAME	63	/* from RFC */
# define MAXLEN_SERVERDESC	80
# define MAXLEN_LANGCODE	10	// Keep small..
# define MIN_INVITE_TIMEOUT	30	/* 30 second min timeout */
# define MAX_GARBO_DELAY_TIME	30	/* max delay per garbo run */
# define MAX_GARBO_RUN_ITEMS	100	/* number of garbo items before breaking garbo run - this is 'low' */
# define MAX_OUTQUEUE_DUMP_SIZE	2048	/* amount of data, in bytes, forced out of an output queue per main loop */
# define MAX_MODES_PER_COMMAND 	6	/* this only applies to irc2 user really */

/* Stuff that needs to be rewritten, or re-worked/re-thought, and put in the 
 * Config class for flexibility reasons. Hrrm.
 */
# define LANG_FILE_PREFIX		"ircdlang."
# define LANG_FILE_PREFIX_LEN		9


/* Define this if you want the server to send a ping with a random value to
 * the client upon connection and wait until a responce is returned before
 * allowing them to continue registration. 
 */
# define USER_CONNECTION_PINGPONG


/***************************************************************************
 ***************************************************************************
 ***************************************************************************/


/* Define this if you want PRIVMSG and NOTICE to work traditionally for IRC
 * operators in the respect that they cannot send simply to $* or #*, but need
 * to include at least one decimal point, eg #*.au for all users from 
 * Australia. With modern networks this is pretty wasteful global broadcasts
 * are often the case sometimes and most networks have their servers on the
 * same domain name. Undefining this will allow $* and #* to work perfectly.
 * 
 * Ok, enough of my rambling, I hope that made sense. :)
 */
#define TRADITIONAL_IRCOP_MESSAGES


/* These are the modes a user is automatically given upon registration.
 * Check user.h for more usermodes you can 'or' together.
 * Undefining this will stop users automatically being set a mode.
 */
#define USER_REGISTRATION_MODES		User::M_VWORLD


/* Maximum protocol line lengths (pre-compressor) */
# define MAXLEN_IRC2USER_OUTPUT		512
# define MAXLEN_P13SERVER_OUTPUT	2048


/* Define this to limit the connection data during a registration to the
 * number of lines specified. Undefining this allows unlimited lines to be
 * sent during the registration phase. Don't change the value of this
 * unless you know the protocol :) This number also includes availability for
 * different nick requests, but not many.
 */
# define MAX_REGISTRATION_LINES		5


/* Time-out value for select(). Everytime select() times out, the daemon does
 * a few checks to make sure there isn't anything dead lurking around and the
 * like, basically cleans itself up. If you have a sort of busy server, such
 * an average of 500 clients, you'd be best leaving this at '1' so that stale
 * connections, memory allocations etc can be cleaned out quickly. If you run
 * a server that doesn't have to handle that many connections, say less than
 * 100, you might want to change this to something a little higher (longer) 
 * if you want to reduce CPU usage. Values over 10 might be a little crazy,
 * since the garbage collection would most likely run slower.
 * 
 * If any of that went right over your head, leave this at 1 :)
 */
# define SELECT_TIMEOUT			1


/* These define what 'extra' modes local and global operators get. Undefining
 * these do not give the operators any extra modes. Check the user.h include
 * file for what modes are available. These modes are given additional to the
 * +O/+o which is given to the user. Modes should be or'd together with the
 * '|' character.
 */
# define GIVE_MODES_GLOBALOP		User::M_WALLOPS | \
					User::M_ROUTINGSTAFF
/* # define GIVE_MODES_LOCALOP */


/* These are just for over-the-top confirmation, probably best to leave this
 * as is, but they might need to be 'tweaked' later
 */
# define REGISTRATION_EOL_CHARS		"\r\n"
/* IRC2USER_EOL_CHARS is determined automatically (below) */
# define P13SERVER_EOL_CHARS		"\n"
# define P14SERVER_EOL_CHARS		"\n"


/******************************************************************************
*                                                                             *
* Hey wait!  Stuff below here is determined by the auto-configuration script! *
*                                                                             *
*  If you want to edit anything down here, use the configure script to do so  *
*                                                                             *
******************************************************************************/

@TOP@


/* Define if we are going to do debugging. 
 * Note: DEBUG must be define if the others are also wanted 
 */
# undef DEBUG
# ifdef DEBUG
#  define DEBUG_ASSERT /* we force this on. bad move? oh well. */
#  undef DEBUG_EXTENDED
#  ifdef DEBUG_EXTENDED
#   undef DEBUG_PSYCHO
#  endif
#endif
  
@BOTTOM@

/******************************************************************************
*                                                                             *
*       SERIOUSLY DO NOT TOUCH ANYTHING BELOW THIS UGLY BLOCK OF TEXT!!!      *
*                                                                             *
*      Stuff down here is determined by stuff above this, so don't touch!     *
*                                                                             *
******************************************************************************/

  
/* Work out the client end of line characters and the max ircII line length;
 * change the BLOODY_IRCII_KLUGE define rather than edit these.. The name
 * irc2user refers to the protocol name, not the ircII client, btw.
 */
# ifdef HAVE_IRC2USER_PROTOCOL
/* #  ifdef BLOODY_IRCII_KLUGE */
#   define MAXLEN_IRC2USER_OUTPUT_CALC	(MAXLEN_IRC2USER_OUTPUT - 2)
#   define IRC2USER_EOL_CHARS		"\r\n"
/* #  else
 * #   define MAXLEN_IRC2USER_OUTPUT_CALC	(MAXLEN_IRC2USER_OUTPUT - 1)
 * #   define IRC2USER_EOL_CHARS		"\n"
 * #  endif
 */
# endif

  
/* Fix the server name mask if it is not here */
# ifndef SERVER_NAME_MASK
#  define SERVER_NAME_MASK		"*"
# endif
  
#endif
