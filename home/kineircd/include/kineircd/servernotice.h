/* servernotice.h
 * ServerNotice class
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
