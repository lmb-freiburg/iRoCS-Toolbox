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
 *  \file ATBPolyline.hh
 *  \brief Simple Polyline class providing a function to fit a polyline
 *    with defined precision to a BSpline for fast distance computations.
 */
/*======================================================================*/

#ifndef ATBPOLYLINE_HH
#define ATBPOLYLINE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <list>

#include "ATBSpline.hh"

namespace atb
{

/*======================================================================*/
/*!
 *  \class Polyline <>
 *  \brief The Polyline class provides a simple polyline representation
 *    of a BSpline for fast point to spline distance approximations.
 */
/*======================================================================*/
  template<int Dim>
  class Polyline
  {

  public:
  
    typedef blitz::TinyVector<double,Dim> PointT;
    typedef typename std::map<double,PointT>::iterator PointIteratorT;
    typedef typename std::map<double,PointT>::const_iterator
    PointConstIteratorT;

/*======================================================================*/
/*! 
 *   Default constructor. This constructor generates an empty polyline.
 */
/*======================================================================*/
    Polyline();
  
/*======================================================================*/
/*! 
 *   Copy constructor.
 *
 *   \param polyline  The Polyline object to copy
 */
/*======================================================================*/
    Polyline(Polyline<Dim> const &polyline);
  
/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~Polyline();
  
/*======================================================================*/
/*! 
 *   Copy assignment operator
 *
 *   \param polyline  Change the nodes of this polyline to the nodes of
 *     the polyline given.
 *
 *   \return A reference to this updated polyline for operator chaining
 */
/*======================================================================*/ 
    Polyline<Dim> &operator=(Polyline<Dim> const &polyline);

/*======================================================================*/
/*! 
 *   Get the polyline position at curve length u.
 *
 *   \param u  The position along the curve parameterization to evaluate
 *     the polyline at
 *
 *   \return The polyline point at the given curve parameter
 */
/*======================================================================*/
    PointT operator()(double u) const;

/*======================================================================*/
/*! 
 *   Project the given point onto the polyline and get the distance to
 *   the polyline and the projected position along the curve parameterization.
 *
 *   \param point  The point to project onto the polyline
 *   \param uOpt   The projected position along the curve parameterization
 *
 *   \return The point-to-polyline distance
 */
/*======================================================================*/
    double distance(PointT const &point, double &uOpt) const;

/*======================================================================*/
/*! 
 *   Compute the curve integral along the poly line. You can give the lower
 *   and upper bounds of the length computation, if you only want to measure
 *   the length of a part of the polyline.
 *
 *   \param uStart  The lower bound curve parameter
 *   \param uEnd    The upper bound curve parameter
 *
 *   \return The length of the polyline from uStart to uEnd
 */
/*======================================================================*/
    double curveIntegral(double uStart = 0.0, double uEnd = 1.0) const;

/*======================================================================*/
/*! 
 *   Make this polyline optimally fit the given BSpline.
 *
 *   The Polyline is initialized with the spline's knot positions and
 *   iteratively subdivided along the segment with greatest error of fit
 *   until the tolerance is reached.
 *
 *   \param spline   The spline to fit this polyline to
 *   \param eps      The tolerance below which the fit is not further refined
 */
/*======================================================================*/
    void fitToSpline(BSpline<PointT> const &spline, double eps = 1e-4);
  
/*======================================================================*/
/*! 
 *   Save the polyline with corresponding meta-data to an hdf5 file with
 *   given name, using the groupName provided.
 *
 *   \param fileName         The HDF5 file name. If a file of that name
 *                           already exists, the polyline structures will
 *                           be added, otherwise a new file will be created
 *   \param groupName        The HDF5 group to write the polyline data to
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
 *   Load the polyline with corresponding meta-data from an hdf5 file with
 *   given name, using the groupName provided.
 *
 *   \param fileName         The HDF5 file name.
 *   \param groupName        The HDF5 group to read the polyline data from
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
 *   Save the polyline with corresponding meta-data to an hdf5 file with
 *   given name, using the groupName provided.
 *
 *   \param outFile          The HDF5 file object the polyline structures will
 *                           be written to.
 *   \param groupName        The HDF5 group to write the polyline data to
 *
 *   \exception BlitzH5Error If the save fails, this error is thrown
 */
/*======================================================================*/
    void save(BlitzH5File &outFile, std::string const &groupName) const;

/*======================================================================*/
/*! 
 *   Load the polyline with corresponding meta-data from an hdf5 file with
 *   given name, using the groupName provided.
 *
 *   \param inFile           The HDF5 file object.
 *   \param groupName        The HDF5 group to read the polyline data from
 *
 *   \exception BlitzH5Error On load error this error is thrown
 */
/*======================================================================*/
    void load(BlitzH5File const &inFile, std::string const &groupName);
    
  private:
  
    std::map< double, blitz::TinyVector<double,Dim> > _points;
  
    template<int Dim1>
    friend std::ostream &operator<<(std::ostream &, Polyline<Dim1> const &);
  
  };

/*======================================================================*/
/*! 
 *   Insert a pretty-printed version of the polyline into the given stream.
 *
 *   \param os        The stream to append the polyline string to
 *   \param polyline  The polyline to output to the stream
 *
 *   \return A reference to the stream for operator chaining
 */
/*======================================================================*/
  template<int Dim>
  std::ostream &operator<<(std::ostream &os, Polyline<Dim> const &polyline);

}

#include "ATBPolyline.icc"

#endif
