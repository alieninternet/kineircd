/* $Id$
 * Default configuration stuff
 * 
 * It is not recommended to edit this file. Instead, edit your configuration
 * file properly.
 * 
 * Copyright (c) 2002 KineIRCd Development Team 
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

#ifndef _SRC_LIB_CONFIG_DEFAULTS_H_
# define _SRC_LIB_CONFIG_DEFAULTS_H_ 1

# ifdef HAVE_CONFIG_H
#  include "autoconf.h"
# endif
# include "kineircd/kineircdconf.h"

// "ADMINISTRATOR" class
# define DEFAULT_ADMIN_EMAIL \
   "nobody@nowhere"
# define DEFAULT_ADMIN_LOCATION \
   "I need to configure my server"
# define DEFAULT_ADMIN_NAME \
   "Mr. Nobody"

// "NETWORK" class
# define DEFAULT_NETWORK_NAME \
   /* intentionally empty */

// "OPTIONS" class
# define DEFAULT_OPTIONS_DESCRIPTION \
   "Unconfigured server"
# define DEFAULT_OPTIONS_HIDDEN \
   false
# define DEFAULT_OPTIONS_PID_FILE \
   /* intentionally empty - empty means disabled */
# define DEFAULT_OPTIONS_SERVER_NAME \
   "unconfigured.kineircd.server"

// "OPTIONS.LIMITS" class
# define DEFAULT_OPTIONS_LIMITS_MAX_KICK_REASON_LENGTH \
   300
# define DEFAULT_OPTIONS_LIMITS_MAX_MESSAGE_LENGTH \
   400
# define DEFAULT_OPTIONS_LIMITS_MAX_TARGETS \
   4
# define DEFAULT_OPTIONS_LIMITS_MIN_BROADCAST_LENGTH \
   0

// "OPTIONS.LIMITS.CHANNELS" class
# define DEFAULT_OPTIONS_LIMITS_CHANNELS_MAX_BANS \
   50
# define DEFAULT_OPTIONS_LIMITS_CHANNELS_MAX_BAN_EXCEPTIONS \
   50
# define DEFAULT_OPTIONS_LIMITS_CHANNELS_MAX_INVITE_EXCEPTIONS \
   50
# define DEFAULT_OPTIONS_LIMITS_CHANNELS_MAX_NAME_LENGTH \
   40
# define DEFAULT_OPTIONS_LIMITS_CHANNELS_MAX_TOPIC_LENGTH \
   300
					   
// "OPTIONS.LIMITS.USERS" class
# define DEFAULT_OPTIONS_LIMITS_USERS_MAX_ACCEPTS \
   100
# define DEFAULT_OPTIONS_LIMITS_USERS_MAX_CHANNELS \
   10
# define DEFAULT_OPTIONS_LIMITS_USERS_MAX_LANGUAGES \
   6
# define DEFAULT_OPTIONS_LIMITS_USERS_MAX_NICK_NAME_LENGTH \
   9
# define DEFAULT_OPTIONS_LIMITS_USERS_MAX_REAL_NAME_LENGTH \
   50
# define DEFAULT_OPTIONS_LIMITS_USERS_MAX_SILENCES \
   20
# define DEFAULT_OPTIONS_LIMITS_USERS_MAX_WATCHES \
   128

// "OPTIONS.REGISTRAR" class
# define DEFAULT_OPTIONS_REGISTRAR_CLIENT_PING_PROBE_COUNT \
   0

#endif // _SRC_LIB_CONFIG_CONFIGDEFAULT_H_

