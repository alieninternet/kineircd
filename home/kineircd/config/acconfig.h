/* config.h
 * Compile-time configuration settings
 * 
 * This file needs major organisational work :) 
 * 
 */

#ifndef __CONFIG_H_
# define __CONFIG_H_

/* Undefine this if you don't want us to log to syslog */
# define LOG_TO_SYSLOG


/* Change this if you feel like it.. */
# define SYSLOG_IDENT			"ircd"


/* This is the location to the PID file, generated upon startup. Edit to fit
 * your operating system, or undefine this if you do not want a PID file to 
 * be created at all - it's handy to have, though, if you want to kill the 
 * server from a script or something.
 */
# define PID_FILE			"/var/run/ircd.pid"


/* Socket listen maximum backlog.. */
# define LISTEN_MAXQUEUE		50


/* This is the location of the configuration file. It can be overwritten
 * with the '-f' command line option, so don't stress too much about this.
 * Here, we set it to whatever is in the local directory for this test version
 */
# define DEFAULT_CONFIG_FILE		"ircd.conf"


/* Define this if you want the MOTD to be 'forced', eg. using a numeric
 * other than RPL_MOTD. This stops some irc clients from hiding the MOTD
 * at start-up which may result in people ignoring policies they should be
 * aware of, etc.
 * 
 * Remember: You can lead a horse to water, but you can't make it drink
 */
# define FORCE_MOTD

  
/* Define this if you want PRIVMSG and NOTICE to work traditionally for IRC
 * operators in the respect that they cannot send simply to $* or #*, but need
 * to include at least one decimal point, eg #*.au for all users from 
 * Australia. With modern networks this is pretty wasteful global broadcasts
 * are often the case sometimes and most networks have their servers on the
 * same domain name (*.austnet.org, *.dal.net etc). In the case of austnet,
 * for example, to send to everyone the minimum you would have to send to
 * would be $*.org -- pretty wasteful since that's everyone anyway!
 * Undefining this will allow $* and #* to work perfectly.
 * 
 * Ok, enough of my rambling, I hope that made sense. :)
 */
#define TRADITIONAL_IRCOP_MESSAGES


/* Define this to limit the number of lines allowed during a /WHO output.
 * This stops unfriendly people doing /who * and listing everyone possible.
 * This limit does not effect IRC Operators since they are supposed to know
 * what they are doing :)
 * 
 * Keep in mind, when changing this, some people like to use /who to find out
 * who they can turn to for help. "/who 0 O" (or "/who * O") will tell the
 * user any visible IRC Operators that are currently online, for example.
 * 
 * This setting does not truncate channel listings ("/who #channel"), which
 * cannot be of a mask (eg "/who #chan*" doesn't work). This is so that a
 * client is able to get information on the people inside a particular channel.
 */
#define MAX_WHO_LIST_LINES		20


/* These are the modes a user is automatically given upon registration.
 * Check user.h for more usermodes you can 'or' together.
 * Undefining this will stop users automatically being set a mode.
 */
#define USER_REGISTRATION_MODES		User::M_VWORLD


/******************************************************************************
*                                                                             *
*  DO NOT TOUCH ANYTHING BELOW THIS POINT UNLESS YOU KNOW WHAT YOU'RE DOING!  *
*                                                                             *
******************************************************************************/


/* Maximums for various things, probably best not to touch these.
 * The lengths **MUST** be the same as other servers on the network or things
 * could break.
 */
# define MAXLEN_NICKNAME		15
# define MAXLEN_CHANNELNAME		40
# define MAXLEN_REALNAME		50
# define MAXLEN_SERVERNAME		63	/* from RFC */
# define MAXLEN_SERVERDESC		80
# define MAXLEN_TOPIC			300	/* Keep these under 350 */
# define MAXLEN_KICK_REASON		300
# define MAXLEN_LANGCODE		10	// Keep small..
# define MINLEN_OP_BROADCAST		20
# define MAX_CHANNELS_PER_USER		10
# define MIN_INVITE_TIMEOUT		30	/* 30 second min timeout */


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


/* This is the prefix for the language files. Best not to change this unless
 * you really need to for some odd reason. If you do change it, make sure
 * the LANG_FILE_PREFIX_LEN size matches the string.
 */
# define LANG_FILE_PREFIX		"ircdlang."
# define LANG_FILE_PREFIX_LEN		9


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


/* This is the maximum amount of time (in seconds) allowed before a garbage
 * collection run (which cleans up sockets, memory etc) is called. Do not
 * keep this up too high, else the server could run out of resources faster
 * than it can clear them. At the same time, do not keep this too low, else
 * your server could be wasting time by looking for things to clean up when
 * there is nothing to really clean up.
 * 
 * Keep in mind that the garbo is a nice bloke who also sends out pings if
 * they are needed, and deals with timeouts etc. The garbo's called in every
 * time select times out, in other words whenever the server isn't busy, so
 * this should be worked out with SELECT_TIMEOUT (above).
 * 
 * Perhaps a dynamic time system would be more appropriate, but at this stage
 * I'm not sure if it matters that much
 */
# define MAX_GARBO_DELAY_TIME		30


/* This controls how much work our garbage collector is allowed to do per
 * run. This is so that the garbo routine doesn't go nuts if heaps of things
 * need to be cleaned up at once, and causes lag spikes. The garbo doesn't
 * really take that long to do what it has to do, but this should be kept to
 * a realistic level as to avoid lag spikes... or visible ones at least..
 * 
 * Note that an 'item' isn't just one thing being cleaned, it is also a count
 * of how many 'expensive' operations have been called (eg. loops etc).
 * 
 * If this is not defined, the garbage collector routine will just run until
 * it is done (I don't recommend that, but you might want need it if your
 * server is extraordinarily busy and you don't care about speed)
 */
# define MAX_GARBO_RUN_ITEMS		100 /* this is quite low */


/* This controls how much data is forced out of the output queue per
 * main loop. The way this works is the output queue will try to send as much
 * as it can up until the output queue is empty, or the size is about to go
 * over this limit. Be reasonable here :) Keep in mind how tcp/ip works..
 * This value seems ok to me..
 */
# define MAX_OUTQUEUE_DUMP_SIZE		2048


/* This is the mask used when a channel is set +a (anonymous). Change this
 * only if you really need to, since the RFC says it should be set to
 * "anonymous!anonymous@anonymous" but this is meaningless in some languages,
 * or might conflict with a network policy or something *shrugs*
 */
# define ANONYMOUS_IDENT_MASK		"anonymous!anonymous@anonymous"


/* Defaults :) Change at will, but be careful. */
# define DEFAULT_CONFIG_SERVER_DESCRIPTION	"Mystery unconfigured server"
# define DEFAULT_CONFIG_SERVER_MAX_TTL		64 /* 64 is the maximum */
# define DEFAULT_CONFIG_WHOWAS_DECAY		3600 /* 10 minutes */
# define DEFAULT_CONFIG_WHOWAS_MAX_ENTRIES	50
# define DEFAULT_CONFIG_ADMIN_NAME		""
# define DEFAULT_CONFIG_ADMIN_EMAIL		""
# define DEFAULT_CONFIG_ADMIN_LOCATION		""
# define DEFAULT_CONFIG_MOTD_FILE		"@MOTD_FILE@"


/* This define changes how operator password SHA1 hashes are generated.
 * Change if you like, but remember if you modify it your existing passwords
 * WILL NOT WORK! Likewise, they won't work with other servers unless they
 * have the same modification. The main idea of this is so that another oper
 * from another server can't look at your ircd.conf and nick passwords since
 * they are generated differently. The disadvantage is you will have to modify
 * this every time a new version comes out if you want to remain different.
 * Just remember: If you have already configured your server and you don't
 * want to re-do all your passwords, DON'T change this!
 */
# define MAGIC_OPER_PASSWD_STRING	"!@#:)#@!"
							       

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
******************************************************************************/

@TOP@

/* Default maximums for various things - see the INSTALL file for more info */
# undef DEFAULT_MAX_ACCEPTS_PER_USER
# undef DEFAULT_MAX_BANS_PER_CHANNEL
# undef DEFAULT_MAX_LANGS_PER_USER
# undef DEFAULT_MAX_SILENCES_PER_USER
# undef DEFAULT_MAX_WATCHES_PER_USER


/* Constant maximums for various things - see the INSTALL file for more info */
# undef MAX_MODES_PER_COMMAND

 
/* Define this if you want the LANGUAGE command to accept languages that are 
 * not defined/loaded by this server. This is wise to cut down abuse..
 * (eg. x-dickheads doesn't exist and probably never will :)
 */
# undef ACCEPT_UNKNOWN_LANGS
  
  
/* Define these depending on what connections you want to allow/disallow */
# undef ALLOW_CLIENT_CONNECTIONS
# undef ALLOW_SERVER_CONNECTIONS
# undef ALLOW_SERVICE_CONNECTIONS
  
  
/* Define these with an amount in seconds for each flood-lock timeout,
 * or undefine to disable the flood-lock.
 */
# undef FLOODLOCK_ACCEPT_MSG
# undef FLOODLOCK_AWAY_REPLY
# undef FLOODLOCK_AWAY_SET
# undef FLOODLOCK_KNOCK_MSG  
  
  
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


/* *SIGH* Nobody seems to know what a standard means, especially in the IRC
 * world. In the old days, the ircII client used to have a bug where it needed
 * a \r\n at the end of a line rather than a \n. Defining this will send a
 * full \r\n for all clients to work around this. Not only this, but these
 * thick clients do not understand some simple numerics and break if they
 * do not get the same numerics but in a NOTICE form from the server! 
 * Probably not needed these days, but you know the laws of the universe: if 
 * you disable it, someone will need it, but if you enable it, nobody will
 * need it! :) You can't win, safe to leave it on, even though it generates a
 * little more traffic than is necessary, and makes the neat presentation of
 * the server a little less-neat.
 */
# undef BLOODY_IRCII_KLUGE


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


/* Define this if you want to produce a server that does not say anything
 * during the connection registration phase. While this is technically
 * protocol incorrect, it can save a server from clone attacks where the
 * clones are able to connect faster than the server can deny their
 * connection.
 * 
 * I do not like this, but at this stage if the server is in passive
 * registration mode and someone enters in a bad nickname such as one that
 * exists on the network or a ChanOP etc style nickname, this results in
 * them being disconnected rather than an error message being sent and the
 * client fixing the problem. This is one of the problems with passive
 * registration, unfortunately, as leaving that open for attack voids the
 * whole point of having this define's ability.
 * 
 * Remember: Defining this makes your server anal, and can make it a prick
 * to connect to, but it may also protect your server from any unwanted
 * exploiting bots.
 * 
 * In in doubt, leave this undefined and firewall trouble addresses :)
 */
#undef PASSIVE_REGISTRATION


/* Define this if you want registrations to follow protocol, that is
 * unknown command messages being sent to the client during the registration
 * handler phase if they send a command that is unknown. While this follows
 * protocol, this also opens your server up to all sorts of fun - it doesn't
 * take a genius to realise that someone can send one character and a new
 * line (two bytes) faster than your server can return a string telling the
 * person at the other end that their character was an unknown command.
 * 
 * If you have set PASSIVE_REGISTRATION on, this will have the effect of
 * bumping clients off if they send a bad command. This does not allow
 * flexibility in the registration protocol, but the two combined could stop
 * your server being the target of complete idiots.
 * 
 * Weigh it up, your choice. Probably best to just leave it as is.
 */
# undef WARN_UNKNOWN_COMMANDS_IN_REGISTRATION

  
/* Define this if you want our configuration parser to convert TAB characters
 * in your MOTD into several spaces to simulate a TAB. This is a kluge, really,
 * to work around clients that for some rediculous reason do not understand
 * what the TAB character is supposed to do. Hopefully one day this will not
 * be necessary!
 */
# undef CONVERT_MOTD_TABS


/* Define this is you want a handy little feature - 'match counting' - counts
 * the number of matches on certain commands and adds a talley on the
 * 'End of X' reply. This is very handy for operators, but in any case if you
 * want to turn it off then here's your chance!
 */
# undef DO_MATCH_COUNTING


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

  
/* Define these depending on what protocols you want compiled in */
# undef HAVE_PROTOCOL_IRC2USER
# undef HAVE_PROTOCOL_P13SERVER
# undef HAVE_PROTOCOL_P14SERVER

  
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

  
/* Define if /dev/urandom does not exist */
# undef MUST_INIT_PRNG

  
@BOTTOM@

/******************************************************************************
*                                                                             *
*       SERIOUSLY DO NOT TOUCH ANYTHING BELOW THIS UGLY BLOCK OF TEXT!!!      *
*                                                                             *
*      Stuff down here is determined by stuff above this, so don't touch!     *
*                                                                             *
******************************************************************************/

/* Force autoconf before compilation (else stuff might be left out) */
# ifndef HAVE_CONFIG_H
#  error "You must run configure before compiling this software"  
# endif

  
/* Work out the client end of line characters and the max ircII line length;
 * change the BLOODY_IRCII_KLUGE define rather than edit these.. Th name
 * irc2user refers to the protocol name, not the ircII client, btw.
 */
# ifdef BLOODY_IRCII_KLUGE
#  define MAXLEN_IRC2USEROUTPUT		510
#  define IRC2USER_EOL_CHARS		"\r\n"
# else
#  define MAXLEN_IRC2USEROUTPUT		511
#  define IRC2USER_EOL_CHARS		"\n"
# endif

  
/* Work out what to do about the logging priorities */
# ifdef LOG_TO_SYSLOG
#  include <syslog.h>
#  define LOGPRI_INFO		LOG_INFO
#  define LOGPRI_NOTICE		LOG_NOTICE
#  define LOGPRI_WARNING	LOG_WARNING
#  define LOGPRI_ERROR		LOG_ERR
#  define LOGPRI_CRITICAL	LOG_CRIT
# else
#  error "No other logging facility :("
# endif

  
/* Work out some true level values for our maximum file descriptor checks */
# ifndef MAX_FD_PER_PROCESS
#  define MAX_FD_PER_PROCESS		256 /* assume 8-bit system, eek! */
# endif

  
/* Double check the default TTL is appropriate */
# if DEFAULT_CONFIG_SERVER_MAX_TTL > 64
#  error "DEFAULT_CONFIG_SERVER_MAX_TTL is greater than 64! 64 is the maximum."
# endif

  
/* Fix the server name mask if it is not here */
# ifndef SERVER_NAME_MASK
#  define SERVER_NAME_MASK		"*"
# endif
  
#endif
