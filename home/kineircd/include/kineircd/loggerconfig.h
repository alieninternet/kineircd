/* $Id$
 * Configuration helper stuff for logging modules which may need it
 * 
 * If you were expecting definitions to edit, either pass your configuration
 * parameters to the configuration script or edit autoconf.h. Default
 * configuration defines can be found in src/config/configdefault.h, but it 
 * is not recommended to edit that file, but rather to edit your configuration
 * file properly.
 * 
 * Copyright (c) 2002 Simon Butcher <pickle@alien.net.au>
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

#ifndef _INCLUDE_KINEIRCD_LOGGERCONFIG_H_
# define _INCLUDE_KINEIRCD_LOGGERCONFIG_H_ 1

# include <aisutil/config/data.h>
# include <kineircd/logger.h>


namespace Kine {
   // Config class
   class LoggerConfig : public AISutil::ConfigData {
    protected:
      // The logging mask
      Logger::Mask::lazy_type defLogMask;

      // Variable handler to process text for the log mask
      static LIBAISUTIL_CONFIG_VARIABLE_HANDLER(varHandleLogMask);
      
    public:
      // Constructor
      LoggerConfig(void)
	: defLogMask(Logger::Mask::Nothing)
	{};
      
      // Destructor
      virtual ~LoggerConfig(void)
	{};
      
      // Grab the mask
      const Logger::Mask::lazy_type getLogMask(void) const
	{ return defLogMask; };
   };
}; // namespace Kine
   
#endif // _INCLUDE_KINEIRCD_LOGGERCONFIG_H_
   
