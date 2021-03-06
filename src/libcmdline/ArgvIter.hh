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
**       Title: command line arguments iterator
**    $RCSfile$
**   $Revision: 78 $$Name$
**       $Date: 2002-05-14 10:14:15 +0200 (Tue, 14 May 2002) $
**   Copyright: GPL $Author: pigorsch $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2002/05/14 08:13:56  pigorsch
**  - added documentation
**
**  Revision 1.2  2002/04/04 12:02:49  pigorsch
**  - renamed operator() to fetch()
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

#ifndef ARGVITER_HH
#define ARGVITER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ArgIter.hh"

/*======================================================================*/
/*!
 *  \class ArgvIter ArgvIter.hh
 *  \brief The ArgvIter class fetches arguments from a c-style 
 *         char* array.
 *
 *  The ArgvIter class fetches arguments from a c-style array. If you
 *  want to parse arguments that were delivered to your program on the
 *  command line, you use this class for iterating through them.
 *  \par Example:
 *  \code
 *  int main(int argc, char** argv)
 *  {
 *     // read arguments from the command line
 *     ArgvIter iter(argc, argv);
 *     ...
 *     CmdLine cline("testprog", "this is just a test program");
 *     cline.parse(iter);
 *     ...
 *  }
 *  \endcode
 */
/*======================================================================*/
class ArgvIter: public ArgIter
{
public:
  /*====================================================================*/
  /*! 
   *   Constructor.
   *
   *   \param argc number of elements in argv
   *   \param argv array of strings containing the arguments
   */
  /*====================================================================*/
  ArgvIter(
      int argc, 
      const char* const* argv)
          :pCount(argc), 
           pIndex(0), 
           pArray(argv) 
        {};

  /*====================================================================*/
  /*! 
   *   Returns the current argument, advances to the next argument and
   *   returns NULL is there are no arguments anymore.
   *
   *   \return current argument, NULL if no arguments
   */
  /*====================================================================*/ 
  const char*
  fetch()
      {
        const char* s=0;
        
        if (pIndex<pCount && pArray[pIndex]!=0)
        {
          s=pArray[pIndex];
          ++pIndex;
        }
        
        return s;
      };
  
  /*====================================================================*/
  /*! 
   *   Specify new array of strings to be parsed.
   *
   *   \param argc number of elements in argv
   *   \param argv array of strings containing the arguments
   */
  /*====================================================================*/
  void
  reset(
      int argc, 
      const char* const* argv)
        { 
          pCount=argc; 
          pArray=argv; 
          pIndex=0; 
        };
    
private:
  int pCount;
  int pIndex;
  const char* const* pArray;
};

#endif
