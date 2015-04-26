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

#ifndef ATBSPLINE_HH
#define ATBSPLINE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <cstdlib>
#include <vector>
#include <map>
#include <omp.h>

#include <blitz/array.h>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

#include "ATBPolynomial.hh"
#include "RuntimeError.hh"
#include "ATBLinAlg.hh"

// #define ATBSPLINE_DEBUG
// #define ATBSPLINE_PROFILE

#ifdef ATBSPLINE_PROFILE
#include "ATBTiming.hh"
#endif

namespace atb
{

/*======================================================================*/
/*!
 *  \brief The BSpline class provides functions for fitting B-Splines to
 *         point clouds and evaluating them at arbitrary curve position u.
 *         It also supports computation of derivatives and integrals of the
 *         base functions as well as derivatives and line integrals along
 *         the full spline.
 *
 *  Within the B-Spline class a Cache for all basis function evaluations
 *  is maintained to speed up the computations. Changing the number of
 *  control points or the spline degree invalidates the cache and will
 *  immediately clear all cache entries.
 */
/*======================================================================*/
  template<typename ControlPointT>
  class BSpline
  {
    
  public:
    
/*======================================================================*/
/*!
 *  \brief The BaseCacheIndex struct provides a sortable quadrupel for
 *         uniquely identifying the evaluation of the lth indefinite
 *         integral of jth B-spline basis function of degree p at position u.
 */
/*======================================================================*/
    struct BaseCacheIndex
    {
      BaseCacheIndex(double u, size_t index, int degree, int derivative);
      BaseCacheIndex(BaseCacheIndex const &idx);
      bool operator<(BaseCacheIndex const &i) const;
      
      double u;
      size_t j;
      int p, l;
    };
    
/*======================================================================*/
/*!
 *  \brief The BasePolynomialCacheIndex struct provides a sortable quadrupel
 *         to uniquely identify the polynomial of the lth indefinite
 *         integral of the jth B-spline basis function of degree p at
 *         segment m.
 */
/*======================================================================*/
    struct BasePolynomialCacheIndex
    {
      BasePolynomialCacheIndex(
          size_t segment, size_t index, int degree, int derivative);
      BasePolynomialCacheIndex(BasePolynomialCacheIndex const &idx);
      bool operator<(BasePolynomialCacheIndex const &i) const;
      
      size_t m, j;
      int p, l;
    };
    
/*======================================================================*/
/*! 
 *   Default constructor. This constructor generates a B-Spline object
 *   with given degree \f$p\f$ and the specified number of control points
 *   \f$m\f$. The knot vector will be initialized according to those
 *   parameters with a length of \f$m+p+1\f$ knots.
 *
 *   \param degree            The spline degree \f$p\f$
 *   \param nControlPoints    The number of control points \f$m\f$
 */
/*======================================================================*/
    BSpline(int degree = 3, size_t nControlPoints = 0);

/*======================================================================*/
/*! 
 *   This constructor generates a B-Spline object with given degree
 *   \f$p\f$ and the specified control point vector of length
 *   \f$m\f$. The knot vector will be initialized according to those
 *   parameters with a length of \f$m+p+1\f$ knots. To really do evaluations
 *   the knot-vector needs to be filled with useful values before.
 *
 *   \param degree            The spline degree \f$p\f$
 *   \param controlPoints     The control point vector of length \f$m\f$
 */
/*======================================================================*/
    BSpline(int degree, std::vector<ControlPointT> const &controlPoints);

/*======================================================================*/
/*! 
 *   Copy constructor.
 *
 *   This constructor creates a deep copy of a spline including the cache.
 *
 *   \param spline The spline object to copy
 */
/*======================================================================*/
    BSpline(BSpline<ControlPointT> const &spline);

/*======================================================================*/
/*! 
 *   Destructor
 */
/*======================================================================*/
    ~BSpline();
    
/*======================================================================*/
/*! 
 *   Copy assignment operator.
 *
 *   This operator deep copies the given spline including the cache.
 *
 *   \param spline The spline object to copy
 */
/*======================================================================*/
    BSpline<ControlPointT> &operator=(BSpline<ControlPointT> const &spline);

/*======================================================================*/
/*! 
 *   Returns the B-Spline degree
 *
 *   \return degree of the B-Spline
 */
/*======================================================================*/
    int degree() const;

/*======================================================================*/
/*! 
 *   Set the degree of the B-Spline. This implies reinitialization of
 *   the knot vector and clearing the basis function Cache.
 *
 *   \param degree The new B-Spline degree
 */
/*======================================================================*/ 
    void setDegree(int degree);
    
/*======================================================================*/
/*! 
 *   Returns the number of control points of the B-Spline.
 *
 *   \return number of B-Spline control points
 */
/*======================================================================*/
    size_t nControlPoints() const;

/*======================================================================*/
/*! 
 *   Set the number of control points of the B-Spline. This implies
 *   resizing of the knot vector and clearing the basis function
 *   cache.
 *
 *   \param nControlPoints The new number of control points
 */
/*======================================================================*/ 
    void setNControlPoints(size_t nControlPoints);

/*======================================================================*/
/*! 
 *   Replace the current control points by the provided ones. If the
 *   number of control points changes that way, the basis function cache
 *   will be resetted, leading to a reset of the knot vector to open uniform
 *   behaviour.
 *
 *   \param controlPoints   The control points to define the spline shape
 */
/*======================================================================*/
    void setControlPoints(std::vector<ControlPointT> const &controlPoints);

/*======================================================================*/
/*! 
 *   Read accessor to the control point vector of the B-Spline
 *
 *   \return The B-Spline's control point vector
 */
/*======================================================================*/
    std::vector<ControlPointT> const &controlPoints() const;

/*======================================================================*/
/*! 
 *   Read the jth control point.
 *
 *   \param index  The index of the control point to read.
 *
 *   \return A reference to the control point (in the scalar case only a
 *           double)
 */
/*======================================================================*/
    ControlPointT const &controlPoint(size_t index) const;

/*======================================================================*/
/*! 
 *   Set the ControlPoint at the specified index.
 *
 *   \param index  The index of the control point to set.
 *   \param point The control point position
 */
/*======================================================================*/
    void setControlPoint(size_t index, ControlPointT const &point);
    
/*======================================================================*/
/*! 
 *   Returns the number of knots of the B-Spline. The number of knots is
 *   computed from the number of control points and the degree of the spline,
 *   and is thus read-only, and only a convenience-function.
 *
 *   \return number of B-Spline knots
 */
/*======================================================================*/
    size_t nKnots() const;

/*======================================================================*/
/*! 
 *   With this function the knot Array can be set manually. This has to
 *   be done after setting the degree and number of control points. A
 *   change in either the degree or the number of control points resets
 *   the knot vector to behave like an open uniform B-Spline of the
 *   corresponding degree. So make sure, that this function is the last
 *   step in B-Spline preparation!
 *
 *   \param knots  The knot vector to be set
 *
 *   \throw ArrayToolsError If the knot vector has the wrong length or
 *      is not monotoneous this error is thrown
 */
/*======================================================================*/
    void setKnots(std::vector<double> const &knots);

/*======================================================================*/
/*! 
 *   Read accessor to the k'th knot of the B-Spline
 *
 *   \param index The knot index
 *
 *   \return The k'th knot value
 */
/*======================================================================*/
    double knot(size_t index) const;

/*======================================================================*/
/*! 
 *   Read accessor to the knot vector of the B-Spline
 *
 *   \return The B-Spline's knot vector
 */
/*======================================================================*/
    std::vector<double> const &knots() const;

/*======================================================================*/
/*! 
 *   Set the value of the knot at the specified index. If the value would
 *   break the monotonicity of the knot vector the knot vector will not be
 *   changed and an ArrayToolsError is thrown.
 *
 *   \param index The knot index
 *   \param value The knot value
 *
 *   \throw ArrayToolsError If the monotonicity invariant would be broken
 *     after the update this error is thrown
 */
/*======================================================================*/
    void setKnot(size_t index, double value);

/*======================================================================*/
/*! 
 *   Generation of an open uniform B-Spline knot configuration.
 *   This implies clearing the basis function cache.
 *
 *   \param uMin    The minimum spline parameter value
 *   \param uMax    The maximum spline parameter value
 */
/*======================================================================*/
    void setOpenUniformKnots(double uMin = 0.0, double uMax = 1.0);
    
/*======================================================================*/
/*! 
 *   Evalation of the B-Spline basis function index at position u along the
 *   curve. You can also evaluate derivatives and integrals of the spline
 *   by selecting the degree of the indefinte integral as third parameter.
 *
 *   \param u           The position along the curve to evaluate the
 *                      B-Spline basis function at.
 *   \param index       The index of the basis function to evaluate.
 *   \param derivative  The degree of the indefinite integral to evaluate.
 *                      Zero gives the original spline basis function
 *                      while positive values give the corresponding
 *                      integral and negative values the
 *                      corresponding derivative.
 *
 *   \return            The B-Spline basis function evaluation at position u
 */
/*======================================================================*/
    double base(double u, size_t index, int derivative = 0) const;

/*======================================================================*/
/*! 
 *   Evaluation of the (derivative/integral) value of a B-Spline basis
 *   function. Index, position, spline degree and derivative degree are
 *   passed via the BaseCacheIndex struct i.
 *
 *   \param i           The B-Spline basis evaluation parameters.
 *                      \see BaseCacheIndex
 *
 *   \return            The B-Spline basis function evaluation
 */
/*======================================================================*/
    double base(BaseCacheIndex const &i) const;

/*======================================================================*/
/*! 
 *   Get the polynomial representation of the (ell'th derivative/integral)
 *   of the j'th basis function on the m'th B-Spline segment. Warning: The
 *   integral is not the integral of the basis function, it is the
 *   integral of the polynomial, that makes up the basis function in segment
 *   m. To obtain the integral of the basis function add the contributions
 *   of the polynomial pieces for this basis function over all segments.
 *
 *   \param index       The index j of the base function to get the
 *                      polynomial for
 *   \param segment     The segment m
 *   \param derivative  The degree of the indefinite integral to compute.
 *                      Zero gives the polynomial representation of the
 *                      original spline basis function
 *                      while positive values give the corresponding
 *                      integral and negative values the
 *                      corresponding derivative.
 *
 *   \return  The polynomial representation of the j'th basis function
 *            on the m'th segment.
 */
/*======================================================================*/
    Polynomial<double> const &basePolynomial(
        size_t segment, size_t index, int derivative = 0) const;

/*======================================================================*/
/*! 
 *   Get the polynomial representation of the (ell'th derivative/integral)
 *   of the j'th basis function on the m'th B-Spline segment.
 *
 *   \param idx         The B-Spline basis polynomial parameters.
 *                      \see BasePolynomialCacheIndex
 *
 *   \return  The polynomial representation of the j'th basis function
 *            on the m'th segment.
 */
/*======================================================================*/
    Polynomial<double> const &basePolynomial(
        BasePolynomialCacheIndex const &idx) const;

/*======================================================================*/
/*! 
 *   Get a string representation of the (ell'th derivative/integral)
 *   of the j'th basis function polynomial on the m'th B-Spline segment.
 *
 *   \param idx         The B-Spline basis polynomial parameters.
 *                      \see BasePolynomialCacheIndex
 *
 *   \return  The string representation of the j'th basis function polynomial
 *            on the m'th segment.
 */
/*======================================================================*/
    std::string printBasePolynomial(BasePolynomialCacheIndex idx) const;

/*======================================================================*/
/*! 
 *   Compute the index of the spline segment, that corresponds to the
 *   curve position u. If the position given is not within the knot range
 *   the number of knots is returned. Make sure to check this value before
 *   using it as index for accessing the corresponding knots, otherwise a
 *   segmentation fault is very likely to happen or even worse no
 *   segmentation fault will occur and you overwrite foreign memory.
 *
 *   \param u           The position along the curve
 *
 *   \return The spline segment index, or the number of knots of the spline
 *           (one behind the last segment) if u is not in the knot range
 */
/*======================================================================*/
    size_t curvePositionToSegment(double u) const;
    
/*======================================================================*/
/*! 
 *   Compute the index of the spline segment, that corresponds to the
 *   curve position u. If the position given is not within the knot range
 *   the number of knots is returned. Make sure to check this value before
 *   using it as index for accessing the corresponding knots, otherwise a
 *   segmentation fault is very likely to happen or even worse no
 *   segmentation fault will occur and you overwrite foreign memory.
 *   To allow small round-off errors at the spline boundaries, you can
 *   set a tolerance, indicating the allowed deviation from the valid
 *   spline range to be accepted as beginning and end of the spline.
 *
 *   \param u           The position along the curve
 *   \param tolerance   Points within the tolerance interval before or
 *                      after the valid range will still return the
 *                      first or last segment
 *
 *   \return The spline segment index, or the number of knots of the spline
 *           (one behind the last segment) if u is not in the knot range
 */
/*======================================================================*/
    size_t curvePositionToSegment(double u, double tolerance) const;
    
/*======================================================================*/
/*! 
 *   Evaluation of the spline curve at position u.
 *
 *   \param u  The position to evaluate the spline at
 *
 *   \return   The spline value (position on the curve)
 */
/*======================================================================*/
    ControlPointT operator()(double u) const;

/*======================================================================*/
/*! 
 *   Evaluation of the spline curve at position u. This version also takes
 *   a lower bound and an upper bound. If u exceeds these bounds, the
 *   spline value is extrapolated linearly using the spline tangent at
 *   the according bound
 *   \f[ f'(u) = \left\{ \begin{array}{l@{\qquad}l}
 *     f(\mathrm{lBound}) + \frac{\mathrm{d}}{\mathrm{d}u}
 *     f(\mathrm{lBound}) \cdot (u - lBound) & u < \mathrm{lBound} \\
 *     f(u) & \mathrm{lBound} \leq u \leq \mathrm{uBound} \\
 *     f(\mathrm{uBound}) + \frac{\mathrm{d}}{\mathrm{d}u}
 *     f(\mathrm{uBound}) \cdot (u - rBound) & \mathrm{uBound} < u
 *   \end{array} \right. \f]
 *
 *   \param u       The position to evaluate the spline at
 *   \param lBound  The spline curve parameter up to which the spline
 *                  linearly extrapolates
 *   \param uBound  The spline curve parameter from which the spline
 *                  linearly extrapolates
 *
 *   \return   The spline value (position on the curve)
 */
/*======================================================================*/
    ControlPointT operator()(double u, double lBound, double uBound) const;
    
/*======================================================================*/
/*! 
 *   Computation of the derivative of the spline function at position u.
 *
 *   \param u      The position to get the spline curve derivative at
 *   \param derivative The derivative degree
 *
 *   \return The derivative of the spline curve at position u
 */
/*======================================================================*/
    ControlPointT derivative(double u, size_t derivative = 1) const;

/*======================================================================*/
/*! 
 *   Computation of the derivative of the spline function.
 *
 *   \param derivative The degree of the derivative
 *
 *   \return The lower degree spline resulting from derivation of the
 *     spline by the given derivative
 */
/*======================================================================*/
    BSpline<ControlPointT> derivative(size_t derivative = 1) const;

/*======================================================================*/
/*! 
 *   Estimate the curve length from curve position uFrom to curve
 *   position uTo. In fact the lengths of the linear segments of
 *   uStep length are added up. If uFrom is greater than uTo the distance
 *   will be returned with negative sign.
 *
 *   \param uFrom  The position on the curve to start measuring
 *   \param uTo    The position on the curve to stop measuring
 *   \param uStep  The resolution of the approximation. Instead of the
 *                 real curve length, the curve is split up into a
 *                 poly-line with nodes of distance uStep along the curve
 *                 parameterization. The curve length is then the sum of the
 *                 segment lengths.
 *
 *   \return The curve length between curve positions uFrom and uTo
 */
/*======================================================================*/
    double curveIntegral(double uFrom, double uTo, double uStep = 0.001) const;

/*======================================================================*/
/*! 
 *   Get the curve parameter u at the specified distance from the specified
 *   origin.
 *
 *   \param origin       The Reference position on the spline to start
 *                       measuring the distance
 *   \param distance     The distance to travel along the spline
 *
 *   \return  The spline parameter u at distance from origin
 */
/*======================================================================*/
    double curveLengthToU(double origin, double distance) const;
    
/*======================================================================*/
/*! 
 *   Get the curve parameter u at the specified distance from the specified
 *   origin. The Spline is extended linearly beyond its end points.
 *
 *   \param origin       The Reference position on the spline to start
 *                       measuring the distance
 *   \param distance     The distance to travel along the spline
 *
 *   \return  The spline parameter u at distance from origin
 */
/*======================================================================*/
    double extendedCurveLengthToU(double origin, double distance) const;
    
/*======================================================================*/
/*! 
 *   Computation of the derivative of the spline function at position u.
 *   This function also extrapolates. It returns the usual spline derivative
 *   within the [lBound, uBound] interval. Ourside the interval the
 *   derivative at the corresponding bound is returned, which resembles
 *   linear extrapolation.
 *
 *   \param u      The position to get the spline curve derivative at
 *   \param derivative The derivative degree
 *   \param lBound The lower bound up to which linear extrapolation is
 *                 performed
 *   \param uBound The upper bound. Starting from this curve parameter
 *                 the spline is linearly extended
 *
 *   \return The derivative of the spline curve at position u
 */
/*======================================================================*/
    ControlPointT extendedDerivative(
        double u, size_t derivative = 1, double lBound = 0.0,
        double uBound = 1.0) const;

/*======================================================================*/
/*! 
 *   Estimate the curve length from curve position uFrom to curve
 *   position uTo. In fact the lengths of the linear segments of
 *   uStep length are added up. If uFrom is greater than uTo the distance
 *   will be returned with negative sign. This function allows to give
 *   out of bounds values and extrapolates linearly.
 *
 *   \param uFrom  The position on the curve to start measuring
 *   \param uTo    The position on the curve to stop measuring
 *   \param lBound The lower bound up to which the spline is linearly
 *                 extrapolated
 *   \param uBound The upper bound from which linear interpolation starts
 *   \param uStep  The resolution of the approximation. Instead of the
 *                 real curve length, the curve is split up into a
 *                 poly-line with nodes of distance uStep along the curve
 *                 parameterization. The curve length is then the sum of the
 *                 segment lengths.
 *
 *   \return The curve length between curve positions uFrom and uTo
 */
/*======================================================================*/
    double extendedCurveIntegral(
        double uFrom, double uTo, double lBound = 0.0, double uBound = 1.0,
        double uStep = 0.001) const;
    
/*======================================================================*/
/*! 
 *   Save the spline with corresponding meta-data to an hdf5 file with
 *   given name, using the groupName provided.
 *
 *   \param fileName         The HDF5 file name. If a file of that name
 *                           already exists, the spline structures will
 *                           be added, otherwise a new file will be created
 *   \param groupName        The HDF5 group to write the spline data to
 *   \param throwExceptions  If this flag is given the method throws
 *                           exceptions if something goes wrong, otherwise
 *                           it will report a warning via standard out
 *                           and return to the caller
 *
 *   \exception BlitzH5Error If the throwExceptions flag is set this error
 *                           is thrown
 */
/*======================================================================*/
    void save(std::string const &fileName, std::string const &groupName,
              bool throwExceptions = false) const;

/*======================================================================*/
/*! 
 *   Load the spline with corresponding meta-data from an hdf5 file with
 *   given name, using the groupName provided.
 *
 *   \param fileName         The HDF5 file name.
 *   \param groupName        The HDF5 group to read the spline data from
 *   \param throwExceptions  If this flag is given the method throws
 *                           exceptions if something goes wrong, otherwise
 *                           it will report a warning via standard out
 *                           and return to the caller
 *
 *   \exception BlitzH5Error If the throwExceptions flag is set this error
 *                           is thrown
 */
/*======================================================================*/
    void load(std::string const &fileName, std::string const &groupName,
              bool throwExceptions = false);
    
/*======================================================================*/
/*! 
 *   Save the spline with corresponding meta-data to an hdf5 file with
 *   given name, using the groupName provided.
 *
 *   \param outFile          The HDF5 file object the spline structures will
 *                           be written to.
 *   \param groupName        The HDF5 group to write the spline data to
 *
 *   \exception BlitzH5Error If the save fails, this error is thrown
 */
/*======================================================================*/
    void save(BlitzH5File &outFile, std::string const &groupName) const;

/*======================================================================*/
/*! 
 *   Load the spline with corresponding meta-data from an hdf5 file with
 *   given name, using the groupName provided.
 *
 *   \param inFile           The HDF5 file object.
 *   \param groupName        The HDF5 group to read the spline data from
 *
 *   \exception BlitzH5Error On load error this error is thrown
 */
/*======================================================================*/
    void load(BlitzH5File const &inFile, std::string const &groupName);
    
  private:
    
/*======================================================================*/
/*! 
 *   Set the number of knots of the B-Spline. This implies
 *   resizing of the control point vector and clearing the basis function
 *   cache.
 *
 *   \param nKnots The new number of knots
 */
/*======================================================================*/ 
    void _setNKnots(size_t nKnots);

/*======================================================================*/
/*! 
 *   Clear all cached base polynomials
 */
/*======================================================================*/
    void _clearCache() const;

/*======================================================================*/
/*! 
 *   Copy all cached base polynomials
 */
/*======================================================================*/
    void _copyCache(BSpline<ControlPointT> const &spline) const;

/*======================================================================*/
/*! 
 *   Computes all base polynomials and their derivatives and places them
 *   in the cache. The integral cache will be cleared. It will be
 *   filled on demand, so don't parallelize regions including spline integral
 *   evaluations before being sure, that all needed integrals are cached.
 */
/*======================================================================*/
    void _updateCache() const;

    int _degree;
    std::vector<ControlPointT> _controlPoints;
    mutable std::vector<double> _knots;
    
    mutable std::vector<
        std::vector< std::vector< std::vector<Polynomial<double>* > > > >
    _basePolynomialDerivativeCache;
    mutable std::vector<
        std::vector< std::vector< std::vector<Polynomial<double>* > > > >
    _basePolynomialIntegralCache;

    static Polynomial<double> _zeroPolynomial;

    friend void fitSplineToSpline(
        BSpline<double> &spline, BSpline<double> const &reference);

    template<int Dim>
    friend void fitSplineToSpline(
        BSpline< blitz::TinyVector<double,Dim> > &spline,
        BSpline< blitz::TinyVector<double,Dim> > const &reference);

    friend void fitSplineToPointCloud(
        BSpline<double> &spline, const std::vector<double> &u,
        std::vector<double> const &points);

    template<int Dim>
    friend void fitSplineToPointCloud(
        BSpline< blitz::TinyVector<double,Dim> > &spline,
        std::vector<double> const &u,
        std::vector< blitz::TinyVector<double,Dim> > const &points);

    friend double distance(BSpline<double> const &spline, double x, double &u);

    template<int Dim>
    friend double distance(
        BSpline< blitz::TinyVector<double,Dim> > const &spline,
        blitz::TinyVector<double,Dim> const &x, double &u);

    template<int Dim>
    friend double extendedDistance(
        BSpline< blitz::TinyVector<double,Dim> > const &spline,
        blitz::TinyVector<double,Dim> const &x, double &u,
        double lBound, double uBound);

};
 
/*======================================================================*/
/*! 
 *   Pretty print operator for stream output of B-Splines
 *
 *   \param os      Output stream to append the representation of the
 *                  BSpline to
 *   \param spline  The spline to output
 *
 *   \return  The stream for chaining
 */
/*======================================================================*/
  template<typename ControlPointT>
  std::ostream &operator<<(
      std::ostream &os, BSpline<ControlPointT> const &spline);

/*======================================================================*/
/*! 
 *   Fitting of a spline curve to another spline curve.
 *   The function alters the control point positions to make the resulting
 *   spline optimally fit the given spline regarding the mean squared error.
 *
 *   \param spline     The output spline
 *   \param reference  The spline to be fitted
 */
/*======================================================================*/
  void fitSplineToSpline(
      BSpline<double> &spline, BSpline<double> const &reference);
 
/*======================================================================*/
/*! 
 *   Fitting of a spline curve to another spline curve.
 *   The function alters the control point positions to make the resulting
 *   spline optimally fit the given spline regarding the mean squared error.
 *
 *   \param spline     The output spline
 *   \param reference  The spline to be fitted
 */
/*======================================================================*/
  template<int Dim>
  void fitSplineToSpline(
      BSpline< blitz::TinyVector<double,Dim> > &spline,
      BSpline< blitz::TinyVector<double,Dim> > const &reference);
 
/*======================================================================*/
/*! 
 *   Fitting of a spline curve to scalar data. There must be a
 *   functional relation between u and points, thus all entries in u need
 *   to be unique otherwise the resulting system of linear equations will
 *   be singular. Another necessary condition for the fitting to work is,
 *   that each spline segment must at least have one data point to fit it
 *   to, thus the number of control points may not exceed the number of
 *   data points and they should be well distributed in the spline domain.
 *   The function alters the control point positions to make the resulting
 *   spline optimally fit the data regarding the mean squared error.
 *
 *   \param spline The spline to fit to the data values provided
 *   \param u      The curve position of the corresponding point in the
 *                 points vector.
 *   \param points The values to fit the spline to
 */
/*======================================================================*/
  void fitSplineToPointCloud(
      BSpline<double> &spline, std::vector<double> const &u,
      std::vector<double> const &points);
 
/*======================================================================*/
/*! 
 *   Fitting of a spline curve to vectorial data. There must be a
 *   functional relation between u and points, thus all entries in u need
 *   to be unique otherwise the resulting system of linear equations will
 *   be singular. Another necessary condition for the fitting to work is,
 *   that each spline segment must at least have one data point to fit it
 *   to, thus the number of control points may not exceed the number of
 *   data points and they should be well distributed in the spline domain.
 *   The function alters the control point positions to make the resulting
 *   spline optimally fit the data regarding the mean squared error.
 *
 *   \param spline The spline to fit to the data points provided
 *   \param u      The curve position of the corresponding point in the
 *                 points vector.
 *   \param points The points to fit the spline to
 */
/*======================================================================*/
  template<int Dim>
  void fitSplineToPointCloud(
      BSpline< blitz::TinyVector<double,Dim> > &spline,
      std::vector<double> const &u,
      std::vector< blitz::TinyVector<double,Dim> > const &points);

/*======================================================================*/
/*! 
 *   Compute the distance and the corresponding curve position
 *   of the given point to the spline.
 *
 *   \param spline A spline
 *   \param x      A point
 *   \param u      The curve position with minimum distance to x is returned
 *                 in this reference
 *
 *   \return The distance between the spline and the point
 */
/*======================================================================*/
  double distance(BSpline<double> const &spline, double x, double &u);

/*======================================================================*/
/*! 
 *   Compute the distance and the corresponding curve position
 *   of the given point to the spline.
 *
 *   \param spline A spline
 *   \param x      A point
 *   \param u      The curve position with minimum distance to x is returned
 *                 in this reference
 *
 *   \return The distance between the spline and the point
 */
/*======================================================================*/
  template<int Dim>
  double distance(
      BSpline< blitz::TinyVector<double,Dim> > const &spline,
      blitz::TinyVector<double,Dim> const &x, double &u);
 
/*======================================================================*/
/*! 
 *   Compute the distance and the corresponding curve position
 *   of the given point to the spline. This function behaves as if the
 *   spline continues as a straight line beyond the given bounds.
 *
 *   \param spline A spline
 *   \param x      A point
 *   \param u      The curve position with minimum distance to x is returned
 *                 in this reference
 *   \param lBound The lower bound up to which the spline behaves as a
 *                 straight line
 *   \param uBound The lower bound starting from which the spline behaves as a
 *                 straight line
 *
 *   \return The distance between the spline and the point
 */
/*======================================================================*/
  double extendedDistance(
      BSpline<double> const &spline, double x, double &u,
      double lBound = 0.0, double uBound = 1.0);

/*======================================================================*/
/*! 
 *   Compute the distance and the corresponding curve position
 *   of the given point to the spline. This function behaves as if the
 *   spline continues as a straight line beyond the given bounds.
 *
 *   \param spline A spline
 *   \param x      A point
 *   \param u      The curve position with minimum distance to x is returned
 *                 in this reference
 *   \param lBound The lower bound up to which the spline behaves as a
 *                 straight line
 *   \param uBound The lower bound starting from which the spline behaves as a
 *                 straight line
 *
 *   \return The distance between the spline and the point
 */
/*======================================================================*/
  template<int Dim>
  double extendedDistance(
      BSpline< blitz::TinyVector<double,Dim> > const &spline,
      blitz::TinyVector<double,Dim> const &x, double &u,
      double lBound = 0.0, double uBound = 1.0);
 
}

#include "ATBSpline.icc"

#endif
