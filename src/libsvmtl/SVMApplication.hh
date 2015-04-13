/**************************************************************************
 *
 * Copyright (C) 2004-2015 Olaf Ronneberger, Florian Pigorsch, Jörg Mechnich,
 *                         Thorsten Falk
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
**       Title: 
**    $RCSfile$
**   $Revision: 589 $$Name$
**       $Date: 2005-01-28 15:03:30 +0100 (Fri, 28 Jan 2005) $
**   Copyright: GPL $Author: fehr $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.5  2005/01/28 14:03:30  fehr
**  some include bugfixing for external library use
**
**  Revision 1.4  2004/09/13 10:04:04  ronneber
**  - documentation update
**
**  Revision 1.3  2004/09/08 14:35:50  ronneber
**  - adapted to new ParamInfo class
**
**  Revision 1.2  2004/09/03 07:15:11  ronneber
**  - adapted to new updateCache() interface of Kernels
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.1  2003/05/19 11:09:45  ronneber
**  - initial revision
**
**
**
**************************************************************************/


#ifndef SVMAPPLICATION_HH
#define SVMAPPLICATION_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>

#include <cerrno>       // errno
#include <cstring>      // strerror()

#include "BasicSVMAdapter.hh"
#include "BasicSVMFactory.hh"
#include "BasicCVAdapter.hh"
#include "BasicCVFactory.hh"
#include "GridAxis.hh"
#include "GridSearch.hh"
#include "HelpExtractor.hh"
#include "ClassificationStatistics.hh"
#include "PrettyOptionPrinter.hh"
#include "DirectAccessor.hh"

namespace svt
{

  /*======================================================================*/
  /*!
   *  \class SVMApplication
   *  \ingroup commandline
   *  \brief The SVMApplication class specifies a complete command line 
   *         application
   *
   *  see svmtl.cc for an example how to use it
   *
   *  \param FV               feature vector class
   *  \param ALGORITHMS       lists of multi-class, two-class, one-class
   *                           algorithms an kernel functions
   *  \param LOAD_SAVE_POLICY  policy for loading and saving of data
   */
  /*======================================================================*/
  template< typename FV,         
            typename ALGORITHMS, 
            typename LOAD_SAVE_POLICY>
  class SVMApplication : public LOAD_SAVE_POLICY
  {
  public:
    typedef typename LOAD_SAVE_POLICY::STDATA STDATA;
    
    SVMApplication()
            :_prettyPrintColumn(30)
          {}
    

    virtual ~SVMApplication()
          {}

    /*======================================================================*/
    /*! 
     *   main function for an SVM-Application. Call this if you don't
     *   want to go into details 
     *
     *   \param argc  number of command line arguments
     *   \param argv  array of c-style strings containing command line
     *                arguments 
     *   \param os    output stream for help text, progress messages, etc
     *
     *   \return 0: success 1: an error occured
     */
    /*======================================================================*/
    virtual int main( int argc, const char** argv, 
                      std::ostream& os = std::cout);
    
    
    
    /*======================================================================*/
    /*! 
     *   main function for an SVM-Application, after cmdline has been
     *   parsed. Use this, if you want to add your own modes,
     *   e.g. "svmtl hurz ...". 
     *
     *   \param cmdline parsed command line arguments
     *   \param os    output stream for help text, progress messages, etc
     *
     *   \return 0: success 1: an error occured
     */
    /*======================================================================*/
    virtual int main( StDataCmdLine& cmdLine, 
                      std::ostream& os = std::cout);
    

    /*======================================================================*/
    /*! 
     *   return the name of the shell program. This is the string from
     *   argv[0] by default, but could be replaced with
     *   setProgramName() 
     *
     *   \return program name
     */
    /*======================================================================*/
    virtual const std::string& programName() const
          {
            return _programName;
          }
    
    /*======================================================================*/
    /*! 
     *   Overwrite program name that was extracted from argv[0]. The
     *   programName() ist used from the several writeHelp() methods
     *
     *   \param name program name
     */
    /*======================================================================*/
    virtual void setProgramName( const std::string& name)
          {
            _programName = name;
          }
    


  


    /*======================================================================*/
    /*! 
     *   check for wrong ( = unused) parameters in cmdline
     *
     *   \param cmdline command line parameters
     *
     *   \exception svt::CmdLineError
     */
    /*======================================================================*/
    virtual void checkWrongParameters( StDataCmdLine& cmdline);
    

    /*======================================================================*/
    /*! 
     *   do Training with given command line parameters
     *
     *   \param cmdline command line parameters
     *
     *   \exception 
     *
     *   \return 0: OK, otherwise some error occured and was reported
     *           to std::cerr 
     */
    /*======================================================================*/
    virtual int doTraining( StDataCmdLine& cmdline, 
                            std::ostream& os = std::cout);
    

    /*======================================================================*/
    /*! 
     *   do Classification with given command line parameters
     *
     *   \param cmdline command line parameters
     *
     *   \exception 
     *
     *   \return 0: OK, otherwise some error occured and was reported
     *           to std::cerr 
     */
    /*======================================================================*/
    virtual int doClassification( StDataCmdLine& cmdline, 
                                  std::ostream& os = std::cout);
   

    /*======================================================================*/
    /*! 
     *   do cross validation with given command line parameters
     *
     *   \param cmdline command line parameters
     *
     *   \exception 
     *
     *   \return 0: OK, otherwise some error occured and was reported
     *           to std::cerr 
     */
    /*======================================================================*/
    virtual int doCrossValidation( StDataCmdLine& cmdline, 
                                   std::ostream& os = std::cout);
 

    /*======================================================================*/
    /*! 
     *   do cross validation with given command line parameters
     *
     *   \param cmdline command line parameters
     *
     *   \exception 
     *
     *   \return 0: OK, otherwise some error occured and was reported
     *           to std::cerr 
     */
    /*======================================================================*/
    virtual int doGridSearch( StDataCmdLine& cmdline, 
                              std::ostream& os = std::cout);
 


    void setPrettyPrintColumn( int column)
          {
            _prettyPrintColumn = column;
          }
    

  private:
    std::string _programName;
    int _prettyPrintColumn;
    
    
  };
  
}

#include "SVMApplication.icc"


#endif
