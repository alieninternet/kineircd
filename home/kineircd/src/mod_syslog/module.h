/* $Id$
 * 
 * Copyright (c) 2001,2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2002,2003 KineIRCd Development Team
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

#ifndef _SRC_MODULES_MOD_SYSLOG_MODULE_H_
# define _SRC_MODULES_MOD_SYSLOG_MODULE_H_ 1

# ifdef HAVE_CONFIG_H
#  include "autoconf.h"
# endif

# define MOD_SYSLOG_NAME_SHORT			"mod_syslog"
# define MOD_SYSLOG_NAME_LONG			"Syslog logging module"

# define MOD_SYSLOG_COPYRIGHT		\
   "Copyright (c) 2001,2002,2003 KineIRCd Development Team"

# define MOD_SYSLOG_VERSION_MAJOR		KINEIRCD_VERSION_MAJOR
# define MOD_SYSLOG_VERSION_MINOR		KINEIRCD_VERSION_MINOR

#endif // _SRC_MODULES_MOD_SYSLOG_MODULE_H_
