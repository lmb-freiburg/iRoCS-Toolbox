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
**       Title: wrapper for feature vectors to add multiclass coeficients
**    $RCSfile$
**   $Revision: 1292 $$Name$
**       $Date: 2007-11-10 14:51:29 +0100 (Sat, 10 Nov 2007) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2007/11/10 13:51:29  tschmidt
**  - initial revision (taken from polsoft-extsrc)
**
**  Revision 1.4  2002/05/23 19:03:59  ronneber
**  - operator<< now also writes coefficients
**
**  Revision 1.3  2002/05/23 15:22:54  ronneber
**  - added svCoef( int) and operator<<()
**
**  Revision 1.2  2002/05/21 18:22:05  ronneber
**  - added double #include guards
**
**  Revision 1.1  2002/05/13 16:16:25  ronneber
**  initial revision
**
**
**
**************************************************************************/

#ifndef FVWITHMULTICLASSCOEFS_HH
#define FVWITHMULTICLASSCOEFS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>
#include <iostream>


namespace svt
{
  template< typename FV>
  class FVwithMultiClassCoefs
  {
  public:

    /*======================================================================*/
    /*! 
     *   set Coeficients
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    void setSVCoef( unsigned int otherClassLabel, double coef)
          {
            _sVCoefs[otherClassLabel] = coef;
          }

    double sVCoef( unsigned int otherClassLabel) const 
          {
            return _sVCoefs[otherClassLabel];
          }
    
    unsigned int sVCoefs_size() const
          {
            return _sVCoefs.size();
          }
    
    
    const std::vector<double>& sVCoefs() const
          {
            return _sVCoefs;
          }
    
    std::vector<double>& sVCoefs()
          {
            return _sVCoefs;
          }
    

    // essential forwarding methods for TwoClassSVM's
    int getLabel() const      
          { return _fv.getLabel(); }

    // essential forwarding methods for MultiClassSVM's
    void setLabel( int label) 
          { _fv.setLabel( label); }

    // essential forwarding methods for Kernel_... 
    double dotProduct( const svt::FVwithMultiClassCoefs<FV>& fv) const
          { return _fv.dotProduct( fv.origFeatureVector()); }
    
    double square() const
          { return _fv.square(); }
    
    
    // nice to have, but not essential methods
    typedef typename FV::iterator        iterator;
    typedef typename FV::const_iterator  const_iterator;
    typedef typename FV::reference       reference;
    typedef typename FV::const_reference const_reference;
    typedef typename FV::size_type       size_type;
   
    const_reference operator[]( int index) const 
          { return _fv.operator[](index); }

    reference operator[]( int index) 
          { return _fv.operator[](index); }

    const_iterator begin() const
          { return _fv.begin(); }

    iterator begin()
          { return _fv.begin(); }
    
    const_iterator end() const
          { return _fv.end(); }
            
    iterator end()
          { return _fv.end(); }
            
    size_type size() const
          { return _fv.size(); }

    void resize( size_type s) 
          { return _fv.resize( s); }

            
    FV& origFeatureVector()             { return _fv; }
    const FV& origFeatureVector() const { return _fv; }
    
  private:
    FV _fv;
    std::vector<double> _sVCoefs;
  };

  template< typename FV>
  inline std::ostream& 
  operator<<( std::ostream& os, const FVwithMultiClassCoefs<FV>& fv)
  {
    os << "(coefs:";
    for( unsigned int i = 0; i < fv.sVCoefs_size(); ++i)
    {
      os << "  "<< fv.sVCoef(i);
    }
    
    os << ") " << fv.origFeatureVector();
    return os;
    
  }
  
}

#endif
