/* $Id$
 * 
 * Copyright (c) 2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2003 KineIRCd Development Team
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

#ifndef _INCLUDE_KINEIRCD_CLBP_PROTOCOL_H_
# define _INCLUDE_KINEIRCD_CLBP_PROTOCOL_H_ 1

# include <kineircd/libclbp/input.h>
# include <kineircd/libclbp/output.h>


namespace Kine {
   namespace LibCLBP {
      //! The command line based protocol (CLBP) class (both input and output)
      class Protocol : public Input, public Output {
       protected:
	 //! Constructor
	 explicit Protocol(Kine::Connection& _connection,
			   const char* const _eolChars = Output::EOL_CR_LF)
	   : Output(_connection, _eolChars)
	   {};

	 //! Constructor (for migrating I/O queues)
	 explicit Protocol(Kine::Connection& _connection,
			   const std::string& _inputQueue,
			   const std::string& _outputQueue,
			   const char* const _eolChars = Output::EOL_CR_LF)
	   : Input(_inputQueue),
	     Output(_connection, _outputQueue, _eolChars)
	   {};
	 
       public:
	 //! Destructor
	 virtual ~Protocol(void)
	   {};
      }; // class Protocol
   }; // namespace LibCLBP
}; // namespace Kine
   
#endif // _INCLUDE_KINEIRCD_CLBP_PROTOCOL_H_
