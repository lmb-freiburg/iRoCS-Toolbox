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

#include "ConvertMasksToMarkersWorker.hh"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#endif 

#include "ConvertMasksToMarkersParameters.hh"

#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/VisualizationChannelSpecs.hh>
#include <liblabelling_qt4/AnnotationChannelSpecs.hh>
#include <liblabelling_qt4/SurfaceMarker.hh>
#include <liblabelling_qt4/CellMarker.hh>

#include <libArrayToolbox/GaussianFilter.hh>
#include <libArrayToolbox/MarchingCubes.hh>
#include <libArrayToolbox/ATBMorphology.hh>
#include <libArrayToolbox/ATBDataSynthesis.hh>
#include <libArrayToolbox/TypeTraits.hh>

ConvertMasksToMarkersWorker::ConvertMasksToMarkersWorker(
    ConvertMasksToMarkersParameters* dialog, LabellingMainWidget* mainWidget,
    AnnotationChannelSpecs* markerChannel,
    iRoCS::ProgressReporterQt4* progress, QWidget* parent) 
        : QThread(parent), p_dialog(dialog), p_mainWidget(mainWidget),
          p_markerChannel(markerChannel), p_progress(progress)
{}

ConvertMasksToMarkersWorker::~ConvertMasksToMarkersWorker()
{}

void ConvertMasksToMarkersWorker::run()
{
  atb::Array<int,3> *masks = p_dialog->maskChannel()->data();
  blitz::TinyVector<double,3> elSize(masks->elementSizeUm());
  ColorMap const &cmap = p_dialog->maskChannel()->colorMap();

  blitz::Array<atb::BlitzIndexT,3> labels(masks->shape());
  atb::BlitzIndexT nSegments;

  // Show indefinite progress
  if (p_progress != NULL) p_progress->setProgressMax(0);

  if (p_dialog->doConnectedComponentLabelling())
  {
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            tr("Binarizing...").toStdString())) return;
    blitz::Array<bool,3> binary(masks->shape());
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(masks->size()); ++i)
        binary.data()[i] = blitz::any(cmap.color(masks->data()[i]) != 0.0f);
    
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            tr("Connected component labelling...").toStdString())) return;
    
    atb::connectedComponentLabelling(binary, labels, atb::SIMPLE_NHOOD);
    nSegments = blitz::max(labels);
  }
  else
  {
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            tr("Mapping segment intensities to labels").toStdString())) return;
    std::map<atb::BlitzIndexT,atb::BlitzIndexT> labelMap;
    for (size_t i = 0; i < masks->size(); ++i)
    {
      if (blitz::all(cmap.color(masks->data()[i])) == 0.0f)
      {
        labels.data()[i] = 0;
        continue;
      }
      std::map<atb::BlitzIndexT,atb::BlitzIndexT>::iterator it =
          labelMap.find(masks->data()[i]);
      if (it == labelMap.end())
          it = labelMap.insert(
              labelMap.begin(), std::pair<atb::BlitzIndexT,atb::BlitzIndexT>(
                  masks->data()[i], labelMap.size() + 1));
      labels.data()[i] = it->second;
    }
    nSegments = labelMap.size();
  }
  std::cout << "  Found " << nSegments << " segments" << std::endl;

  if (p_progress != NULL) p_progress->setProgressMax(100);

  if (p_progress != NULL && !p_progress->updateProgressMessage(
          tr("Extracting component positions, volumes and bounding "
             "boxes...").toStdString())) return;
  blitz::Array<int,1> label(nSegments);
  blitz::Array<atb::BlitzIndexT,1> volumePx(nSegments);
  std::memset(volumePx.data(), 0, volumePx.size() * sizeof(atb::BlitzIndexT));
  blitz::Array<blitz::TinyVector<double,3>,1> positionUm(nSegments);
  std::memset(positionUm.data(), 0,
              positionUm.size() * sizeof(blitz::TinyVector<double,3>));
  blitz::Array<blitz::TinyVector<atb::BlitzIndexT,3>,1> lb(nSegments);
  lb = labels.shape();
  blitz::Array<blitz::TinyVector<atb::BlitzIndexT,3>,1> ub(nSegments);
  ub = blitz::TinyVector<atb::BlitzIndexT,3>(static_cast<atb::BlitzIndexT>(0));
  
  blitz::TinyVector<atb::BlitzIndexT,3> p;
  atb::BlitzIndexT currentVoxel = 1;
  for (p(0) = 0; p(0) < labels.extent(0); ++p(0))
  {
    if (p_progress != NULL)
        p_progress->updateProgress(
            static_cast<int>(
                10.0 * static_cast<double>(currentVoxel) /
                static_cast<double>(labels.size())));
    for (p(1) = 0; p(1) < labels.extent(1); ++p(1))
    {
      for (p(2) = 0; p(2) < labels.extent(2); ++p(2), ++currentVoxel)
      {
        if (p_progress != NULL && p_progress->isAborted()) return;
        atb::BlitzIndexT seg = labels(p) - 1;
        if (seg < 0) continue;
        label(seg) = (*masks)(p);
        volumePx(seg)++;
        positionUm(seg) += blitz::TinyVector<double,3>(p) * elSize;
        for (int d = 0; d < 3; ++d)
        {
          if (p(d) < lb(seg)(d)) lb(seg)(d) = p(d);
          if (p(d) > ub(seg)(d)) ub(seg)(d) = p(d);
        }
      }
    }
  }
  for (atb::BlitzIndexT i = 0; i < nSegments; ++i)
      positionUm(i) /= static_cast<double>(volumePx(i));

  std::cout << "Extracting surfaces of all components" << std::endl;
  int currentSegment = 1;
#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (atb::BlitzIndexT i = 0; i < nSegments; ++i)
  {
    if (p_progress != NULL)
    {
      if (p_progress->isAborted()) continue;
      int progress;
#ifdef _OPENMP
#pragma omp critical
#endif
      {
        progress = currentSegment;
        ++currentSegment;
      }
      p_progress->updateProgress(
          static_cast<int>(90.0 * static_cast<double>(progress) /
                           static_cast<double>(nSegments) + 10.0));
      p_progress->updateProgressMessage(
          tr("Generating marker %1 / %2").arg(
              progress).arg(nSegments).toStdString());
    }

    // Extract mask
    blitz::Array<float,3> mask(
        blitz::TinyVector<atb::BlitzIndexT,3>(ub(i) - lb(i) + 3));
    mask = -1.0f;
    blitz::TinyVector<atb::BlitzIndexT,3> p2;
    for (p2(0) = 0; p2(0) < mask.extent(0) - 2; ++p2(0))
    {
      if (p_progress != NULL && p_progress->isAborted()) break;
      for (p2(1) = 0; p2(1) < mask.extent(1) - 2; ++p2(1))
      {
        if (p_progress != NULL && p_progress->isAborted()) break;
        for (p2(2) = 0; p2(2) < mask.extent(2) - 2; ++p2(2))
        {
          if (p_progress != NULL && p_progress->isAborted()) break;
          mask(blitz::TinyVector<atb::BlitzIndexT,3>(p2 + 1)) =
              (labels(blitz::TinyVector<atb::BlitzIndexT,3>(p2 + lb(i))) ==
               i + 1) ? 1.0f : -1.0f;
        }
      }
    }

    if (p_progress != NULL && p_progress->isAborted()) continue;

    if (p_dialog->smoothingSigmaPx() > 0.0)
    {
      atb::GaussianFilter<float,3> filter(atb::ValueBT, -1.0f);
      filter.setStandardDeviationUm(p_dialog->smoothingSigmaPx());
      filter.apply(mask, blitz::TinyVector<double,3>(1.0), mask);
    }

    if (p_progress != NULL && p_progress->isAborted()) continue;

    // Extract surface using Marching Cubes
    atb::SurfaceGeometry surface;
    atb::MarchingCubes::triangulate(mask, elSize, surface);
    if (surface.vertices().size() == 0 ||
        (p_progress != NULL && p_progress->isAborted())) continue;

    // Simplify surface
    atb::MarchingCubes::simplifyMesh(surface, p_dialog->simplifyToleranceUm3());
    surface.computeDefaultNormals();
    if (p_progress != NULL && p_progress->isAborted()) continue;

    // Prepare marker
    SurfaceMarker *marker;
    if (p_markerChannel->markerType() == Marker::Surface)
        marker = new SurfaceMarker(positionUm(i), &surface);
    else marker = new CellMarker(positionUm(i), &surface);
    marker->setLabel(label(i));
    if (marker->inherits(Marker::Cell))
    {
      static_cast<CellMarker*>(marker)->setVolumeUm3(
          static_cast<double>(volumePx(i)) * blitz::product(elSize));
    }

    for (size_t j = 0; j < marker->vertices().size(); ++j)
        marker->vertices()[j] += blitz::TinyVector<float,3>(
            blitz::TinyVector<double,3>(lb(i) - 1) *
            elSize - positionUm(i));
#ifdef _OPENMP
#pragma omp critical
#endif
    p_markerChannel->addMarker(marker);
  }
}

void ConvertMasksToMarkersWorker::abort()
{
  p_progress->abort();
}
