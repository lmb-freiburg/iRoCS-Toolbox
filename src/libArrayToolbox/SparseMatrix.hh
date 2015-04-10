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

/*======================================================================*/
/*!
 *  \file SparseMatrix.hh
 *  \brief Classes and functions for sparse matrix generation and handling.
 */
/*======================================================================*/

#ifndef ATBSPARSEMATRIX_LEGACY_HH
#define ATBSPARSEMATRIX_LEGACY_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <map>
#include <iostream>
#include <limits>
#include <cassert>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

#include "RuntimeError.hh"
#include "SparseVector.hh"

namespace atb
{

/*======================================================================*/
/*!
 *  \class SparseMatrixError SparseMatrix.hh "libArrayToolbox/SparseMatrix.hh"
 *  \brief The SparseMatrixError class is the exception type thrown when
 *    SparseMatrix operations fail.
 */
/*======================================================================*/
  class SparseMatrixError : public RuntimeError
  {
  public:
    
/*======================================================================*/
/*! 
 *   Create a new SparseMatrixError object with given error message.
 *
 *   \param message The error message to return to the caller
 */
/*======================================================================*/
    SparseMatrixError(std::string const &message = "")
            : RuntimeError(message)
          {}

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~SparseMatrixError() throw()
          {}
    
  };

/*======================================================================*/
/*!
 *  \class SparseMatrix SparseMatrix.hh "libArrayToolbox/SparseMatrix.hh"
 *  \brief The SparseMatrix class provides a sparse matrix data structure
 *    and corresponding arithmetics.
 */
/*======================================================================*/
  template <typename T>
  class SparseMatrix 
  {

  public:

/*======================================================================*/
/*! 
 *   Create a new empty SparseMatrix object.
 */
/*======================================================================*/
    SparseMatrix();

/*======================================================================*/
/*! 
 *   Create a new empty SparseMatrix object with given number of rows and
 *   colums.
 *
 *   \param r The number of matrix rows
 *   \param c The number of matrix columns
 */
/*======================================================================*/
    SparseMatrix(size_t r, size_t c);

/*======================================================================*/
/*! 
 *   Create a new deep copy of the given SparseMatrix object.
 *
 *   \param m The sparse matrix to copy
 */
/*======================================================================*/
    SparseMatrix(SparseMatrix const &m);

/*======================================================================*/
/*! 
 *   Create a new SparseMatrix object from a dense blitz++ Array.
 *
 *   SparseMatrix dimensions and content will be taken from the given Array.
 *
 *   \param m The blitz++ Array containing the values to copy into the new
 *     SparseMatrix object
 */
/*======================================================================*/
    SparseMatrix(blitz::Array<T,2> const &m);

/*======================================================================*/
/*! 
 *   Create a new SparseMatrix object from a dense blitz++ TinyMatrix.
 *
 *   SparseMatrix dimensions and content will be taken from the given
 *   TinyMatrix.
 *
 *   \param m The blitz++ TinyMatrix containing the values to copy into
 *     the new SparseMatrix object
 */
/*======================================================================*/
    template<int NRows, int NCols>
    SparseMatrix(blitz::TinyMatrix<T,NRows,NCols> const &m);

/*======================================================================*/
/*! 
 *   Destroy the SparseMatrix object and release all its ressources.
 *
 *   Any existing references to this SparseMatrix or views of its rows and
 *   columns get invalidated.
 */
/*======================================================================*/
    ~SparseMatrix();

/*======================================================================*/
/*! 
 *   Resize the SparseMatrix to the given number of rows and columns.
 *
 *   \param r The number of matrix rows
 *   \param c The number of matrix columns
 */
/*======================================================================*/
    void resize(size_t r, size_t c);

/*======================================================================*/
/*! 
 *   Get the number of rows of this SparseMatrix.
 *
 *   \return The number of matrix rows
 */
/*======================================================================*/
    size_t nRows() const;

/*======================================================================*/
/*! 
 *   Get the number of rows of this SparseMatrix.
 *
 *   \return The number of matrix columns
 */
/*======================================================================*/
    size_t nColumns() const;

/*======================================================================*/
/*! 
 *   Get the shape of this SparseMatrix.
 *
 *   \return The matrix shape (nRows, nColumns)
 */
/*======================================================================*/
    blitz::TinyVector<size_t,2> shape() const;

/*======================================================================*/
/*! 
 *   Get the number of non-zero entries of this SparseMatrix.
 *
 *   \return The number of non-zero matrix entries
 */
/*======================================================================*/
    size_t nNonZeroEntries() const;

/*======================================================================*/
/*! 
 *   Set all entries of this matrix to zero.
 */
/*======================================================================*/
    void clear();

/*======================================================================*/
/*! 
 *   Set all entries of the given matrix row to zero.
 *
 *   \param r The row to clear
 */
/*======================================================================*/
    void clearRow(size_t r);

/*======================================================================*/
/*! 
 *   Set all entries of the given matrix column to zero.
 *
 *   \param c The column to clear
 */
/*======================================================================*/
    void clearColumn(size_t c);

/*======================================================================*/
/*! 
 *   Get a read-only sparse vector view of the selected matrix row.
 *
 *   This is a O(1) operation.
 *
 *   \param r The matrix row to get a view for
 *
 *   \return A read-only view on the r'th row of this matrix
 */
/*======================================================================*/
    SparseVector<T> const &row(size_t r) const;

/*======================================================================*/
/*! 
 *   Get a read-only sparse vector view of the selected matrix column.
 *  
 *   Getting a matrix column for the first time is quite expensive because
 *   a new SparseVector must be created and columns entries searched in the
 *   matrix. Collecting the column entries requires to iterate over the rows.
 *   Later access to the same column is an O(1) operation if column
 *   entries did not change (Requesting a random-access reference is regarded
 *   as change! However, when changing a column through an old random-access
 *   reference will not be recognized. At best never store random-access
 *   references!).
 *
 *   \param c The matrix column to get a view for
 *
 *   \return A read-only view on the c'th column of this matrix
 */
/*======================================================================*/
    SparseVector<T> const &column(size_t c) const;

/*======================================================================*/
/*! 
 *   Check whether the given matrix entry is non-zero.
 *
 *   \param r The row index
 *   \param c The column index
 *
 *   \return \c true if the matrix entry at the given row and column is
 *     non-zero, \c false otherwise
 */
/*======================================================================*/
    bool nonZero(size_t r, size_t c) const;

/*======================================================================*/
/*! 
 *   Get a read-only reference on the entry at the specified position in
 *   the matrix.
 *
 *   \param r The row index
 *   \param c The column index
 *
 *   \return A read only reference to the matrix entry at the given position.
 */
/*======================================================================*/
    T const &operator()(size_t r, size_t c) const;

/*======================================================================*/
/*! 
 *   Get a random-access reference on the entry at the specified position in
 *   the matrix. If the entry contains zero (is not yet contained in the
 *   matrix) it will be created. Using this operator invalidates any views
 *   on the specified column, because it can potentially change. Only use
 *   these references directly. When using "old" previously stored
 *   references the column cache will not be updated!
 *
 *   \param r The row index
 *   \param c The column index
 *
 *   \return A random-access reference to the matrix entry at the given
 *     position.
 */
/*======================================================================*/
    T &operator()(size_t r, size_t c);

/*======================================================================*/
/*! 
 *   Clear the matrix entry at the given position (set it zero).
 *
 *   \param r The row index
 *   \param c The column index
 *
 *   \return A read-only iterator to the next non-zero element in the row
 *     or end() if no non-zero elements follow column c.
 */
/*======================================================================*/
    typename SparseVector<T>::const_iterator erase(size_t r, size_t c);

/*======================================================================*/
/*! 
 *   Get a dense blitz++ Array copy of this SparseMatrix.
 *
 *   \return A dense blitz++ Array
 */
/*======================================================================*/
    blitz::Array<T,2> toBlitz() const;

/*======================================================================*/
/*! 
 *   Add the given matrix to this matrix.
 *
 *   \param m The matrix to add to this matrix
 */
/*======================================================================*/
    void operator+=(SparseMatrix<T> const &m);

/*======================================================================*/
/*! 
 *   Subtract the given matrix from this matrix.
 *
 *   \param m The matrix to subtract from this matrix
 */
/*======================================================================*/
    void operator-=(SparseMatrix<T> const &m);

/*======================================================================*/
/*! 
 *   Multiply every entry of this matrix with the given scalar.
 *
 *   \param alpha The scalar the matrix entries are scaled with
 */
/*======================================================================*/
    void operator*=(T const &alpha);

/*======================================================================*/
/*! 
 *   Divide every entry of this matrix by the given scalar.
 *
 *   \param alpha The scalar the matrix entries are divided by
 */
/*======================================================================*/
    void operator/=(T const &alpha);

/*======================================================================*/
/*! 
 *   Save this SparseMatrix to an HDF5 group in a file with given name.
 *
 *   \param fileName    The name of the HDF5 file
 *   \param groupName   The name of the HDF5 group
 *   \param throwErrors If \c false is given the program aborts on error
 *     with exit code -1, otherwise a BlitzH5Error is thrown
 *
 *   \exception BlitzH5Error If the matrix cannot be saved and throwErrors
 *     is set to \c true.
 */
/*======================================================================*/
    void save(std::string const &fileName, std::string const &groupName,
              bool throwErrors = false);

/*======================================================================*/
/*! 
 *   Load the SparseMatrix from an HDF5 group in a file with given name.
 *
 *   This function resizes the matrix accordingly and fills in the values
 *   stored in the HDF5 file. Previous content of this SparseMatrix is lost.
 *
 *   \param fileName    The name of the HDF5 file
 *   \param groupName   The name of the HDF5 group
 *   \param throwErrors If \c false is given the program aborts on error
 *     with exit code -1, otherwise a BlitzH5Error is thrown
 *
 *   \exception BlitzH5Error If the matrix cannot be loaded and throwErrors
 *     is set to \c true.
 */
/*======================================================================*/
    void load(std::string const &fileName, std::string const &groupName,
              bool throwErrors = false);

  private:
  
    void updateColumn(size_t c) const;

    size_t _nRows, _nColumns;
    std::vector< SparseVector<T> > _rows;

    // Internal structures that need to be modified by const methods
    mutable std::vector<bool> _columnsValid;
    mutable std::vector< SparseVector<T> > _columns;
  
  };

/*======================================================================*/
/*! 
 *   \relates atb::SparseMatrix
 *   Matrix-vector product for sparse matrices and sparse vectors.
 *
 *   \param A The left-hand-side matrix
 *   \param x The right-hand-side vector
 *
 *   \return The sparse vector of dot products of all rows of A with x.
 */
/*======================================================================*/
  template<typename T>
  SparseVector<T> operator*(SparseMatrix<T> const &A, SparseVector<T> const &x);

/*======================================================================*/
/*! 
 *   \relates atb::SparseMatrix
 *   Matrix-vector product for sparse matrices and dense 1-D blitz++ Arrays.
 *
 *   \param A The left-hand-side matrix
 *   \param x The right-hand-side vector
 *
 *   \return The dense 1-D blitz++ Array of dot products of all rows of A
 *     with x.
 */
/*======================================================================*/
  template<typename T>
  blitz::Array<T,1> operator*(
      SparseMatrix<T> const &A, blitz::Array<T,1> const &x);

/*======================================================================*/
/*! 
 *   \relates atb::SparseMatrix
 *   Matrix-vector product for quadratic sparse matrices and blitz++
 *   TinyVectors.
 *
 *   \param A The left-hand-side matrix
 *   \param x The right-hand-side vector
 *
 *   \return The blitz++ TinyVector of dot products of all rows of A
 *     with x.
 */
/*======================================================================*/
  template<typename T, int Dim>
  blitz::TinyVector<T,Dim> operator*(
      SparseMatrix<T> const &A, blitz::TinyVector<T,Dim> const &x);

/*======================================================================*/
/*! 
 *   \relates atb::SparseMatrix
 *   Matrix-Matrix product for sparse matrices.
 *
 *   \param A The left-hand-side matrix (n x k)
 *   \param B The right-hand-side matrix (k x m)
 *
 *   \return The sparse matrix (n x m) resulting from the matrix-matrix
 *     product.
 */
/*======================================================================*/
  template<typename T>
  SparseMatrix<T> operator*(SparseMatrix<T> const &A, SparseMatrix<T> const &B);

/*======================================================================*/
/*! 
 *   \relates atb::SparseMatrix
 *   Matrix-Matrix product for blitz++ Array and sparse matrix.
 *
 *   \param A The left-hand-side matrix (n x k)
 *   \param B The right-hand-side matrix (k x m)
 *
 *   \return The sparse matrix (n x m) resulting from the matrix-matrix
 *     product.
 */
/*======================================================================*/
  template<typename T>
  SparseMatrix<T> operator*(
      blitz::Array<T,2> const &A, SparseMatrix<T> const &B);

/*======================================================================*/
/*! 
 *   \relates atb::SparseMatrix
 *   Matrix-Matrix product for sparse matrix and blitz++ Array.
 *
 *   \param A The left-hand-side matrix (n x k)
 *   \param B The right-hand-side matrix (k x m)
 *
 *   \return The sparse matrix (n x m) resulting from the matrix-matrix
 *     product.
 */
/*======================================================================*/
  template<typename T>
  SparseMatrix<T> operator*(
      SparseMatrix<T> const &A, blitz::Array<T,2> const &B);

}

#include "SparseMatrix.icc"

#endif
