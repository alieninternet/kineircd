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
# define CONFIG_FILE_ENV_VAR			"KINEIRCD_CONF"

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


/* Number of descriptors off the max to stop incoming user connections */
# define MAX_FD_SAFETY_NUM		32


/* Number of descriptors off the max to stop all connections in/out. Why
 * this many below? Well linux and other unicies I've noticed CANNOT close()
 * after the maximum has been reached. I cannot explain it, but it happens,
 * so it appears that these descriptors must always be wasted
 */
# define MAX_FD_PEAK_CUTOFF		4 


/* Number of descriptors off the max where we should start to get worried.
 * Usually 512 off the max is enough, but if you have a kernel that supports
 * something like 4096 you may want to increase this to say warn around 3072
 * descriptors etc
 */
# define MAX_FD_WARN_LEFT		512


/* Work out our levels for FD sizes, best to leave these alone.. */
# define MAX_FD_NO_MORE_USERS		MAX_FD_PER_PROCESS - MAX_FD_SAFETY_NUM
# define MAX_FD_NO_MORE_CONNS		MAX_FD_PER_PROCESS - MAX_FD_PEAK_CUTOFF
# define MAX_FD_HIGH_WATERMARK		MAX_FD_PER_PROCESS - MAX_FD_WARN_LEFT


/* Fix up select()'s fd_set size. Surprisingly some unicies set this to a
 * level that's quite low. In many cases the default standard of 1024 will not
 * match the file descriptor limit set by the system (usually)... 64-bit
 * applications usually have FD_SETSIZE set to 65536, which is excessive
 * compared to our use. This should fix everything to a reasonable level.
 */
/* # define FD_SETSIZE			MAX_FD_PER_PROCESS + 1 */


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

/* This is the location of the configuration file. It can be overwritten
 * with the '-f' command line option, so don't stress too much about this.
 * Here, we set it to whatever is in the local directory for this test version
 */
# undef DEFAULT_CONFIG_FILE


/* This is the location to the PID file, generated upon startup. Edit to fit
 * your operating system, or undefine this if you do not want a PID file to 
 * be created at all - it's handy to have, though, if you want to kill the 
 * server from a script or something.
 */
# undef PID_FILE


/* Define this if you want the LANGUAGE command to accept languages that are 
 * not defined/loaded by this server. This is wise to cut down abuse..
 * (eg. x-dickheads doesn't exist and probably never will :)
 */
# undef ACCEPT_UNKNOWN_LANGS
  
  
/* When enabled, this turns on logging to the syslog interface. When
 * disabled, logging will not occur. A string must be specified with this
 * define - it will be the identity of the process as far as syslog is 
 * concerned.
 */
# undef SYSLOG_IDENT


  /* Socket listen maximum backlog.. */
# undef LISTEN_MAXQUEUE


/* Define these depending on what commands you want available */
# undef ALLOW_COMMAND_DIE

  
/* Define these with an amount in seconds for each flood-lock timeout,
 * or undefine to disable the flood-lock.
 */
# undef FLOODLOCK_ACCEPT_MSG
# undef FLOODLOCK_AWAY_REPLY
# undef FLOODLOCK_AWAY_SET
# undef FLOODLOCK_KNOCK_MSG  
  
  
/* Define this if you want the MOTD to be 'forced', eg. using a numeric
 * other than RPL_MOTD. This stops some irc clients from hiding the MOTD
 * at start-up which may result in people ignoring policies they should be
 * aware of, etc.
 * 
 * Remember: You can lead a horse to water, but you can't make it drink
 */
# undef FORCE_MOTD

  
/* When this is defined, idle information will not be presented unless the
 * idle time is greater than or equal to the given time (in seconds)
 */
# undef TIME_TO_BE_IDLE			
  
 
/* Define this if you want the server to send a ping with a random value to
 * the client upon connection and wait until a responce is returned before
 * allowing them to continue registration. 
 */
# undef USER_CONNECTION_PINGPONG


/* Define this if your operators are paranoid and cannot live their lives
 * without knowing who is doing a /WHOIS on them. This only works locally,
 * but is enough to satisfy those paranoid +O's by telling them when someone
 * does a /WHOIS that will contain their idle time.
 */
# undef NOTIFY_PARANOID_OPERS_ON_WHOIS


/* Define this if you want strict channel names to be allowed. This means
 * channel names are not allowed to have any control characters in them, nor
 * the character 0xA0, which is a blank character in the windows character set.
 * The result of this is nobody can fake 'official' channels without it
 * looking like an obvious fake.
 */
# undef STRICT_CHANNEL_NAMES


/* Define this if you want to drop servers which are out of sync, or appear
 * out of sync by sending 'spoofed' messages that come from users, servers,
 * or channels that are not in the local server's lists.
 * 
 * This could be bad on large networks were vague discrepancies just happen
 */
# undef DROP_BRAINDEAD_SERVERS


/* Define this if you want connections to be strict. This means someone who
 * connects as 'SERVER' first, then changes their mind to 'USER' will be
 * booted off with a "cannot re-register" message. This can minimise damage
 * caused by some attacks which exploit the fact that the server is so
 * braindead and flood the connection with registration requests.
 */
# undef STRICT_REGISTRATIONS


/* Define this if you want to allow IRC operators to use the WALLOPS command.
 * Usually it's ok to allow this, but some networks do not like this very much
 * as it is often abused. Many clients/scripts turn +w on automatically like
 * BitchX to be 'cool' and 'hip' with the 'oper scene', when in actual fact
 * these days most of the network server notices, global operator messages etc
 * are shuffled via local server notices and global notices.
 */
# undef ALLOW_OPER_WALLOPS


/* Define this if you want operators to be able to use the TRACEROUTE command
 * on this server. Sometimes this may 'clash' with network policies or it might
 * be abused; Whatever the reason, here is your chance to turn it off.
 */
# undef HAVE_CMD_TRACEROUTE


/* Define this if you want helpers and operators to be able to 'see' user
 * modes. This means a helper/operator can /mode nickname to receive the modes
 * a user has currently set, but the helper/operator CANNOT change the modes
 * for the user.
 */
# undef HELPERS_CAN_SEE_USER_MODES

  
/* Define these depending on which transport types you wish to support */
# undef HAVE_TRANSPORT_TCP_IPV4
# undef HAVE_TRANSPORT_TCP_IPV6
# undef HAVE_TRANSPORT_SPX_IPX
  
  
/* Define these depending on what protocols you want compiled in */
# undef HAVE_IRC2USER_PROTOCOL
# undef HAVE_IRC3USER_PROTOCOL
# undef HAVE_P13SERVER_PROTOCOL
# undef HAVE_P14SERVER_PROTOCOL

  
/* Define these depending on what compression you want added to the above */
# ifdef HAVE_IRC2USER_PROTOCOL
#  undef HAVE_IRC2USER_IRCII_KLUGE
#  undef HAVE_IRC2USER_MATCH_COUNTING
# endif
# ifdef HAVE_P13SERVER_PROTOCOL
#  undef HAVE_P13SERVER_COMPRESS
#  undef HAVE_P13SERVER_EXTENDED
# endif
# ifdef HAVE_P14SERVER_PROTOCOL
#  undef HAVE_P14SERVER_COMPRESS
# endif

  
/* Define if we want to compile the old-style configuration subparser. When
 * compiled in, users will be able to add a line like this to their config
 * file to include an old-style configuration file of the same format used by
 * other/older IRC daemons:
 *
 * includeOldConfig "oldircd.conf";
 * 
 * It's the same format used by traditional irc servers and seems to have 
 * quite an amazing lifespan considering it is often considered quite a 
 * confusing configuration file format. Hey, people still like it for some 
 * reason! It's considered obsolete, but for the sake of completeness support 
 * is available.
 */
# undef OLD_CONFIG_SUBPARSER
  
  
/* Define if the function tzset() and variables timezone and daylight exist */
# undef HAVE_TZSET_AND_FRIENDS
  

/* Define if the C++ compiler has STL (Standard Template Library) support */
# undef HAVE_CXX_STL

  
/* Define if you have the variant of the STL which includes hash/hash_map */
# undef STL_HAS_HASH

  
/* Define if you have the variant of the STL which includes slist */
# undef STL_HAS_SLIST

  
/* Define if signal() has a third variable (struct sigcontext *) */
# undef HAVE_STRUCT_SIGCONTEXT

  
/* Define if the signal SIGCHLD is dodgey */
# undef SIGCHLD_BRAIN_DAMAGE

  
/* The total number of file descriptors allowed per process on this machine */
# undef MAX_FD_PER_PROCESS

  
/* The total number of bits in the fd_set bitset for select() */
# undef SELECT_FDSET_NUMFDS

  
/* Define if OpenSSL is installed */
# undef HAVE_OPENSSL
  
  
/* Define if SSL support is allowed */
# undef WITH_SSL

  
/* Define if we are going to incorporate SNMP stuff */
# undef HAVE_SNMP_AGENT
  
  
/* Define if the SNMP agent is to be a master and not a subagent? */
# undef HAVE_SNMP_AGENT_MASTER
# ifdef HAVE_SNMP_AGENT_MASTER
#  undef HAVE_SNMP_AGENT_MASTER_PORT
# endif
  
  
/* Define if /dev/urandom does not exist */
# undef MUST_INIT_PRNG

  
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

  
/* Work out some true level values for our maximum file descriptor checks */
# ifndef MAX_FD_PER_PROCESS
#  warning "No MAX_FD_PER_PROCESS defined: Presuming 256 File Descriptors!!"
#  define MAX_FD_PER_PROCESS		256
# endif

  
/* Double check the default TTL is appropriate */
# if DEFAULT_CONFIG_SERVER_MAX_TTL > 64
#  error "DEFAULT_CONFIG_SERVER_MAX_TTL is greater than 64! 64 is the maximum."
# endif

  
/* Fix the server name mask if it is not here */
# ifndef SERVER_NAME_MASK
#  define SERVER_NAME_MASK		"*"
# endif

  
/* Work out the highest protocol major number we support */
# ifdef HAVE_P14SERVER_PROTOCOL
#  define HIGHEST_SUPPORTED_SERVER_PROTOCOL 14
# else
#  if HAVE_P13SERVER_PROTOCOL
#   define HIGHEST_SUPPORTED_SERVER_PROTOCOL 13
#  else
#   define HIGHEST_SUPPORTED_SERVER_PROTOCOL 0 /* blind */
#  endif
# endif

#endif
