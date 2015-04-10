/**************************************************************************
 *
 * Copyright (C) 2015 Thorsten Falk
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
