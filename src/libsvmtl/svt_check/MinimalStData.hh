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
**   $Revision: 4820 $$Name$
**       $Date: 2011-11-08 10:57:01 +0100 (Tue, 08 Nov 2011) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**     dummy class that contains the minimal interface for StData-classes 
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef MINIMALSTDATA_HH
#define MINIMALSTDATA_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>

#include "RequireHelpers.hh"
#include "MinimalFeatureVector.hh"
#include "RequireForwardIter.hh"

namespace svt_check
{
  
  class MinimalStData
  {
  public:
    
    void setExceptionFlag( bool)   {}
    
    bool exceptionFlag() const  { return true;}

    bool valueExists(const std::string&) const { return true;}
    
    
    void setValue( const std::string&, const bool& ) {}
    void setValue( const std::string&, const int& ) {}
    void setValue( const std::string&, const unsigned int& ) {}
    void setValue( const std::string&, const float& ) {}
    void setValue( const std::string&, const double& ) {}
    void setValue( const std::string&, const std::string& ) {}
    
    template<typename ForwardIter>
    void setArray( const std::string&, const ForwardIter& arrBegin,
                   size_t)
          {
            if(false)
            {
              CHECK_MEMBER_TEMPLATE( RequireForwardIter<ForwardIter> );
              checkSetArray_ValueTypes( *arrBegin);
            }
          }
    
  private:
    void checkSetArray_ValueTypes( double) {}
    void checkSetArray_ValueTypes( unsigned int) {}
    
  public:
    template<typename ForwardIter>
    void setFVArray( const std::string& key, const ForwardIter& arrBegin,
                     size_t size)
          {
            if( false)
            {
              CHECK_MEMBER_TEMPLATE( RequireForwardIter<ForwardIter> );
              checkSetFVArray_ValueTypes( *arrBegin);
            }
            
          }
    
  private:
    void checkSetFVArray_ValueTypes( MinimalFeatureVector*) {}
    
  public:
    void getValue( const std::string&, const bool& ) {}
    void getValue( const std::string&, const int& ) {}
    void getValue( const std::string&, const unsigned int& ) {}
    void getValue( const std::string&, const float& ) {}
    void getValue( const std::string&, const double& ) {}
    void getValue( const std::string&, const std::string& ) {}
    
    unsigned int getArraySize( std::string) 
          { return 0;}  
    
    unsigned int getFVArraySize( std::string) 
          { return 0;}  
    
    template<typename ForwardIter> 
    void getArray( const std::string&, const ForwardIter& arrBegin, int)
          {
            if(false)
            {
              CHECK_MEMBER_TEMPLATE( RequireForwardIter<ForwardIter> );
              checkSetArray_ValueTypes( *arrBegin);
            }
          }
    
    template<typename ForwardIter> 
    void getFVArray( const std::string& key, const ForwardIter& arrBegin, 
                     int containerSize=-1)
          {
            if(false)
            {
              CHECK_MEMBER_TEMPLATE( RequireForwardIter<ForwardIter> );
              checkSetFVArray_ValueTypes( *arrBegin);
            }
            
          }
    
  };
}



#endif
