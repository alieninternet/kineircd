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

#ifndef _INCLUDE_KINEIRCD_CLBP_INPUT_H_
# define _INCLUDE_KINEIRCD_CLBP_INPUT_H_ 1

# include <string>
# include <sstream>
# include <kineircd/protocol.h>


namespace Kine {
   namespace LibCLBP {
      //! The command line based protocol (CLBP) input handling class
      class Input : virtual public Kine::Protocol {
       private:
	 // Our input queue
	 std::string inputQueue;
	 
       protected:
	 //! Constructor
	 Input(Kine::Connection& _connection)
	   : Kine::Protocol(_connection)
	   {};

	 //! Constructor (for migrating I/O queues)
	 explicit Input(Kine::Connection& _connection,
			const std::string& _inputQueue)
	   : Kine::Protocol(_connection),
	     inputQueue(_inputQueue)
	   {};
	 
       public:
	 //! Destructor
	 virtual ~Input(void)
	   {};

       private:
	 //! Parse a line (you must replace this)
	 virtual void parseLine(const std::string& line) = 0;

       public:
	 //! Handle incoming data
	 void handleInput(std::stringstream& data);
	 
       protected:
	 //! Grab a read-only version of the input queue
	 const std::string& getInputQueue(void) const
	   { return inputQueue; };
      }; // class Input
   }; // namespace LibCLBP
}; // namespace Kine
   
#endif // _INCLUDE_KINEIRCD_CLBP_INPUT_H_
