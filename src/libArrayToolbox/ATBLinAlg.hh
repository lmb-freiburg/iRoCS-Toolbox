#ifndef ATBLINALG_HH
#define ATBLINALG_HH

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_permute_vector.h>
#include <gsl/gsl_sort_vector.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_permutation.h>
#include "ATBGSLWrapper.hh"
#include "Quaternion.hh"

#if defined HAVE_CONFIG_H && !defined PACKAGE
#include <config.hh>
#endif

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

namespace atb
{
  
  enum SortingType { None, Ascending, Descending, AbsAscending, AbsDescending };

/*======================================================================*/
/*! 
 *   Compute the value of expression \f$\frac{n!}{k!}\f$. Instead of
 *   explicitely computing the factorials, they are first cancelled
 *   out against each other as far as possible, resulting in the following
 *   implementation:
 *   \f[\frac{n!}{k!} = \prod_{i=k+1}^{n} i\f]
 *
 *   \param n Argument of the factorial of the numerator
 *   \param k Argument of the factorial of the denominator
 *
 *   \return \f$\frac{n!}{k!}\f$
 */
/*======================================================================*/
  double factorialFraction(int n, int k);
  
/*======================================================================*/
/*! 
 *   Strictly positive modulo, to compute valid indices for Array access
 *   modulo Array size.
 *   \f[(a \;\mathrm{mod}\; b) \in \mathds{N}\f]
 *   The given and returned integers are ptrdiff_t, because that's
 *   the index type of blitz Arrays, but the function will bail
 *   out if a negative modulus is given, and will only return
 *   positive integers.
 *
 *   \param a An integer (lefthandside of the modulo operator)
 *   \param b A positive integer (The base)
 *
 *   \return \f$(a \;\mathrm{mod}\; b) \in \mathds{N}\f$
 */
/*======================================================================*/
  ptrdiff_t modulo(ptrdiff_t a, ptrdiff_t b);
  
/*======================================================================*/
/*! 
 *   Strictly positive vectorial modulo, to compute valid indices for
 *   Array access modulo Array size.
 *   \f[\forall i \in \{1, \ldots, \mathds{Dim}\}:
 *               (a_i \;\mathrm{mod}\; b_i) \in \mathds{N}\f]
 *   The given and returned integers are ptrdiff_t, because that's
 *   the index type of blitz Arrays, but the function will bail
 *   out if a negative modulus is given, and will only return
 *   positive integers.
 *
 *   \param a An integer-Vector (lefthandside of the modulo operator)
 *   \param b A positive integer-Vector (The base)
 *
 *   \return \f$(a \;\mathrm{mod}\; b) \in \mathds{N}\f$
 */
/*======================================================================*/
  template<int Dim>
  blitz::TinyVector<ptrdiff_t,Dim> modulo(
      blitz::TinyVector<ptrdiff_t,Dim> const &a,
      blitz::TinyVector<ptrdiff_t,Dim> const &b);

/*======================================================================*/
/*! 
 *   Convert the given vector to homogeneous coordinates
 *
 *   \param pos Euclidean vector to convert
 *
 *   \return The homogeneous representation of the given vector
 */
/*======================================================================*/ 
  template<typename DataT,int Dim>
  blitz::TinyVector<DataT,Dim+1>
  euclideanToHomogeneous(blitz::TinyVector<DataT,Dim> const &pos);
  
/*======================================================================*/
/*! 
 *   Convert the given homogeneous vector to euclidean coordinates
 *
 *   \param pos Homogeneous vector to convert
 *
 *   \return The equivalent euclidean representation of the vector
 */
/*======================================================================*/ 
  template<typename DataT,int Dim>
  blitz::TinyVector<DataT,Dim-1>
  homogeneousToEuclidean(blitz::TinyVector<DataT,Dim> const &pos);
  
/*======================================================================*/
/*! 
 *   Get the spherical (theta, phi, r) coordinates of the given euclidean
 *   point.
 *
 *   \param eucl    The euclidean coordinates (z, y, x) of the point to
 *                  transform
 *   \param trafo   A homogeneous transformation to apply to the
 *                  euclidean position prior to spherical transformation.
 *                  Basically the transform should consist of a translation
 *                  to fix the sphere center and a rotation to define the
 *                  sphere orientation and zero meridian.
 *
 *   \return        The (theta, phi, r) vector corresponding to the
 *                  euclidean point
 */
/*======================================================================*/
  blitz::TinyVector<double,3>
  euclideanToSpherical(blitz::TinyVector<double,3> const &eucl,
                       blitz::Array<double,2> const &trafo);

/*======================================================================*/
/*! 
 *   Get the spherical (theta, phi, r) coordinates of the given euclidean
 *   point.
 *
 *   \param eucl    The euclidean coordinates (z, y, x) of the point to
 *                  transform
 *
 *   \return        The (theta, phi, r) vector corresponding to the
 *                  euclidean point
 */
/*======================================================================*/
  blitz::TinyVector<double,3>
  euclideanToSpherical(blitz::TinyVector<double,3> const &eucl);

/*======================================================================*/
/*! 
 *   Get the euclidean (z, y, x)  coordinates of the given point on the
 *   2-sphere.
 *
 *   \param spherical  The spherical coordinates (theta, phi, r) of the
 *                     point to transform
 *   \param trafo      A homogeneous transformation to apply the inverse to
 *                     the euclidean position after spherical
 *                     back-transformation. Basically the transform should
 *                     consist of a translation to fix the sphere center
 *                     and a rotation to define the sphere orientation and
 *                     zero meridian.
 *
 *   \return           The euclidean (z, y, x) vector corresponding to the
 *                     spherical point relative to the given sphere parameters
 */
/*======================================================================*/
  blitz::TinyVector<double,3>
  sphericalToEuclidean(blitz::TinyVector<double,3> const &spherical,
                       blitz::Array<double,2> const &trafo);

/*======================================================================*/
/*! 
 *   Get the euclidean (z, y, x)  coordinates of the given point on the
 *   2-sphere.
 *
 *   \param spherical  The spherical coordinates (theta, phi, r) of the
 *                     point to transform
 *
 *   \return           The euclidean (z, y, x) vector corresponding to the
 *                     spherical point
 */
/*======================================================================*/
  blitz::TinyVector<double,3>
  sphericalToEuclidean(blitz::TinyVector<double,3> const &spherical);

/*======================================================================*/
/*! 
 *   Rotate the given 3-D vector by a specified angle angle around the
 *   given axis
 *
 *   \param vec   The vector to rotate
 *   \param axis  The rotation axis
 *   \param angle The rotation angle in radians
 *
 *   \return The rotated vector
 */
/*======================================================================*/
  template<typename DataT>
  blitz::TinyVector<DataT,3>
  rotate(blitz::TinyVector<DataT,3> const &vec,
         blitz::TinyVector<DataT,3> const &axis,
         DataT angle);

/*======================================================================*/
/*! 
 *   Calculation of the inner product (dot product) of the given
 *   vectors
 *
 *   \param vec1 Left side vector
 *   \param vec2 Right side vector
 *
 *   \return The dot product of vec1 and vec2
 */
/*======================================================================*/
  template<typename DataT>
  double dot(blitz::Array<DataT,1> const &vec1,
             blitz::Array<DataT,1> const &vec2);

/*======================================================================*/
/*! 
 *   Calculation of the outer product of two vectors.
 *
 *   \param vec1 Left hand side vector
 *   \param vec2 Right hand side vector
 *
 *   \return The matrix containing the outer product of vec1 and vec2
 */
/*======================================================================*/
  template<typename DataT, int Dim1, int Dim2>
  blitz::TinyMatrix<DataT,Dim1,Dim2>
  outerProduct(blitz::TinyVector<DataT,Dim1> const &vec1,
               blitz::TinyVector<DataT,Dim2> const &vec2);
  
/*======================================================================*/
/*! 
 *   Compute the transpose of the given Matrix.
 *
 *   \param in  The Matrix to transpose
 *
 *   \return The transpose of in
 */
/*======================================================================*/
  template<typename DataT, int Dim1, int Dim2>
  blitz::TinyMatrix<DataT,Dim2,Dim1>
  transpose(blitz::TinyMatrix<DataT,Dim1,Dim2> const &in);
      
/*======================================================================*/
/*! 
 *   Compute the transpose of the given Matrix. Make sure to resize the
 *   matrix to store the transpose to appropriately before using this
 *   function.
 *
 *   \param in  The Matrix to transpose
 *
 *   \return The transpose of in
 */
/*======================================================================*/
  template<typename DataT>
  blitz::Array<DataT,2> transpose(blitz::Array<DataT,2> const &in);
      
/*======================================================================*/
/*! 
 *   Quadratic Matrix * Vector product (generic). Warning matrix bounds
 *   are not checked, and the Matrix class must overload the
 *   operator()(int, int) for indexing the Matrix entries
 *
 *   \param m The left hand side quadratic matrix
 *   \param v The right hand side vector (is handled as column vector)
 *
 *   \return The resulting vector from the product
 */
/*======================================================================*/
  template<typename MatrixT, typename BaseT, int Dim>
  blitz::TinyVector<BaseT,Dim>
  mvMult(MatrixT const &m, blitz::TinyVector<BaseT,Dim> const &v);

/*======================================================================*/
/*! 
 *   Quadratic Matrix * Vector product
 *
 *   \param m The left hand side quaratic matrix
 *   \param v The right hand side vector (is handled as column vector)
 *
 *   \return The resulting vector from the product
 */
/*======================================================================*/
  template<typename BaseT, int Dim>
  blitz::TinyVector<BaseT,Dim>
  mvMult(blitz::Array<BaseT,2> const &m, blitz::TinyVector<BaseT,Dim> const &v);

/*======================================================================*/
/*! 
 *   Matrix * Vector product. Make sure to resize the
 *   vector to store the result to appropriately before using this
 *   function.
 *
 *   \param m The left hand side matrix
 *   \param v The right hand side vector (is handled as column vector)
 *
 *   \return The resulting vector from the product
 */
/*======================================================================*/
  template<typename BaseT>
  blitz::Array<BaseT,1>
  mvMult(blitz::Array<BaseT,2> const &m, blitz::Array<BaseT,1> const &v);

/*======================================================================*/
/*! 
 *   Naive matrix multiplication A * B = C. Make sure to resize the
 *   matrix to store the result appropriately before using this
 *   function.
 *
 *   \param A  left hand side matrix
 *   \param B  right hand side matrix
 *
 *   \return The matrix product A * B
 */
/*======================================================================*/
  template<typename DataT>
  blitz::Array<DataT,2>
  mmMult(blitz::Array<DataT,2> const &A, blitz::Array<DataT,2> const &B);

/*======================================================================*/
/*! 
 *   Naive matrix multiplication A * B = C.
 *
 *   \param A  left hand side matrix
 *   \param B  right hand side matrix
 *
 *   \return The matrix product A * B
 */
/*======================================================================*/
  template<typename DataT, int Dim1, int Dim2, int Dim3>
  blitz::TinyMatrix<DataT,Dim1,Dim3>
  mmMult(blitz::TinyMatrix<DataT,Dim1,Dim2> const &A,
         blitz::TinyMatrix<DataT,Dim2,Dim3> const &B);

/*======================================================================*/
/*! 
 *   Compute the inverse of matrix A
 *
 *   \param A     The matrix to compute the inverse of
 *   \param Ainv  The inverse of matrix A
 */
/*======================================================================*/
  void invert(blitz::Array<double,2> const &A, blitz::Array<double,2> &Ainv);

/*======================================================================*/
/*! 
 *   Compute the inverse of matrix A
 *
 *   \param A     The matrix to compute the inverse of
 *
 *   \return      The inverse of matrix A (make sure that the Array
 *                you assign the inverse to is resized properly
 */
/*======================================================================*/
  blitz::Array<double,2>
  invert(blitz::Array<double,2> const &A);

/*======================================================================*/
/*! 
 *   Compute the inverse of matrix A
 *
 *   \param A     The matrix to compute the inverse of
 *   \return      The inverse of matrix A
 */
/*======================================================================*/
  template<int Dim>
  blitz::TinyMatrix<double,Dim,Dim>
  invert(blitz::TinyMatrix<double,Dim,Dim> const &A);

/*======================================================================*/
/*! 
 *   Compute the determinant of matrix A
 *
 *   \param A   The matrix to compute the determinant for
 *
 *   \return The determinant of matrix A
 */
/*======================================================================*/
  template<int Dim>
  double determinant(blitz::TinyMatrix<double,Dim,Dim> const &A);

/*======================================================================*/
/*! 
 *   Computation of the eigen value decomposition of real symmetric
 *   matrices. This version only returns the eigenvalues.
 *
 *   \param A       A real symmetric matrix to compute the eigenvalues and
 *                  vectors of
 *   \param lambda  Vector to hold the computed eigenvalues
 *   \param sort    The eigenvalue sorting strategy
 */
/*======================================================================*/
  template<int Dim>
  void eigenvalueDecompositionRealSymmetric(
      blitz::TinyMatrix<double,Dim,Dim> const &A,
      blitz::TinyVector<double,Dim> &lambda,
      SortingType sort = Descending);

/*======================================================================*/
/*! 
 *   Computates the eigenvectors of the real symmetric matrix A given the
 *   eigenvalues.
 *
 *   \param A       A real symmetric matrix to compute the eigenvalues and
 *                  vectors of
 *   \param U       The matrix of eigenvectors
 *   \param lambda  Vector to hold the computed eigenvalues
 */
/*======================================================================*/
  template<int Dim>
  void computeEigenvectors(
      blitz::TinyMatrix<double,Dim,Dim> const &A,
      blitz::TinyMatrix<double,Dim,Dim> &U,
      blitz::TinyVector<double,Dim> const &lambda);

/*======================================================================*/
/*! 
 *   Computation of the eigen value decomposition of real symmetric
 *   matrices. It is computed using the GSL function gsl_eigen_symmv.
 *
 *   \param A       A real symmetric matrix to compute the eigenvalues and
 *                  vectors of
 *   \param U       Matrix to hold the computed eigenvectors. The eigenvectors
 *                  correspond to the columns of the U-matrix
 *   \param lambda  Vector to hold the computed eigenvalues
 *   \param sort    The eigenvalue sorting strategy
 */
/*======================================================================*/
  template<int Dim>
  void eigenvalueDecompositionRealSymmetric(
      blitz::TinyMatrix<double,Dim,Dim> const &A,
      blitz::TinyMatrix<double,Dim,Dim> &U,
      blitz::TinyVector<double,Dim> &lambda,
      SortingType sort = Descending);

/*======================================================================*/
/*! 
 *   Sort the values of the given TinyVector according to the given
 *   SortingType.
 *
 *   \param v The vector to sort
 *   \param sort The SortingType to apply
 */
/*======================================================================*/
  template<typename DataT>
  void sortElements(blitz::TinyVector<DataT,3> &v, SortingType sort);

/*======================================================================*/
/*! 
 *   Computation of the eigen value decomposition of real symmetric
 *   matrices. It is computed using the GSL function gsl_eigen_symmv.
 *
 *   \param A       A real symmetric matrix to compute the eigenvalue and
 *                  vectors of
 *   \param U       Matrix to hold the computed eigenvectors. The eigenvectors
 *                  correspond to the columns of the U-matrix.
 *   \param lambda  Vector to hold the computed eigenvalues
 *   \param sort    The eigenvalue sorting strategy
 */
/*======================================================================*/
  void eigenvalueDecompositionRealSymmetric(
      blitz::Array<double,2> const &A,
      blitz::Array<double,2> &U, blitz::Array<double,1> &lambda,
      SortingType sort = Descending);

}

#include "ATBLinAlg.icc"

#endif
