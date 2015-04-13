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

#include "AttachIRoCSSCTToCellSegmentationWorker.hh"

#include <libArrayToolbox/GaussianFilter.hh>
#include <libArrayToolbox/ATBMorphology.hh>
#include <libArrayToolbox/algo/helper.hh>
#include <libArrayToolbox/algo/lmorph.hh>
#include <libArrayToolbox/algo/lrootShapeAnalysis.hh>

namespace iRoCS
{
  
  void boundaryDetection(
      blitz::Array<bool,3> const &segmentation, blitz::Array<bool,3> &border)
  {
    blitz::Array<bool,3> *res = &border;
    if (&border == &segmentation) res = new blitz::Array<bool,3>();
    res->resize(segmentation.shape());

    int lvls = res->extent(0);
    int cols = res->extent(1);
    int rows = res->extent(2);

    int rStride = res->stride(2);
    int cStride = res->stride(1);
    int lStride = res->stride(0);

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (int i = 1; i < lvls - 1; i++)
    {
      bool const *pBaseL = segmentation.data() + i * lStride;
      bool* pBaseL2 = res->data() + i * lStride;
      bool const *p;
      bool* p2;
      for (int j = 1; j < cols - 1; j++)
      {
        p = pBaseL + j * cStride;
        p2 = pBaseL2 + j * cStride;
        for (int k = 1; k < rows - 1; k++)
        {
          *p2 = *p && !(*(p + rStride) && *(p - rStride) &&
                        *(p + cStride) && *(p - cStride) &&
                        *(p + lStride) && *(p - lStride));
          p++;
          p2++;
        }
      }
    }
    if (&border == &segmentation)
    {
      std::memcpy(border.data(), res->data(), border.size() * sizeof(bool));
      delete res;
    }
  }

  void extractPosition(
      blitz::Array<bool,3> &shell,
      std::vector< blitz::TinyVector<double,3> > &shellPoints)
  {
    for (blitz::Array<bool, 3>::iterator it = shell.begin();
         it != shell.end(); ++it)
        if (*it) shellPoints.push_back(it.position());
  }
  
  void attachIRoCS(
      ShellCoordinateTransform &sct, atb::Array<int,3> const &segmentation,
      int backgroundLabel, int downSampleRatio, double segmentLength,
      double margin, blitz::TinyVector<double,3> const &posQC,
      blitz::TinyVector<double,3> const &xDirection,
      std::string const &debugFileName, atb::Array<double,3> *l,
      atb::Array<double,3> *r, atb::Array<double,3> *p,
      iRoCS::ProgressReporter *pr)
  {
    // Setup progress reporting
    int pMin = (pr != NULL) ? pr->taskProgressMin() : 0;
    int pScale = (pr != NULL) ? (pr->taskProgressMax() - pMin) : 100;
    if (pr != NULL && !pr->updateProgress(pMin)) return;
    std::vector<std::string> mVec;
    std::vector<int> pVec;
    mVec.push_back("Binarizing labels");
    pVec.push_back(1);
    if (debugFileName != "")
    {
      mVec.push_back("Saving '" + debugFileName + ":/bw");
      pVec.push_back(5);
    }
    mVec.push_back("Smoothing binary labels");
    pVec.push_back(5);
    mVec.push_back("Thresholding");
    pVec.push_back(1);
    mVec.push_back("Connected component labelling");
    pVec.push_back(5);
    mVec.push_back("Volume computation");
    pVec.push_back(1);
    if (debugFileName != "")
    {
      mVec.push_back("Saving '" + debugFileName + ":/bwL");
      pVec.push_back(5);
    }
    mVec.push_back("Selecting largest connected component");
    pVec.push_back(1);
    mVec.push_back("Boundary detection");
    pVec.push_back(2);
    if (debugFileName != "")
    {
      mVec.push_back("Saving '" + debugFileName + ":/shell");
      pVec.push_back(5);
    }
    mVec.push_back("Sampling shell points");
    pVec.push_back(1);
    mVec.push_back("Fitting iRoCS shell coordinate transform");
    pVec.push_back(40);
    if (debugFileName != "" && l != NULL && r != NULL && p != NULL)
    {
      mVec.push_back("Computing dense coordinates (this will take a while)");
      pVec.push_back(200);
      mVec.push_back("Saving '" + debugFileName + ":/l'");
      pVec.push_back(5);
      mVec.push_back("Saving '" + debugFileName + ":/r'");
      pVec.push_back(5);
      mVec.push_back("Saving '" + debugFileName + ":/p'");
      pVec.push_back(5);
    }
    for (size_t i = 1; i < pVec.size(); ++i) pVec[i] += pVec[i - 1];
    for (size_t i = 0; i < pVec.size(); ++i)
    {
      pVec[i] = static_cast<int>(
          pMin + pScale * static_cast<double>(pVec[i]) /
          static_cast<double>(pVec.back()));
    }
    size_t pState = 0;

    atb::Array<double,3> bw(segmentation.shape(), segmentation.elementSizeUm());

    if (pr != NULL && !pr->updateProgressMessage(mVec[pState])) return;
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(segmentation.size()); ++i)
        bw.data()[i] = (segmentation.data()[i] > 0 &&
                        segmentation.data()[i] != backgroundLabel);

    if (pr != NULL && !pr->updateProgress(pVec[pState])) return;
    pState++;

    if (debugFileName != "")
    {
      if (pr != NULL)
      {
        if (!pr->updateProgressMessage(mVec[pState])) return;
        pr->setTaskProgressMin(pVec[pState - 1]);
        pr->setTaskProgressMax(pVec[pState]);
      }
      try
      {
        bw.save(debugFileName, "/bw", 1, pr);
      }
      catch (BlitzH5Error &e)
      {
        std::string msg =
            "Could not save '" + debugFileName + ":/bw': " + e.what();
        if (pr != NULL) pr->abortWithError(msg);
        else std::cerr << msg << std::endl;
        return;
      }
      pState++;
    }

    //smooth bw
    if (pr != NULL)
    {
      if (!pr->updateProgressMessage(mVec[pState])) return;
      pr->setTaskProgressMin(pVec[pState - 1]);
      pr->setTaskProgressMax(pVec[pState]);
    }
    atb::GaussianFilter<double,3> filter(
        blitz::TinyVector<double,3>(10.0), atb::RepeatBT);
    filter.apply(bw, segmentation.elementSizeUm(), bw, pr);
    pState++;

    if (pr != NULL && !pr->updateProgressMessage(mVec[pState])) return;
    atb::Array<bool,3> shell(
        segmentation.shape(), segmentation.elementSizeUm());
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(bw.size()); ++i)
        shell.data()[i] = bw.data()[i] > 0.5;

    if (pr != NULL && !pr->updateProgress(pVec[pState])) return;
    pState++;

    if (pr != NULL)
    {
      if (!pr->updateProgressMessage(mVec[pState])) return;
      pr->setTaskProgressMin(pVec[pState - 1]);
      pr->setTaskProgressMax(pVec[pState]);
    }
    atb::Array<int,3> bwL(segmentation.shape(), segmentation.elementSizeUm());
    atb::connectedComponentLabelling(shell, bwL, atb::COMPLEX_NHOOD, pr);
    pState++;

    if (pr != NULL && !pr->updateProgressMessage(mVec[pState])) return;
    blitz::Array<double,1> volumeBuffer;
    volume(bwL, volumeBuffer);

    if (pr != NULL && !pr->updateProgress(pVec[pState])) return;
    pState++;

    if (debugFileName != "")
    {
      if (pr != NULL)
      {
        if (!pr->updateProgressMessage(mVec[pState])) return;
        pr->setTaskProgressMin(pVec[pState - 1]);
        pr->setTaskProgressMax(pVec[pState]);
      }
      try
      {
        bwL.save(debugFileName, "/bwL", 1, pr);
      }
      catch (BlitzH5Error &e)
      {
        std::string msg =
            "Could not save '" + debugFileName + ":/bwL': " + e.what();
        if (pr != NULL) pr->abortWithError(msg);
        else std::cerr << msg << std::endl;
        return;
      }
      pState++;
    }

    if (pr != NULL && !pr->updateProgressMessage(mVec[pState])) return;
    int largestComponentIndex = blitz::maxIndex(volumeBuffer)(0) + 1;
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(bwL.size()); ++i)
        shell.data()[i] = (bwL.data()[i] == largestComponentIndex);

    if (pr != NULL && !pr->updateProgress(pVec[pState])) return;
    pState++;

    if (pr != NULL && !pr->updateProgressMessage(mVec[pState])) return;
    boundaryDetection(shell, shell);

    if (pr != NULL && !pr->updateProgress(pVec[pState])) return;
    pState++;

    if (debugFileName != "")
    {
      if (pr != NULL)
      {
        if (!pr->updateProgressMessage(mVec[pState])) return;
        pr->setTaskProgressMin(pVec[pState - 1]);
        pr->setTaskProgressMax(pVec[pState]);
      }
      try
      {
        atb::Array<unsigned char,3> shellU(
            shell.shape(), shell.elementSizeUm(), shell.transformation());
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(shell.size()); ++i)
            shellU.data()[i] = shell.data()[i] ? 255 : 0;
        shellU.save(debugFileName, "/shell", 1, pr);
      }
      catch (BlitzH5Error &e)
      {
        std::string msg =
            "Could not save '" + debugFileName + ":/shell': " + e.what();
        if (pr != NULL) pr->abortWithError(msg);
        else std::cerr << msg << std::endl;
        return;
      }
      pState++;
    }
  
    if (pr != NULL && !pr->updateProgressMessage(mVec[pState])) return;
    std::vector<blitz::TinyVector<double, 3> > shellPoints;
    extractPosition(shell, shellPoints);
    std::vector<blitz::TinyVector<double, 3> > points;
    downSample(shellPoints, points, downSampleRatio);
    shellPoints.clear();
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (int i = 0; i < int(points.size()); i++)
        points[i] *= segmentation.elementSizeUm();
    std::cout << "  Npoints:" << points.size() << std::endl;

    bool posQCGiven =
        blitz::all(posQC != -std::numeric_limits<double>::infinity());
    bool xDirectionGiven =
        blitz::all(xDirection != -std::numeric_limits<double>::infinity());

    if (pr != NULL && !pr->updateProgress(pVec[pState])) return;
    pState++;

    if (pr != NULL)
    {
      if (!pr->updateProgressMessage(mVec[pState])) return;
      pr->setTaskProgressMin(pVec[pState - 1]);
      pr->setTaskProgressMax(pVec[pState]);
    }
    sct.fitModel(points, segmentLength, margin, posQCGiven, posQC,
                 xDirectionGiven, xDirection, pr);
    pState++;

    if (l != NULL && r != NULL && p != NULL)
    {
      // Dense coordinates requested... that is very slow, but if the user
      // really wants it, we compute it
      if (pr != NULL && !pr->updateProgressMessage(mVec[pState])) return;
      l->resize(segmentation.shape());
      l->setElementSizeUm(segmentation.elementSizeUm());
      r->resize(segmentation.shape());
      r->setElementSizeUm(segmentation.elementSizeUm());
      p->resize(segmentation.shape());
      p->setElementSizeUm(segmentation.elementSizeUm());
      atb::BlitzIndexT i = 0;
      int prMin = pVec[pState - 1];
      double prScale = static_cast<double>(pVec[pState] - prMin) /
          static_cast<double>(segmentation.extent(0) - 1);
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (int z = 0; z < segmentation.extent(0); z++)
      {
        int progress;
#ifdef _OPENMP
#pragma omp critical
#endif
        {
          progress = static_cast<int>(prMin + prScale * i);
          ++i;
        }
        if (pr != NULL && !pr->updateProgress(progress)) continue;
        for (int y = 0; y < segmentation.extent(1); y++)
        {
          for (int x = 0; x < segmentation.extent(2); x++)
          {
            if (pr != NULL && pr->isAborted()) break;
            blitz::TinyVector<double, 3> pos(
                z * segmentation.elementSizeUm()(0),
                y * segmentation.elementSizeUm()(1),
                x * segmentation.elementSizeUm()(2));
            pos = sct.getCoordinatesWithNormalizedRadius(pos);
            (*l)(z, y, x) = double(pos(0));
            (*r)(z, y, x) = double(pos(1));
            (*p)(z, y, x) = double(pos(2));
          }
        }
      }
      pState++;
      
      try
      {
        if (pr != NULL)
        {
          if (!pr->updateProgressMessage(mVec[pState])) return;
          pr->setTaskProgressMin(pVec[pState - 1]);
          pr->setTaskProgressMax(pVec[pState]);
        }
        l->save(debugFileName, "/l", 1, pr);
        pState++;
        if (pr != NULL)
        {
          if (!pr->updateProgressMessage(mVec[pState])) return;
          pr->setTaskProgressMin(pVec[pState - 1]);
          pr->setTaskProgressMax(pVec[pState]);
        }
        r->save(debugFileName, "/r", 1, pr);
        pState++;
        if (pr != NULL)
        {
          if (!pr->updateProgressMessage(mVec[pState])) return;
          pr->setTaskProgressMin(pVec[pState - 1]);
          pr->setTaskProgressMax(pVec[pState]);
        }
        p->save(debugFileName, "/p", 1, pr);
        pState++;
      }
      catch (BlitzH5Error &e)
      {
        std::string msg =
            "Could not save '" + debugFileName + ":/l,r,p': " + e.what();
        if (pr != NULL) pr->abortWithError(msg);
        else std::cerr << msg << std::endl;
      }
    }
    if (pr != NULL)
    {
      pr->setTaskProgressMin(pMin);
      pr->setTaskProgressMax(pMin + pScale);
      pr->updateProgress(pMin + pScale);
    }
  }

}
