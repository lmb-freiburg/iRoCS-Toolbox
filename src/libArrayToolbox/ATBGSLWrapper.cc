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

#include "ATBGSLWrapper.hh"

namespace atb
{
  
  void atb_gsl_error_handler(char const *reason, char const *file,
                             int line, int gsl_errno)
  {
    std::stringstream msg;
    msg << "gsl_error " << gsl_errno << " while invoking " << file << ":"
        << line << ": " << reason;
    throw RuntimeError(msg.str().c_str());    
  }

  gsl_matrix *GSLWrapper::getGSLView(blitz::Array<double,2> &data)
  {
    if (!data.isStorageContiguous())
        throw RuntimeError(
            "The given blitz::Array cannot be wrapped. Its storage is not "
            "continuous.");
    gsl_matrix *A = new gsl_matrix;
    A->size1 = data.extent(0);
    A->size2 = data.extent(1);
    A->tda = data.extent(1);
    A->data = data.data();
    A->block = NULL;
    A->owner = 0;
    return A;
  }
  
  gsl_vector *GSLWrapper::getGSLView(blitz::Array<double,1> &data)
  {
    if (!data.isStorageContiguous())
        throw RuntimeError(
            "The given blitz::Array cannot be wrapped. Its storage is not "
            "continuous.");
    gsl_vector *v = new gsl_vector;
    v->size = data.size();
    v->stride = 1;
    v->data = data.data();
    v->block = NULL;
    v->owner = 0;
    return v;
  }

}
