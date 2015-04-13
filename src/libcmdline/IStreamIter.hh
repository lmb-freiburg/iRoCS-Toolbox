/**************************************************************************
 *
 * Copyright (C) 2005-2015 Olaf Ronneberger, Jörg Mechnich, Florian Pigorsch,
 *                         Mario Emmenlauer, Thorsten Schmidt
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

/**************************************************************************
**       Title: input stream iterator
**    $RCSfile$
**   $Revision: 93 $$Name$
**       $Date: 2002-10-28 14:11:33 +0100 (Mon, 28 Oct 2002) $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.5  2002/10/28 13:11:33  mechnich
**  made everything compile with gcc 3.2
**  removed promptUser() from CmdLine.*
**
**  Revision 1.4  2002/05/14 08:14:15  pigorsch
**  - added documentation
**
**  Revision 1.3  2002/04/10 10:25:51  pigorsch
**  - included iostream insead of istream
**
**  Revision 1.2  2002/04/04 12:03:09  pigorsch
**  - renamed operator() to fetch()
**  - renamed class CmdIstreamMultiIter to class IStreamIter
**  - removed function temporary()
**
**  Revision 1.1  2002/03/26 07:36:28  ronneber
**  restructuring for autoconf
**
**  Revision 1.1.1.1  2002/03/22 13:45:07  pigorsch
**  moved from polsoft repository
**
**
**
**************************************************************************/

#ifndef ISTREAMITER_HH
#define ISTREAMITER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <iostream>
#include <string>

#include "ArgIter.hh"

/*======================================================================*/
/*!
 *  \class IStreamIter IStreamIter.hh "libcmdline/IStreamIter.hh"
 *  \brief The IStreamIter class fetches arguments from an input stream.
 *
 *  The ArgvIter class fetches arguments from an input stream. If you
 *  want to parse arguments from an input stream like a file input stream
 *  or std::cin, etc., you use this class for iterating through them.
 *  
 *  \par Example:
 *  \code
 *  int main(int argc, char** argv)
 *  {
 *     // read arguments from stdin
 *     IStreamIter iter(std::cin);
 *     ...
 *     CmdLine cline("testprog", "this is just a test program");
 *     cline.parse(iter);
 *     ...
 *  }
 *  \endcode
 */
/*======================================================================*/
class IStreamIter: public ArgIter
{
public:
  /*====================================================================*/
  /*! 
   *   Constructor. 
   *
   *   \param is input stream to read arguments from
   *   \param separator string that marks the eof
   */
  /*====================================================================*/
  IStreamIter( 
      std::istream& is, 
      const std::string& separator="---")
          :pIs(is),
           pSeparator(separator)
        {};
  
  /*====================================================================*/
  /*! 
   *   Returns the current argument, advances to the next argument and
   *   returns NULL is there are no arguments anymore or if the specified
   *   separator string occured.
   *
   *   \return current argument, NULL if no arguments
   */
  /*====================================================================*/   
  const char* 
  fetch()
        {
          pIs >> word;
          if (!pIs) return 0;
          if (word==pSeparator) return 0;
          return word.c_str();
        };

private:
  std::istream& pIs;
  std::string word;
  std::string pSeparator;
};

#endif
