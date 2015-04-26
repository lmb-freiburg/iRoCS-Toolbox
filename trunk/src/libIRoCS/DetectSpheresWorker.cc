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

#include "DetectSpheresWorker.hh"

#include <libArrayToolbox/GaussianFilter.hh>
#include <libArrayToolbox/HoughTransform.hh>
#include <libArrayToolbox/LocalMaximumExtraction.hh>

namespace iRoCS
{
  
  void detectSpheres(
      atb::Array<float,3> const &data, std::vector<atb::Nucleus> &spheres,
      atb::Array<float,3> &response, atb::Array<float,3> &radiusUm,
      blitz::TinyVector<double,2> const &radiusRangeUm, double radiusStepUm,
      double preSmoothingSigmaUm, double postSmoothingSigmaUm,
      double minMagnitude, bool invertGradients, double gamma,
      ProgressReporter *pr)
  {
    double pStart = (pr != NULL) ? pr->taskProgressMin() : 0.0;
    double pScale = (pr != NULL) ? (pr->taskProgressMax() - pStart) : 1.0;
    
    atb::Array<float,3> dataProc(data.shape(), data.elementSizeUm());
    if (gamma != 1.0) 
    {
      if (pr != NULL && !pr->updateProgressMessage(
              "Applying gamma correction")) return;
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(dataProc.size()); ++i)
          dataProc.data()[i] = (data.data()[i] >= 0.0f) ?
              std::pow(data.data()[i], static_cast<float>(1.0 / gamma)) :
              -std::pow(-data.data()[i], static_cast<float>(1.0 / gamma));
    }
    else std::memcpy(dataProc.data(), data.data(), data.size() * sizeof(float));

    if (pr != NULL && !pr->updateProgress(0.02 * pScale + pStart)) return;

    // Apply pre-smoothing
    if (preSmoothingSigmaUm > 0.0)
    {
      if (pr != NULL) pr->updateProgressMessage("Pre-smoothing");
      atb::GaussianFilter<float,3> filter(atb::RepeatBT);
      filter.setStandardDeviationUm(
          blitz::TinyVector<double,3>(preSmoothingSigmaUm));
      if (pr != NULL)
      {
        pr->setTaskProgressMin(pr->progress());
        pr->setTaskProgressMax(0.05 * pScale + pStart);
      }
      filter.apply(dataProc, dataProc, pr);
    }
    if (pr != NULL && !pr->updateProgress(0.05 * pScale + pStart)) return;

    // Extend search range to discard out-of-range structures
    blitz::TinyVector<double,2> radiusRange(radiusRangeUm);
    radiusRange(0) -= 5.0f * radiusStepUm;
    radiusRange(1) += 5.0f * radiusStepUm;
    if (radiusRange(0) < 0.0f) radiusRange(0) = 0.0f;

    // Do hough transform
    if (pr != NULL)
    {
      pr->updateProgressMessage("Hough-Transform");
      pr->setTaskProgressMin(pr->progress());
      pr->setTaskProgressMax(0.9 * pScale + pStart);
    }
    atb::houghTransform(
        dataProc, response, radiusUm, radiusRange, radiusStepUm, minMagnitude,
        invertGradients, pr);
    if (pr != NULL && !pr->updateProgress(0.9 * pScale + pStart)) return;

    // Apply post-smoothing
    if (postSmoothingSigmaUm > 0.0)
    {
      if (pr != NULL) pr->updateProgressMessage("Post-smoothing");
      atb::GaussianFilter<float,3> filter(atb::RepeatBT);
      filter.setStandardDeviationUm(
          blitz::TinyVector<double,3>(postSmoothingSigmaUm));
      if (pr != NULL)
      {
        pr->setTaskProgressMin(pr->progress());
        pr->setTaskProgressMax(0.95 * pScale + pStart);
      }
      filter.apply(response, response);
    }

    if (pr != NULL && !pr->updateProgressMessage("Extracting local maxima"))
        return;

    // Extract local maxima
    std::vector< blitz::TinyVector<atb::BlitzIndexT,3> > lcMax;
    if (pr != NULL)
    {
      pr->setTaskProgressMin(pr->progress());
      pr->setTaskProgressMax(0.98 * pScale + pStart);
    }
    atb::extractLocalMaxima(response, lcMax, atb::COMPLEX_NHOOD, pr);
    std::cout << "  Found " << lcMax.size() << " local maxima" << std::endl;

    // Generate markers
    if (pr != NULL && !pr->updateProgressMessage("Generating markers")) return;
    blitz::TinyMatrix<double,4,4> trafo = atb::invert(data.transformation());
    for (size_t i = 0; i < lcMax.size(); ++i)
    {
      if (pr != NULL)
      {
        std::stringstream msg;
        msg << "Generating marker " << i + 1 << " / " << lcMax.size();
        if (!pr->updateProgressMessage(msg.str().c_str())) return;
      }
      if (radiusUm(lcMax[i]) >= radiusRangeUm(0) &&
          radiusUm(lcMax[i]) <= radiusRangeUm(1))
      {
        atb::Nucleus marker;
        marker.setPositionUm(
            atb::homogeneousToEuclidean(
                trafo * atb::euclideanToHomogeneous(
                    blitz::TinyVector<double,3>(
                        lcMax[i] * data.elementSizeUm()))));
        marker.setRadiusUm(radiusUm(lcMax[i]));
        marker.setValue(response(lcMax[i]));
        spheres.push_back(marker);
      }
    }
    if (pr != NULL) pr->updateProgress(pScale + pStart);
    std::cout << "  Added " << spheres.size() << " markers" << std::endl;
  }

}


