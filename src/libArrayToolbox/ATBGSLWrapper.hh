/**************************************************************************
**       Title: GSL wrapper methods
**    $RCSfile$
**   $Revision: 3896 $ $Name$
**       $Date: 2008-07-16 15:17:47 +0200 (Wed, 16 Jul 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Converters to and from GSL for own data structures
**
**************************************************************************/

/*======================================================================*/
/*!
 *  \file ATBGSLWrapper.hh
 *  \brief ATBGSLWrapper.hh provides wrappers and setup routines to use
 *     GSL functions with ArrayToolbox data structures.
 */
/*======================================================================*/

#ifndef ATBGSLWRAPPER_HH
#define ATBGSLWRAPPER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <gsl/gsl_linalg.h>

#include <blitz/array.h>

#include "RuntimeError.hh"

namespace atb
{
  
/*======================================================================*/
/*! 
 *   Default Arraytoolbox_Qt4 GSL error handler.
 *
 *   This function converts any thrown gsl error into a corresponding
 *   ArrayToolbox_Qt4_Error and throws it.
 *
 *   \param reason A buffer to hold the error string
 *   \param file The file causing the error
 *   \param line The line that caused the error
 *   \param gsl_errno The GSL error code
 *
 *   \exception ArrayToolbox_Qt4_Error This exception is thrown when calling
 *      this function, containing a string representation of the provided
 *      GSL error data
 */
/*======================================================================*/
  extern void atb_gsl_error_handler(char const *reason, char const *file,
                                    int line, int gsl_errno);

/*======================================================================*/
/*!
 *  \class GSLWrapper <>
 *  \brief The GSLWrapper class provides static wrapper functions
 *     to obtain GSL views onto Array data structures.
 *
 *  The GSL views directly operate on the original data, only the gsl
 *  specific metadata are added to allow usage of the data within gsl calls.
 */
/*======================================================================*/
  class GSLWrapper
  {
    
  public:
    
/*======================================================================*/
/*! 
 *   Create a gsl_matrix view to the given blitz++ Array.
 *
 *   The Array is not copied, only gsl control structures are added, thus
 *   any processing will be performed directly on the blitz++ Array data.
 *   The user is responsible for freeing the space used up by the control
 *   structures, so please delete the gsl_matrix after you don't need gsl
 *   support for the blitz++ Array any more. The Array data will not be
 *   touched when freeing the gsl_matrix.
 *
 *   \param data The blitz++ Array to wrap
 *
 *   \return A gsl_matrix view of the data
 */
/*======================================================================*/
    static gsl_matrix *getGSLView(blitz::Array<double,2> &data);

/*======================================================================*/
/*! 
 *   Create a gsl_matrix view to the given blitz++ TinyMatrix.
 *
 *   The TinyMatrix is not copied, only gsl control structures are added,
 *   thus any processing will be performed directly on the blitz++
 *   TinyMatrix data. The user is responsible for freeing the space used
 *   up by the control structures, so please delete the gsl_matrix after
 *   you don't need gsl support for the blitz++ TinyMatrix any more.
 *   The TinyMatrix data will not be touched when freeing the gsl_matrix.
 *
 *   \param data The blitz++ TinyMatrix to wrap
 *
 *   \return A gsl_matrix view of the data
 */
/*======================================================================*/
    template<int Dim1, int Dim2>
    static gsl_matrix *getGSLView(blitz::TinyMatrix<double,Dim1,Dim2> &data);

/*======================================================================*/
/*! 
 *   Create a gsl_vector view to the given 1-D blitz++ Array.
 *
 *   The Array is not copied, only gsl control structures are added, thus
 *   any processing will be performed directly on the blitz++ Array data.
 *   The user is responsible for freeing the space used up by the control
 *   structures, so please delete the gsl_vector after you don't need gsl
 *   support for the blitz++ Array any more. The Array data will not be
 *   touched when freeing the gsl_vector.
 *
 *   \param data The 1-D blitz++ Array to wrap
 *
 *   \return A gsl_vector view of the data
 */
/*======================================================================*/
    static gsl_vector *getGSLView(blitz::Array<double,1> &data);

/*======================================================================*/
/*! 
 *   Create a gsl_vector view to the given blitz++ TinyVector.
 *
 *   The TinyVector is not copied, only gsl control structures are added,
 *   thus any processing will be performed directly on the blitz++
 *   TinyVector data. The user is responsible for freeing the space used
 *   up by the control structures, so please delete the gsl_vector after
 *   you don't need gsl support for the blitz++ TinyVector any more. The
 *   TinyVector data will not be touched when freeing the gsl_vector.
 *
 *   \param data The blitz++ TinyVector to wrap
 *
 *   \return A gsl_vector view of the data
 */
/*======================================================================*/
    template<int Dim>
    static gsl_vector *getGSLView(blitz::TinyVector<double,Dim> &data);

  };

}

#include "ATBGSLWrapper.icc"

#endif
