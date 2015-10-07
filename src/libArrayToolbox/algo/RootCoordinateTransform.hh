/**************************************************************************
 *
 * Copyright (C) 2015 Kun Liu, Thorsten Falk
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

#ifndef ROOTCOORDINATETRANSFORM_HH
#define ROOTCOORDINATETRANSFORM_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>
#include <blitz/array.h>
/*
 * modified on Thorsten's code
 */
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class RootCoordinateTransform
{

public:

  RootCoordinateTransform();

  RootCoordinateTransform(
      const blitz::TinyVector<double,3>& qcPosition,
      const std::vector< blitz::TinyVector<double,3> >& controlPoints);

  RootCoordinateTransform(
      const blitz::TinyVector<double,3>& qcPosition,const blitz::TinyVector<double,3>& pointOnAxis,
      const std::vector< blitz::TinyVector<double,3> >& points,
      const size_t nSegments);

  ~RootCoordinateTransform();

  void refineModel(const std::vector< blitz::TinyVector<double,3> >& points);

  blitz::TinyVector<double,3> getCoordinates(
      const blitz::TinyVector<double,3>& pos);

  blitz::TinyVector<double,3> getCoordinatesWithNormalizedRadius(
      const blitz::TinyVector<double,3>& pos);

  blitz::TinyVector<double, 3> getCoordinatesWithNormalizedRadius2(
      const blitz::TinyVector<double, 3>& pos);

  const std::vector< blitz::TinyVector<double,3> >& controlPoints() const;

  void drawFittingResult(blitz::Array<char,3>& paper,blitz::TinyVector<double, 3>& elSize);

  void extractLocalAxis(const blitz::TinyVector<double,3>& origin, blitz::TinyVector<double,3>& xAxis, blitz::TinyVector<double,3>& yAxis, blitz::TinyVector<double,3>& zAxis);

  void extractLocalAxis(const blitz::TinyVector<double,3>& origin, blitz::Array<double,2>& axes);

#if HAVE_BLITZHDF5

  void save(const std::string& fileName, const std::string& groupName) const;
  void load(const std::string& fileName, const std::string& groupName);

#endif

private:

  void extrapolateControlPoints(int tail);
  void smoothControlPoints(int tail);


  blitz::TinyVector<double,3> normalizedCoordinates(
      const blitz::TinyVector<double,3>& pos,
      const size_t segment);

  blitz::TinyVector<double,3> denormalizedCoordinates(
      const blitz::TinyVector<double,3>& pos,
      const size_t segment);

  double computeDistanceToLine(
      const blitz::TinyVector<double,3>& pos,
      const size_t segment,
      double& offset, double& radialDistance, double& angle);

  double computeDistanceToLineSegment(
      const blitz::TinyVector<double,3>& pos,
      const size_t segment,
      double& offset, double& radialDistance, double& angle);

  void updateDerivedVariables();

  blitz::TinyVector<double,3> _qcPos;
  std::vector< blitz::TinyVector<double,3> > _controlPoints;
  double _qcOffset;
  std::vector< blitz::TinyVector<double,3> > _segments;
  std::vector<double> _segmentStartOffset;
  std::vector< blitz::TinyVector<double,3> > _segmentDirection;
  std::vector<double> _segmentLength;
  std::vector< blitz::TinyVector<double,3> > _axis;
  std::vector<double> _angle;
  double _minX,_maxX,_step;
  //Ellipse
  std::vector<double> _ra;
  std::vector<double> _rb;
  std::vector<double> _angleToAxis;
  std::vector<blitz::Array<double,2> > _eT;
  std::vector<blitz::Array<double,2> > _eTinv;
};

#endif
