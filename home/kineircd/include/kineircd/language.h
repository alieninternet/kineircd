/* language.h
 * Text in a centralised location so it can be easily changed/fixed etc
 */

#ifndef __LANGUAGE_H_
# define __LANGUAGE_H_

/* Do not change these lines unless you know how they are used/referenced or
 * integrate with the IRC protocol etc. Often white-space before/after a 
 * string of text is there for a very specific reason.
 */

// Greeting lines sent when a user connects
# define LNG_PINGPONG_NOTICE \
	":*** If you are having problems connecting due to ping timeouts, type /raw PONG %s or /quote PONG %s now. If you are still having trouble connecting, please e-mail help@austnet.org with the name and version of the client you are using, and the server you tried to connect to (%s)."
# define LNG_RPL_WELCOME \
	":Welcome to the AustNet IRC Network %s -- For more information visit http://www.austnet.org"
# define LNG_RPL_YOURHOST \
	":Your server is %s, running version %s"
# define LNG_RPL_YOURHOST_IRCII_KLUGE_NOTICE \
	"NOTICE %s :*** Your host is %s, running version %s"
# define LNG_RPL_CREATED \
	":This server was created %s"
# define LNG_RPL_ISUPPORT_TAG \
	":are supported by this server"
# define LNG_RPL_TIMEONSERVERIS \
	"%lu %ld %c%02u%02u %u :time according to server"
# define LNG_RPL_MOTDSTART \
	":- %s Message of the day -"
# define LNG_RPL_MOTD_SHORT1 \
	":- Please read the MOTD by typing /motd"
# define LNG_RPL_MOTD_SHORT2 \
	":- Connection to this server from this point onwards assumes that"
# define LNG_RPL_MOTD_SHORT3 \
	":- you have read and agree to all terms and conditions in the MOTD"
# define LNG_RPL_MOTD_PREFIX \
	":- "
# define LNG_RPL_ENDOFMOTD \
	":End of MOTD"
# define LNG_ERR_NOMOTD \
	":MOTD File is missing"
# define LNG_ERR_NOMOTD_NOREMOTE \
	":MOTD on %s cannot be viewed remotely"
# define LNG_RPL_SPAM_LINE1 \
	": "
# define LNG_RPL_SPAM_LINE2 \
	": Please be advised that use of this service constitutes consent to"
# define LNG_RPL_SPAM_LINE3 \
	":  all network policies and server conditions of use, which can be"
# define LNG_RPL_SPAM_LINE4 \
	": sighted at http://www.austnet.org/agreement/ and is in accordance"
# define LNG_RPL_SPAM_LINE5 \
	": with the Electronic Communications Privacy Act,  18 USC 2701-2711"
# define LNG_RPL_SPAM_LINE6 \
	": "

// Generic responces
# define LNG_ERR_UNKNOWNCOMMAND \
	" :Unknown command"
# define LNG_ERR_PASSWDMISMATCH \
	":Password incorrect"
# define LNG_ERR_SERVERTOOFULL \
	":This server cannot accept any more user connections"
# define LNG_ERR_NEEDMOREPARAMS \
	" :Not enough parameters"
# define LNG_ERR_ALREADYREGISTERED \
	":You may only claim registration type once"
# define LNG_ERR_SUMMONDISABLED \
	":SUMMON has been disabled"
# define LNG_ERR_USERSDISABLED \
	":USERS has been disabled"
# define LNG_ERR_NOPRIVILEGES \
	":Permission Denied - You're not an AustNet IRC operator"


// Nickname related errors
# define LNG_ERR_NONICKNAMEGIVEN \
	":No nickname given"
# define LNG_ERR_ERRONEUSNICKNAME \
	" :Erroneous nickname"
# define LNG_ERR_ERRONEUSNICKNAME_W_REASON \
	"%s :Bad nickname specified: %s"
# define LNG_ERR_NICKNAMEINUSE \
	" :Nickname is already in use"
# define LNG_ERR_EVENTNICKCHANGE \
	" :Sorry, cannot change nickname during a channel event (+E)"


// Channel related messages
# define LNG_ERR_CHANOPRIVSNEEDED \
	" :You're not a channel operator"
# define LNG_ERR_CHANOPRIVSNEEDED_HALFOPERVSOPER \
	" :You're not a full channel operator"
# define LNG_ERR_UNIQOPRIVSNEEDED \
	":You're not the original channel operator"
# define LNG_ERR_NOSUCHCHANNEL \
	" :No such channel"
# define LNG_ERR_BADCHANNAME \
	" :Bad channel name"
# define LNG_ERR_BADCHANNAME_SAFECHAN \
	" :Safe channels must be created first"
# define LNG_ERR_BADCHANNAME_FAILMASK \
	"%s :Channel name is unavailable: %s"
# define LNG_ERR_NOTONCHANNEL \
	" :You're not on that channel"
# define LNG_ERR_TOOMANYCHANNELS \
	" :You have joined too many channels"
# define LNG_ERR_BANNEDFROMCHAN \
	" :Cannot join channel (+b)"
# define LNG_ERR_INVITEONLYCHAN \
	" :Cannot join channel (+i)"
# define LNG_ERR_CHANNELISFULL \
	" :Cannot join channel (+l)"
# define LNG_ERR_BADCHANNELKEY \
	" :Cannot join channel (+k)"
# define LNG_RPL_CHANREDIR \
	"%s %s :You are being redirected"
# define LNG_RPL_NOTOPIC \
	" :No topic is set"


// Mode related messages
# define LNG_RPL_ENDOFBANLIST \
	" :End of channel ban list"
# define LNG_RPL_ENDOFEXCEPTLIST \
	" :End of channel ban exception list"
# define LNG_RPL_ENDOFINVITELIST \
	" :End of channel invite list"
# define LNG_ERR_USERSDONTMATCH_MODE \
	":Cannot change mode for other users"
# define LNG_ERR_UMODEUNKNOWNFLAG \
	" :Unknown user MODE flag"
# define LNG_ERR_UNKNOWNMODE \
	"%c :is an unknown mode char to me for %s"
# define LNG_ERR_UNKNOWNSERVERMODE \
	"%c :is an unknown server mode"
# define LNG_ERR_NOPRIVILEGES_PLUS_IRCOPER \
	":Permission Denied - Use the OPER command instead"
# define LNG_ERR_NOPRIVILEGES_PLUS_HELPER \
	":Permission Denied - Identify yourself to services"
# define LNG_ERR_NOPRIVILEGES_PLUS_SERVNOTICES \
	":Permission Denied - Only network operators are allowed to see server notices"
# define LNG_ERR_SERVERMODELOCK \
	" :Cannot change server modes (+L)"
# define LNG_RPL_MODECHANGEWARN_MINUS_VWORLD \
	"-v :Warning - You have changed your mode to -v (Virtual World). You are now vulnerable to Denial of Service attacks."
# define LNG_RPL_MODECHANGEWARN_IRCOPER_MINUS_WALLOPS \
	"-w :You may miss important messages by disabling the receive wallops mode"
# define LNG_RPL_MODECHANGEWARN_HELPER_PLUS_REGNICKSMSG \
	"+R :Setting +R (Only registered nicks can send to you) is not recommended for helpers"
# define LNG_ERR_CANNOTCHANGEUMODE \
	"%c :Cannot toggle this user mode"
# define LNG_ERR_CANNOTCHANGECHANMODE \
	"%c :Cannot modify this channel mode"
# define LNG_ERR_CANNOTCHANGESERVERMODE \
	"%c :Cannot modify this server mode"
# define LNG_ERR_NOCHANMODES \
	" :Channel doesn't support modes"
# define LNG_ERR_KEYSET \
	" :Channel key already set"



// Who/Whois/Whowas messages
# define LNG_ERR_WHOTRUNC \
	":Who list too long - Truncated."
# define LNG_RPL_ENDOFWHO \
	"%s :End of WHO list"
# define LNG_RPL_ENDOFWHO_NOMATCH \
	"%s :End of WHO list (no match)"
# define LNG_RPL_ENDOFWHO_MATCHES \
	"%s :End of WHO list (%d matches)"
# define LNG_RPL_WHOISVIRT \
	" :on Virtual World as "
# define LNG_RPL_WHOISIDLE \
	" %lu %lu :seconds idle, signon time"
# define LNG_RPL_WHOISOPERATOR \
	" :is an AustNet IRC Operator"
# define LNG_RPL_WHOISSECURE \
	":is connected via a secure connection"
# define LNG_RPL_ENDOFWHOIS \
	" :End of WHOIS list"


// Misc
# define LNG_RPL_ENDOFHELP \
	"%s :End of HELP"
# define LNG_RPL_ENDOFHELP_NOMATCH \
	"%s :End of HELP (no match)"
# define LNG_RPL_ENDOFHELP_MATCHES \
	"%s :End of HELP (%d matches)"
# define LNG_RPL_NOWAWAY \
	":You have been marked as being away"
# define LNG_RPL_UNAWAY \
	":You are no longer marked as being away"


# define LNG_REQUESTED_SHUTDOWN \
	" requested shutdown"
# define LNG_BYE_BYE_USER \
	"Exit: %s"
# define LNG_ERROR_CLOSING_LINK \
	"ERROR :Closing Link: %s"
# define LNG_ERROR_CLOSING_LINK_DEFAULT_REASON \
	"Unknown error"


# define LNG_RPL_ENDOFINFO \
	":End of INFO"
# define LNG_RPL_LISTEND \
	":End of LIST"
# define LNG_RPL_ENDOFNAMES \
	"%s :End of NAMES list"
# define LNG_RPL_ENDOFNAMES_NOMATCH \
	"%s :End of NAMES list (no match)"
# define LNG_RPL_ENDOFNAMES_MATCHES \
	"%s :End of NAMES list (%d matches)"
# define LNG_RPL_ENDOFSTATS \
	" :End of STATS report"

# define LNG_RPL_VERSION \
	"%s %s :%s (Type /INFO for more details)"

# define LNG_RPL_ADMINME \
	" :Administration information"

# define LNG_RPL_STATSUPTIME \
	":Server Up %d days %d:%02d:%02d"
# define LNG_RPL_STATSCONN \
	":Highest connection count: %u (%u clients)"

# define LNG_RPL_LUSERCLIENT \
	":There are %d users and %d services on %d servers"
# define LNG_RPL_LUSEROP \
	"%u :operator(s) online"
# define LNG_RPL_LUSERHELPERS \
	"%u :helper(s) online"
# define LNG_RPL_LUSERUNKNOWN \
	"%u :unknown connection(s)"
# define LNG_RPL_LUSERCHANNELS \
	"%u :channels formed"
# define LNG_RPL_LUSERME \
	":I have %u clients and %u servers"
# define LNG_RPL_LOCALUSERS \
	":Current local users: %u Peak: %u"
# define LNG_RPL_GLOBALUSERS \
	":Current global users: %u Peak: %u"

# define LNG_ERR_NONONREG_CHANNEL \
	" :You must be identified with NickOP to join this channel"
# define LNG_ERR_NONONREG_USER \
	" :You must be identified with NickOP to talk privately with this user"

# define LNG_ERR_NOTONCHANNEL \
	" :You're not on that channel"
# define LNG_ERR_USERNOTINCHANNEL \
	"%s %s :They aren't on that channel"

# define LNG_ERR_NOSUCHNICK \
	" :No such nickname or channel"
# define LNG_ERR_NOSUCHNICK_NICK \
	" :No such nickname"
# define LNG_ERR_NOSUCHNICK_CHANNEL \
	" :No such channel"
# define LNG_ERR_NOSUCHNICK_OR_SERVER \
	" :No such nickname, channel or server"

# define LNG_ERR_CANNOTSENDTONICK_MUSTID \
	" :You must be identified to send to this person"

# define LNG_ERR_CANNOTSENDTOCHAN \
	" :Cannot send to channel"
# define LNG_ERR_CANNOTSENDTOCHAN_NOOUTSIDEMSG \
	" :Cannot send to channel, you are not on that channel (+n)"
# define LNG_ERR_CANNOTSENDTOCHAN_BANNED \
	" :Cannot send to channel, you are banned (+b)"
# define LNG_ERR_CANNOTSENDTOCHAN_MODERATED \
	" :Cannot send to channel, channel is moderated (+m)"
# define LNG_ERR_CANNOTSENDTOCHAN_REGNICKSONLY \
	" :Cannot send to channel, you need to be identified (+R)"


# define LNG_DEFAULT_QUIT_MESSAGE \
	"Austnet Chat Network"


# define LNG_ERR_NOOPERHOST \
	":Access Denied: No operator details configured for your host"
# define LNG_ERR_NOOPERHOST_NOOP \
	":Access Denied: This server is in NOOP mode."
# define LNG_RPL_YOUREOPER \
	":You are now an unpaid network babysitter"
# define LNG_RPL_YOUREOPER_ALREADY \
	":You're ALREADY an IRC Operator!"
# define LNG_RPL_REHASHING \
	" :Rehashing"

# define LNG_RPL_ENDOFLINKS \
	" :End of LINKS list"

/* Server notice output strings
 * Note: This ties in with GLOBOPS, HELPME and LOCOPS broadcasts too since they
 *       are sent by the same notice mechanism.
 */
# define LNG_SERVNOTICE_LINK \
	"Link with %s established"
# define LNG_SERVNOTICE_DELINK \
	"Link with %s cancelled: %s"
# define LNG_SERVNOTICE_RECV_EOB \
	"Completed receiving Burst Data from %s"
# define LNG_SERVNOTICE_CMD_DIE \
	"%s initiated server shutdown"
# define LNG_SERVNOTICE_CMD_REHASH \
	"%s started server rehash"
# define LNG_SERVNOTICE_GARBO \
	"Garbologist completed cleaning"
# define LNG_SERVNOTICE_GARBO_ITEMS \
	"Garbologist completed cleaning %d items"
# define LNG_SERVNOTICE_GLOBOPS \
	"*** Global -- OPER from %s: %s"
# define LNG_SERVNOTICE_HELPME \
	"*** Help -- from %s: %s"
# define LNG_SERVNOTICE_LOCOPS \
	"*** Local -- from %s: %s"
# define LNG_SERVNOTICE_NOTIFY_ON_CMD_STATS \
	"*** Notice -- STATS %s by %s (%s)"

# define LNG_NOTIFY_PARANOID_OPERS_ON_WHOIS \
	"*** Notice -- %s (%s) did a /whois on you"

# define LNG_ERR_NOSUCHSERVER \
	" :No such server"

# define LNG_ERR_ISCHANSERVICE \
	"%s %s :AustNet services cannot be kicked or deoped"
# define LNG_ERR_CANTKILLSERVICES \
	":You are not allowed to kill Austnet Services!"

# define LNG_QUIT_KILLED \
	"Killed (%s (%s))"
# define LNG_RPL_KILLDONE \
	" :Killed."

# define LNG_RPL_ENDOFSILELIST \
	" :End of SILENCE list"
# define LNG_ERR_SILELISTFULL \
	" :Your silence list is full"

# define LNG_ERR_NORECIPIENT \
	":No recipient given "
# define LNG_ERR_NOTEXTTOSEND \
	":No text to send"

# define LNG_ERR_TOOMANYWATCH \
	"%s :Maximum size for WATCH-list is %d entries"
# define LNG_RPL_LOGOFF_SERVER \
	":has net-split"
# define LNG_RPL_LOGOFF_CHANNEL \
	":has disappeared"
# define LNG_RPL_LOGOFF_USER \
	":has logged off"
# define LNG_RPL_LOGON_SERVER \
	":has net-joined"
# define LNG_RPL_LOGON_CHANNEL \
	":has been created"
# define LNG_RPL_LOGON_USER \
	":has logged on"
# define LNG_RPL_NOWON_SERVER \
	":is connected"
# define LNG_RPL_NOWON_CHANNEL \
	":exists"
# define LNG_RPL_NOWON_USER \
	":is online"
# define LNG_RPL_NOWOFF \
	":is offline"
# define LNG_RPL_WATCHOFF \
	":stopped watching"
# define LNG_RPL_WATCHSTAT \
	":You have %d and are on %d WATCH entries"
# define LNG_RPL_ENDOFWATCHLIST \
	":End of WATCH %c"

#endif

