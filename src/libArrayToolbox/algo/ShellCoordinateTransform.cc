#include "ShellCoordinateTransform.hh"

#include "../ATBLinAlg.hh"
#include "../TinyMatrixOperators.hh"
#include "../TypeTraits.hh"

#include "math.h"

#include "helper.hh"

template<typename MatrixT, typename BaseT, int Dim>
blitz::TinyVector<BaseT, Dim>
mvMult_transpose(MatrixT const &m, blitz::TinyVector<BaseT,Dim> const &v)
{
  blitz::TinyVector<BaseT,Dim> res;
  res = 0;
  for (int d1 = 0; d1 < Dim; ++d1)
      for (int d2 = 0; d2 < Dim; ++d2)
          res(d1) += m(d2, d1) * v(d2);
  return res;
}

ShellCoordinateTransform::ShellCoordinateTransform()
        : _nLatitudes(100), _nLongitudes(50), _surface()
{}

ShellCoordinateTransform::ShellCoordinateTransform(
    std::vector< blitz::TinyVector<double,3> > const &points,
    double lSegments, double margin,
    bool QCset, blitz::TinyVector<double,3> const &QC,
    bool Xset, blitz::TinyVector<double,3> const &X,
    iRoCS::ProgressReporter *pr)
        : _nLatitudes(100), _nLongitudes(50), _surface()
{
  fitModel(points, lSegments, margin, QCset, QC, Xset, X, pr);
}

ShellCoordinateTransform::~ShellCoordinateTransform()
{}

void ShellCoordinateTransform::fitModel(
    std::vector< blitz::TinyVector<double,3> > const &points,
    double lSegments, double margin,
    bool QCset, blitz::TinyVector<double,3> const &QC,
    bool Xset, blitz::TinyVector<double,3> const &X,
    iRoCS::ProgressReporter *pr)
{
  int pMin = (pr != NULL) ? pr->taskProgressMin() : 0;
  int pScale = (pr != NULL) ? (pr->taskProgressMax() - pMin) : 100;
  if (pr != NULL && !pr->updateProgress(pMin)) return;

  // Compute point cloud center of gravity
  blitz::TinyVector<double,3> cog(0.0f);
  for (size_t i = 0; i < points.size(); i++) cog += points[i];
  cog /= static_cast<double>(points.size());

  if (pr != NULL && !pr->updateProgress(static_cast<int>(pMin + 0.01 * pScale)))
      return;

  // Compute point cloud covariance
  blitz::TinyMatrix<double,3,3> cov;
  cov = atb::traits< blitz::TinyMatrix<double,3,3> >::zero;
  blitz::TinyVector<double,3> tmp;
  for (std::vector< blitz::TinyVector<double,3> >::const_iterator it =
           points.begin(); it != points.end(); ++it)
  {
    tmp = *it - cog;
    cov = cov + atb::outerProduct(tmp, tmp);
  }
  cov = cov / static_cast<double>(points.size());

  if (pr != NULL && !pr->updateProgress(static_cast<int>(pMin + 0.03 * pScale)))
      return;

  // Compute the axes of the point cloud
  blitz::TinyMatrix<double,3,3> A;
  blitz::TinyVector<double,3> lambda;
  atb::eigenvalueDecompositionRealSymmetric(cov, A, lambda, atb::Ascending);
  blitz::TinyMatrix<double,3,3> Ainv;
  Ainv = atb::invert(A);

  if (pr != NULL && !pr->updateProgress(static_cast<int>(pMin + 0.04 * pScale)))
      return;

  // Assure that the main axis points to the right
  blitz::TinyVector<double,3> centralAxis(A(0, 2), A(1, 2), A(2, 2));
  if (!Xset && blitz::dot(
          centralAxis, blitz::TinyVector<double,3>(0.0, 0.0, 1.0)) < 0.0)
  {
    A = -A;
    Ainv = -Ainv;
  }
  else if (Xset && blitz::dot(centralAxis, X) < 0.0)
  {
    A = -A;
    Ainv = -Ainv;
  }

  if (pr != NULL && !pr->updateProgress(static_cast<int>(pMin + 0.05 * pScale)))
      return;

  // Compute axis range in main direction
  _minX = std::numeric_limits<double>::infinity();
  _maxX = -std::numeric_limits<double>::infinity();
  _margin = margin;
  blitz::TinyVector<double,3> p;
  for (size_t i = 0; i < points.size(); i++)
  {
    p = atb::mvMult(Ainv, points[i]);
    if (p(2) < _minX) _minX = p(2);
    if (p(2) > _maxX) _maxX = p(2);
  }

  if (pr != NULL && !pr->updateProgress(static_cast<int>(pMin + 0.06 * pScale)))
      return;

  // Generate as many control points as required with the given control point
  // distance
  _step = lSegments;
  _nControlPoint = floor(((_maxX - _minX)) / _step);

  // If QC is set use it as reference point otherwise use the center of
  // gravity of the point cloud
  blitz::TinyVector<double,3> startPos,endPos;
  if (!QCset)
  {
    blitz::TinyVector<double, 3> tCoG(atb::mvMult(Ainv, cog));
    startPos = atb::mvMult(
        A, blitz::TinyVector<double,3>(tCoG(0), tCoG(1), _minX));
    endPos = atb::mvMult(
        A, blitz::TinyVector<double,3>(tCoG(0), tCoG(1), _maxX));
  }
  else
  {
    blitz::TinyVector<double, 3> tQC(atb::mvMult(Ainv, QC));
    startPos = atb::mvMult(
        A, blitz::TinyVector<double, 3>(tQC(0), tQC(1), _minX));
    endPos = atb::mvMult(
        A, blitz::TinyVector<double, 3>(tQC(0), tQC(1), _maxX));
  }

  if (pr != NULL && !pr->updateProgress(static_cast<int>(pMin + 0.07 * pScale)))
      return;

  // Initialize the control points to the point cloud's main axis
  _controlPoints.resize(_nControlPoint);
  for (int i = 0; i < _nControlPoint; ++i)
      _controlPoints(i) =
          startPos + (i + 0.5) * (endPos - startPos) / (_nControlPoint - 1);

  // Initialize to get the derived variables for the control points
  //  matlab_scatter3(_controlPoints, "initilization");
  updateDerivedVariables();

  if (pr != NULL) pr->setTaskProgressMin(static_cast<int>(pMin + 0.1 * pScale));

  refineModel(points, pr);

  if (pr != NULL && pr->isAborted()) return;
  
  //set offset of QC to zero
  if (QCset)
  {
    blitz::TinyVector<double, 3> nQC = getCoordinates(QC);
    _controlPointOffset -= nQC(0);
  }

  if (pr != NULL)
  {
    pr->setTaskProgressMin(pMin);
    pr->updateProgress(pMin + pScale);
  }
}

void
ShellCoordinateTransform::refineModel(
    std::vector< blitz::TinyVector<double,3> > const &points,
    iRoCS::ProgressReporter *pr)
{
  int pMin = (pr != NULL) ? pr->taskProgressMin() : 0;
  int pScale = (pr != NULL) ? (pr->taskProgressMax() - pMin) : 100;
  if (pr != NULL && !pr->updateProgress(pMin)) return;

  int tail = std::ceil(_margin / _step);
  // std::cout << "  tail: " << tail << std::endl;

  double prScale = 0.025 * pScale; // 0.1 * pScale / 4
  for (int iter = 0; iter < 5; iter++)
  {
    if (pr != NULL && !pr->updateProgress(
            static_cast<int>(pMin + prScale * iter))) return;
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (int i = tail; i < _nControlPoint - tail; ++i)
    {
      blitz::TinyVector<double,3> cog = 0.0;
      double cumWeight = 0.0;
      for (size_t j = 0; j < points.size(); j++)
      {
        double weight = normalizedCoordinates(points[j], i)(2);
        weight = exp(-0.5 * blitz::pow2(weight / (_segmentLength(i) / 2)));
        {
          cumWeight += weight;
          cog += weight * points[j];
        }
      }
      cog /= cumWeight;
      blitz::TinyVector<double, 3> tmpPos(normalizedCoordinates(cog, i));
      tmpPos(2) = 0;
      _controlPoints(i) = denormalizedCoordinates(tmpPos, i);
    }
    smoothControlPoints(tail);
    updateDerivedVariables();
  }

  /*
   * fit ellipse
   */
  _ra.resize(_nControlPoint);
  _rb.resize(_nControlPoint);
  _angleToAxis.resize(_nControlPoint);
  _controlEllipses.resize(_nControlPoint);
  prScale = 0.45 / static_cast<double>(_nControlPoint - 2 * tail - 1);
  for (int outiter = 0; outiter < 2; outiter++)
  {
    double prOffset = 0.1 + 0.45 * outiter;
    if (pr != NULL && !pr->updateProgress(
            static_cast<int>(pMin + pScale * (0.1 + 0.55 * outiter)))) return;

    //get the points in segment
    for (int i = tail; i < _nControlPoint - tail; ++i)
    {
      if (pr != NULL && !pr->updateProgress(
              static_cast<int>(
                  pMin + pScale * (prOffset + prScale * (i - tail))))) return;
      std::vector<cv::Point2f> p;
      double range = _segmentLength(i) / 2;
      do
      {
        p.resize(0);
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (int j = 0; j < int(points.size()); j++)
        {
          blitz::TinyVector<double, 3> tmp(
              normalizedCoordinates(points[j], i));
          if (std::abs(tmp(2)) < range)
          {
#ifdef _OPENMP
#pragma omp critical
#endif
            p.push_back(
                cv::Point2f(
                    static_cast<double>(tmp(0)), static_cast<double>(tmp(1))));
          }
        }
        range *= 1.5;
      }
      while (p.size() < 20);

      //RANSAC
      int nIter = 300;
      double maxConsensus = -1;
      double distThreshold = 0.05;
      blitz::TinyMatrix<double,2,2> bestEllipse;
      blitz::TinyVector<double,2> bestCenter;
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (int iter = 1; iter <= nIter; iter++)
      {
        // Choose arbitrary subset of six points spanning an ellipsoid
        std::vector<cv::Point2f> subset;
        randomSample(p, subset, 6);

        // Fit ellipse
        cv::Mat m(p);
        cv::RotatedRect ellipse = cv::fitEllipse(m);
        double ra = ellipse.size.width / 2;
        double rb = ellipse.size.height / 2;
        double angleToAxis = ellipse.angle * M_PI / 180;
        blitz::TinyVector<double,2> cc(ellipse.center.x, ellipse.center.y);
        blitz::TinyMatrix<double,2,2> a;
        a(0, 0) = cos(angleToAxis);
        a(1, 0) = sin(angleToAxis);
        a(0, 1) = -sin(angleToAxis);
        a(1, 1) = cos(angleToAxis);
        blitz::TinyMatrix<double,2,2> b;
        b = atb::transpose(a);
        blitz::TinyMatrix<double,2,2> d;
        d(0, 0) = 1 / (ra * ra);
        d(1, 0) = 0;
        d(0, 1) = 0;
        d(1, 1) = 1 / (rb * rb);
        a = atb::mmMult(atb::mmMult(a, d), b);

        // Compute consensus set for the ellipse
        double consensus = 0;
        for (std::vector<cv::Point2f>::const_iterator it = p.begin();
             it != p.end(); ++it)
        {
          blitz::TinyVector<double, 2> v(it->x, it->y);
          v -= cc;
          double dist = std::sqrt(blitz::dot(a * v, v));
          if (dist < distThreshold)
              consensus += blitz::pow2(distThreshold - dist);
        }

        // Update optimal ellipse if necessary
        if (consensus > maxConsensus)
        {
#ifdef _OPENMP
#pragma omp critical
#endif
          {
            maxConsensus = consensus;
            bestEllipse = a;
            bestCenter = cc;
          }
        }
      }

      // Compute final ellipse using all points from consensus set
      std::vector<cv::Point2f> subset;
      for (std::vector<cv::Point2f>::const_iterator it = p.begin();
           it != p.end(); ++it)
      {
        blitz::TinyVector<double, 2> v(it->x, it->y);
        v -= bestCenter;
        double dist = std::abs(std::sqrt(blitz::dot(bestEllipse * v, v)) - 1);
        if (dist < distThreshold) subset.push_back(*it);
      }
      cv::Mat m(subset);
      cv::RotatedRect ellipse = cv::fitEllipse(m);
      _ra(i) = ellipse.size.width / 2;
      _rb(i) = ellipse.size.height / 2;
      _angleToAxis(i) = ellipse.angle * M_PI / 180;

      blitz::TinyVector<double,3> cc(
          ellipse.center.x, ellipse.center.y, 0);
      _controlPoints(i) = denormalizedCoordinates(cc, i);
      blitz::TinyMatrix<double,2,2> a;
      a(0, 0) = cos(_angleToAxis(i));
      a(1, 0) = sin(_angleToAxis(i));
      a(0, 1) = -sin(_angleToAxis(i));
      a(1, 1) = cos(_angleToAxis(i));
      blitz::TinyMatrix<double,2,2> b;
      b = atb::transpose(a);
      blitz::TinyMatrix<double,2,2> d;
      d(0, 0) = 1 / (_ra(i) * _ra(i));
      d(1, 0) = 0;
      d(0, 1) = 0;
      d(1, 1) = 1 / (_rb(i) * _rb(i));
      _controlEllipses(i) = atb::mmMult(atb::mmMult(a, d), b);
    }
    smoothControlPoints(tail);
    smoothControlEllipses(tail);
    updateDerivedVariables();
  }

  if (pr != NULL) pr->updateProgress(pMin + pScale);
}

blitz::TinyVector<double,3> ShellCoordinateTransform::getAxisPosition(
    double u) const
{
  double posOnCurve = u * (_controlPointOffset(_nControlPoint - 1) +
                           _segmentLength(_nControlPoint - 1));

  blitz::TinyVector<double,3> res(0.0f);
  double cumWeight = 0.0;
  for (int i = 0; i < _nControlPoint; ++i)
  {
    double weight = posOnCurve - _controlPointOffset(i);
    weight = std::exp(-0.5 * blitz::pow2(weight / (_segmentLength(i) / 2)));
    cumWeight += weight;
    res += weight * _controlPoints(i);
  }
  res /= cumWeight;

  return res;
}

blitz::TinyMatrix<double,3,3> ShellCoordinateTransform::getAxisCoordinateSystem(
    double u) const
{
  double posOnCurve = u * (_controlPointOffset(_nControlPoint - 1) +
                           _segmentLength(_nControlPoint - 1));

  blitz::TinyMatrix<double,3,3> T;
  T = 0.0f;
  double cumWeight = 0.0;
  for (int i = 0; i < _nControlPoint; ++i)
  {
    double weight = posOnCurve - _controlPointOffset(i);
    weight = std::exp(-0.5 * blitz::pow2(weight / (_segmentLength(i) / 2)));
    cumWeight += weight;
    T = T + weight * _controlAxes(i);
  }
  T = T / cumWeight;
  return T;
}

blitz::TinyMatrix<double,2,2> ShellCoordinateTransform::getAxisEllipse(
    double u) const
{
  double posOnCurve = u * (_controlPointOffset(_nControlPoint - 1) +
                           _segmentLength(_nControlPoint - 1));

  blitz::TinyMatrix<double,2,2> ellipse;
  ellipse = 0.0f;
  double cumWeight = 0.0;
  for (int i = 0; i < _nControlPoint; ++i)
  {
    double weight = posOnCurve - _controlPointOffset(i);
    weight = std::exp(-0.5 * blitz::pow2(weight / (_segmentLength(i) / 2)));
    cumWeight += weight;
    ellipse = ellipse + weight * _controlEllipses(i);
  }
  ellipse = ellipse / cumWeight;
  return ellipse;
}

blitz::TinyVector<double,3> ShellCoordinateTransform::getAxisDirection(
    double u) const
{
  double posOnCurve = u * (_controlPointOffset(_nControlPoint - 1) +
                           _segmentLength(_nControlPoint - 1));

  blitz::TinyMatrix<double,3,3> T;
  T = 0.0f;
  double cumWeight = 0.0;
  for (int i = 0; i < _nControlPoint; ++i)
  {
    double weight = posOnCurve - _controlPointOffset(i);
    weight = std::exp(-0.5 * blitz::pow2(weight / (_segmentLength(i) / 2)));
    cumWeight += weight;
    T = T + weight * _controlAxes(i);
  }
  T = T / cumWeight;

  return blitz::TinyVector<double,3>(-T(0, 2), -T(1, 2), T(2, 2));
}

double ShellCoordinateTransform::axisLengthUm() const
{
  return _controlPointOffset(_nControlPoint - 1) +
      _segmentLength(_nControlPoint - 1);
}

blitz::TinyVector<double,3> ShellCoordinateTransform::getCoordinates(
    blitz::TinyVector<double,3> const &pos) const
{
  blitz::TinyVector<double,3> res(0.0f);

  // Will finally store the corresponding interpolated position on the axis
  blitz::TinyVector<double,3> cc(0.0f);

  // Will finally store the interpolated local coordinate system on the axis
  blitz::TinyMatrix<double,3,3> T;
  T = 0.0f;

  // Compute axis position, transformation and length
  double offset0 = 0;
  double cumWeight = 0.0;
  for (int i = 0; i < _nControlPoint; ++i)
  {
    double weight = normalizedCoordinates(pos, i)(2);
    weight = std::exp(-0.5 * blitz::pow2(weight / (_segmentLength(i) / 2)));
    cumWeight += weight;
    cc += weight * _controlPoints(i);
    T = T + weight * _controlAxes(i);
    offset0 += weight * _controlPointOffset(i);
  }
  cc /= cumWeight;
  offset0 /= cumWeight;
  T = T / cumWeight;

  // Rotate the difference vector to the axis point according to the local
  // transformation
  res = T * blitz::TinyVector<double,3>(pos - cc);

  // Compute coordinates
  double angle = std::atan2(res(1), res(0));
  double radialDistance = std::sqrt(res(1) * res(1) + res(0) * res(0));
  double offset = res(2) + offset0;
  return blitz::TinyVector<double, 3>(offset, radialDistance, angle);
}

blitz::TinyVector<double,3>
ShellCoordinateTransform::getCoordinatesWithNormalizedRadius(
    blitz::TinyVector<double,3> const &pos) const
{
  blitz::TinyVector<double,3> res(0.0f);

  // Will finally store the corresponding position on the axis
  blitz::TinyVector<double,3> cc(0.0f);

  blitz::TinyMatrix<double,3,3> T;
  blitz::TinyMatrix<double,2,2> ellipse;
  T = 0;
  ellipse = 0;
  double offset0 = 0;
  double weight, cumWeight = 0.0;
  for (int i = 0; i < _nControlPoint; ++i)
  {
    weight = normalizedCoordinates(pos, i)(2);
    weight = std::exp(-0.5 * blitz::pow2(weight / (_segmentLength(i) / 2))) +
        1e-10;
    cumWeight += weight;
    cc += weight * _controlPoints(i);
    T = T + weight * _controlAxes(i);
    offset0 += weight * _controlPointOffset(i);
    ellipse = ellipse + weight * _controlEllipses(i);
  }
  cc /= cumWeight;
  offset0 /= cumWeight;
  T = T / cumWeight;
  ellipse = ellipse / cumWeight;
  res = T * blitz::TinyVector<double, 3>(pos - cc);
  double angle = atan2(res(1), res(0));
  blitz::TinyVector<double, 2> v(res(0), res(1));
  double radialDistance = std::sqrt(blitz::dot(ellipse * v, v));
  double offset = res(2) + offset0;
  return blitz::TinyVector<double, 3>(offset, radialDistance, angle);
}

blitz::Array<blitz::TinyVector<double,3>,1> const
&ShellCoordinateTransform::controlPoints() const
{
  return _controlPoints;
}

int ShellCoordinateTransform::nLatitudes() const
{
  return _nLatitudes;
}

void ShellCoordinateTransform::setNLatitudes(int nLatitudes)
{
  _nLatitudes = nLatitudes;
}

int ShellCoordinateTransform::nLongitudes() const
{
  return _nLongitudes;
}

void ShellCoordinateTransform::setNLongitudes(int nLongitudes)
{
  _nLongitudes = nLongitudes;
}

atb::SurfaceGeometry const &ShellCoordinateTransform::surface() const
{
  std::vector<atb::SurfaceGeometry::VertexT> &vertices = _surface.vertices();
  if (static_cast<int>(vertices.size()) != _nLatitudes * _nLongitudes)
  {
    std::vector<atb::SurfaceGeometry::NormalT> &normals = _surface.normals();
    std::vector<atb::SurfaceGeometry::IndexT> &indices = _surface.indices();
    vertices.resize(_nLatitudes * _nLongitudes);
    normals.resize(_nLatitudes * _nLongitudes);
    indices.resize(6 * (_nLatitudes - 1) * _nLongitudes);

    // Compute vertices
    for (ptrdiff_t thetaIdx = 0; thetaIdx < _nLatitudes; ++thetaIdx)
    {
      double u = 
          static_cast<double>(thetaIdx) /
          static_cast<double>(_nLatitudes - 1);
      blitz::TinyVector<double,3> pos(getAxisPosition(u));
      blitz::TinyMatrix<double,3,3> coords(getAxisCoordinateSystem(u));
      blitz::TinyMatrix<double,2,2> ellipse(getAxisEllipse(u));
      for (ptrdiff_t phiIdx = 0; phiIdx < _nLongitudes; ++phiIdx)
      {
        double phi = 2.0 * M_PI * static_cast<double>(phiIdx) /
            static_cast<double>(_nLongitudes);
        blitz::TinyVector<double,2> e1Rot(std::cos(phi), std::sin(phi));
        double lambda = 1.0 / std::sqrt(blitz::dot(ellipse * e1Rot, e1Rot));
        blitz::TinyVector<double,3> e1RotScaled3(
            lambda * e1Rot(0), lambda * e1Rot(1), 0.0);
        vertices[thetaIdx * _nLongitudes + phiIdx] =
            atb::invert(coords) * e1RotScaled3 + pos;
      }
    }

    // Set indices
    for (ptrdiff_t thetaIdx = 0; thetaIdx < _nLatitudes - 1;
         ++thetaIdx)
    {
      for (ptrdiff_t phiIdx = 0; phiIdx < _nLongitudes - 1; ++phiIdx)
      {
        indices[6 * (thetaIdx * _nLongitudes + phiIdx)] =
            thetaIdx * _nLongitudes + phiIdx;
        indices[6 * (thetaIdx * _nLongitudes + phiIdx) + 1] =  
            (thetaIdx + 1) * _nLongitudes + (phiIdx + 1);
        indices[6 * (thetaIdx * _nLongitudes + phiIdx) + 2] =
            (thetaIdx + 1) * _nLongitudes + phiIdx;
        indices[6 * (thetaIdx * _nLongitudes + phiIdx) + 3] =
            thetaIdx * _nLongitudes + phiIdx;
        indices[6 * (thetaIdx * _nLongitudes + phiIdx) + 4] =
            thetaIdx * _nLongitudes + (phiIdx + 1);
        indices[6 * (thetaIdx * _nLongitudes + phiIdx) + 5] =
            (thetaIdx + 1) * _nLongitudes + (phiIdx + 1);
      }
      indices[6 * (thetaIdx * _nLongitudes + (_nLongitudes - 1))] =
          thetaIdx * _nLongitudes + (_nLongitudes - 1);
      indices[6 * (thetaIdx * _nLongitudes + (_nLongitudes - 1)) + 1] =
          (thetaIdx + 1) * _nLongitudes;
      indices[6 * (thetaIdx * _nLongitudes + (_nLongitudes - 1)) + 2] =
          (thetaIdx + 1) * _nLongitudes + (_nLongitudes - 1);
      indices[6 * (thetaIdx * _nLongitudes + (_nLongitudes - 1)) + 3] =
          thetaIdx * _nLongitudes + (_nLongitudes - 1);
      indices[6 * (thetaIdx * _nLongitudes + (_nLongitudes - 1)) + 4] =
          thetaIdx * _nLongitudes;
      indices[6 * (thetaIdx * _nLongitudes + (_nLongitudes - 1)) + 5] =
          (thetaIdx + 1) * _nLongitudes;
    }

    // Finally interpolate the normals
    _surface.computeDefaultNormals();
  }
  return _surface;
}

void ShellCoordinateTransform::save(
    BlitzH5File &outfile, std::string const &groupName) const
{
  outfile.writeAttribute(
      std::string("Shell Coordinate Transform"), "Type", groupName);
  outfile.writeAttribute(_nControlPoint, "nControlPoints", groupName);
  outfile.writeDataset(_controlPoints, groupName + "/controlPoints");
  outfile.writeDataset(_controlAxes, groupName + "/controlAxes");
  outfile.writeDataset(_segments, groupName + "/segments");
  outfile.writeDataset(_controlPointOffset, groupName + "/controlPointOffset");
  outfile.writeDataset(_segmentDirection, groupName + "/segmentDirection");
  outfile.writeDataset(_segmentLength, groupName + "/segmentLength");
  outfile.writeDataset(_axis, groupName + "/axis");
  outfile.writeDataset(_angle, groupName + "/angle");
  outfile.writeAttribute(_minX, "minX", groupName);
  outfile.writeAttribute(_minX, "maxX", groupName);
  outfile.writeAttribute(_minX, "step", groupName);
  outfile.writeAttribute(_minX, "margin", groupName);
  outfile.writeDataset(_ra, groupName + "/ra");
  outfile.writeDataset(_rb, groupName + "/rb");
  outfile.writeDataset(_angleToAxis, groupName + "/angleToAxis");
  outfile.writeDataset(_controlEllipses, groupName + "/controlEllipses");
}

void ShellCoordinateTransform::save(
    std::string const &fileName, std::string const &groupName) const
{
  BlitzH5File outFile(fileName, BlitzH5File::WriteOrNew);
  save(outFile, groupName);
}

void ShellCoordinateTransform::load(
    BlitzH5File const &infile, std::string const &groupName)
{
  std::string type;
  infile.readAttribute(type, "Type", groupName);
  if (type != "Shell Coordinate Transform") throw BlitzH5Error(
      "Error while reading Shell coordinate transform: The group " +
      groupName + " does not contain a Shell Coordinate transform.");
  infile.readAttribute(_nControlPoint, "nControlPoints", groupName);
  infile.readDataset(_controlPoints, groupName + "/controlPoints");
  infile.readDataset(_controlAxes, groupName + "/controlAxes");
  infile.readDataset(_segments, groupName + "/segments");
  infile.readDataset(_controlPointOffset, groupName + "/controlPointOffset");
  infile.readDataset(_segmentDirection, groupName + "/segmentDirection");
  infile.readDataset(_segmentLength, groupName + "/segmentLength");
  infile.readDataset(_axis, groupName + "/axis");
  infile.readDataset(_angle, groupName + "/angle");
  infile.readAttribute(_minX, "minX", groupName);
  infile.readAttribute(_minX, "maxX", groupName);
  infile.readAttribute(_minX, "step", groupName);
  infile.readAttribute(_minX, "margin", groupName);
  infile.readDataset(_ra, groupName + "/ra");
  infile.readDataset(_rb, groupName + "/rb");
  infile.readDataset(_angleToAxis, groupName + "/angleToAxis");
  infile.readDataset(_controlEllipses, groupName + "/controlEllipses");
}

void ShellCoordinateTransform::load(
    std::string const &fileName, std::string const &groupName)
{
  BlitzH5File inFile(fileName);
  load(inFile, groupName);
}

blitz::TinyVector<double,3>
ShellCoordinateTransform::normalizedCoordinates(
    blitz::TinyVector<double,3> const &pos, int controlPoint) const
{
  return _controlAxes(controlPoint) *
      blitz::TinyVector<double, 3>(pos - _controlPoints(controlPoint));
}

blitz::TinyVector<double,3>
ShellCoordinateTransform::denormalizedCoordinates(
    blitz::TinyVector<double,3> const &pos, int controlPoint) const
{
  return mvMult_transpose(_controlAxes(controlPoint), pos) +
      _controlPoints(controlPoint);
}

double ShellCoordinateTransform::computeDistanceToLine(
    blitz::TinyVector<double,3> const &pos, int controlPoint,
    double &offset, double &radialDistance, double &angle) const
{
  // Project point onto segment
  blitz::TinyVector<double, 3> posN = normalizedCoordinates(pos, controlPoint);

  angle = atan2(posN(1), posN(0));
  radialDistance = std::sqrt(posN(1) * posN(1) + posN(0) * posN(0));
  offset = posN(2);
  blitz::TinyVector<double,3> d(
      _controlPoints(controlPoint) + offset * _segmentDirection(controlPoint) -
      pos);
  return std::sqrt(blitz::dot(d, d));
}

double ShellCoordinateTransform::computeDistanceToLineSegment(
    blitz::TinyVector<double,3> const &pos, int controlPoint,
    double &offset, double &radialDistance, double &angle) const
{
  double dist = computeDistanceToLine(
      pos, controlPoint, offset, radialDistance, angle);
  if (offset >= -_segmentLength(controlPoint) / 2 &&
      offset < _segmentLength(controlPoint) / 2) return dist;
  if (offset < -_segmentLength(controlPoint) / 2)
      offset = -_segmentLength(controlPoint) / 2;
  if (offset > _segmentLength(controlPoint) / 2)
      offset = _segmentLength(controlPoint) / 2;
  blitz::TinyVector<double,3> d(
      _controlPoints(controlPoint) + offset * _segmentDirection(controlPoint) -
      pos);
  return std::sqrt(blitz::dot(d, d));
}

void ShellCoordinateTransform::updateDerivedVariables()
{
  blitz::TinyVector<double,3> e1(0.0, 0.0, 1.0);
  blitz::TinyVector<double,3> e2(0.0, 1.0, 0.0);
  blitz::TinyVector<double,3> e3(1.0, 0.0, 0.0);
  _segments.resize(_nControlPoint);
  _segmentLength.resize(_nControlPoint);
  _controlPointOffset.resize(_nControlPoint);
  _controlPointOffset(0) = 0.0;
  _segmentDirection.resize(_nControlPoint);
  _axis.resize(_nControlPoint);
  _angle.resize(_nControlPoint);
  _controlAxes.resize(_nControlPoint);

  for (int i = 0; i < _nControlPoint; ++i)
  {
    if (i == _nControlPoint - 1) _segments(i) = _segments(i - 1);
    else _segments(i) = _controlPoints(i + 1) - _controlPoints(i);
    _segmentLength(i) = std::sqrt(blitz::dot(_segments(i), _segments(i)));
    if (i < _nControlPoint - 1)
        _controlPointOffset(i + 1) = _controlPointOffset(i) + _segmentLength(i);
  }

  for (int i = 0; i < _nControlPoint; ++i)
  {
    if (i > 0) _segments(i) = 0.5 * (_segments(i) + _segments(i - 1));
    _segmentLength(i) = std::sqrt(blitz::dot(_segments(i), _segments(i)));
    _segmentDirection(i) = _segments(i) / _segmentLength(i);
    _axis(i) = blitz::cross(_segmentDirection(i), e1);
    _angle(i) = acos(blitz::dot(_segmentDirection(i), e1));
    if (_angle(i) == 0) _axis(i) = e2;
    blitz::TinyVector<double,3> d(
        atb::rotate(_segmentDirection(i), _axis(i), _angle(i)) - e1);
    if (blitz::dot(d, d) > 1.0) _angle(i) = -_angle(i);

    blitz::TinyVector<double,3> Te1(atb::rotate(e1, _axis(i), _angle(i)));
    blitz::TinyVector<double,3> Te2(atb::rotate(e2, _axis(i), _angle(i)));
    blitz::TinyVector<double,3> Te3(atb::rotate(e3, _axis(i), _angle(i)));

    for (int j = 0; j < 3; ++j)
    {
      _controlAxes(i)(j, 0) = Te3(j);
      _controlAxes(i)(j, 1) = Te2(j);
      _controlAxes(i)(j, 2) = Te1(j);
    }
  }
}

void ShellCoordinateTransform::smoothControlPoints(int tail)
{
  blitz::Array<blitz::TinyVector<double,3>,1> controlPointsBuff(
      _nControlPoint);

  //smooth control points
  controlPointsBuff(tail) =
      (_controlPoints(tail) + 2 * _controlPoints(tail + 1) -
       _controlPoints(tail + 2)) / 2;
  for (int i = tail + 1; i < _nControlPoint - tail - 1; ++i)
      controlPointsBuff(i) =
          (_controlPoints(i - 1) + _controlPoints(i) +
           _controlPoints(i + 1)) / 3;
  int end = _nControlPoint - tail - 1;
  controlPointsBuff(end) =
      (_controlPoints(end) + 2 * _controlPoints(end - 1) -
       _controlPoints(end - 2)) / 2;

  for (int i = tail; i < _nControlPoint - tail; ++i)
      _controlPoints(i) = controlPointsBuff(i);

  //extrapolate
  for (int i = tail - 1; i >= 0; --i)
      _controlPoints(i) =
          _controlPoints(i + 1) +
          (_controlPoints(i + 1) - _controlPoints(i + 3)) / 2;
  for (int i = _nControlPoint - tail; i < _nControlPoint; ++i)
      _controlPoints(i) =
          _controlPoints(i - 1) +
          (_controlPoints(i - 1) - _controlPoints(i - 3)) / 2;
}

void ShellCoordinateTransform::smoothControlEllipses(int tail)
{
  blitz::Array<blitz::TinyMatrix<double,2,2>,1> controlPointsBuff(
      _nControlPoint);

  //smooth control points
  controlPointsBuff(tail) =
      0.5 * (_controlEllipses(tail) + 2.0 * _controlEllipses(tail + 1) -
              _controlEllipses(tail + 2));
  for (int i = tail + 1; i < _nControlPoint - tail - 1; ++i)
      controlPointsBuff(i) =
          (1.0 / 3.0) * (_controlEllipses(i - 1) + _controlEllipses(i) +
           _controlEllipses(i + 1));
  int end = _nControlPoint - tail - 1;
  controlPointsBuff(end) =
      0.5 * (_controlEllipses(end) + 2.0 * _controlEllipses(end - 1) -
              _controlEllipses(end - 2));
  for (int i = tail; i < _nControlPoint - tail; ++i)
      _controlEllipses(i) = controlPointsBuff(i);

  //extrapolate
  for (int i = tail - 1; i >= 0; --i)
      _controlEllipses(i) = _controlEllipses(i + 1) +
          0.5 * (_controlEllipses(i + 1) - _controlEllipses(i + 3));
  for (int i = _nControlPoint - tail; i < _nControlPoint; ++i)
      _controlEllipses(i) = _controlEllipses(i - 1) +
          0.5 * (_controlEllipses(i - 1) - _controlEllipses(i - 3));
}

