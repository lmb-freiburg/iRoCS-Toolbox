#ifndef SHELLCOORDINATETRANSFORM_HH
#define SHELLCOORDINATETRANSFORM_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>
#include <blitz/array.h>
#include <opencv/cv.h>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

#include <libArrayToolbox/SurfaceGeometry.hh>

#include <libProgressReporter/ProgressReporter.hh>

class ShellCoordinateTransform
{

public:

  ShellCoordinateTransform();

  ShellCoordinateTransform(
      std::vector< blitz::TinyVector<double,3> > const &points,
      double lSegments, double margin,
      bool QCset, blitz::TinyVector<double,3> const &QC,
      bool Xset, blitz::TinyVector<double,3> const &X,
      iRoCS::ProgressReporter *pr = NULL);

  ~ShellCoordinateTransform();

  void fitModel(
      std::vector< blitz::TinyVector<double,3> > const &points,
      double lSegments, double margin,
      bool QCset, blitz::TinyVector<double,3> const &QC,
      bool Xset, blitz::TinyVector<double,3> const &X,
      iRoCS::ProgressReporter *pr = NULL);
  
  void refineModel(
      std::vector< blitz::TinyVector<double,3> > const &points,
      iRoCS::ProgressReporter *pr = NULL);

  blitz::TinyVector<double,3> getAxisPosition(double u) const;
  blitz::TinyMatrix<double,3,3> getAxisCoordinateSystem(double u) const;
  blitz::TinyMatrix<double,2,2> getAxisEllipse(double u) const;
  blitz::TinyVector<double,3> getAxisDirection(double u) const;
  double axisLengthUm() const;

  blitz::TinyVector<double,3> getCoordinates(
      blitz::TinyVector<double,3> const &pos) const;

  blitz::TinyVector<double,3> getCoordinatesWithNormalizedRadius(
      blitz::TinyVector<double,3> const &pos) const;

  blitz::Array<blitz::TinyVector<double,3>,1> const &controlPoints() const;

  int nLatitudes() const;
  void setNLatitudes(int nLatitudes);
  int nLongitudes() const;
  void setNLongitudes(int nLongitudes);
  atb::SurfaceGeometry const &surface() const;

  void save(BlitzH5File &outfile, std::string const &groupName) const;
  void save(std::string const &fileName, std::string const &groupName) const;
  void load(BlitzH5File const &infile, std::string const &groupName);
  void load(std::string const &fileName, std::string const &groupName);

private:

  blitz::TinyVector<double,3> normalizedCoordinates(
      blitz::TinyVector<double,3> const &pos, int controlPoint) const;

  blitz::TinyVector<double,3> denormalizedCoordinates(
      blitz::TinyVector<double,3> const &pos, int controlPoint) const;

  double computeDistanceToLine(
      blitz::TinyVector<double,3> const &pos, int segment,
      double &offset, double &radialDistance, double &angle) const;

  double computeDistanceToLineSegment(
      blitz::TinyVector<double,3> const &pos, int segment,
      double &offset, double &radialDistance, double &angle) const;

  void updateDerivedVariables();

  void smoothControlPoints(int tail);

  void smoothControlEllipses(int tail);

  // The number of control points (equals the length of all following Arrays)
  int _nControlPoint;

  // The axis control point positions
  blitz::Array<blitz::TinyVector<double,3>,1> _controlPoints;

  // The local coordinate systems at the control points
  blitz::Array<blitz::TinyMatrix<double,3,3>,1> _controlAxes;

  // The vectors from control point i to control point i+1
  blitz::Array<blitz::TinyVector<double,3>,1> _segments;

  // The cummulated curve length up to control point i
  blitz::Array<double,1> _controlPointOffset;

  // The directions of the segment vectors
  blitz::Array<blitz::TinyVector<double,3>,1> _segmentDirection;

  // The magnitudes of the segment vectors
  blitz::Array<double,1> _segmentLength;

  // The orthonormal direction to the plane spanned by the segment vector
  // and the (0,0,1) vector
  blitz::Array<blitz::TinyVector<double,3>,1> _axis;

  // The angle between the (0,0,1) vector and the segment direction
  blitz::Array<double,1> _angle;

  // The data extreme points in axis direction, the control point step and
  // the margin (for whatever it is used)
  double _minX, _maxX, _step, _margin;

  //Ellipse
  blitz::Array<double,1> _ra;
  blitz::Array<double,1> _rb;
  blitz::Array<double,1> _angleToAxis;
  blitz::Array<blitz::TinyMatrix<double,2,2>,1> _controlEllipses;

  ptrdiff_t _nLatitudes, _nLongitudes;
  mutable atb::SurfaceGeometry _surface;

};

#endif
