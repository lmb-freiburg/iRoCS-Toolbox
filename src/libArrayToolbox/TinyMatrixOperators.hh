/**************************************************************************
 **       Title: General blitz++ TinyMatrix extension operators
 **    $RCSfile$
 **   $Revision: 3896 $ $Name$
 **       $Date: 2008-07-16 15:17:47 +0200 (Wed, 16 Jul 2008) $
 **   Copyright: GPL $Author: tschmidt $
 ** Description:
 **
 **    This file provides some additional blitz::TinyMatrix operators that
 **    are not implemented in blitz++
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file TinyMatrixOperators.hh
 *  \brief TinyMatrixOperators.hh provides some missing operators when
 *     processing blitz++ TinyMatrices.
 */
/*======================================================================*/

#ifndef ATBTINYMATRIXOPERATORS_HH
#define ATBTINYMATRIXOPERATORS_HH

namespace blitz
{

/*======================================================================*/
/*! 
 *   Negation of a blitz::TinyMatrix.
 *
 *   \param m The matrix
 *
 *   \return The matrix with all values sign-changed
 */
/*======================================================================*/
  template<typename DataT, int NRows, int NColumns>
  TinyMatrix<DataT,NRows,NColumns> operator-(
      TinyMatrix<DataT,NRows,NColumns> const &m)
  {
    TinyMatrix<DataT,NRows,NColumns> res;
    for (int i = 0; i < NRows * NColumns; ++i) res.data()[i] = -m.data()[i];
    return res;
  }

/*======================================================================*/
/*! 
 *   Product of a blitz::TinyMatrix and a blitz::TinyVector.
 *
 *   \param m The matrix
 *   \param v The vector to multiply to matrix m
 *
 *   \return The resulting vector of the matrix vector multiplication
 */
/*======================================================================*/
  template<typename DataT, int Dim1, int Dim2>
  TinyVector<DataT,Dim2> operator*(
      TinyMatrix<DataT,Dim2,Dim1> const &m,
      TinyVector<DataT,Dim1> const &v)
  {
    TinyVector<DataT,Dim2> res(DataT(0));
    for (int r = 0; r < Dim2; ++r)
        for (int c = 0; c < Dim1; ++c) res(r) += m(r,c) * v(c);
    return res;
  }

/*======================================================================*/
/*! 
 *   Product of a blitz::TinyVector and a blitz::TinyMatrix. The vector
 *   is interpreted as row vector and left-multiplied to the matrix.
 *
 *   \param v The vector
 *   \param m The matrix
 *
 *   \return The resulting vector of the vector matrix multiplication
 */
/*======================================================================*/
  template<typename DataT, int Dim1, int Dim2>
  TinyVector<DataT,Dim1> operator*(
      TinyVector<DataT,Dim2> const &v,
      TinyMatrix<DataT,Dim2,Dim1> const &m)
  {
    TinyVector<DataT,Dim1> res(DataT(0));
    for (int r = 0; r < Dim2; ++r)
        for (int c = 0; c < Dim1; ++c) res(c) += m(r,c) * v(r);
    return res;
  }

/*======================================================================*/
/*! 
 *   Product of a scalar and a blitz::TinyMatrix.
 *
 *   \param alpha The scalar to multiply to the matrix
 *   \param m The matrix
 *
 *   \return The product of the scalar alpha with the matrix m
 */
/*======================================================================*/
  template<typename DataT, int NRows, int NColumns>
  TinyMatrix<DataT,NRows,NColumns> operator*(
      DataT const &alpha, TinyMatrix<DataT,NRows,NColumns> const &m)
  {
    TinyMatrix<DataT,NRows,NColumns> res;
    for (int i = 0; i < NRows * NColumns; ++i)
    {
      res.data()[i] = m.data()[i] * alpha;
    }
    return res;
  }

/*======================================================================*/
/*! 
 *   Product of a scalar and a blitz::TinyMatrix.
 *
 *   \param m The matrix
 *   \param alpha The scalar to multiply to the matrix
 *
 *   \return The product of the scalar alpha with the matrix m
 */
/*======================================================================*/
  template<typename DataT, int NRows, int NColumns>
  TinyMatrix<DataT,NRows,NColumns> operator*(
      TinyMatrix<DataT,NRows,NColumns> const &m, DataT const &alpha)
  {
    TinyMatrix<DataT,NRows,NColumns> res;
    for (int i = 0; i < NRows * NColumns; ++i)
    {
      res.data()[i] = m.data()[i] * alpha;
    }
    return res;
  }

/*======================================================================*/
/*! 
 *   Division of a blitz::TinyMatrix by a scalar.
 *
 *   \param m The matrix
 *   \param alpha The scalar divident
 *
 *   \return The result of the scalar division of m by alpha
 */
/*======================================================================*/
  template<typename DataT, int NRows, int NColumns>
  TinyMatrix<DataT,NRows,NColumns> operator/(
      TinyMatrix<DataT,NRows,NColumns> const &m, DataT const &alpha)
  {
    TinyMatrix<DataT,NRows,NColumns> res;
    for (int i = 0; i < NRows * NColumns; ++i)
    {
      res.data()[i] = m.data()[i] / alpha;
    }
    return res;
  }

/*======================================================================*/
/*! 
 *   Subtraction of a blitz::TinyMatrix from another blitz::TinyMatrix.
 *
 *   \param lhs The left-hand-side matrix
 *   \param rhs The right-hand-side matrix
 *
 *   \return The difference lhs - rhs
 */
/*======================================================================*/
  template<typename DataT, int NRows, int NColumns>
  TinyMatrix<DataT,NRows,NColumns> operator-(
      TinyMatrix<DataT,NRows,NColumns> const &lhs,
      TinyMatrix<DataT,NRows,NColumns> const &rhs)
  {
    TinyMatrix<DataT,NRows,NColumns> res;
    for (int i = 0; i < NRows * NColumns; ++i)
    {
      res.data()[i] = lhs.data()[i] - rhs.data()[i];
    }
    return res;
  }

/*======================================================================*/
/*! 
 *   Addition of a blitz::TinyMatrix to another blitz::TinyMatrix.
 *
 *   \param lhs The left-hand-side matrix
 *   \param rhs The right-hand-side matrix
 *
 *   \return The sum lhs + rhs
 */
/*======================================================================*/
  template<typename DataT, int NRows, int NColumns>
  TinyMatrix<DataT,NRows,NColumns> operator+(
      TinyMatrix<DataT,NRows,NColumns> const &lhs,
      TinyMatrix<DataT,NRows,NColumns> const &rhs)
  {
    TinyMatrix<DataT,NRows,NColumns> res;
    for (int i = 0; i < NRows * NColumns; ++i)
    {
      res.data()[i] = lhs.data()[i] + rhs.data()[i];
    }
    return res;
  }

/*======================================================================*/
/*! 
 *   Elementwise comparison for equality of two blitz::TinyMatrices.
 *
 *   \param lhs The left-hand-side matrix
 *   \param rhs The right-hand-side matrix
 *
 *   \return The boolean lhs == rhs
 */
/*======================================================================*/
  template<typename DataT, int NRows, int NColumns>
  TinyMatrix<bool,NRows,NColumns> operator==(
      TinyMatrix<DataT,NRows,NColumns> const &lhs,
      TinyMatrix<DataT,NRows,NColumns> const &rhs)
  {
    TinyMatrix<bool,NRows,NColumns> res;
    for (int i = 0; i < NRows * NColumns; ++i)
        res.data()[i] = (lhs.data()[i] == rhs.data()[i]);
    return res;
  }

/*======================================================================*/
/*! 
 *   Elementwise comparison for inequality of two blitz::TinyMatrices.
 *
 *   \param lhs The left-hand-side matrix
 *   \param rhs The right-hand-side matrix
 *
 *   \return The boolean lhs != rhs
 */
/*======================================================================*/
  template<typename DataT, int NRows, int NColumns>
  TinyMatrix<bool,NRows,NColumns> operator!=(
      TinyMatrix<DataT,NRows,NColumns> const &lhs,
      TinyMatrix<DataT,NRows,NColumns> const &rhs)
  {
    TinyMatrix<bool,NRows,NColumns> res;
    for (int i = 0; i < NRows * NColumns; ++i)
        res.data()[i] = (lhs.data()[i] != rhs.data()[i]);
    return res;
  }

/*======================================================================*/
/*! 
 *   Elementwise comparison of two blitz::TinyMatrices using the less
 *   than operator.
 *
 *   \param lhs The left-hand-side matrix
 *   \param rhs The right-hand-side matrix
 *
 *   \return The boolean lhs < rhs
 */
/*======================================================================*/
  template<typename DataT, int NRows, int NColumns>
  TinyMatrix<bool,NRows,NColumns> operator<(
      TinyMatrix<DataT,NRows,NColumns> const &lhs,
      TinyMatrix<DataT,NRows,NColumns> const &rhs)
  {
    TinyMatrix<bool,NRows,NColumns> res;
    for (int i = 0; i < NRows * NColumns; ++i)
        res.data()[i] = (lhs.data()[i] < rhs.data()[i]);
    return res;
  }

/*======================================================================*/
/*! 
 *   Elementwise comparison of two blitz::TinyMatrices using the less
 *   than or equals operator.
 *
 *   \param lhs The left-hand-side matrix
 *   \param rhs The right-hand-side matrix
 *
 *   \return The boolean lhs <= rhs
 */
/*======================================================================*/
  template<typename DataT, int NRows, int NColumns>
  TinyMatrix<bool,NRows,NColumns> operator<=(
      TinyMatrix<DataT,NRows,NColumns> const &lhs,
      TinyMatrix<DataT,NRows,NColumns> const &rhs)
  {
    TinyMatrix<bool,NRows,NColumns> res;
    for (int i = 0; i < NRows * NColumns; ++i)
        res.data()[i] = (lhs.data()[i] <= rhs.data()[i]);
    return res;
  }

/*======================================================================*/
/*! 
 *   Elementwise comparison of two blitz::TinyMatrices using the greater
 *   than operator.
 *
 *   \param lhs The left-hand-side matrix
 *   \param rhs The right-hand-side matrix
 *
 *   \return The boolean lhs > rhs
 */
/*======================================================================*/
  template<typename DataT, int NRows, int NColumns>
  TinyMatrix<bool,NRows,NColumns> operator>(
      TinyMatrix<DataT,NRows,NColumns> const &lhs,
      TinyMatrix<DataT,NRows,NColumns> const &rhs)
  {
    TinyMatrix<bool,NRows,NColumns> res;
    for (int i = 0; i < NRows * NColumns; ++i)
        res.data()[i] = (lhs.data()[i] > rhs.data()[i]);
    return res;
  }

/*======================================================================*/
/*! 
 *   Elementwise comparison of two blitz::TinyMatrices using the greater
 *   than or equals operator.
 *
 *   \param lhs The left-hand-side matrix
 *   \param rhs The right-hand-side matrix
 *
 *   \return The boolean lhs >= rhs
 */
/*======================================================================*/
  template<typename DataT, int NRows, int NColumns>
  TinyMatrix<bool,NRows,NColumns> operator>=(
      TinyMatrix<DataT,NRows,NColumns> const &lhs,
      TinyMatrix<DataT,NRows,NColumns> const &rhs)
  {
    TinyMatrix<bool,NRows,NColumns> res;
    for (int i = 0; i < NRows * NColumns; ++i)
        res.data()[i] = (lhs.data()[i] >= rhs.data()[i]);
    return res;
  }

/*======================================================================*/
/*! 
 *   all() reduction for boolean blitz::TinyMatrix. This function returns
 *   true, if all elements of the given matrix are true.
 *
 *   \param matrix The matrix to check
 *
 *   \return \c true if all elements of the given matrix are true, \c false
 *     otherwise
 */
/*======================================================================*/
  template<int NRows, int NColumns>
  bool all(blitz::TinyMatrix<bool,NRows,NColumns> const &matrix)
  {
    for (int i = 0; i < NRows * NColumns; ++i)
        if (!matrix.data()[i]) return false;
    return true;
  }

/*======================================================================*/
/*! 
 *   any() reduction for boolean blitz::TinyMatrix. This function returns
 *   true, if any element of the given matrix is true.
 *
 *   \param matrix The matrix to check
 *
 *   \return \c true if any element of the given matrix is true, \c false
 *     otherwise
 */
/*======================================================================*/
  template<int NRows, int NColumns>
  bool any(blitz::TinyMatrix<bool,NRows,NColumns> const &matrix)
  {
    for (int i = 0; i < NRows * NColumns; ++i)
        if (matrix.data()[i]) return true;
    return false;
  }

}

#endif
