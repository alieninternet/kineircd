/* $Id$
 * Support class for modules
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

#ifndef _INCLUDE_KINEIRCD_MODULE_H_
# define _INCLUDE_KINEIRCD_MODULE_H_

class Module {
 private:
   /* This structure defines information about the module itself. Each module
    * must have one of these present to define parameters about the module
    * so that the server has some idea of how to handle it, what version it
    * is and so on.
    */
   struct modInfo_type {
      // Type of the module, so we know how to install it correctly
      enum type_type {
	 MODTYPE_PROTOCOL,			// Protocol module (pmod)
	 MODTYPE_SERVICE,			// Service module (smod)
	 MODTYPE_EXTENTION			// Core extention module (emod)
      } const type;
      
      /* Name and version information of the module.
       * Note that the full module version will be seen as follows:
       * <name>-<versionMajor>.<versionMinor>.<versionPatchLevel><versionExtra>
       * Eg. FooMod-1.2.3b
       */
      const char *name;
      const char *copyright;
      const unsigned char versionMajor;
      const unsigned char versionMinor;
      const unsigned short versionPatchLevel;
      const char *versionExtra;
   } moduleInfo;
   
 public:
   Module(void)
     {};
   ~Module(void)
     {};
};

#endif
