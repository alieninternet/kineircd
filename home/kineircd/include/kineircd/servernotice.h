/* servernotice.h
 * ServerNotice class
 * 
 * Copyright (c) 2001,2002 AustHex Development Team
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

#ifndef __SERVNOTICE_H_
# define __SERVNOTICE_H_

// Locally connected user class
class ServerNotice {
 public:
   // Server notices mask
   typedef unsigned long servnotice_t;
    
   static const unsigned int NUM_TYPES;
   
   // These are 'server notice' types, used to determine what is sent to users.
   static servnotice_t const SN_BURST		= 0x00000001; // +b
   static servnotice_t const SN_DEBUG		= 0x00000002; // +D
   static servnotice_t const SN_GLOBOPS		= 0x00000004; // +G
   static servnotice_t const SN_HELPME		= 0x00000008; // +h
   static servnotice_t const SN_HTM		= 0x00000010; // +H
   static servnotice_t const SN_IPMISMATCH	= 0x00000020; // +I
   static servnotice_t const SN_NETJOIN_MINOR	= 0x00000040; // +j
   static servnotice_t const SN_NETJOIN_MAJOR	= 0x00000080; // +J
   static servnotice_t const SN_KILL_LOCAL	= 0x00000100; // +k
   static servnotice_t const SN_KILL_REMOTE	= 0x00000200; // +K
   static servnotice_t const SN_LOCOPS		= 0x00000400; // +L
   static servnotice_t const SN_OPERING_LOCAL	= 0x00000800; // +o
   static servnotice_t const SN_OPERING_GLOBAL	= 0x00001000; // +O
   static servnotice_t const SN_SIGNON		= 0x00002000; // +s
   static servnotice_t const SN_SIGNOFF		= 0x00004000; // +S
   static servnotice_t const SN_SERVER		= 0x00008000; // +z
   static servnotice_t const SN_HOUSEKEEPING	= 0x00010000; // +Z
   static servnotice_t const SN_NETSPLIT_MINOR	= 0x00020000; // +n
   static servnotice_t const SN_NETSPLIT_MAJOR	= 0x00040000; // +N

   // The server notice char 
   struct typeTableStruct {
      char type;				// Type of server notice
      servnotice_t mask;			// Server notice mask (above)
   }; 
   static struct typeTableStruct const typeTable[];
};
#endif
