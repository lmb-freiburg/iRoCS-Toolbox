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
**       Title: Triangular Matrix used to store Models for One Vs. One SMV's
**    $RCSfile$
**   $Revision: 4820 $$Name$
**       $Date: 2011-11-08 10:57:01 +0100 (Tue, 08 Nov 2011) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.6  2003/05/19 11:24:19  ronneber
**  - fixed segfault when calling resize( 0)
**
**  Revision 1.5  2002/10/28 16:00:52  mechnich
**  fixed compatibility problems with gcc3
**
**  Revision 1.4  2002/05/23 19:06:48  ronneber
**  - created Constructor with empty arguments
**
**  Revision 1.3  2002/05/14 10:17:46  ronneber
**  - now representing an _upper_ triangular matrix (to be compatible with
**    orig libsvm)
**  - added begin() and end() methods to access the raw data
**
**  Revision 1.2  2002/05/06 12:26:03  ronneber
**  - first functional version
**
**  Revision 1.1  2002/05/03 13:55:52  ronneber
**  corrected typo in filename
**
**  Revision 1.1  2002/05/02 15:43:55  ronneber
**  initial revision
**
**
**
**************************************************************************/

#ifndef TRIANGULARMATRIX_HH
#define TRIANGULARMATRIX_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>

namespace svt
{

  /*======================================================================*/
  /*!
   *  \class TriangularMatrix TriangularMatrix.hh
   *  \brief The TriangularMatrix class is an upper triangular matrix
   *         without diagonal elements 
   *  
   */
  /*======================================================================*/


  template< typename T>
  class TriangularMatrix
  {
  public:
    typedef T                                        value_type;
    typedef typename std::vector<T>::size_type       size_type;
    typedef typename std::vector<T>::reference       reference;
    typedef typename std::vector<T>::const_reference const_reference;
    typedef typename std::vector<T>::iterator        iterator;
    typedef typename std::vector<T>::const_iterator  const_iterator;
    
    TriangularMatrix()
            :_width( 0)
          {
          }
    
    TriangularMatrix( size_type width, const T& defaultVal = T())
          {
            resizeWidth( width, defaultVal);
          }
    /*======================================================================*/
    /*! 
     *   resize triangular matrix to new width. Number of resulting 
     *   elements will be width*(width-1)/2  
     *
     *   \param width      new width
     *   \param defaultVal default value
     */
    /*======================================================================*/
    void resizeWidth( size_type width, const T& defaultVal = T())
          {
            _width = width;
            if( width == 0)
            {
              _data.resize(0);
              _rowStart.resize(0);
              return;
            }
            
            _data.resize( width*(width-1)/2, defaultVal);
            _rowStart.resize( width-1);
            int lineWidth = static_cast<int>(width-2);
            int rowStartIndex = -1;
            for(int i = 0; i < static_cast<int>(width - 1); ++i)
            {
              _rowStart[i] = rowStartIndex;
              rowStartIndex += lineWidth;
              --lineWidth;
            }
          }
    

    size_type width() const
          {
            return _width;
          }
    

    size_type nElements() const
          {
            return _data.size();
          }
    
    /*======================================================================*/
    /*! 
     *   to be compatible with other standard containers
     *
     *   \return nElements of Triangluar Matrix
     */
    /*======================================================================*/
    size_type size() const
          {
            return _data.size();
          }
    
   
    /*======================================================================*/
    /*! 
     *   2dim element access. Ensure that (row < col < width), because it
     *   is an upper triangular matrix without diagonal elements
     *
     *   \param row   Matrix row (0 ... width-2)
     *   \param col   Matrix col (row+1 ... width-1)
     *
     *   \return reference to the element
     */
    /*======================================================================*/
    reference operator()(size_type row, size_type col)
          {
            return _data[_rowStart[row]+col];
          }

    /*======================================================================*/
    /*! 
     *   const 2dim element access. Ensure that (row < col < width), because it
     *   is an upper triangular matrix without diagonal elements
     *
     *   \param row   Matrix row (0 ... width-2)
     *   \param col   Matrix col (row+1 ... width-1)
     *
     *   \return const reference to the element
     */
    /*======================================================================*/
    const_reference operator()(size_type row, size_type col) const
          {
            return _data[_rowStart[row]+col];
          }
    
    /*======================================================================*/
    /*! 
     *   1dim element access, accessing data in lexicographical order
     *   (e.g. for a matrix with width of 4 this results in: 
     *
     *        m[0] == m(0,1)
     *        m[1] == m(0,2)
     *        m[2] == m(0,3)
     *        m[3] == m(1,2)
     *        m[4] == m(1,3)
     *        m[5] == m(2,3)
     *
     *   \param index  index (0 ... size()-1)
     *
     *   \return reference to the element
     */
    /*======================================================================*/
    reference operator[](size_type index)
          {
            return _data[index];
          }
    
    const_reference operator[](size_type index) const
          {
            return _data[index];
          }
    
    
    const_iterator begin() const { return _data.begin(); }
    iterator       begin()       { return _data.begin(); }
    const_iterator end() const   { return _data.end(); }
    iterator       end()         { return _data.end(); }

  private:
    std::vector<int> _rowStart; // indizes of row starts
    std::vector<T> _data;
    size_type _width;
  };
}


#endif
