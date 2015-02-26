/**************************************************************************
**       Title: ArrayToolbox Polynomial ring implementation
**    $RCSfile$
**   $Revision: 3896 $ $Name$
**       $Date: 2008-07-16 15:17:47 +0200 (Wed, 16 Jul 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This file contains the Polynomial class for computations within
**    a polynomial ring
**
**************************************************************************/

/*======================================================================*/
/*!
 *  \file ATBPolynomial.hh
 *  \brief ATBPolynomial.hh provides the ATB::Polynomial class to store
 *     and do arithmetics within the Polynomial ring.
 */
/*======================================================================*/

#ifndef ATBPOLYNOMIAL_HH
#define ATBPOLYNOMIAL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "TypeTraits.hh"

#include <complex>
#include <vector>
#include <iostream>
#include <cmath>

// #define PROFILE

#ifdef PROFILE
#include "ATBTiming.hh"
#endif

namespace atb
{
/*======================================================================*/
/*!
 *  \brief The Polynomial class provides arithmetical operations with
 *         polynomials as well as a root finding algorithm.
 *
 *  Basic operations with polynomials, like addition, multiplication,
 *  evaluation and root finding are implemented within this class. It
 *  additionally provides a pretty-printer to produce neat verbose polynomial 
 *  expressions from a set of coefficients.
 */
/*======================================================================*/
  template<typename CoeffT>
  class Polynomial 
  {
    
  public:
    
/*======================================================================*/
/*! 
 *   Default constructor. All coefficients will be zero, yielding the
 *   constant zero function.
 */
/*======================================================================*/ 
    Polynomial();

/*======================================================================*/
/*! 
 *   Creates a new polynomial with the given coefficient vector.
 *
 *   \param coeffs  The coefficient vector \f$\vec{c}\f$ to initialize
 *                  the polynomial with. The polynomial will be evaluated
 *                  as follows:
 *                  \f[ p(x) = \sum_{i=0}^p c_i x^i \f]
 *                  where \$p\$ is the polynomial degree.
 */
/*======================================================================*/
    Polynomial(const std::vector<CoeffT>& coeffs);

/*======================================================================*/
/*! 
 *   Copy constructor. The coefficient vector will be trimmed removing
 *   leading zeros, so if you explicitely need to set the highest
 *   coefficients to zero using the read/write accessor, you can remove
 *   them with this constructor with some memory allocation overhead,
 *   though.
 *
 *   \param p  The polynomial to copy
 */
/*======================================================================*/
    Polynomial(const Polynomial<CoeffT>& p);

/*======================================================================*/
/*! 
 *   Destructor
 */
/*======================================================================*/
    ~Polynomial();
    
/*======================================================================*/
/*! 
 *   Copy assignment operator. The coefficient vector will be trimmed
 *   removing leading zeros, so if you explicitely need to set the highest
 *   coefficients to zero using the read/write accessor, you can remove
 *   them with this assignent operator with some memory allocation overhead,
 *   though.
 *
 *   \param p  The polynomial to copy
 */
/*======================================================================*/
    Polynomial<CoeffT> &operator=(const Polynomial<CoeffT>& p);

/*======================================================================*/
/*! 
 *   Retrieve the degree of the polynomial.
 *
 *   \return The polynomial degree
 */
/*======================================================================*/
    size_t degree() const;
    
/*======================================================================*/
/*! 
 *   Read-Only accessor to the i'th polynomial coefficient
 *
 *   \param i The index of the coefficient to retrieve
 *   \return A const reference to the coefficient at index i
 */
/*======================================================================*/
    const CoeffT& coefficient(size_t i) const;

/*======================================================================*/
/*! 
 *   Read/Write accessor to the i'th polynomial coefficient. If an index
 *   above the polynomial degree is requested the coefficient vector
 *   will be resized to fit all coefficients including the requested one.
 *   This may also have an impact on the degree() method, which has to
 *   search the highest non-zero coefficient for degree computation. Avoid
 *   adding leading zero coefficients, if they result from a computation
 *   they will be trimmed, but if you explicitely set them to zero using
 *   this accessor, the class cannot recognize it and degree computation,
 *   which is needed in other functions as well will be slow.
 *
 *   \param i The index of the coefficient to modify
 *   \return A reference to the coefficient at index i
 */
/*======================================================================*/
    CoeffT& coefficient(size_t i);
    
/*======================================================================*/
/*! 
 *   Evaluates the polynomial at position x
 *
 *   \param x The position to evaluate the polynomial at
 *   \return The function value at position x
 */
/*======================================================================*/
    template<typename DomainT>
    CoeffT operator()(DomainT x) const;

/*======================================================================*/
/*! 
 *   Computes and returns the vector of zero crossings of the polynomial.
 *   Any polynomial of degree \$p\$ has \$p\$ complex roots, which are
 *   computed and returned by this function.
 *
 *   \param polish  The found roots will be polished in a second pass to
 *                  increase numerical precision
 *   \param sort    If given the returned roots are ordered by increasing
 *                  real part
 *   \return A vector containing the complex roots of the polynomial
 */
/*======================================================================*/
  std::vector<typename traits<CoeffT>::ComplexT> roots(
      bool polish = true, bool sort = false);
    
/*======================================================================*/
/*! 
 *   Computes the n'th derivative of this polynomial
 *
 *   \param degree The degree of the derivative
 *
 *   \return The n'th derivative polynomial
 */
/*======================================================================*/
    Polynomial<CoeffT> derivative(size_t degree = 1);
    
/*======================================================================*/
/*! 
 *   Computes the n'th indefinite Integral of this polynomial
 *
 *   \param degree the degree of the integral
 *
 *   \return The polynomial obtained after n integrations of this polynomial
 */
/*======================================================================*/
    Polynomial<CoeffT> indefiniteIntegral(size_t degree = 1);

/*======================================================================*/
/*! 
 *   Comparison of the given polynomial with this polynomial.
 *
 *   \param p The polynomial to compare this polynomial with
 *
 *   \return If all coefficients of the polynomials are identical \c true is
 *           returned, \c false otherwise.
 */
/*======================================================================*/
  bool operator==(const Polynomial<CoeffT>& p);

/*======================================================================*/
/*! 
 *   Comparison of the given polynomial with this polynomial.
 *
 *   \param p The polynomial to compare this polynomial with
 *
 *   \return If all coefficients of the polynomials are different \c true is
 *           returned, \c false otherwise.
 */
/*======================================================================*/
  bool operator!=(const Polynomial<CoeffT>& p);

/*======================================================================*/
/*! 
 *   Unary minus operator.
 *
 *   \return A polynomial with all coefficients negated
 */
/*======================================================================*/
    Polynomial<CoeffT> operator-() const;

/*======================================================================*/
/*! 
 *   Plus assignment operator. Adds the given polynomial to this polynomial.
 *
 *   \param p  The polynomial to add to this one
 *   \return A reference to this polynomial for arithmetic chaining
 */
/*======================================================================*/
    Polynomial<CoeffT>& operator+=(const Polynomial<CoeffT>& p);

/*======================================================================*/
/*! 
 *   Minus assignment operator. Subtracts the given polynomial from this
 *   polynomial.
 *
 *   \param p  The polynomial to subtract from this one
 *   \return A reference to this polynomial for arithmetic chaining
 */
/*======================================================================*/
    Polynomial<CoeffT>& operator-=(const Polynomial<CoeffT>& p);

/*======================================================================*/
/*! 
 *   Multiplication assignment operator. Multiplies the given polynomial
 *   to this polynomial.
 *
 *   \param p  The polynomial to multiply to this one
 *   \return A reference to this polynomial for arithmetic chaining
 */
/*======================================================================*/
    Polynomial<CoeffT>& operator*=(const Polynomial<CoeffT>& p);

/*======================================================================*/
/*! 
 *   Multiplication assignment operator. Multiplies the given scalar value
 *   to this polynomial.
 *
 *   \param alpha  The scalar to multiply to this polynomial
 *   \return A reference to this polynomial for arithmetic chaining
 */
/*======================================================================*/
    Polynomial<CoeffT>& operator*=(const CoeffT& alpha);

/*======================================================================*/
/*! 
 *   Division assignment operator. Divides this polynomial by the given
 *   scalar value.Multiplies the given scalar value.
 *
 *   \param alpha  The scalar to divide this polynomial by
 *   \return A reference to this polynomial for arithmetic chaining
 */
/*======================================================================*/
    Polynomial<CoeffT>& operator/=(const CoeffT& alpha);

/*======================================================================*/
/*! 
 *   Get the neutral element for addition of the Polynomial ring.
 *
 *   \return The polynomial with all coefficients being zero
 */
/*======================================================================*/
    static Polynomial<CoeffT> const &zero();

/*======================================================================*/
/*! 
 *   Get the neutral element for multiplication of the Polynomial ring.
 *
 *   \return The polynomial with all coefficients equalling zero except
 *      for the zeroth equalling one
 */
/*======================================================================*/
    static Polynomial<CoeffT> const &one();

/*======================================================================*/
/*! 
 *   Get the normalized Hermite polynomial of given degree.
 *
 *   The normalized Hermite polynomials multiplied with a normal distribution
 *   result in the derivative of the normal distribution of corresponding
 *   degree. The normalized Hermite polynomials are recursively defined as
 *   follows:
 *
 *   \f[ H_n^{\sigma}\left( x \right) := \left\{
       \begin{array}{l@{\quad}l}
         0 & n < 0 \\
         1 & n = 0 \\
         \frac{1}{\sigma^2} x H_{n-1}^{\sigma}\left( x \right) -
         \frac{1}{\sigma^2} (n - 1) H_{n-2}^{\sigma}\left( x \right)
         & \textrm{otherwise}
       \end{array} \right. \f]
 *
 *   \param n The polynomial degree
 *   \param sigma The Standard deviation of the normal distribution to compute
 *      the derivative for
 *
 *   \return The normalized Hermite polynomial of degree n
 */
/*======================================================================*/
    static Polynomial<CoeffT> hermite(
        int n, double sigma = 1.0 / std::sqrt(2.0));

  private:
    
    void _trimCoefficients();

    static std::complex<double>& _laguer(
        const std::vector< std::complex<double> >& a,
        std::complex<double>& x);

    std::vector<CoeffT> _coeffs;
    mutable size_t _degree;
    mutable bool _degreeCached;

    static CoeffT const _zeroCoeff;

    static Polynomial<CoeffT> const _zero;
    static Polynomial<CoeffT> const _one;

#ifdef PROFILE
    mutable ptrdiff_t __p1, __p2, __p3;
    mutable ptrdiff_t __n1, __n2, __n3;
#endif

  }; 
  
/*======================================================================*/
/*! 
 *   Plus operator. Adds the given polynomials.
 *
 *   \param p1  The left-hand-side polynomial
 *   \param p2  The right-hand-side polynomial
 *   \return p1 + p2
 */
/*======================================================================*/
  template<typename CoeffT>
  Polynomial<CoeffT> operator+(const Polynomial<CoeffT>& p1,
                              const Polynomial<CoeffT>& p2);

/*======================================================================*/
/*! 
 *   Minus operator. Subtracts p2 from p1.
 *
 *   \param p1  The left-hand-side polynomial
 *   \param p2  The right-hand-side polynomial
 *   \return p1 - p2
 */
/*======================================================================*/
  template<typename CoeffT>
  Polynomial<CoeffT> operator-(const Polynomial<CoeffT>& p1,
                              const Polynomial<CoeffT>& p2);

/*======================================================================*/
/*! 
 *   Multiplication operator. Multiplies p1 and p2.
 *
 *   \param p1  The left-hand-side polynomial
 *   \param p2  The right-hand-side polynomial
 *   \return p1 * p2
 */
/*======================================================================*/
  template<typename CoeffT>
  Polynomial<CoeffT> operator*(const Polynomial<CoeffT>& p1,
                              const Polynomial<CoeffT>& p2);

/*======================================================================*/
/*! 
 *   Power operator. Computes the exponential's power of the polynomial.
 *
 *   \param p            The polynomial
 *   \param exponential  The exponent 
 *   \return \f$\mathrm{p}^{\mathrm{exponential}}\f$
 */
/*======================================================================*/
  template<typename CoeffT>
  Polynomial<CoeffT> pow(const Polynomial<CoeffT>& p, int exponential);

/*======================================================================*/
/*! 
 *   Multiplication operator. Multiplies p and alpha.
 *
 *   \param p      A polynomial
 *   \param alpha  A scalar value
 *   \return p * alpha
 */
/*======================================================================*/
  template<typename CoeffT>
  Polynomial<CoeffT> operator*(const Polynomial<CoeffT>& p,
                              const CoeffT& alpha);

/*======================================================================*/
/*! 
 *   Multiplication operator. Multiplies p and alpha.
 *
 *   \param alpha  A scalar value
 *   \param p      A polynomial
 *   \return alpha * p
 */
/*======================================================================*/
  template<typename CoeffT>
  Polynomial<CoeffT> operator*(const CoeffT& alpha,
                              const Polynomial<CoeffT>& p);

/*======================================================================*/
/*! 
 *   Division operator. Divides p by alpha.
 *
 *   \param p      A polynomial
 *   \param alpha  A scalar value
 *   \return p / alpha
 */
/*======================================================================*/
  template<typename CoeffT>
  Polynomial<CoeffT> operator/(const Polynomial<CoeffT>& p,
                              const CoeffT& alpha);

/*======================================================================*/
/*! 
 *   Pretty printer.
 *
 *   \param os  The stream to output the polynomial to
 *   \param p   The polynomial to write to the stream
 *
 *   \return A reference to the output stream for chaining
 */
/*======================================================================*/
  template<typename CoeffT>
  std::ostream& operator<<(std::ostream& os, const Polynomial<CoeffT>& p);

}

#include "ATBPolynomial.icc"

#endif
