/* $Id$
 * 
 * Copyright (c) 2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2002,2003 KineIRCd Development Team
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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include <kineircd/daemon.h>

#include "mod_channellog/channellog.h"

using namespace Kine::mod_channellog;


/* ChannelLog - Constructor
 * Original 01/11/2002 simonb
 */
ChannelLog::ChannelLog(mod_channellog::Config& c)
  : Kine::Logger(c.getLogMask()),
    config(c)
{
}


/* ~FileLog - Destructor
 * Original 01/11/2002 simonb
 */
ChannelLog::~ChannelLog(void)
{
}


/* logLine - Log a line to the channel
 * Original 01/11/2002 simonb
 */
void ChannelLog::logLine(const std::string& str,
			 const Kine::Logger::Mask::type mask)
{
   if (ok()) {
   }
}
