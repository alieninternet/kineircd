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

#ifndef _INCLUDE_KINEIRCD_IRC2_INPUT_H_
# define _INCLUDE_KINEIRCD_IRC2_INPUT_H_ 1

# include <string>
# include <vector>
# include <aisutil/string/string.h>
# include <kineircd/libclbp/input.h>

namespace Kine {
   namespace LibIRC2 {
      /*!
       * \brief The IRC-2 protocol input base class
       *
       * \ingroup LibIRC2
       */
      class Input : public Kine::LibCLBP::Input {
       protected:
	 // The type of a 'parameter list'
	 typedef std::vector < AIS::Util::String > parameters_type;


       private:
	 //! Incoming message counter
	 messageCount_type messageCount;

	 // Appropriately parse a protocol message
	 virtual void parseMessage(const std::string& origin,
				   const std::string& command,
				   const parameters_type& parameters) = 0;

	 // Break up a protocol message into its components
	 virtual void parseLine(const std::string& line);


       protected:
	 //! Constructor
	 Input(void)
	   : messageCount(0)
	   {};

	 //! Constructor (for migrating I/O queues)
	 explicit Input(const std::string& _inputQueue)
	   : LibCLBP::Input(_inputQueue),
	     messageCount(0)
	   {};
	 
       public:
	 //! Destructor
	 virtual ~Input(void)
	   {};


	 // Return the number of messages received through this protocol
	 const messageCount_type getReceivedMessageCount(void) const
	   { return messageCount; };
      }; // class Input
   }; // namespace LibIRC2
}; // namespace Kine
   
#endif // _INCLUDE_KINEIRCD_IRC2_INPUT_H_
