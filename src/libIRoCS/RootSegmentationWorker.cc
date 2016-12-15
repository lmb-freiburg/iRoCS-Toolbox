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

#include "RootSegmentationWorker.hh"

#include <algorithm>

#include <libArrayToolbox/ATBDataSynthesis.hh>
#include <libArrayToolbox/Normalization.hh>
#include <libArrayToolbox/MedianFilter.hh>
#include <libArrayToolbox/GaussianFilter.hh>
#include <libArrayToolbox/AnisotropicDiffusionFilter.hh>
#include <libArrayToolbox/ATBLinAlg.hh>
#include <libArrayToolbox/ATBMorphology.hh>
#include <libArrayToolbox/algo/ltransform.hh> // For randomColorMapping
#include <libArrayToolbox/algo/lmorph.hh> // For watershed
#include <libArrayToolbox/algo/lrootShapeAnalysis.hh> // For volume and eraseMarkers

namespace iRoCS
{

  void varianceNormalization(
      blitz::Array<double,3> &data,
      blitz::TinyVector<double,3> const &elementSizeUm, double sigmaUm,
      double epsilon, iRoCS::ProgressReporter *pr)
  {
    int pMin = (pr != NULL) ? pr->taskProgressMin() : 0;
    int pScale = (pr != NULL) ? (pr->taskProgressMax() - pMin) : 100;
    blitz::Array<double,3> dataMean(data.shape());
    blitz::Array<double,3> dataVariance(data.shape());
  
    if (pr != NULL)
    {
      if (!pr->updateProgressMessage(
              "Computing Gaussian weighted local mean")) return;
      pr->setTaskProgressMin(pMin);
      pr->setTaskProgressMax(static_cast<int>(pMin + 0.5 * pScale));
    }
    atb::GaussianFilter<double,3> filter(atb::RepeatBT);
    filter.setStandardDeviationUm(sigmaUm);
    filter.apply(data, elementSizeUm, dataMean, pr);
  
    if (pr != NULL && !pr->updateProgressMessage("Subtracting local mean"))
        return;
    data = data - dataMean;
    dataMean.free();

    if (pr != NULL && !pr->updateProgressMessage(
            "Computing squared intensities")) return;
    dataVariance = blitz::pow2(data);

    if (pr != NULL)
    {
      if (!pr->updateProgressMessage(
              "Computing Gaussian weighted local variance")) return;
      pr->setTaskProgressMin(static_cast<int>(pMin + 0.5 * pScale));
      pr->setTaskProgressMax(pMin + pScale);
    }
    filter.apply(dataVariance, elementSizeUm, dataVariance, pr);
  
    if (pr != NULL && !pr->updateProgressMessage(
            "Normalizing to unit standard deviation")) return;
    data /= blitz::sqrt(dataVariance) + epsilon;

    if (pr != NULL)
    {
      pr->setTaskProgressMin(pMin);
      pr->setTaskProgressMax(pMin + pScale);
      pr->updateProgress(pMin + pScale);
    }
  }

  void segmentCells(
      atb::Array<double,3> &data, atb::Array<int,3> &segmentation, double gamma,
      int normalizationType, int medianWidthPx, double processingElementSizeUm,
      double varSigmaUm, double varEpsilon, float sigmaHessianUm,
      bool preDiffusion, int nDiffusionIterations, float zCompensationFactor,
      double kappa, float deltaT, float l1Threshold, float volumeThresholdUm,
      int boundaryThicknessPx, std::string const &debugFileName,
      iRoCS::ProgressReporter *pr)
  {
    double proc = (processingElementSizeUm <= 0.0) ?
        blitz::min(data.elementSizeUm()) : processingElementSizeUm;
    if (sigmaHessianUm <= 0.0f) sigmaHessianUm = proc;

    // Setup progress reporting
    int pMin = (pr != NULL) ? pr->taskProgressMin() : 0;
    int pScale = (pr != NULL) ? (pr->taskProgressMax() - pMin) : 100;
    if (pr != NULL && !pr->updateProgress(pMin)) return;
    std::vector<std::string> mVec;
    std::vector<int> pVec;
    if (gamma > 0.0 && gamma != 1.0)
    {
      mVec.push_back("Gamma correction");
      pVec.push_back(1);
    }
    if (normalizationType > 0 && normalizationType < 4)
    {
      mVec.push_back("Normalization");
      pVec.push_back(1);
    }
    if (medianWidthPx > 0)
    {
      mVec.push_back("Median filtering");
      pVec.push_back(
          (2 * medianWidthPx + 1) * (2 * medianWidthPx + 1) *
          (2 * medianWidthPx + 1));
      if (debugFileName != "")
      {
        mVec.push_back("Saving '" + debugFileName + ":/medianFiltered'");
        pVec.push_back(1);
      }
    }
    if (blitz::any(data.elementSizeUm() != processingElementSizeUm))
    {
      std::stringstream msg;
      msg << "Rescaling data to cubic voxels with edge length " << proc
          << " micron";
      mVec.push_back(msg.str());
      pVec.push_back(2);
    }
    if (debugFileName != "")
    {
      mVec.push_back("Saving '" + debugFileName + ":/rawI'");
      pVec.push_back(1);
    }
    if (varSigmaUm > 0.0)
    {
      mVec.push_back("Variance normalization");
      pVec.push_back(static_cast<int>(18 * varSigmaUm + 1));
      if (debugFileName != "")
      {
        mVec.push_back("Saving '" + debugFileName + ":/varNormalized'");
        pVec.push_back(1);
      }
    }
    if (preDiffusion)
    {
      mVec.push_back("Pre-diffusion");
      pVec.push_back(200 * nDiffusionIterations);
      if (debugFileName != "")
      {
        mVec.push_back("Saving '" + debugFileName + ":/diffused'");
        pVec.push_back(1);
      }
    }
    mVec.push_back("Computing hessian matrix");
    pVec.push_back(100);
    mVec.push_back("Eigenvalue decomposition");
    pVec.push_back(100);
    if (debugFileName != "")
    {
      mVec.push_back("Saving '" + debugFileName + ":/hessian/l1'");
      pVec.push_back(1);
      mVec.push_back("Saving '" + debugFileName + ":/hessian/v1'");
      pVec.push_back(1);
      mVec.push_back("Saving '" + debugFileName + ":/compensatedL1'");
      pVec.push_back(1);
    }
    mVec.push_back("Thresholding");
    pVec.push_back(1);
    if (debugFileName != "")
    {
      mVec.push_back("Saving '" + debugFileName + ":/marker'");
      pVec.push_back(1);
    }
    mVec.push_back("Connected component labelling");
    pVec.push_back(100);
    mVec.push_back("Region growing");
    pVec.push_back(100);
    if (debugFileName != "")
    {
      mVec.push_back("Saving '" + debugFileName + ":/L'");
      pVec.push_back(1);
      mVec.push_back("Saving '" + debugFileName + ":/randomColor'");
      pVec.push_back(1);
    }
    mVec.push_back("Removing too small segments");
    pVec.push_back(100);
    mVec.push_back("Region growing");
    pVec.push_back(100);
    mVec.push_back("Setting background label to 1");
    pVec.push_back(1);
    if (boundaryThicknessPx >= 1)
    {
      mVec.push_back("Thickening boundaries");
      pVec.push_back(
          boundaryThicknessPx * boundaryThicknessPx * boundaryThicknessPx);
    }
    if (debugFileName != "")
    {
      mVec.push_back("Saving '" + debugFileName + ":/L2'");
      pVec.push_back(1);
      mVec.push_back("Saving '" + debugFileName + ":/randomColor2'");
      pVec.push_back(1);
    }
    for (size_t i = 1; i < pVec.size(); ++i) pVec[i] += pVec[i - 1];
    for (size_t i = 0; i < pVec.size(); ++i)
    {
      pVec[i] = static_cast<int>(
          pMin + pScale * static_cast<double>(pVec[i]) /
          static_cast<double>(pVec.back()));
    }
    size_t pState = 0;

    if (gamma > 0.0 && gamma != 1.0)
    {
      if (pr != NULL && !pr->updateProgressMessage(mVec[pState])) return;
      atb::normalize(data, data, atb::MINMAX);
      if (gamma == 0.5)
      {
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(data.size()); ++i)
            data.data()[i] = std::sqrt(data.data()[i]);
      }
      else if (gamma == 2.0)
      {
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(data.size()); ++i)
            data.data()[i] *= data.data()[i];
      }
      else
      {
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(data.size()); ++i)
            data.data()[i] = std::pow(data.data()[i], gamma);
      }
      if (pr != NULL && !pr->updateProgress(pVec[pState])) return;
      pState++;
    }

    if (normalizationType > 0 && normalizationType < 4)
    {
      if (pr != NULL && !pr->updateProgressMessage(mVec[pState])) return;
      switch (normalizationType)
      {
      case 1:
        atb::normalize(data, data, atb::MINMAX);
        break;
      case 2:
        atb::normalize(data, data, atb::STDDEV);
        break;
      case 3:
        atb::normalize(data, data, atb::ZERO_MEAN_STDDEV);
        break;
      default:
        break;
      }
      if (pr != NULL && !pr->updateProgress(pVec[pState])) return;
      pState++;
    }

    if (medianWidthPx > 0)
    {
      if (pr != NULL)
      {
        if (!pr->updateProgressMessage(mVec[pState])) return;
        int pMin = (pState > 0) ? pVec[pState - 1] : 0;
        pr->setTaskProgressMin(pMin);
        pr->setTaskProgressMax(pVec[pState]);
      }
      atb::MedianFilter<double,3>::apply(
          data, data, blitz::TinyVector<atb::BlitzIndexT,3>(medianWidthPx), pr);
      pState++;
      if (debugFileName != "")
      {
        if (pr != NULL)
        {
          if (!pr->updateProgressMessage(mVec[pState])) return;
          pr->setTaskProgressMin((pState > 0) ? pVec[pState - 1] : 0);
          pr->setTaskProgressMax(pVec[pState]);
        }
        try
        {
          data.save(debugFileName, "/medianFiltered", 1, pr);
        }
        catch (BlitzH5Error &e)
        {
          std::cerr << "Could not save debugging information: " << e.what()
                    << std::endl;
        }
        pState++;
      }
    }

    if (blitz::any(data.elementSizeUm() != processingElementSizeUm))
    {
      if (pr != NULL)
      {
        if (!pr->updateProgressMessage(mVec[pState])) return;
        pr->setTaskProgressMin((pState > 0) ? pVec[pState - 1] : 0);
        pr->setTaskProgressMax(pVec[pState]);
      }
      data.rescale(blitz::TinyVector<double,3>(proc), pr);
      pState++;
    }
  
    if (debugFileName != "")
    {
      if (pr != NULL)
      {
        if (!pr->updateProgressMessage(mVec[pState])) return;
        pr->setTaskProgressMin((pState > 0) ? pVec[pState - 1] : 0);
        pr->setTaskProgressMax(pVec[pState]);
      } 
      try
      {
        data.save(debugFileName, "/rawI", 1, pr);
      }
      catch (BlitzH5Error &e)
      {
        std::cerr << "Could not save debugging information: " << e.what()
                  << std::endl;
      }
      pState++;
    }
  
    if (varSigmaUm > 0.0)
    {
      if (pr != NULL)
      {
        if (!pr->updateProgressMessage(mVec[pState])) return;
        pr->setTaskProgressMin((pState > 0) ? pVec[pState - 1] : 0);
        pr->setTaskProgressMax(pVec[pState]);
      }
      varianceNormalization(
          data, data.elementSizeUm(), varSigmaUm, varEpsilon, pr);
      switch (normalizationType)
      {
      case 1:
        atb::normalize(data, data, atb::MINMAX);
        break;
      case 2:
        atb::normalize(data, data, atb::STDDEV);
        break;
      case 3:
        atb::normalize(data, data, atb::ZERO_MEAN_STDDEV);
        break;
      default:
        break;
      }
      pState++;
      if (debugFileName != "")
      {
        if (pr != NULL)
        {
          if (!pr->updateProgressMessage(mVec[pState])) return;
          pr->setTaskProgressMin((pState > 0) ? pVec[pState - 1] : 0);
          pr->setTaskProgressMax(pVec[pState]);
        }
        try
        {
          data.save(debugFileName, "/varNormalized", 1, pr);
        }
        catch (BlitzH5Error &e)
        {
          std::cerr << "Could not save debugging information: " << e.what()
                    << std::endl;
        }
        pState++;
      }
    }

    if (preDiffusion)
    {
      if (pr != NULL)
      {
        if (!pr->updateProgressMessage(mVec[pState])) return;
        pr->setTaskProgressMin((pState > 0) ? pVec[pState - 1] : 0);
        pr->setTaskProgressMax(pVec[pState]);
      }
      atb::AnisotropicDiffusionFilter<double,3> anisotropicDiffusionFilter(
          kappa, sigmaHessianUm, deltaT, zCompensationFactor,
          nDiffusionIterations, 4, atb::RepeatBT);
      anisotropicDiffusionFilter.apply(data, data, pr);
      pState++;
      if (debugFileName != "")
      {
        if (pr != NULL)
        {
          if (!pr->updateProgressMessage(mVec[pState])) return;
          pr->setTaskProgressMin((pState > 0) ? pVec[pState - 1] : 0);
          pr->setTaskProgressMax(pVec[pState]);
        }
        try
        {
          data.save(debugFileName, "/diffused", 1, pr);
        }
        catch (BlitzH5Error &e)
        {
          std::cerr << "Could not save debugging information: " << e.what()
                    << std::endl;
        }
        pState++;
      }
    }

    if (pr != NULL)
    {
      if (!pr->updateProgressMessage(mVec[pState])) return;
      pr->setTaskProgressMin((pState > 0) ? pVec[pState - 1] : 0);
      pr->setTaskProgressMax(pVec[pState]);
    }
    atb::GaussianFilter<double,3> gaussianFilter(atb::RepeatBT);
    gaussianFilter.setStandardDeviationUm(
        blitz::TinyVector<double,3>(sigmaHessianUm));
    gaussianFilter.apply(data, data);

    atb::Array<blitz::TinyVector<double,6>,3> hessian(
        data.shape(), data.elementSizeUm());
    atb::CentralHessianUTFilter<double,3> hessianFilter(atb::MirrorBT);
    hessianFilter.apply(data, hessian, pr);

    pState++;

    if (pr != NULL && !pr->updateProgressMessage(mVec[pState])) return;
    atb::Array<double,3> l1(data.shape(), data.elementSizeUm());
    atb::Array<blitz::TinyVector<double,3>,3> v1(
        data.shape(), data.elementSizeUm());
    atb::Array<double,3> v1z(data.shape(), data.elementSizeUm());
    double varSum = 0.0;
    int p = 0;
    double prStart = (pState > 0) ? pVec[pState - 1] : 0;
    double prScale = static_cast<double>(pVec[pState] - prStart) /
        static_cast<double>(data.size() - 1);
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(data.size()); ++i)
    {
      if (pr != NULL)
      {
        if (pr->isAborted()) continue;
        int progress = 0;
#ifdef _OPENMP
#pragma omp critical
#endif
        {
          progress = p;
          ++p;
        }
        if (progress % std::max(1, static_cast<int>(data.size() / 100)) == 0)
            pr->updateProgress(
                static_cast<int>(prStart + progress * prScale));
      }
      blitz::TinyMatrix<double,3,3> m;
      blitz::TinyVector<double,3> lambda;
      int k = 0;
      for (int r = 0; r < 3; ++r)
      {
        m(r, r) = hessian.data()[i](k++);
        for (int c = r + 1; c < 3; ++c)
            m(r, c) = m(c, r) = hessian.data()[i](k++);
      }
      atb::eigenvalueDecompositionRealSymmetric(m, m, lambda, atb::Ascending);
      l1.data()[i] = lambda(0);
      v1.data()[i] = m(0, 0), m(1, 0), m(2, 0);
      v1z.data()[i] = std::abs(m(0, 0));
#ifdef _OPENMP
#pragma omp critical
#endif
      varSum += blitz::pow2(lambda(0));      
    }
    if (pr != NULL && pr->isAborted()) return;
    double stddevInv = 1.0 / std::sqrt(varSum / hessian.size());

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(data.size()); ++i)
        l1.data()[i] *= stddevInv;

    pState++;

    if (debugFileName != "")
    {
      int pStateStart = pState;
      try
      {
        if (pr != NULL)
        {
          if (!pr->updateProgressMessage(mVec[pState])) return;
          pr->setTaskProgressMin((pState > 0) ? pVec[pState - 1] : 0);
          pr->setTaskProgressMax(pVec[pState]);
        }
        l1.save(debugFileName, "/hessian/l1", 1, pr);
        pState++;
        if (pr != NULL)
        {
          if (!pr->updateProgressMessage(mVec[pState])) return;
          pr->setTaskProgressMin((pState > 0) ? pVec[pState - 1] : 0);
          pr->setTaskProgressMax(pVec[pState]);
        }
        v1.save(debugFileName, "/hessian/v1", 1, pr);
        pState++;
        if (pr != NULL)
        {
          if (!pr->updateProgressMessage(mVec[pState])) return;
          pr->setTaskProgressMin((pState > 0) ? pVec[pState - 1] : 0);
          pr->setTaskProgressMax(pVec[pState]);
        }
        atb::Array<double,3> buff(data.shape(), data.elementSizeUm());
        buff = l1 + v1z * zCompensationFactor * l1Threshold;
        buff.save(debugFileName, "/compensatedL1", 1, pr);
      }
      catch (BlitzH5Error &e)
      {
        std::cerr << "Could not save debugging information: " << e.what()
                  << std::endl;
      }
      pState = pStateStart + 3;
    }
    v1.free();

    if (pr != NULL && !pr->updateProgressMessage(mVec[pState])) return;

    blitz::Array<double,3> directionalAdaptiveThreshold(data.shape());
    directionalAdaptiveThreshold =
        -blitz::abs(v1z) * zCompensationFactor * l1Threshold + l1Threshold;
  
    blitz::Array<bool,3> marker(data.shape());
    marker = (l1 > directionalAdaptiveThreshold);
    pState++;

    if (debugFileName != "")
    {
      if (pr != NULL)
      {
        if (!pr->updateProgressMessage(mVec[pState])) return;
        pr->setTaskProgressMin((pState > 0) ? pVec[pState - 1] : 0);
        pr->setTaskProgressMax(pVec[pState]);
      }
      atb::Array<unsigned char,3> buffer2(data.shape(), data.elementSizeUm());
      buffer2 = blitz::where(marker, 1, 0);
      try
      {
        buffer2.save(debugFileName, "/marker", 1, pr);
      }
      catch (BlitzH5Error& e)
      {
        std::cerr << "Could not save debugging information: " << e.what()
                  << std::endl;
      }
      pState++;
    }
    directionalAdaptiveThreshold.free();

    if (pr != NULL)
    {
      if (!pr->updateProgressMessage(mVec[pState])) return;
      pr->setTaskProgressMin((pState > 0) ? pVec[pState - 1] : 0);
      pr->setTaskProgressMax(pVec[pState]);
    }
    segmentation.resize(data.shape());
    segmentation.setElementSizeUm(data.elementSizeUm());
    atb::connectedComponentLabelling(
        marker, segmentation, atb::COMPLEX_NHOOD, pr);
    pState++;

    l1 = -l1;
  
    if (pr != NULL && !pr->updateProgressMessage(mVec[pState])) return;
    morphWatershed(l1, segmentation, 26);
    if (pr != NULL && !pr->updateProgress(pVec[pState])) return;
    pState++;

    if (debugFileName != "")
    {
      int pStateStart = pState;
      blitz::Array<unsigned char, 4> randomColor(
          data.extent(0), data.extent(1), data.extent(2), 3);
      randomColorMapping(segmentation, randomColor);
      try
      {
        if (pr != NULL)
        {
          if (!pr->updateProgressMessage(mVec[pState])) return;
          pr->setTaskProgressMin((pState > 0) ? pVec[pState - 1] : 0);
          pr->setTaskProgressMax(pVec[pState]);
        }
        segmentation.save(debugFileName, "/L", 1, pr);
        pState++;
        if (pr != NULL)
        {
          if (!pr->updateProgressMessage(mVec[pState])) return;
          pr->setTaskProgressMin((pState > 0) ? pVec[pState - 1] : 0);
          pr->setTaskProgressMax(pVec[pState]);
        }
        BlitzH5File outFile(debugFileName, BlitzH5File::WriteOrNew);
        outFile.writeDataset(randomColor, "/randomColor", 1, pr);
        outFile.writeAttribute(
            data.elementSizeUm(), "element_size_um", "/randomColor");
      }
      catch (BlitzH5Error &e)
      {
        std::cerr << "Could not save debugging information: " << e.what()
                  << std::endl;
      }
      pState = pStateStart + 2;
    }

    if (pr != NULL && !pr->updateProgressMessage(mVec[pState])) return;
    blitz::Array<double,1> volumes;
    volume(segmentation, volumes);
    volumes = volumes * (data.elementSizeUm()(0) * data.elementSizeUm()(1) *
                         data.elementSizeUm()(2));
    blitz::Array<bool,1> flagTooSmall(volumes.size());
    flagTooSmall = (volumes < volumeThresholdUm);
    eraseMarkers(segmentation, marker, flagTooSmall);
    if (pr != NULL && !pr->updateProgress(pVec[pState])) return;
    pState++;

    if (pr != NULL && !pr->updateProgressMessage(mVec[pState])) return;
    morphWatershed(l1, marker, segmentation, 26);
    volume(segmentation, volumes);
    if (pr != NULL && !pr->updateProgress(pVec[pState])) return;
    pState++;

    if (pr != NULL && !pr->updateProgressMessage(mVec[pState])) return;
    int backgroundLabel = (blitz::maxIndex(volumes))(0) + 1;
    if (backgroundLabel != 1)
    {
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(segmentation.size());
           ++i)
      {
        if (segmentation.data()[i] == 1)
            segmentation.data()[i] = backgroundLabel;
        else if (segmentation.data()[i] == backgroundLabel)
            segmentation.data()[i] = 1;
      }
    }
    if (pr != NULL && !pr->updateProgress(pVec[pState])) return;
    pState++;

    if (boundaryThicknessPx >= 1)
    {
      if (pr != NULL && !pr->updateProgressMessage(mVec[pState])) return;
      atb::Array<char,3> border(data.shape(), data.elementSizeUm());
      border = (segmentation == 0);
      morphClose(border, boundaryThicknessPx);
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(segmentation.size());
           ++i) if (border.data()[i] != 0) segmentation.data()[i] = 0;
      if (pr != NULL && !pr->updateProgress(pVec[pState])) return;
      pState++;
    }
    if (debugFileName != "")
    {
      int pStateStart = pState;
      blitz::Array<unsigned char, 4> randomColor(
          data.extent(0), data.extent(1), data.extent(2), 3);
      randomColorMapping(segmentation, randomColor);
      try
      {
        if (pr != NULL)
        {
          if (!pr->updateProgressMessage(mVec[pState])) return;
          pr->setTaskProgressMin((pState > 0) ? pVec[pState - 1] : 0);
          pr->setTaskProgressMax(pVec[pState]);
        }
        segmentation.save(debugFileName, "/L2", 1, pr);
        pState++;
        if (pr != NULL)
        {
          if (!pr->updateProgressMessage(mVec[pState])) return;
          pr->setTaskProgressMin((pState > 0) ? pVec[pState - 1] : 0);
          pr->setTaskProgressMax(pVec[pState]);
        }
        BlitzH5File outFile(debugFileName, BlitzH5File::Write);
        outFile.writeDataset(randomColor, "/randomColor2", 1, pr);
        outFile.writeAttribute(
            data.elementSizeUm(), "element_size_um", "/randomColor2");
      }
      catch (BlitzH5Error& e)
      {
        std::cerr << "Could not save debugging information: " << e.what()
                  << std::endl;
      }
      pState = pStateStart + 2;
    }
    if (pr != NULL)
    {
      pr->setTaskProgressMin(pMin);
      pr->setTaskProgressMax(pMin + pScale);
      pr->updateProgress(pMin + pScale);
    }
  }

}
