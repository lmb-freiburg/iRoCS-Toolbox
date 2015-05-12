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

#include "ComputeCellFeaturesWorker.hh"

#include <iostream>

#include <libArrayToolbox/ATBLinAlg.hh>
#include <libArrayToolbox/Neighborhood.hh>
#include <libArrayToolbox/algo/lrootShapeAnalysis.hh>

#include <libArrayToolbox/ATBTiming.hh>

namespace iRoCS
{

  void extractRD(
      atb::Array<int,3> const &L, int label,
      blitz::TinyVector<double,3> const &center,
      blitz::TinyMatrix<double,3,3> const &axes,
      blitz::TinyVector<double,26> &rd)
  {
    rd = 0.0;
    blitz::TinyVector<atb::BlitzIndexT,3> cc(center / L.elementSizeUm() + 0.5);
    if (blitz::any(cc < 0 || cc >= L.shape())) return;

    int feaIdx = 0;
    for (int dz = -1; dz <= 1; ++dz)
    {
      for (int dy = -1; dy <= 1; ++dy)
      {
        for (int dx = -1; dx <= 1; ++dx)
        {
          if (dx != 0 || dy != 0 || dz != 0)
          {
            blitz::TinyVector<double,3> step(dz, dy, dx);
            step = axes * step;
            step /= std::sqrt(blitz::dot(step, step));
            double unit = std::sqrt(
                blitz::dot(step * L.elementSizeUm(), step * L.elementSizeUm()));
            
            int d1 = 0;
            blitz::TinyVector<double,3> current(cc);
            blitz::TinyVector<atb::BlitzIndexT,3> currentI(current);
            int last = 0;
            
            // This loop casts a ray in the given direction. It will search
            // for voxels with the given label until either the volume
            // boundary or - if it once hit the segment with the given label -
            // a segment with different label is hit
            bool segmentFound = false;
            while (blitz::all(currentI >= 0 && currentI < L.shape()) &&
                   (!segmentFound || L(currentI) == label ||
                    L(currentI) <= 0))
            {
              if (L(currentI) == label)
              {
                last = d1;
                segmentFound = true;
              }
              current += step;
              currentI = blitz::TinyVector<atb::BlitzIndexT,3>(current);
              d1++;
            }
            rd(feaIdx) = last * unit;
            ++feaIdx;
          }
        }
      }
    }
  }

  void computeVoxelSetsPerLabel(
      atb::Array<int,3> const &L,
      blitz::Array<std::vector< blitz::TinyVector<atb::BlitzIndexT,3> >,1>
      &voxelSets, int backgroundLabel, ProgressReporter *pr)
  {
    int pMin = (pr != NULL) ? pr->taskProgressMin() : 0;
    double pScale = (pr != NULL) ? static_cast<double>(
        pr->taskProgressMax() - pMin) / static_cast<double>(L.size() - 1) : 100;
    if (pr != NULL && !pr->updateProgress(pMin)) return;

    int p = 0;
    for (atb::BlitzIndexT z = 0; z < L.extent(0); ++z)
    {
      for (atb::BlitzIndexT y = 0; y < L.extent(1); ++y)
      {
        for (atb::BlitzIndexT x = 0; x < L.extent(2); ++x, ++p)
        {
          if (pr != NULL && p % (L.size() / 100) == 0 &&
              !pr->updateProgress(static_cast<int>(pMin + pScale * p))) return;
          int lbl = L(z, y, x);
          if (lbl > 0 && lbl != backgroundLabel &&
              lbl - 1 < static_cast<int>(voxelSets.size()))
          {
            voxelSets(lbl - 1).push_back(
                blitz::TinyVector<atb::BlitzIndexT,3>(z, y, x));
          }
        }
      }
    }
    if (pr != NULL) pr->updateProgress(pr->taskProgressMax());
  }
  
  double computeConvexity(
      atb::Array<int,3> const &L,
      std::vector< blitz::TinyVector<atb::BlitzIndexT,3> > const &voxelSet,
      int nRandomPairs)
  {
    if (voxelSet.size() == 0) return 0.0;
    if (voxelSet.size() == 1) return 1.0;

    blitz::TinyVector<atb::BlitzIndexT,3> p;
    int a = static_cast<int>(
        (voxelSet.size() - 1) * static_cast<double>(std::rand()) /
        static_cast<double>(RAND_MAX));
    int convexCount = 0;
    for (int i = 0; i < nRandomPairs; i++)
    {
      int b = static_cast<int>(
          (voxelSet.size() - 1) * static_cast<double>(std::rand()) /
          static_cast<double>(RAND_MAX));
      p = (voxelSet[a] + voxelSet[b]) / 2;
      if (L(p) == L(voxelSet[0])) convexCount++;
      a = b;
    }
    return static_cast<double>(convexCount) / static_cast<double>(nRandomPairs);
  }

  void computeCellFeatures(
      atb::Array<int,3> const &L, ShellCoordinateTransform const &sct,
      double volumeThresholdUm, std::string const &outFileName,
      std::string const &featureGroup, int backgroundLabel,
      ProgressReporter *pr)
  {
    int pMin = (pr != NULL) ? pr->taskProgressMin() : 0;
    int pScale = (pr != NULL) ? (pr->taskProgressMax() - pMin) : 100;
    if (pr != NULL && !pr->updateProgress(pMin)) return;

    //Variables
    blitz::Array<double,1> volumes;
    blitz::Array<blitz::TinyVector<double,3>,1> centers;

    // Compute centers (in um) and volume (in um3) and get the number of
    // segments
    if (pr != NULL && !pr->updateProgressMessage(
            "Computing segment centers and volumes")) return;
    centerAndVolume(L, centers, volumes, -1, L.elementSizeUm());
    std::cout << "  Processing " << centers.size() << " segments" << std::endl;
    int LMin = blitz::min(L);
    int LMax = blitz::max(L);
    if (LMin < 0 || LMax != static_cast<int>(centers.size()))
    {
      std::cerr << "Labels are not positive and contiguous... that's "
                << "currently not supported" << std::endl;
      std::cerr << "  Smallest label = " << LMin << ", Largest label = "
                << LMax << std::endl;
      std::cerr << "  nSegments = " << centers.size() << std::endl;
      if (pr != NULL) pr->abort();
      return;
    }

    if (pr != NULL && !pr->updateProgress(
            static_cast<int>(pMin + pScale * 0.01))) return;

    //prepare for coordinate fitting
    if (backgroundLabel < 0)
    {
      if (pr != NULL && !pr->updateProgressMessage(
              "Searching background label")) return;
      backgroundLabel = (blitz::maxIndex(volumes))(0) + 1;
    }
    std::cout << "  background label = " << backgroundLabel << std::endl;

    if (pr != NULL && !pr->updateProgress(
            static_cast<int>(pMin + pScale * 0.02))) return;

    // Set all background and too small segments invalid
    if (pr != NULL && !pr->updateProgressMessage(
            "Removing background and too small segments")) return;
    blitz::Array<unsigned char,1> validFlag(centers.shape());
    validFlag = blitz::where(volumes < volumeThresholdUm, 0, 1);
    validFlag(backgroundLabel - 1) = 0;

    if (pr != NULL && !pr->updateProgress(
            static_cast<int>(pMin + pScale * 0.03))) return;

    // remove all the cells which are cut off by image border
    if (pr != NULL && !pr->updateProgressMessage(
            "Removing cells cut by boundary")) return;
    blitz::Array<unsigned char, 1> borderFlag(centers.shape());
    borderFlag = true;
    labelOnBorder(L, borderFlag);

    if (pr != NULL && !pr->updateProgress(
            static_cast<int>(pMin + pScale * 0.04))) return;

    // Get the iRoCS coordinates for each segment center
    if (pr != NULL && !pr->updateProgressMessage(
            "Computing iRoCS coordinates of segment centers")) return;
    blitz::Array<blitz::TinyVector<double,3>,1> iRoCSCenters(centers.shape());
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (atb::BlitzIndexT i = 0; i < centers.extent(0); ++i)
        iRoCSCenters(i) = sct.getCoordinatesWithNormalizedRadius(centers(i));

    if (pr != NULL && !pr->updateProgress(
            static_cast<int>(pMin + pScale * 0.05))) return;

    // Compute local coordinate axes for each segment
    if (pr != NULL && !pr->updateProgressMessage(
            "Computing local iRoCS axes per segment")) return;
    blitz::Array<blitz::TinyMatrix<double,3,3>,1> localAxes(centers.shape());
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (atb::BlitzIndexT i = 0; i < centers.extent(0); i++)
    {
      blitz::TinyVector<double,3> x, y;
      for (int d = 0; d < 3; ++d)
      {
        blitz::TinyVector<double,3> nbPos(centers(i));
        nbPos(d) += L.elementSizeUm()(d);
        blitz::TinyVector<double,3> nbCoords(
            sct.getCoordinatesWithNormalizedRadius(nbPos));
        x(d) = (nbCoords(0) - iRoCSCenters(i)(0)) / L.elementSizeUm()(d);
        y(d) = (nbCoords(1) - iRoCSCenters(i)(1)) / L.elementSizeUm()(d);
      }
      x /= std::sqrt(blitz::dot(x, x));
      blitz::TinyVector<double,3> z(blitz::cross(x, y));
      z /= std::sqrt(blitz::dot(z, z));
      y = blitz::cross(z, x);
      localAxes(i) = z(0), y(0), x(0), z(1), y(1), x(1), z(2), y(2), x(2);
    }

    if (pr != NULL)
    {
      pr->updateProgress(static_cast<int>(pMin + pScale * 0.06));
      if (!pr->updateProgressMessage("Computing per-label voxel sets")) return;
      pr->setTaskProgressMin(static_cast<int>(pMin + pScale * 0.06));
      pr->setTaskProgressMax(static_cast<int>(pMin + pScale * 0.2));      
    }
    blitz::Array<std::vector< blitz::TinyVector<atb::BlitzIndexT,3> >,1>
        voxelSets(centers.shape());
    computeVoxelSetsPerLabel(L, voxelSets, backgroundLabel, pr);

    if (pr != NULL && !pr->updateProgressMessage(
            "Computing local shape features")) return;
    blitz::Array<blitz::TinyVector<double,26>,1> RD(centers.shape());
    blitz::Array<blitz::TinyVector<double,3>,1> blockSize(
        centers.shape()); // lengths of cells' main axes
    blitz::Array<double,1> volumeOverBlock(centers.shape());
    blitz::Array<double,1> convexity(centers.shape());
    double prScale = 0.5 / static_cast<double>(centers.size() - 1);
    for (atb::BlitzIndexT i = 0; i < centers.extent(0); ++i)
    {
      if (pr != NULL && i % (centers.extent(0) / 100) == 0 &&
          !pr->updateProgress(
              static_cast<int>(pMin + pScale * (0.2 + prScale * i)))) return;
      if (validFlag(i) == 1)
      {
        extractRD(L, i + 1, centers(i), localAxes(i), RD(i));
        blockSize(i) =
            RD(i)(12) + RD(i)(13),
            RD(i)(10) + RD(i)(15),
            RD(i)(4) + RD(i)(21);
        convexity(i) = computeConvexity(L, voxelSets(i));
        volumeOverBlock(i) = volumes(i) / std::max(
            1.0, blitz::product(blockSize(i)));
      }
      else
      {
        RD(i) = 0.0;
        blockSize(i) = 0.0;
        convexity(i) = 0.0;
        volumeOverBlock = 0.0;
      }
    }

    // Find neighboring cells
    if (pr != NULL)
    {
      pr->updateProgress(static_cast<int>(pMin + pScale * 0.7));
      if (!pr->updateProgressMessage("Analyzing cell neighborhood")) return;
    }
    blitz::Array<std::set<int>,1> nbSets(centers.shape());
    atb::Neighborhood<3> nh(atb::Neighborhood<3>::Complex);
    prScale = 0.29 / static_cast<double>(L.size() - 1);
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(L.size()); ++i)
    {
      if (pr != NULL && i % (L.size() / 100) == 0 &&
          !pr->updateProgress(
              static_cast<int>(pMin + pScale * (0.7 + prScale * i))))
          continue;

      // If not on boundary => nothing to do
      if (L.dataFirst()[i] != 0) continue;

      blitz::TinyVector<atb::BlitzIndexT,3> posPx;
      ptrdiff_t tmp = i;
      for (int d = 2; d >= 0; --d)
      {
        posPx(d) = tmp % L.extent(d);
        tmp /= L.extent(d);
      }
      std::set<int> s;
      for (atb::Neighborhood<3>::const_iterator it = nh.begin();
           it != nh.end(); ++it)
      {
        blitz::TinyVector<atb::BlitzIndexT,3> nbPos(posPx + *it);
        if (blitz::all(nbPos >= 0 && nbPos < L.shape()) &&
            L(nbPos) > 0) s.insert(L(nbPos));
      }
      if (s.size() > 1)
      {
        for (std::set<int>::const_iterator it = s.begin();
             it != s.end(); ++it)
        {
          std::set<int>::const_iterator it2 = it;
          ++it2;
          for (; it2 != s.end(); ++it2)
          {
            nbSets(*it - 1).insert(*it2);
            nbSets(*it2 - 1).insert(*it);
          }
        }
      }
    }
    if (pr != NULL)
    {
      if (pr->isAborted()) return;
      pr->updateProgressMessage("Searching largest neighborhood");
    }
    size_t maxNeighbors = 0;
    for (size_t i = 0; i < nbSets.size(); ++i)
        if (nbSets(i).size() > maxNeighbors) maxNeighbors = nbSets(i).size();
    std::cout << "  Segment with largest neighborhood has " << maxNeighbors
              << " neighbors." << std::endl;
    blitz::Array<int,2> neighbors(
        static_cast<atb::BlitzIndexT>(centers.size()),
        static_cast<atb::BlitzIndexT>(maxNeighbors));
    neighbors = -1;
    for (int i = 0; i < static_cast<int>(nbSets.size()); ++i)
    {
      int j = 0;
      for (std::set<int>::const_iterator it = nbSets(i).begin();
           it != nbSets(i).end(); ++it, ++j) neighbors(i, j) = *it;
    }

    if (pr != NULL && !pr->updateProgressMessage(
            "Saving local shape features")) return;
    try
    {
      BlitzH5File outFile(outFileName, BlitzH5File::WriteOrNew);
      outFile.writeDataset(validFlag, featureGroup + "/validFlag", 3);
      outFile.writeDataset(borderFlag, featureGroup + "/borderFlag", 3);
      outFile.writeDataset(volumes, featureGroup + "/volumes", 3);
      outFile.writeDataset(centers, featureGroup + "/centers", 3);
      outFile.writeAttribute(backgroundLabel, "backgroundLabel", featureGroup);
      outFile.writeAttribute(centers.size(), "maxLabel", featureGroup);
      outFile.writeDataset(iRoCSCenters, featureGroup + "/normCenters", 3);
      outFile.writeDataset(localAxes, featureGroup + "/localAxes", 3);
      outFile.writeDataset(RD, featureGroup + "/RD", 3);
      outFile.writeDataset(blockSize, featureGroup + "/blockSize", 3);
      outFile.writeDataset(
          volumeOverBlock, featureGroup + "/volumeOverBlock", 3);
      outFile.writeDataset(convexity, featureGroup + "/convexity", 3);
      outFile.writeDataset(neighbors, featureGroup + "/neighbor", 3);
    }
    catch (BlitzH5Error& e)
    {
      std::cerr << "Something went wrong while processing '" << outFileName
                << "': " << e.str() << std::endl;
    }

    if (pr != NULL)
    {
      pr->setTaskProgressMin(pMin);
      pr->setTaskProgressMax(pMin + pScale);      
      pr->updateProgress(pMin + pScale);
    }
  }

}
