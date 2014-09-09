#include "RootCoordinateTransform.hh"

#include "../ATBLinAlg.hh"
#include "../TinyMatrixOperators.hh"

#include "math.h"

#include "helper.hh"

double calculateEllipseAngle(double x, double y, double a, double b)
{
  return atan2(a * y, b * x);
}

double
calculateEllipseRadius(double theta, double a, double b)
{
  double bb = b * cos(theta);
  double aa = a * sin(theta);
  return a * b / sqrt(bb * bb + aa * aa);
}

RootCoordinateTransform::RootCoordinateTransform()
{
}

RootCoordinateTransform::RootCoordinateTransform(const blitz::TinyVector<double, 3>& qcPosition, const std::vector<
    blitz::TinyVector<double, 3> >& controlPoints) :
  _qcPos(qcPosition), _controlPoints(controlPoints)
{
  updateDerivedVariables();
}

RootCoordinateTransform::RootCoordinateTransform(const blitz::TinyVector<double, 3>& qcPosition,  const blitz::TinyVector<double,3>& pointOnAxis, const std::vector<
    blitz::TinyVector<double, 3> >& points, const size_t nSegments) :
  _qcPos(qcPosition)
{
  blitz::TinyVector<double, 3> cog(0.0, 0.0, 0.0);
  for (std::vector<blitz::TinyVector<double, 3> >::const_iterator it = points.begin(); it != points.end(); ++it)
    {
      cog += *it;
    }
  cog /= static_cast<double> (points.size());
  blitz::TinyMatrix<double,3,3> cov;
  cov = 0.0;
  blitz::TinyVector<double,3> tmp;
  for (std::vector<blitz::TinyVector<double, 3> >::const_iterator it =
           points.begin(); it != points.end(); ++it)
  {
    tmp = *it - cog;
    cov = cov + atb::outerProduct(tmp, tmp);
  }
  for (ptrdiff_t i = 0; i < 9; ++i)
      cov.data()[i] /= static_cast<double>(points.size());

  LOG << cog << cov << std::endl;

  blitz::TinyMatrix<double,3,3> A;
  blitz::TinyVector<double,3> lambda;
  atb::eigenvalueDecompositionRealSymmetric(cov, A, lambda, atb::Ascending);
  blitz::TinyMatrix<double,3,3> Ainv;
  Ainv = atb::invert(A);

  blitz::TinyVector<double, 3> _centralAxis(A(0, 2), A(1, 2), A(2, 2));
  if (blitz::dot(_centralAxis, blitz::TinyVector<double, 3>(0, 0, 1)) < 0)
  {
    for (ptrdiff_t i = 0; i < 9; ++i)
    {
      A.data()[i] = -A.data()[i];
      Ainv.data()[i] = -Ainv.data()[i];
    }
  }

  LOG << A << std::endl;

  if (pointOnAxis(0) != -1.0)
    {
      blitz::TinyVector<double, 3> xAxis, yAxis, zAxis;
      xAxis = pointOnAxis - qcPosition;
      xAxis /= std::sqrt(blitz::dot(_centralAxis, _centralAxis));
      yAxis = 1;
      yAxis = yAxis - blitz::dot(yAxis, xAxis) * xAxis;
      double normy = std::sqrt(blitz::dot(yAxis, yAxis));
      yAxis = normy > 0 ?
          blitz::TinyVector<double,3>(yAxis / normy) :
          blitz::TinyVector<double,3>(xAxis(2), 0, -xAxis(0));
      zAxis = blitz::cross(xAxis, yAxis);
      A(0, 0) = zAxis(0), A(1, 0) = zAxis(1), A(2, 0) = zAxis(2);
      A(0, 1) = yAxis(0), A(1, 1) = yAxis(1), A(2, 1) = yAxis(2);
      A(0, 2) = xAxis(0), A(1, 2) = xAxis(1), A(2, 2) = xAxis(2);
      Ainv = atb::invert(A);
    }


  _minX = std::numeric_limits<double>::infinity();
  _maxX = -std::numeric_limits<double>::infinity();

  blitz::TinyVector<double, 3> p;
  for (std::vector<blitz::TinyVector<double, 3> >::const_iterator it = points.begin(); it != points.end(); ++it)
    {
      p = atb::mvMult(Ainv, *it);
      if (p(2) < _minX)
        _minX = p(2);
      if (p(2) > _maxX)
        _maxX = p(2);
    }

//  _minX = _minX + 20;
//  _maxX = _maxX - 10;
  _step = (_maxX - _minX) / nSegments;

  blitz::TinyVector<double, 3> _tQC = atb::mvMult(Ainv, _qcPos);
  blitz::TinyVector<double, 3> startPos = atb::mvMult(A, blitz::TinyVector<double, 3>(_tQC(0), _tQC(1), _minX));
  blitz::TinyVector<double, 3> endPos = atb::mvMult(A, blitz::TinyVector<double, 3>(_tQC(0), _tQC(1), _maxX));
  _controlPoints.resize(nSegments + 1);
  for (size_t i = 0; i < nSegments + 1; ++i)
    {
      _controlPoints[i] = startPos + static_cast<double> (i) * (endPos - startPos) / static_cast<double> (nSegments);
      LOG << _controlPoints[i] << std::endl;
    }
  // Initialize to get the derived variables for the control points
  updateDerivedVariables();
  refineModel(points);
}

RootCoordinateTransform::~RootCoordinateTransform()
{
}

void
RootCoordinateTransform::refineModel(const std::vector<blitz::TinyVector<double, 3> >& points)
{
  int tail = ceil(30 / _step);
  LOG << "tail" << tail<< std::endl;
  for (size_t i = 0; i < _controlPoints.size() - tail; ++i)
    {
      blitz::TinyVector<double, 3> cog = 0.0;
      double weight, cumWeight = 0.0;
      for (std::vector<blitz::TinyVector<double, 3> >::const_iterator it = points.begin(); it != points.end(); ++it)
        {
          weight = normalizedCoordinates(*it, i)(2);
          weight = exp(-0.5 * blitz::pow2(weight / (_segmentLength[i]/2)));
          cumWeight += weight;
          cog += weight * *it;
        }
      cog /= cumWeight;
      blitz::TinyVector<double, 3> tmpPos = normalizedCoordinates(cog, i);
      LOG << cog << std::endl;
      tmpPos(2) = 0;
      _controlPoints[i] = denormalizedCoordinates(tmpPos, i);
      LOG << _controlPoints[i] << std::endl;
    }
  smoothControlPoints(tail);
  //extrapolate
  for (size_t i = _controlPoints.size()-tail; i < _controlPoints.size(); ++i)
    {
      _controlPoints[i] = _controlPoints[i-1] + (_controlPoints[i-1] - _controlPoints[i-2]);
      LOG << _controlPoints[i] << std::endl;
    }
  updateDerivedVariables();

  //distribute the points into segments
  std::vector<cv::Point2f> p[_controlPoints.size()];
  for (std::vector<blitz::TinyVector<double, 3> >::const_iterator it = points.begin(); it != points.end(); ++it)
    {
      double dist, minDist;
      int nSeg = -1;
      minDist = std::numeric_limits<double>::infinity();
      blitz::TinyVector<double, 3> tmp;
      blitz::TinyVector<double, 3> tp(0.0);
      for (size_t i = 0; i < _controlPoints.size()-tail; ++i)
        {
          dist = computeDistanceToLineSegment(*it, i, tmp(0), tmp(1), tmp(2));
          if (dist < minDist)
            {
              minDist = dist;
              tp = tmp;
              nSeg = i;
            }
        }
      p[nSeg].push_back(cv::Point2f(float(tp(1) * cos(tp(2))), float(tp(1) * sin(tp(2)))));
//      LOG << nSeg << ":"<< tp(2) << "," << cos(tp(3)) <<"," <<   sin(tp(3)) << std::endl;
    }

  //fit local ellipse
  for (size_t i = 0; i < _controlPoints.size()-tail-1; ++i)
    {
      cv::Mat m(p[i]);
      LOG << i << "-" << m.cols << ":"<< m.rows <<"," <<   m.elemSize() << std::endl;
      cv::RotatedRect ellipse = cv::fitEllipse(m);
      //        _ellipse.push_back(ellipse);
      // cv::Size2f size = ellipse.size;
      _ra.push_back(ellipse.size.width / 2);
      _rb.push_back(ellipse.size.height / 2);
      double angle = ellipse.angle * M_PI / 180;
      _angleToAxis.push_back(angle);
      LOG << _ra[i] << "-" <<  _rb[i] << ":"<< _angleToAxis[i] << std::endl;
      //      blitz::Array<double, 2> rot(3, 3);
      //      rot = cos(angle), -sin(angle), 0, sin(angle), cos(angle), 0, 0, 0, 1;
      //      _eTinv.push_back(rot);
      //      blitz::Array<double, 2> rotInv(3, 3);
      //      rotInv = cos(angle), sin(angle), 0, -sin(angle), cos(angle), 0, 0, 0, 1;
      //      _eT.push_back(rotInv);
    }
  for (size_t i = _controlPoints.size()-tail-1; i < _controlPoints.size(); ++i)
    {
      _ra.push_back(_ra[i-1]);
      _rb.push_back(_rb[i-1]);
      _angleToAxis.push_back(_angleToAxis[i-1]);
    }
}

blitz::TinyVector<double, 3>
RootCoordinateTransform::getCoordinates(const blitz::TinyVector<double, 3>& pos)
{
  blitz::TinyVector<double, 3> res;
  double dist, minDist;
  minDist = std::numeric_limits<double>::infinity();
  blitz::TinyVector<double, 3> tmp;
  for (size_t i = 0; i < _controlPoints.size() - 1; ++i)
    {
      dist = computeDistanceToLineSegment(pos, i, tmp(0), tmp(1), tmp(2));
      if (dist < minDist)
        {
          minDist = dist;
          res = tmp;
          res(0) += _segmentStartOffset[i];
        }
    }
  res(0) -= _qcOffset;
  return res;
}

blitz::TinyVector<double, 3>
RootCoordinateTransform::getCoordinatesWithNormalizedRadius(const blitz::TinyVector<double, 3>& pos)
{
  blitz::TinyVector<double, 3> res;
  double dist, minDist;
  int nSeg = 0;
  minDist = std::numeric_limits<double>::infinity();
  blitz::TinyVector<double, 3> tmp;
  for (size_t i = 0; i < _controlPoints.size() - 1; ++i)
    {
      dist = computeDistanceToLineSegment(pos, i, tmp(0), tmp(1), tmp(2));
      if (dist < minDist)
        {
          minDist = dist;
          res = tmp;
          nSeg = i;
          res(0) += _segmentStartOffset[i];
        }
    }
  res(0) -= _qcOffset;
  double phi = res(2) - _angleToAxis[nSeg];
  double er = calculateEllipseRadius(phi, _ra[nSeg], _rb[nSeg]);
  res(1) = res(1) / er;
  return res;
}

blitz::TinyVector<double, 3>
RootCoordinateTransform::getCoordinatesWithNormalizedRadius2(
    const blitz::TinyVector<double, 3>& pos)
{
  blitz::TinyVector<double, 3> res, res2;
  double dist, minDist, minDist2;
  int nSeg = 0, nSeg2 = 0;
  minDist = std::numeric_limits<double>::infinity();
  minDist2 = std::numeric_limits<double>::infinity();
  blitz::TinyVector<double, 3> tmp;
  for (size_t i = 0; i < _controlPoints.size() - 1; ++i)
    {
      dist = computeDistanceToLineSegment(pos, i, tmp(0), tmp(1), tmp(2));
      if (dist < minDist2)
        {
          if (dist < minDist)
            {
              minDist2 = minDist;
              nSeg2 = nSeg;
              minDist = dist;
              nSeg = i;
            }
          else
            {
              minDist2 = dist;
              nSeg2 = i;
            }
        }
    }

  minDist = computeDistanceToLineSegment(pos, nSeg, res(0), res(1), res(2));
  minDist2 = computeDistanceToLineSegment(pos, nSeg2, res2(0), res2(1), res2(2));
  minDist = abs(res(0) - 0.5 * _segmentLength[nSeg])+1e-10;
  minDist2 = abs(res2(0) - 0.5 * _segmentLength[nSeg2])+1e-10;
  res(0) =( (res(0) + _segmentStartOffset[nSeg]) * minDist2 + (res2(0) + _segmentStartOffset[nSeg2]) * minDist )/ (minDist+minDist2);
  res(0) -= _qcOffset;

  double phi = res(2) - _angleToAxis[nSeg];
  double er = calculateEllipseRadius(phi, _ra[nSeg], _rb[nSeg]);
  res(1) = res(1) / er;
  phi = res2(2) - _angleToAxis[nSeg2];
  er = calculateEllipseRadius(phi, _ra[nSeg2], _rb[nSeg2]);
  res2(1) = res2(1) / (er+1e-10);
  res(1) =( res(1) * minDist2 + res2(1) * minDist )/ (minDist+minDist2) ;

  double angleDiff = res(2) - res2(2);
  angleDiff = (angleDiff > M_PI) ? angleDiff - 2 * M_PI : angleDiff;
  angleDiff = (angleDiff < -M_PI) ? angleDiff + 2 * M_PI : angleDiff;
  res(2) = res(2) + minDist / (minDist+minDist2) * angleDiff;
  res(2) = (res(2) > M_PI) ? res(2) - 2 * M_PI : res(2);
  res(2) = (res(2) < -M_PI) ? res(2) + 2 * M_PI : res(2);
  return res;
}

const std::vector<blitz::TinyVector<double, 3> >&
RootCoordinateTransform::controlPoints() const
{
  return _controlPoints;
}

void
RootCoordinateTransform::extractLocalAxis(const blitz::TinyVector<double, 3>& origin,
                                          blitz::TinyVector<double, 3>& xAxis, blitz::TinyVector<double, 3>& yAxis,
                                          blitz::TinyVector<double, 3>& zAxis)
{
  double dist, minDist;
  int nSeg = 0;
  minDist = std::numeric_limits<double>::infinity();
  blitz::TinyVector<double, 3> tmp;
  for (size_t i = 0; i < _controlPoints.size() - 1; ++i)
    {
      dist = computeDistanceToLineSegment(origin, i, tmp(0), tmp(1), tmp(2));
      if (dist < minDist)
        {
          minDist = dist;
          nSeg = i;
        }
    }
  xAxis = _segmentDirection[nSeg];
  yAxis = origin - _controlPoints[nSeg];
  yAxis = yAxis - blitz::dot(yAxis, xAxis) * xAxis;
  double normy = std::sqrt(blitz::dot(yAxis, yAxis));
  yAxis = normy > 0 ?
      blitz::TinyVector<double,3>(yAxis / normy) :
      blitz::TinyVector<double,3>(xAxis(2), 0, -xAxis(0));
  zAxis = blitz::cross(xAxis, yAxis);
}

void
RootCoordinateTransform::extractLocalAxis(const blitz::TinyVector<double, 3>& origin, blitz::Array<double, 2>& axes)
{
  axes.resize(3, 3);
  blitz::TinyVector<double, 3> xAxis, yAxis, zAxis;
  double dist, minDist;
  int nSeg = 0;
  minDist = std::numeric_limits<double>::infinity();
  blitz::TinyVector<double, 3> tmp;
  for (size_t i = 0; i < _controlPoints.size() - 1; ++i)
    {
      dist = computeDistanceToLineSegment(origin, i, tmp(0), tmp(1), tmp(2));
      if (dist < minDist)
        {
          minDist = dist;
          nSeg = i;
        }
    }
  xAxis = _segmentDirection[nSeg];
  yAxis = origin - _controlPoints[nSeg];
  yAxis = yAxis - blitz::dot(yAxis, xAxis) * xAxis;
  double normy = std::sqrt(blitz::dot(yAxis, yAxis));
  yAxis = (normy > 0) ?
      blitz::TinyVector<double,3>(yAxis / normy) :
      blitz::TinyVector<double, 3>(xAxis(2), 0, -xAxis(0));
  zAxis = blitz::cross(xAxis, yAxis);
  axes(0, 0) = zAxis(0), axes(1, 0) = zAxis(1), axes(2, 0) = zAxis(2);
  axes(0, 1) = yAxis(0), axes(1, 1) = yAxis(1), axes(2, 1) = yAxis(2);
  axes(0, 2) = xAxis(0), axes(1, 2) = xAxis(1), axes(2, 2) = xAxis(2);
}

void
RootCoordinateTransform::drawFittingResult(blitz::Array<char, 3>& paper, blitz::TinyVector<double, 3>& elSize)
{
#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (int i = 0; i < int(_controlPoints.size() - 1); i++)
    {
      blitz::TinyVector<double, 3> cc = _controlPoints[i];
      blitz::TinyVector<int, 3> pos;
      pos = cc / elSize;
      paper(pos) = 6;
      //draw ellipse
      blitz::Array<double, 2> tt(3, 3);
      double angle = _angleToAxis[i];
      tt = cos(angle), -sin(angle), 0, sin(angle), cos(angle), 0, 0, 0, 1;
      double a = _ra[i], b = _rb[i];
      double np = 180;
      for (int x = 0; x < np; x++)
        {
          double phi = x / np * 2 * M_PI;
          double er = calculateEllipseRadius(phi - angle, a, b);
          blitz::TinyVector<double, 3> point(er * cos(phi), er * sin(phi), _segmentLength[i]/2);
          point = denormalizedCoordinates(point, i) / elSize;
          keepInRange3D(point, blitz::TinyVector<int, 3>(0, 0, 0), paper.shape());
          paper(point) =8;
        }
    }
}

#if HAVE_BLITZHDF5

void RootCoordinateTransform::save(
    const std::string& fileName, const std::string& groupName) const
  {
    BlitzH5File outFile(fileName, BlitzH5File::WriteOrNew);
    outFile.writeAttribute(_qcPos, groupName + "/qcPosition_um", "");
    blitz::Array<blitz::TinyVector<double,3>,1> tmp(_controlPoints.size());
    for (ssize_t i = 0; i < tmp.size(); ++i) tmp(i) = _controlPoints[i];
    outFile.writeDataSetSimple(tmp, groupName + "/controlPoints");
    outFile.writeAttribute(_qcOffset, groupName + "/qcOffset", "");
    for (ssize_t i = 0; i < tmp.size(); ++i) tmp(i) = _segments[i];
    outFile.writeDataSetSimple(tmp, groupName + "/segments");
    blitz::Array<double,1> tmp2(_controlPoints.size());
    for (ssize_t i = 0; i < tmp2.size(); ++i) tmp2(i) = _segmentStartOffset[i];
    outFile.writeDataSetSimple(tmp2, groupName + "/segmentStartOffset");
    for (ssize_t i = 0; i < tmp.size(); ++i) tmp(i) = _segmentDirection[i];
    outFile.writeDataSetSimple(tmp, groupName + "/segmentDirection");
    for (ssize_t i = 0; i < tmp2.size(); ++i) tmp2(i) = _segmentLength[i];
    outFile.writeDataSetSimple(tmp2, groupName + "/segmentLength");
    for (ssize_t i = 0; i < tmp.size(); ++i) tmp(i) = _axis[i];
    outFile.writeDataSetSimple(tmp, groupName + "/axis");
    for (ssize_t i = 0; i < tmp2.size(); ++i) tmp2(i) = _angle[i];
    outFile.writeDataSetSimple(tmp2, groupName + "/angle");

    for (ssize_t i = 0; i < tmp2.size(); ++i) tmp2(i) = _ra[i];
    outFile.writeDataSetSimple(tmp2, groupName + "/_ra");
    for (ssize_t i = 0; i < tmp2.size(); ++i) tmp2(i) = _rb[i];
    outFile.writeDataSetSimple(tmp2, groupName + "/_rb");
    for (ssize_t i = 0; i < tmp2.size(); ++i) tmp2(i) = _angleToAxis[i];
    outFile.writeDataSetSimple(tmp2, groupName + "/_angleToAxis");
  }

void RootCoordinateTransform::load(
    const std::string& fileName, const std::string& groupName)
  {
    BlitzH5File inFile(fileName);
    inFile.readAttribute(_qcPos, groupName + "/qcPosition_um", "");
    blitz::Array<blitz::TinyVector<double,3>,1> tmp;
    inFile.readDataSetSimple(tmp, groupName + "/controlPoints");
    _controlPoints.resize(tmp.size());
    for (ssize_t i = 0; i < tmp.size(); ++i) _controlPoints[i] = tmp(i);
    inFile.readAttribute(_qcOffset, groupName + "/qcOffset", "");
    inFile.readDataSetSimple(tmp, groupName + "/segments");
    _segments.resize(tmp.size());
    for (ssize_t i = 0; i < tmp.size(); ++i) _segments[i] = tmp(i);
    blitz::Array<double,1> tmp2;
    inFile.readDataSetSimple(tmp2, groupName + "/segmentStartOffset");
    _segmentStartOffset.resize(tmp2.size());
    for (ssize_t i = 0; i < tmp2.size(); ++i) _segmentStartOffset[i] = tmp2(i);
    inFile.readDataSetSimple(tmp, groupName + "/segmentDirection");
    _segmentDirection.resize(tmp.size());
    for (ssize_t i = 0; i < tmp.size(); ++i) _segmentDirection[i] = tmp(i);
    inFile.readDataSetSimple(tmp2, groupName + "/segmentLength");
    _segmentLength.resize(tmp2.size());
    for (ssize_t i = 0; i < tmp2.size(); ++i) _segmentLength[i] = tmp2(i);
    inFile.readDataSetSimple(tmp, groupName + "/axis");
    _axis.resize(tmp.size());
    for (ssize_t i = 0; i < tmp.size(); ++i) _axis[i] = tmp(i);
    inFile.readDataSetSimple(tmp2, groupName + "/angle");
    _angle.resize(tmp2.size());
    for (ssize_t i = 0; i < tmp2.size(); ++i) _angle[i] = tmp2(i);

    inFile.readDataSetSimple(tmp2, groupName + "/_ra");
    _ra.resize(tmp2.size());
    for (ssize_t i = 0; i < tmp2.size(); ++i) _ra[i] = tmp2(i);
    inFile.readDataSetSimple(tmp2, groupName + "/_rb");
    _rb.resize(tmp2.size());
    for (ssize_t i = 0; i < tmp2.size(); ++i) _rb[i] = tmp2(i);
    inFile.readDataSetSimple(tmp2, groupName + "/_angleToAxis");
    _angleToAxis.resize(tmp2.size());
    for (ssize_t i = 0; i < tmp2.size(); ++i) _angleToAxis[i] = tmp2(i);
  }

#endif

blitz::TinyVector<double, 3>
RootCoordinateTransform::normalizedCoordinates(
    const blitz::TinyVector<double, 3>& pos, const size_t segment)
{
  return atb::rotate(blitz::TinyVector<double, 3>(pos - _controlPoints[segment]), _axis[segment], _angle[segment]);
}

blitz::TinyVector<double, 3>
RootCoordinateTransform::denormalizedCoordinates(
    const blitz::TinyVector<double, 3>& pos, const size_t segment)
{
  return atb::rotate(blitz::TinyVector<double, 3>(pos), _axis[segment], -_angle[segment]) + _controlPoints[segment];
}

double
RootCoordinateTransform::computeDistanceToLine(
    const blitz::TinyVector<double, 3>& pos, const size_t segment,
    double& offset, double& radialDistance, double& angle)
{
  // Project point onto segment
  blitz::TinyVector<double, 3> posN = normalizedCoordinates(pos, segment);

  angle = atan2(posN(1), posN(0));
  radialDistance = std::sqrt(posN(1) * posN(1) + posN(0) * posN(0));
  offset = posN(2);

  if (posN(2) < 0)
    offset = 0.0;
  else if (posN(2) > _segmentLength[segment])
    offset = _segmentLength[segment];
  else
    offset = posN(2);
  blitz::TinyVector<double,3> dVec(
      _controlPoints[segment] + offset * _segmentDirection[segment] - pos);
  return std::sqrt(blitz::dot(dVec, dVec));
}

double
RootCoordinateTransform::computeDistanceToLineSegment(
    const blitz::TinyVector<double, 3>& pos, const size_t segment,
    double& offset, double& radialDistance, double& angle)
{
  double dist = computeDistanceToLine(
      pos, segment, offset, radialDistance, angle);
  if (offset >= 0 && offset < _segmentLength[segment])
    return dist;
  if (offset < 0)
    offset = 0;
  if (offset > _segmentLength[segment])
    offset = _segmentLength[segment];
  blitz::TinyVector<double,3> dVec(
      _controlPoints[segment] + offset * _segmentDirection[segment] - pos);
  return std::sqrt(blitz::dot(dVec, dVec));
}


void
RootCoordinateTransform::extrapolateControlPoints(int tail)
{
  //extrapolate
  for (size_t i = _controlPoints.size()-tail; i < _controlPoints.size(); ++i)
    {
      _controlPoints[i] = _controlPoints[i-1] +
          (_controlPoints[i-1] - _controlPoints[i-2]);
    }

}

void
RootCoordinateTransform::smoothControlPoints(int tail)
{
  std::vector< blitz::TinyVector<double,3> > controlPointsBuff;
  controlPointsBuff.resize(_controlPoints.size());
  //smooth control points
  controlPointsBuff[0] = (_controlPoints[0] + 2 * _controlPoints[1] - _controlPoints[2]) / 2;
  for (size_t i = 1; i < _controlPoints.size()-tail-1; ++i)
      {
        controlPointsBuff[i] = (_controlPoints[i-1] + _controlPoints[i] + _controlPoints[i+1]) / 3;
      }
  int end= _controlPoints.size()-tail-1;
  controlPointsBuff[end] = 2 * _controlPoints[end-1] - _controlPoints[end-2];
  for (size_t i = 0; i < _controlPoints.size()-tail; ++i)
  {
  _controlPoints[i] = controlPointsBuff[i];
  }
}

void
RootCoordinateTransform::updateDerivedVariables()
{
  blitz::TinyVector<double, 3> e1(0.0, 0.0, 1.0);
  _segments.resize(_controlPoints.size());
  _segmentLength.resize(_controlPoints.size());
  _segmentStartOffset.resize(_controlPoints.size() + 1);
  _segmentStartOffset[0] = 0.0;
  _segmentDirection.resize(_controlPoints.size());
  _axis.resize(_controlPoints.size());
  _angle.resize(_controlPoints.size());
  for (size_t i = 0; i < _controlPoints.size(); ++i)
    {
      if (i == _controlPoints.size() - 1)
        _segments[i] = _segments[i - 1];
      else
        _segments[i] = _controlPoints[i + 1] - _controlPoints[i];
      _segmentLength[i] = std::sqrt(blitz::dot(_segments[i], _segments[i]));
      _segmentStartOffset[i + 1] = _segmentStartOffset[i] + _segmentLength[i];
      _segmentDirection[i] = _segments[i] / _segmentLength[i];
      _axis[i] = blitz::cross(_segmentDirection[i], e1);
      _angle[i] = acos(blitz::dot(_segmentDirection[i], e1));
      blitz::TinyVector<double,3> d(
          atb::rotate(_segmentDirection[i], _axis[i], _angle[i]) - e1);
      if (blitz::dot(d, d) > 1.0) _angle[i] = -_angle[i];
    }

  double dist, minDist;
  minDist = std::numeric_limits<double>::infinity();
  double offset, radial, phi;
  for (size_t i = 0; i < _controlPoints.size() - 1; ++i)
    {
      dist = computeDistanceToLineSegment(_qcPos, i, offset, radial, phi);
      if (dist < minDist)
        {
          minDist = dist;
          _qcOffset = _segmentStartOffset[i] + offset;
        }
    }
}
