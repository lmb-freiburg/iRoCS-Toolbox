/**************************************************************************
**       Title: StDataASCII with additional load/save methods
**    $RCSfile$
**   $Revision: 519 $$Name$
**       $Date: 2004-09-08 16:36:36 +0200 (Wed, 08 Sep 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2004/09/08 14:36:36  ronneber
**  - changed key for two class svm selection from "svm_type" to "two_class_type"
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef STDATAASCIIFILE_HH
#define STDATAASCIIFILE_HH

#include "StDataASCII.hh"

namespace svt
{
  class StDataASCIIFile : public StDataASCII
  {
  public:
   
    /*======================================================================*/
    /*! 
     *   loads the map from given istream in the style 
     *   key[space]value[newline], e.g.,
     *      two_class_type c_svc
     *      kernel_type rbf
     *      gamma 40
     *      ...
     *   To be compatible with  Chih-Jen Lin's libsvm model files 
     *   set the compatibilityFlag to 1. 
     *   Attention: the map is not cleared before loading the new 
     *              key/value pairs!
     *
     *   \param is                 input stream
     *   \param compatibilityFlag  0 or 1
     *
     *   \exception LoadError incompatible file format
     */
    /*======================================================================*/
    void load( std::istream& is, int compatibilityFlag = 0);
    

    /*======================================================================*/
    /*! 
     *   saves the map to given ostream in the style key[space]value[newline], 
     *   e.g.,
     *      two_class_type c_svc
     *      kernel_type rbf
     *      gamma 40
     *      ...
     *   To be compatible with  Chih-Jen Lin's libsvm model files 
     *   set the compatibilityFlag to 1
     *
     *   \param os                 output stream
     *   \param compatibilityFlag  0 or 1
     */
    /*======================================================================*/
    void save( std::ostream& os, int compatibilityFlag = 0) const;
    


  };
 
}



#endif
