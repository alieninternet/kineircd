/* module.h
 * Support class for modules
 * 
 * Copyright (c) 2002 AustHex Development Team
 * (See DEV-TEAM file for details)
 *
 * This file is a part of AustHex.
 * 
 * AustHex is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * AustHex is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with AustHex; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _INCLUDE_AUSTHEX_MODULE_H_
# define _INCLUDE_AUSTHEX_MODULE_H_

class Module {
 private:
   enum modType_type {
      MOD_PROTOCOL,				// Protocol module (pmod)
      MOD_SERVICE,				// Service module (smod)
      MOD_EXTENTION				// Core extention module (emod)
   } const type;
   
 public:
   Module(modType_type t)
     : type(t)
     {};
   ~Module(void)
     {};
};

#endif
