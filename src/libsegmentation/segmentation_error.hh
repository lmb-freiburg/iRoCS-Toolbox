/**************************************************************************
 *
 * Copyright (C) 2015 Margret Keuper, Thorsten Falk
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

#ifndef LIBMARGRET_SRC_MARGRETERROR_HH
#define LIBMARGRET_SRC_MARGRETERROR_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <stdexcept>

namespace segmentation 
{
  
class LabellingError
 : public std::exception
{
    
public:
    
  LabellingError
(const char* message);
  ~LabellingError
() throw();

  const char* what() const throw();

private:
    
  const char* _message;
    
};
 
}

#endif
