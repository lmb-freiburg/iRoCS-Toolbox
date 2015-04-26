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
**       Title: Kernel_FILE
**    $RCSfile$
**   $Revision: 740 $$Name$
**       $Date: 2005-07-19 15:42:59 +0200 (Tue, 19 Jul 2005) $
**   Copyright: LGPL $Author: haasdonk $
** Description: kernel accessing complete kernel matrix from File
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2005/07/19 13:42:59  haasdonk
**  adapted header for automatic CVS-Information extraction
**
**
**************************************************************************/

#ifndef KERNEL_FILE_HH
#define KERNEL_FILE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// std includes
#include <vector>
#include <algorithm>
// libsvmtl includes
#include "svm_defines.hh"
#include "SVMError.hh"
#include "ProgressReporter.hh"

// requirements of template parameters
#include "svt_check/RequireStData.hh"
#include "svt_check/RequireLoadSaveParameters.hh"
#include "svt_check/RequireNameDescriptionParamInfos.hh"
#include "svt_check/RequireFeatureVectorUniqueID.hh"

namespace svt
{
  template< typename FILETYPE>
  class Kernel_FILE
  {

  public:
    Kernel_FILE()
            : _file(),
              _data(0),
              _rowStarts(0),
              _gIDCache1(0),
              _gIDCache2(0),
              _maxgID1(0),
              _maxgID2(0),
              _height(0),
              _width(0),
              _cacheIsUpToDate(false)        
          {}
    
/*-------------------------------------------------------------------------
 *  if you really want to copy Kernel_FILE's, which might be very
 *  expensive, you can define  ALLOW_KERNEL_MATRIX_COPY
 *-------------------------------------------------------------------------*/
#ifdef ALLOW_KERNEL_MATRIX_COPY
  public:
#else
  private:
#endif

    Kernel_FILE( const Kernel_FILE<FILETYPE>& orig)
            : _file(0),
              _data(0),
              _rowStarts(0),
              _gIDCache1(0),
              _gIDCache2(0),
              _maxgID1(0),
              _maxgID2(0),
              _height(0),
              _width(0),
              _cacheIsUpToDate(false)        
          {
            operator=( orig);
          }
    
    void operator=( const Kernel_FILE<FILETYPE>& orig)
          {
            _cacheIsUpToDate = orig._cacheIsUpToDate;
            _file = orig._file;

            resizeCache( orig._height, orig._width, 
                         orig._maxgID1, orig._maxgID2);
            
           
            if (orig._gIDCache1 != 0) std::copy(orig._gIDCache1,
                                                _gIDCache1+_maxgID1+1, 
                                                _gIDCache1);
            else // delete single entry allocated by resizeCache
            {
              delete[] _gIDCache1;
              _gIDCache1 = 0;
            }
            
            
            if  (orig._gIDCache2 != 0) std::copy(orig._gIDCache2,
                                                 _gIDCache2+_maxgID2+1, 
                                                 _gIDCache2);
            else // delete single entry allocated by resizeCache
            {
              delete[] _gIDCache2;
              _gIDCache2 = 0;
            }
            
            if( orig._height !=0 && orig._width != 0)
            {
              unsigned int size = _height*_width;
              std::copy( orig._data, orig._data + size, _data);
            } 
          }

  public:
    

    ~Kernel_FILE()
          {
            clearCache();
          };

    void resizeCache( unsigned int height, unsigned int width, 
                      unsigned int maxgID1, unsigned int maxgID2) const
          {
            
            clearCache();
            
            _width = width;
            _height = height;
            _maxgID1 = maxgID1;
            _maxgID2 = maxgID2;
            
            if( _width != 0 && _height != 0)
            {
              _data = new float[height*width];
              _rowStarts = new float*[height];
              
              for( unsigned int i = 0; i < height; ++i)
              {
                _rowStarts[i] = _data + i*width;
              }
            }
            else
            {
              _data = 0;
              _rowStarts = 0;
            }
            
            _gIDCache1 = new unsigned int[maxgID1+1];
            _gIDCache2 = new unsigned int[maxgID2+1];
          }
    
    void clearCache() const
          {
            if( _data != 0)
            {
              delete[] _data;
              _data = 0;
            }
            
            if( _rowStarts != 0)
            {
              delete[] _rowStarts;
              _rowStarts = 0;
            }

            if( _gIDCache1 != 0)
            {
              delete[] _gIDCache1;
              _gIDCache1 = 0;
            }

            if( _gIDCache2 != 0)
            {
              delete[] _gIDCache2;
              _gIDCache2 = 0;
            }

            _maxgID1 = 0;
            _maxgID2 = 0;
            _width = 0;
            _height = 0;
            _cacheIsUpToDate = false;
          }

    // updateCache version with one FV-list    

    template< typename ForwardIter, typename Accessor>
    void updateCache( const ForwardIter& fvBegin,  
                      const ForwardIter& fvEnd,
                      Accessor accessor,
                      ProgressReporter* pr = 0) const
          {
            updateCache(fvBegin,fvEnd,accessor,fvBegin,fvEnd,accessor,pr);
          }
    
    // updateCache version with two FV-lists, using globalIDs    
    
    template< typename ForwardIter1, typename Accessor1,
              typename ForwardIter2, typename Accessor2>
    void updateCache( const ForwardIter1& fvBegin1,  
                      const ForwardIter1& fvEnd1,
                      Accessor1 accessor1,
                      const ForwardIter2& fvBegin2,  
                      const ForwardIter2& fvEnd2,
                      Accessor2 accessor2,
                      ProgressReporter* pr = 0) const
          {    
            // collect global indices and sort rising
            std::vector<unsigned int> gIDs1;
            std::vector<unsigned int> gIDs2;
            std::vector<unsigned int>::iterator git;
            
            for (ForwardIter1 fi = fvBegin1; fi != fvEnd1; fi++)
                gIDs1.push_back(accessor1(fi).getGlobalID());
            std::sort(gIDs1.begin(),gIDs1.end());
            
            for (ForwardIter2 fi = fvBegin2; fi != fvEnd2; fi++)
                gIDs2.push_back(accessor2(fi).getGlobalID());
            std::sort(gIDs2.begin(),gIDs2.end());

            // determine maximum global indices
            _maxgID1 = 0;
            _maxgID2 = 0;
            
            if (gIDs1.begin() != gIDs1.end())
                _maxgID1 = *(gIDs1.end()-1);
            if (gIDs2.begin() != gIDs2.end())
                _maxgID2 = *(gIDs2.end()-1);
            
            resizeCache(gIDs1.size(),gIDs2.size(),_maxgID1,_maxgID2);
            
            // fill index-tables
            int i = 0;
            for (git=gIDs1.begin();
                 git!=gIDs1.end();git++,i++)
                _gIDCache1[*git]=i;
            
            i = 0;
            for (git=gIDs2.begin();
                 git!=gIDs2.end();git++,i++)
                _gIDCache2[*git]=i;

            // fill Cache-matrix 
            _file.load_matrix(_rowStarts,gIDs1,gIDs2);
            
            if( pr != 0)
            {
              pr->reportProgress( TASK_LEVEL_CROSS_VAL,
                                  "full kernel matrix read", 1.0,
                                  "");
            }

//             output read kernel matrix
//             std::cout << "kernel matrix: \n";
//             for (unsigned int i = 0; i< _height; i++)
//             {
//               std::cout << "km-row " << i << ": ";
//               for (unsigned int j = 0; j< _width; j++)
//                   std::cout << _rowStarts[i][j] << " ";              
//               std::cout << "\n"; 
//             }

//             std::cout << "gid-list 1: \n";
//             for (unsigned int i = 0; i< _height; i++)
//             {
//               std::cout << gIDs1[i] << " ";              
//             }
//               std::cout << "\n"; 
//             std::cout << "gid-list 2: \n";
//             for (unsigned int i = 0; i< _width; i++)
//             {
//               std::cout << gIDs2[i] << " ";              
//             }
//             std::cout << "\n"; 

            _cacheIsUpToDate = true;
          }
    

    template< typename FV>
    double k_function( const FV& x, const FV& y) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireFeatureVectorUniqueID<FV>);            
            SVM_ASSERT( _cacheIsUpToDate == true );

            unsigned int gID1 = x.getGlobalID();
            unsigned int gID2 = y.getGlobalID();
 
            SVM_ASSERT( gID1 <= _maxgID1 );
            SVM_ASSERT( gID2 <= _maxgID2 );
                                    
            return _rowStarts[ _gIDCache1[gID1] ][ _gIDCache2[gID2] ];
          };    

    template<typename STDATA>
    void loadParameters( STDATA& stData)
          {
             _file.loadParameters( stData);
          };
    
    template<typename STDATA>
    void saveParameters( STDATA& stData) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            _file.saveParameters( stData);
            stData.setValue( "kernel_type", name());
          };

    static std::string name()
          {
            return std::string( "file_")+ FILETYPE::name();
          }

    static std::string description()
          {
            return std::string( "cached kernel matrix file, ")+ 
              FILETYPE::description();
          }

    static void getParamInfos( std::vector<ParamInfo>& p)
          {
            FILETYPE::getParamInfos( p);
          }
    
  private:
    mutable FILETYPE _file;
    mutable float*      _data;
    mutable float**     _rowStarts;
    mutable unsigned int* _gIDCache1;    
    mutable unsigned int* _gIDCache2;    
    mutable unsigned int _maxgID1;
    mutable unsigned int _maxgID2;
    mutable unsigned int _height;
    mutable unsigned int _width;
    mutable bool         _cacheIsUpToDate;    
  };

}

#endif
