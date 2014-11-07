#include "iRoCS.hh"

#include "ATBDataSynthesis.hh"
#include "ATBMorphology.hh"
#include "SeparableConvolutionFilter.hh"

namespace atb
{
  
  IRoCS::IRoCS(iRoCS::ProgressReporter *progressReporter)
          : p_progress(progressReporter), _nLatitudes(100), _nLongitudes(50)
  {
    p_ccm = new CoupledBSplineModel<3>();
    _trafo = traits< blitz::TinyMatrix<double,4,4> >::one;
    _trafoInv = _trafo;
    _vertices.free();
    _indices.free();
    _normals.free();
    p_curveLengthCache = new double[32768];
  }

  IRoCS::~IRoCS()
  {
    delete p_ccm;
    delete[] p_curveLengthCache;
  }
  
  void IRoCS::setProgressReporter(iRoCS::ProgressReporter *progressReporter)
  {
    p_progress = progressReporter;
  }

  void IRoCS::fit(
      blitz::TinyVector<double,3> const &qcPositionUm,
      std::list<Nucleus> const &nuclei, double kappa, double lambda,
      double mu, int nIter, double tau, double searchRadiusUm)
  {
    // Extract Epidermis nuclei and call fit for point cloud
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            "Extracting Epidermis nuclei")) return;
    std::vector< blitz::TinyVector<double,3> > positionsUm;
    for (std::list<Nucleus>::const_iterator it = nuclei.begin();
         it != nuclei.end(); ++it)
        if (it->label() == 2) positionsUm.push_back(it->positionUm());
    fit(qcPositionUm, positionsUm, kappa, lambda, mu, nIter, tau,
        searchRadiusUm);
  }

  void IRoCS::fit(
      blitz::TinyVector<double,3> const &qcPositionUm,
      std::vector<Nucleus> const &nuclei, double kappa, double lambda,
      double mu, int nIter, double tau, double searchRadiusUm)
  {
    // Extract Epidermis nuclei and call fit for point cloud
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            "Extracting Epidermis nuclei")) return;
    std::vector< blitz::TinyVector<double,3> > positionsUm;
    for (std::vector<Nucleus>::const_iterator it = nuclei.begin();
         it != nuclei.end(); ++it)
        if (it->label() == 2) positionsUm.push_back(it->positionUm());
    fit(qcPositionUm, positionsUm, kappa, lambda, mu, nIter, tau,
        searchRadiusUm);
  }

  void IRoCS::fit(
      blitz::TinyVector<double,3> const &qcPositionUm,
      Array<int,3> const &segmentation, double kappa, double lambda,
      double mu, int nIter, double tau, double searchRadiusUm)
  {
    // Create foreground mask
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            "Computing foreground mask")) return;
    Array<float,3> bw(segmentation.shape(), segmentation.elementSizeUm());
    bw = blitz::where(segmentation > 1, 1.0f, 0.0f);

    // Down-sample foreground mask by factor 4
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            "Downsampling foreground mask")) return;
    Array<float,3> bwScaled(
        blitz::TinyVector<BlitzIndexT,3>(bw.shape() / 4),
        blitz::TinyVector<double,3>(segmentation.elementSizeUm() * 4));
    for (BlitzIndexT z = 0; z < bwScaled.extent(0); ++z)
        for (BlitzIndexT y = 0; y < bwScaled.extent(1); ++y)
            for (BlitzIndexT x = 0; x < bwScaled.extent(2); ++x)
                bwScaled(z, y, x) = bw(4 * z, 4 * y, 4 * x);
    bw.resize(bwScaled.shape());
    bw.setElementSizeUm(bwScaled.elementSizeUm());
    std::memcpy(bw.data(), bwScaled.data(), bwScaled.size() * sizeof(float));
    bwScaled.free();

    // Convolve foreground mask with Gaussian
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            "Smoothing foreground mask")) return;
    SeparableConvolutionFilter<float,3> smoothingFilter(RepeatBT);
    blitz::Array<float,1> kernel;
    for (int d = 0; d < 3; ++d)
    {
      gaussian(
          kernel, blitz::TinyVector<double,1>(10.0),
          blitz::TinyVector<double,1>(bw.elementSizeUm()(d)), NORMALIZE);
      smoothingFilter.setKernelForDim(&kernel, d);
    }
    smoothingFilter.apply(bw, bw.elementSizeUm(), bw);

    // Only keep compact regions
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            "Removing fragmented regions")) return;
    std::cout << "Removing fragmented regions" << std::endl;
    blitz::Array<bool,3> shell(bw.shape());
    shell = bw > 0.5f;

    // Enumerate the regions
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            "Labelling remaining regions")) return;
    Array<BlitzIndexT,3> bwL(bw.shape(), bw.elementSizeUm());
    connectedComponentLabelling(shell, bwL);
    
    // And get their volumes
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            "Computing segment volumes")) return;
    ptrdiff_t nSeg = blitz::max(bwL);
    blitz::Array<ptrdiff_t,1> volumes(nSeg + 1);
    volumes = 0;
    for (size_t i = 0; i < static_cast<size_t>(bwL.size()); ++i)
        volumes(bwL.data()[i])++;
    volumes(0) = 0; // Exclude the background region
    ptrdiff_t largestLabel = (blitz::maxIndex(volumes))(0);
    std::cout << "  Volumes (vx): " << volumes << std::endl;
    
    // Get boundary (shell) of the largest compact region
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            "Detecting segment boundary")) return;
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (size_t i = 0; i < static_cast<size_t>(shell.size()); ++i)
    {
      shell.data()[i] = false;
      if (bwL.data()[i] != largestLabel) continue;
      
      ptrdiff_t tmp = i;
      blitz::TinyVector<ptrdiff_t,3> pos;
      for (int d = 2; d >= 0; --d)
      {
        pos(d) = tmp % shell.extent(d);
        tmp /= shell.extent(d);
      }

      for (int d = 0; d < 3; ++d)
      {
        blitz::TinyVector<ptrdiff_t,3> nPos(pos);
        nPos(d) = pos(d) - 1;
        if (nPos(d) >= 0 && bwL(nPos) != largestLabel)
        {
          shell.data()[i] = true;
          break;
        }
        nPos(d) = pos(d) + 1;
        if (nPos(d) < bwL.extent(d) && bwL(nPos) != largestLabel)
        {
          shell.data()[i] = true;
          break;
        }
      }
    }
    
    // Get the shell points
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            "Extracting boundary points")) return;
    std::vector< blitz::TinyVector<double,3> > positionsUm;
    for (size_t i = 0; i < static_cast<size_t>(shell.size()); ++i)
    {
      if (shell.data()[i]) 
      {
        ptrdiff_t tmp = i;
        blitz::TinyVector<double,3> pos;
        for (int d = 2; d >= 0; --d)
        {
          pos(d) = static_cast<double>(tmp % shell.extent(d));
          tmp /= shell.extent(d);
        }
        positionsUm.push_back(
            blitz::TinyVector<double,3>(pos * bw.elementSizeUm()));
      }
    }
    
     // Call point based fitting
    fit(qcPositionUm, positionsUm, kappa, lambda, mu, nIter, tau,
        searchRadiusUm);
  }

  void IRoCS::fit(
      blitz::TinyVector<double,3> const &qcPositionUm,
      std::vector< blitz::TinyVector<double,3> > &positionsUm,
      double kappa, double lambda, double mu, int nIter, double tau,
      double searchRadiusUm)
  {
    _kappa = kappa;
    _lambda = lambda;
    _mu = mu;
    _tau = tau;
    _nIter = nIter;
    _qcPos = qcPositionUm;
    _searchRadiusUm = searchRadiusUm;
    _vertices.free();
    _normals.free();
    _indices.free();

    // Get euclidean transformation using eigenvalue decomposition of
    // the autocovariance matrix of the point cloud
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            "Computing euclidean normalization transformation")) return;
    
    // Computing center of gravity
    blitz::TinyVector<double,3> center(0.0);
    for (std::vector< blitz::TinyVector<double,3> >::const_iterator it =
             positionsUm.begin(); it != positionsUm.end(); ++it)
        center += *it;
    center /= static_cast<double>(positionsUm.size());
    blitz::TinyMatrix<double,4,4> T(
        traits< blitz::TinyMatrix<double,4,4> >::one);
    for (int d = 0; d < 3; ++d) T(d, 3) = -center(d);
    std::cout << "  Translation (um): " << center << std::endl;

    // Computing auto-covariance matrix
    blitz::TinyMatrix<double,3,3> cov(
        traits< blitz::TinyMatrix<double,3,3> >::zero);
    for (std::vector< blitz::TinyVector<double,3> >::const_iterator it =
             positionsUm.begin(); it != positionsUm.end(); ++it)
    {
      blitz::TinyVector<double,3> p(*it - center);
      cov = cov + outerProduct(p, p);
    }
    cov = 1.0 / static_cast<double>(positionsUm.size()) * cov;

    // Performing eigenvalue decomposition
    blitz::TinyMatrix<double,3,3> rotation;
    blitz::TinyVector<double,3> scales;
    eigenvalueDecompositionRealSymmetric(cov, rotation, scales, Descending);
    blitz::TinyMatrix<double,4,4> R(
        traits< blitz::TinyMatrix<double,4,4> >::one);
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c) R(r, c) = rotation(r, c);

    // If the eigensystem contains a flip, change the sign of the first
    // eigenvector to get a proper (right-handed) rotation
    if (atb::determinant(rotation) < 0.0)
        for (int d = 0; d < 3; ++d) rotation(d, 0) = -rotation(d, 0);

    std::cout << "  Rotation        : " << rotation << std::endl;
    std::cout << "  Scale           : "
              << blitz::TinyVector<double,3>(blitz::sqrt(scales))
              << std::endl;

    // Transforms measured points to normalized coordinates
    _trafoInv = mmMult(transpose(R), T);

    // Transforms normalized points to data coordinate system
    _trafo = invert(_trafoInv);

    // Ensure that the root points in negative z-direction
    blitz::TinyVector<double,3> qcPosNormalized(
        homogeneousToEuclidean(
            mvMult(_trafoInv, euclideanToHomogeneous(_qcPos))));
    if (qcPosNormalized(0) > 0)
    {
      // Rotate the root by 180 degrees around the shortest axis
      blitz::TinyMatrix<double,4,4> rotPiShortAxis;
      rotPiShortAxis =
          -1.0, 0.0, 0.0, 0.0,
          0.0, -1.0, 0.0, 0.0,
          0.0, 0.0, 1.0, 0.0,
          0.0, 0.0, 0.0, 1.0;
      _trafo = mmMult(_trafo, rotPiShortAxis);
      _trafoInv = invert(_trafo);
      qcPosNormalized = homogeneousToEuclidean(
          mvMult(_trafoInv, euclideanToHomogeneous(_qcPos)));
    }
    std::cout << "  Transformation  : " << _trafo << std::endl;

    // Transform points and sort them by z-coordinate
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            "Transforming nuclei")) return;
    std::multimap< double, blitz::TinyVector<double,2> > mappedPoints;
    for (std::vector< blitz::TinyVector<double,3> >::iterator it =
             positionsUm.begin(); it != positionsUm.end(); ++it)
    {
      *it = homogeneousToEuclidean(
          mvMult(_trafoInv, euclideanToHomogeneous(*it)));
      mappedPoints.insert(
          std::pair< double,blitz::TinyVector<double,2> >(
              (*it)(0), blitz::TinyVector<double,2>((*it)(1), (*it)(2))));
    }
    std::cout << "  Nucleus range along main axis is: "
              << mappedPoints.begin()->first << " - "
              << (--mappedPoints.end())->first << " um" << std::endl;
    std::cout << "  Normalized QC position is " << qcPosNormalized << " um"
              << std::endl;

    // Initialize CoupledBSplineModel
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            "Initializing Coupled Curves Model")) return;
    std::vector<double> knots(p_ccm->axis().nKnots());
    for (size_t i = 0;
         i < static_cast<size_t>(p_ccm->axis().degree()); ++i)
        knots[i] = 0.0;
    for (size_t i = p_ccm->axis().degree();
         i < p_ccm->axis().nKnots() - p_ccm->axis().degree() - 1; ++i)
        knots[i] = static_cast<double>(i - p_ccm->axis().degree());
    for (size_t i = p_ccm->axis().nKnots() - p_ccm->axis().degree() - 1;
         i < p_ccm->axis().nKnots(); ++i)
        knots[i] = static_cast<double>(
            p_ccm->axis().nKnots() - 2 * p_ccm->axis().degree() - 1);
    p_ccm->axis().setKnots(knots);
    p_ccm->thickness().setKnots(knots);

    double rootLength = (--mappedPoints.end())->first -
        mappedPoints.begin()->first;
    double ellipseFraction = 0.2;
    blitz::TinyVector<double,3> leftEndPoint(
        -0.5 * ellipseFraction * rootLength, 0.0, 0.0);

    double avgDist = 0.0;
    int nPoints = 0;
    for (std::multimap< double, blitz::TinyVector<double,2> >::iterator it =
             mappedPoints.lower_bound(-0.5 * ellipseFraction * rootLength);
         it != mappedPoints.lower_bound(0.5 * ellipseFraction * rootLength);
         ++it)
    {
      avgDist += std::sqrt(blitz::dot(it->second, it->second));
      nPoints++;
    }
    avgDist /= static_cast<double>(nPoints);

    for (int i = 0; i < 4; ++i)
    {
      // Set axis to straight line of 20% root length
      p_ccm->axis().setControlPoint(
          i, leftEndPoint +
          static_cast<double>(i) / 3.0 *
          blitz::TinyVector<double,3>(ellipseFraction * rootLength, 0.0, 0.0));
      // Set initial thickness to average point distance
      p_ccm->thickness().setControlPoint(i, avgDist);
    }
    std::vector<double> regularizerParameters;
    if (searchRadiusUm <= 0.0) regularizerParameters.push_back(avgDist / 2.0);
    else regularizerParameters.push_back(searchRadiusUm);
    
    std::cout << "Initial axis: " << p_ccm->axis() << std::endl;
    std::cout << "Initial thickness: " << p_ccm->thickness() << std::endl;    

    // Fit Coupled curves model
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            "Fitting Coupled Curves Model")) return;
    std::cout << "Fitting Coupled Curves Model" << std::endl;
    fitCoupledSplines(
        positionsUm, *p_ccm, _kappa, _lambda, _mu, _nIter, _tau,
        std::numeric_limits<double>::infinity(), tikhonovCutoffRegularizer,
        dTikhonovCutoffRegularizer, regularizerParameters, p_progress);

    // Transform axis to recording coordinates
    for (size_t i = 0; i < p_ccm->axis().nControlPoints(); ++i)
    {
      p_ccm->axis().setControlPoint(
          i, homogeneousToEuclidean(
              mvMult(_trafo, euclideanToHomogeneous(
                         p_ccm->axis().controlPoint(i)))));
    }

    // Initialize curve length cache
    if (p_progress != NULL &&
        !p_progress->updateProgressMessage(
            "Initializing Curve Integral cache")) return;
    double qcDist = extendedDistance(p_ccm->axis(), _qcPos, _uQC);
    std::cout << "  QC: u = " << _uQC << ", dist = " << qcDist << std::endl;
    double uMin = p_ccm->axis().knot(0);
    double uMax = p_ccm->axis().knot(p_ccm->axis().nKnots() - 1);
    double uRange = uMax - uMin;
    p_curveLengthCache[0] = p_ccm->axis().extendedCurveIntegral(
        _uQC, uMin - uRange);
    for (int i = 1; i < 32768; ++i)
    {
      double u = static_cast<double>(i) / 32768.0 * 3.0 * uRange +
          uMin - uRange;
      double uOld = static_cast<double>(i - 1) / 32768.0 * 3.0 * uRange +
          uMin - uRange;
      p_curveLengthCache[i] = p_curveLengthCache[i - 1] +
          p_ccm->axis().extendedCurveIntegral(uOld, u);
    }

    if (p_progress != NULL && !p_progress->updateProgress(100)) return;
  }
  
  blitz::TinyVector<double,3> IRoCS::getCoordinates(
      const blitz::TinyVector<double,3>& pos) const
  {
    blitz::TinyVector<double,3> res;
    double uOpt;
    
    double uMin = p_ccm->axis().knot(0);
    double uMax = p_ccm->axis().knot(p_ccm->axis().nKnots() - 1);
    double uRange = uMax - uMin;

    res(1) = extendedDistance(p_ccm->axis(), pos, uOpt);
    ptrdiff_t index = static_cast<ptrdiff_t>(
        (uOpt - uMin + uRange) * 32768.0 / (3.0 * uRange));
    if (index < 0) index = 0;
    if (index >= 32768) index = 32767;
    double uIndex =
        static_cast<double>(index) / 32768.0 * 3.0 * uRange + uMin - uRange;
    res(0) = p_curveLengthCache[index] +
        p_ccm->axis().extendedCurveIntegral(uIndex, uOpt);
    blitz::TinyVector<double,3> nPos(
        homogeneousToEuclidean(
            mvMult(_trafoInv, euclideanToHomogeneous(pos))));
    blitz::TinyVector<double,3> axisPos(
        homogeneousToEuclidean(
            mvMult(_trafoInv, euclideanToHomogeneous(
                       getAxisPosition(uOpt)))));
    res(2) = std::atan2(nPos(2) - axisPos(2), nPos(1) - axisPos(1));
    return res;
  }
  
/*======================================================================*/
/*! 
 *   Get the signed euclidean distance of the given point to the tubular
 *   surface. Negative values indicate, that the point is inside the
 *   tubular structure, positive values mean it is outside.
 *
 *   \param pos The point position
 *
 *   \return The distance of the given position to the surface of the tubular
 *     structure.
 */
/*======================================================================*/
  double IRoCS::getDistanceToSurface(
      blitz::TinyVector<double,3> const &pos) const
  {
    double uOpt;
    double distToAxis = extendedDistance(p_ccm->axis(), pos, uOpt);
    return distToAxis - p_ccm->thickness()(
        uOpt, p_ccm->axis().knot(0), p_ccm->axis().knot(
            p_ccm->axis().nKnots() - 1));
  }
  
  blitz::TinyVector<double,3> IRoCS::getAxisPosition(
      double u) const
  {
    return p_ccm->axis()(
        u, p_ccm->axis().knot(0), p_ccm->axis().knot(
            p_ccm->axis().nKnots() - 1));
  }
  
  double IRoCS::thickness(double u) const
  {
    return p_ccm->thickness()(
        u, p_ccm->axis().knot(0), p_ccm->axis().knot(
            p_ccm->axis().nKnots() - 1));
  }
  
  CoupledBSplineModel<3> const &IRoCS::ccm() const
  {
    return *p_ccm;
  }

  BSpline< blitz::TinyVector<double,3> > const &IRoCS::axisSpline() const
  {
    return (*p_ccm).axis();
  }

  BSpline<double> const &IRoCS::thicknessSpline() const
  {
    return (*p_ccm).thickness();
  }

  double IRoCS::uQC() const
  {
    return _uQC;
  }

  blitz::TinyMatrix<double,4,4> const &IRoCS::normalizationTransformation()
      const
  {
    return _trafo;
  }
  
  blitz::TinyMatrix<double,4,4> const
  &IRoCS::inverseNormalizationTransformation() const
  {
    return _trafoInv;
  }
  
  blitz::Array<blitz::TinyVector<double,3>,1> &IRoCS::vertices()
  {
    if (_vertices.size() == 0)
    {
      _vertices.resize(_nLatitudes * _nLongitudes);
      _normals.resize(_nLatitudes * _nLongitudes);
      for (ptrdiff_t thetaIdx = 0; thetaIdx < _nLatitudes; ++thetaIdx)
      {
        double u = 
            static_cast<double>(thetaIdx) /
            static_cast<double>(_nLatitudes - 1) *
            (p_ccm->axis().knot(p_ccm->axis().nKnots() - 1) -
             p_ccm->axis().knot(0)) + p_ccm->axis().knot(0);
        blitz::TinyVector<double,3> pos(p_ccm->axis()(u));
        double t = p_ccm->thickness()(u);
        blitz::TinyVector<double,3> tangent(p_ccm->axis().derivative(u));
        tangent /= std::sqrt(blitz::dot(tangent, tangent));
        blitz::TinyVector<double,3> normal(1.0, 0.0, 0.0);
        normal -= blitz::dot(normal, tangent) * tangent;
        normal /= std::sqrt(blitz::dot(normal, normal));
        for (ptrdiff_t phiIdx = 0; phiIdx < _nLongitudes; ++phiIdx)
        {
          double phi = 2.0 * M_PI * static_cast<double>(phiIdx) /
              static_cast<double>(_nLongitudes);
          _normals(thetaIdx * _nLongitudes + phiIdx) =
              rotate(normal, tangent, phi);
          _vertices(thetaIdx * _nLongitudes + phiIdx) =
              pos + _normals(thetaIdx * _nLongitudes + phiIdx) * t;
        }
      }
    }
    return _vertices;
  }
  
  blitz::Array<blitz::TinyVector<ptrdiff_t,3>,1> &IRoCS::indices()
  {
    if (_indices.size() == 0)
    {
      if (_vertices.size() == 0) vertices();
      _indices.resize(2 * (_nLatitudes - 1) * _nLongitudes);
      for (ptrdiff_t thetaIdx = 0; thetaIdx < _nLatitudes - 1;
           ++thetaIdx)
      {
        for (ptrdiff_t phiIdx = 0; phiIdx < _nLongitudes - 1; ++phiIdx)
        {
          _indices(2 * (thetaIdx * _nLongitudes + phiIdx)) =
              thetaIdx * _nLongitudes + phiIdx,
              (thetaIdx + 1) * _nLongitudes + (phiIdx + 1),
              (thetaIdx + 1) * _nLongitudes + phiIdx;
          _indices(2 * (thetaIdx * _nLongitudes + phiIdx) + 1) =
              thetaIdx * _nLongitudes + phiIdx,
              thetaIdx * _nLongitudes + (phiIdx + 1),
              (thetaIdx + 1) * _nLongitudes + (phiIdx + 1);
        }
        _indices(2 * (thetaIdx * _nLongitudes + (_nLongitudes - 1))) =
            thetaIdx * _nLongitudes + (_nLongitudes - 1),
            (thetaIdx + 1) * _nLongitudes,
            (thetaIdx + 1) * _nLongitudes + (_nLongitudes - 1);
        _indices(2 * (thetaIdx * _nLongitudes + (_nLongitudes - 1)) + 1) =
            thetaIdx * _nLongitudes + (_nLongitudes - 1),
            thetaIdx * _nLongitudes,
            (thetaIdx + 1) * _nLongitudes;
      }
    }
    return _indices;
  }
  
  blitz::Array<blitz::TinyVector<double,3>,1> &IRoCS::normals()
  {
    if (_normals.size() == 0) vertices();
    return _normals;
  }

  void IRoCS::save(
      const std::string& fileName, const std::string& groupName,
      bool throwErrors) const
  {
    try
    {
      BlitzH5File outFile(fileName, BlitzH5File::WriteOrNew);
      save(outFile, groupName);
    }
    catch (BlitzH5Error& e)
    {
      if (throwErrors) throw e;
      else
      {
        std::cerr << "Error: Could not save axisModel: " << e.what()
                  << std::endl;
        exit(-1);
      }
    }
  }
  
  void IRoCS::load(
      const std::string& fileName, const std::string& groupName,
      bool throwErrors)
  {
    try
    {
      BlitzH5File inFile(fileName);
      load(inFile, groupName);
    }
    catch (BlitzH5Error& e)
    {
      if (throwErrors) throw e;
      else
      {
        std::cerr << "Error: Could not load axisModel: " << e.what()
                  << std::endl;
        exit(-1);
      }
    }
  }
  
  void IRoCS::save(
      BlitzH5File &outFile, const std::string& groupName) const
  {
    p_ccm->axis().save(outFile, groupName + "/axis");
    p_ccm->thickness().save(outFile, groupName + "/thickness");
    outFile.writeDataset(_trafo, groupName + "/normalizeTrafo");
    outFile.writeAttribute(_qcPos, "qcPositionUm", groupName);
    outFile.writeAttribute(_kappa, "kappa", groupName);
    outFile.writeAttribute(_lambda, "lambda", groupName);
    outFile.writeAttribute(_mu, "mu", groupName);
    outFile.writeAttribute(_tau, "tau", groupName);
    outFile.writeAttribute(_nIter, "nIter", groupName);
  }
  
  void IRoCS::load(
      BlitzH5File const &inFile, const std::string& groupName)
  {
    _vertices.free();
    _indices.free();
    _normals.free();

    p_ccm->axis().load(inFile, groupName + "/axis");
    p_ccm->thickness().load(inFile, groupName + "/thickness");
    inFile.readDataset(_trafo, groupName + "/normalizeTrafo");
    _trafoInv = invert(_trafo);
    inFile.readAttribute(_qcPos, "qcPositionUm", groupName);

    extendedDistance(p_ccm->axis(), _qcPos, _uQC);
    double uMin = p_ccm->axis().knot(0);
    double uMax = p_ccm->axis().knot(p_ccm->axis().nKnots() - 1);
    double uRange = uMax - uMin;
    p_curveLengthCache[0] = p_ccm->axis().extendedCurveIntegral(
        _uQC, uMin - uRange);
    for (int i = 1; i < 32768; ++i)
    {
      double u = static_cast<double>(i) / 32768.0 * 3.0 * uRange +
          uMin - uRange;
      double uOld = static_cast<double>(i - 1) / 32768.0 * 3.0 * uRange +
          uMin - uRange;
      p_curveLengthCache[i] = p_curveLengthCache[i - 1] +
          p_ccm->axis().extendedCurveIntegral(uOld, u);
    }

    inFile.readAttribute(_kappa, "kappa", groupName);
    inFile.readAttribute(_lambda, "lambda", groupName);
    inFile.readAttribute(_mu, "mu", groupName);
    inFile.readAttribute(_tau, "tau", groupName);
    inFile.readAttribute(_nIter, "nIter", groupName);
  }
  
}
