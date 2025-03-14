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

#ifndef _INCLUDE_KINEIRCD_CLBP_OUTPUT_H_
# define _INCLUDE_KINEIRCD_CLBP_OUTPUT_H_ 1

# include <string>
# include <kineircd/protocol/output.h>

namespace Kine {
   namespace LibCLBP {
      /*!
       * \brief The command line based protocol (CLBP) output handling class
       * 
       * \ingroup LibCLBP
       */
      class Output : public Kine::Protocol::Output {
       public:
	 static const char* const EOL_CR_LF;	//!< '\\r\\n' end of line (MS-DOS Derived)
	 static const char* const EOL_CR;	//!< '\\r' end of line (Mac)
	 static const char* const EOL_LF;	//!< '\\n' end of line (Unix)

       private:
	 //! The output data queue
	 std::string outputQueue;

       protected:
	 //! The character(s) used to terminate a line.
	 const char* eolChars;


	 //! Constructor
	 explicit Output(Kine::Connection& _connection,
			 const char* const _eolChars = EOL_CR_LF)
	   : Kine::Protocol::Output(_connection),
	     eolChars(_eolChars)
	   {};

	 //! Constructor (for migrating I/O queues)
	 explicit Output(Kine::Connection& _connection,
			 const std::string& _outputQueue,
			 const char* const _eolChars = EOL_CR_LF)
	   : Kine::Protocol::Output(_connection),
	     outputQueue(_outputQueue),
	     eolChars(_eolChars)
	   {};

       public:
	 //! Destructor
	 virtual ~Output(void)
	   {};

       protected:
	 // Add data to the output queue to be sent
	 void outputRaw(const std::string& message)
	   { outputQueue += message; };
	 
	 // Add a 'line' to the output queue to be sent (appends EOL chars)
	 void outputLine(const std::string& message)
	   {
	      outputRaw(message);
	      outputRaw(eolChars);
	   };
	 
       public:
	 // Remove up to the amount of octets given from the output queue
	 const std::string
	   withdrawOutput(const AIS::Util::Socket::Socket::blockSize_type
			  amount);
	 
	 // Return true should there be anything in the output queue to send
	 const bool moreOutput(void) const
	   { return (!outputQueue.empty()); };
	 
       protected:
	 //! Grab a read-only version of the input queue
	 const std::string& getOutputQueue(void) const
	   { return outputQueue; };
      }; // class Output
   }; // namespace LibCLBP
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_CLBP_OUTPUT_H_
   
