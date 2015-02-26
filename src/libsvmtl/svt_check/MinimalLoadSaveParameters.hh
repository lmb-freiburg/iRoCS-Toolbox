/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 538 $$Name$
**       $Date: 2004-09-13 12:04:04 +0200 (Mon, 13 Sep 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2004/09/13 10:04:04  ronneber
**  - documentation update
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef MINIMALLOADSAVEPARAMETERS_HH
#define MINIMALLOADSAVEPARAMETERS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "RequireHelpers.hh"
#include "RequireStData.hh"

namespace svt_check
{
  /*======================================================================*/
  /*!
   *  \class MinimalLoadSaveParameters
   *  \brief The MinimalLoadSaveParameters provides a minimal
   *         implementation that fulfills the
   *         svt_check::RequireLoadSaveParameters interface 
   */
  /*======================================================================*/
  class MinimalLoadSaveParameters
  {
  public:
    /*======================================================================*/
    /*! 
     *   Load all internal variables of this class from the given
     *   structured data object    
     *
     *   \param stdata a structured data object containing the data
     *
     */
    /*======================================================================*/
    template<class STDATA>
    void loadParameters( STDATA& stdata)
          {
            if( false)
            {
              RequireStData<STDATA> x;
              avoidUnunsedVariableWarning( x);
            }
          }
    
    
    
    /*======================================================================*/
    /*! 
     *   Save all internal variables of this class to the given
     *   structured data object    
     *
     *   \param stdata a structured data object containing the data
     *
     */
    /*======================================================================*/
    template<class STDATA>
    void saveParameters( STDATA& stdata)
          {
            if( false)
            {
              RequireStData<STDATA> x;
              avoidUnunsedVariableWarning( x);
            }
          }
    
  };
}

#endif
