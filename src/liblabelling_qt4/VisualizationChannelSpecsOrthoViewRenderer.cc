/**************************************************************************
 *
 * This file belongs to the iRoCS Toolbox.
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

// Include QFont to avoid usage of incomplete Implementation
#include <QtGui/QFont>
#include <QtGui/QMouseEvent>

#include "VisualizationChannelSpecsOrthoViewRenderer.hh"

#include "ColorMap.hh"

#include "MultiChannelModel.hh"
#include "VisualizationChannelSpecs.hh"
#include "OrthoViewWidget.hh"
#include "OrthoViewPlane.hh"
#include "OrthoViewUserInteractionEvent.hh"

#ifdef _OPENMP
#include <omp.h>
#endif

VisualizationChannelSpecsOrthoViewRenderer::
VisualizationChannelSpecsOrthoViewRenderer(
    VisualizationChannelSpecs* channel, OrthoViewWidget* view)
        : ChannelSpecsOrthoViewRenderer(channel, view)
{}

VisualizationChannelSpecsOrthoViewRenderer::
~VisualizationChannelSpecsOrthoViewRenderer()
{}

void VisualizationChannelSpecsOrthoViewRenderer::userInteractionEvent(
    UserInteractionEvent *event) {

  event->ignore();

  if (!p_channel->visible()) return;

  VisualizationChannelSpecs* channel{
    static_cast<VisualizationChannelSpecs*>(p_channel)};
  OrthoViewUserInteractionEvent* e{
    static_cast<OrthoViewUserInteractionEvent*>(event)};

  if (e->mouseEvent() == NULL) return;

  if (e->mouseEvent()->type() == QEvent::MouseButtonRelease) {

    if (e->mouseEvent()->button() == Qt::LeftButton) {

      if (e->mouseEvent()->modifiers().testFlag(Qt::ShiftModifier)) {

        // If left mouse button is released while shift is pressed, fill region
        channel->floodFill(
            e->orthoViewPlane()->mousePositionUm(
                e->mouseEvent()->x(), e->mouseEvent()->y()));
        e->accept();
        return;
      }
      else if (e->mouseEvent()->modifiers().testFlag(Qt::ControlModifier)) {

        // If left mouse button is released while ctrl is pressed, fill region
        // and remove 1px gaps to adjacent regions with same label
        // ==> fix over-segmentation
        channel->floodFill(
            e->orthoViewPlane()->mousePositionUm(
                e->mouseEvent()->x(), e->mouseEvent()->y()), true);
        e->accept();
        return;
      }
    }

    // If right mouse button is released while shift or ctrl is pressed,
    // set fill value
    if (e->mouseEvent()->button() == Qt::RightButton &&
        (e->mouseEvent()->modifiers().testFlag(Qt::ShiftModifier) ||
         e->mouseEvent()->modifiers().testFlag(Qt::ControlModifier))) {
      channel->pickFillValue(
          e->orthoViewPlane()->mousePositionUm(
              e->mouseEvent()->x(), e->mouseEvent()->y()));
      e->accept();
      return;
    }
  }
}

void VisualizationChannelSpecsOrthoViewRenderer::render(QPainter*) const
{
  // This is directly done by the OrthoView based on the current cache
}

void VisualizationChannelSpecsOrthoViewRenderer::render(
    QXmlStreamWriter &,
    blitz::TinyVector<double,3> const &,
    blitz::TinyVector<double,3> const &,
    blitz::TinyVector<double,3> const &,
    std::string const &, double, iRoCS::ProgressReporter *) const
{
  // Dummy stub... Visualization channel specs are rendered into an Array
  // in the ViewWidget and then directly written to SVG
}

blitz::TinyVector<atb::BlitzIndexT,3>
VisualizationChannelSpecsOrthoViewRenderer::cacheOffsetPx() const
{
  VisualizationChannelSpecs *channel =
      static_cast<VisualizationChannelSpecs*>(p_channel);
  MultiChannelModel *model = channel->model();
  return blitz::TinyVector<atb::BlitzIndexT,3>(
      blitz::floor(channel->lowerBoundUm() / model->elementSizeUm() + 0.5));
}

blitz::TinyVector<atb::BlitzIndexT,3>
VisualizationChannelSpecsOrthoViewRenderer::cacheShapePx() const
{
  VisualizationChannelSpecs *channel =
      static_cast<VisualizationChannelSpecs*>(p_channel);
  MultiChannelModel *model = channel->model();
  blitz::TinyVector<atb::BlitzIndexT,3> lbPx(cacheOffsetPx());
  blitz::TinyVector<atb::BlitzIndexT,3> ubPx(
      blitz::ceil(channel->upperBoundUm() / model->elementSizeUm()));
  return blitz::TinyVector<atb::BlitzIndexT,3>(ubPx - lbPx + 1);
}

blitz::Array<blitz::TinyVector<float,3>,2> const
&VisualizationChannelSpecsOrthoViewRenderer::cache(int direction) const
{
#ifdef DEBUG
  std::cerr << "VisualizationChannelSpecsOrthoViewRenderer::cache(" << direction
            << ")" << std::endl;
#endif
  OrthoViewWidget *view = static_cast<OrthoViewWidget*>(p_view);
  OrthoViewPlane *plane = view->orthoViewPlane(direction);
  blitz::TinyVector<int,2> dims(plane->dimensions());

  if (_cache(direction).extent(0) != cacheShapePx()(dims(0)) ||
      _cache(direction).extent(1) != cacheShapePx()(dims(1)))
      updateCache(direction);
  return _cache(direction);
}

void VisualizationChannelSpecsOrthoViewRenderer::updateCache(int direction)
    const
{
  if (!cacheUpdatesEnabled()) return;
#ifdef DEBUG
  std::cerr << "VisualizationChannelSpecsOrthoViewRenderer::updateCache("
            << direction << ")" << std::endl;
#endif

  OrthoViewWidget *view = static_cast<OrthoViewWidget*>(p_view);
  blitz::TinyVector<double,3> const &viewPosUm = view->positionUm();

  OrthoViewPlane *plane = view->orthoViewPlane(direction);
  blitz::TinyVector<int,2> dims(plane->dimensions());

  blitz::TinyVector<atb::BlitzIndexT,3> lbPx(cacheOffsetPx());
  blitz::TinyVector<atb::BlitzIndexT,3> shapePx(cacheShapePx());
  _cache(direction).resize(shapePx(dims(0)), shapePx(dims(1)));

  VisualizationChannelSpecs *channel =
      static_cast<VisualizationChannelSpecs*>(p_channel);
  blitz::TinyVector<double,3> const &channelLbUm = channel->lowerBoundUm();
  blitz::TinyVector<double,3> const &channelUbUm = channel->upperBoundUm();
  ColorMap &cmap = channel->colorMap();
  float alpha = channel->alpha();

  MultiChannelModel *model = channel->model();
  blitz::TinyVector<double,3> const &modelElSizeUm = model->elementSizeUm();

  if (viewPosUm(direction) >= channelLbUm(direction) -
      modelElSizeUm(direction) / 2.0 &&
      viewPosUm(direction) <= channelUbUm(direction) +
      modelElSizeUm(direction) / 2.0)
  {
    atb::Array<int,3>* data = channel->data();
    blitz::TinyVector<double,3> const &dataElSizeUm = data->elementSizeUm();

    blitz::TinyMatrix<double,4,4> const &trafo = data->transformation();
    bool simpleScaleTranslateTransform =
        trafo(0, 1) == 0.0 && trafo(0, 2) == 0.0 &&
        trafo(1, 0) == 0.0 && trafo(1, 2) == 0.0 &&
        trafo(2, 0) == 0.0 && trafo(2, 1) == 0.0 &&
        trafo(3, 0) == 0.0 && trafo(3, 1) == 0.0 &&
        trafo(3, 2) == 0.0 && trafo(3, 3) == 1.0;
    blitz::TinyVector<double,3> scaling(1.0);
    blitz::TinyVector<double,3> translation(0.0);
    if (simpleScaleTranslateTransform)
    {
      scaling = trafo(0, 0), trafo(1, 1), trafo(2, 2);
      translation = trafo(0, 3), trafo(1, 3), trafo(2, 3);
    }

    if (simpleScaleTranslateTransform)
    {
      if (blitz::all(scaling == 1.0) &&
          blitz::all(translation + lbPx * modelElSizeUm == 0.0) &&
          blitz::all(dataElSizeUm == modelElSizeUm))
      {
        // Direct copy
        atb::BlitzIndexT z = static_cast<atb::BlitzIndexT>(
            std::floor((viewPosUm(direction) + translation(direction)) /
                       dataElSizeUm(direction) + 0.5));
#ifdef DEBUG
        std::cerr << "VisualizationChannelSpecsOrthoViewRenderer::updateCache("
                  << direction << "): Identity transform detected: Direct copy "
                  << "of slice " << z << std::endl;
#endif
        if (z >= 0 && z < data->extent(direction))
        {
#ifdef _OPENMP
#pragma omp parallel for
#endif
          for (atb::BlitzIndexT y = 0; y < _cache(direction).extent(0); ++y)
          {
            blitz::TinyVector<atb::BlitzIndexT,3> srcPos;
            srcPos(direction) = z;
            srcPos(dims(0)) = y;
            if (srcPos(dims(0)) >= 0 && srcPos(dims(0)) < data->extent(dims(0)))
            {
              for (atb::BlitzIndexT x = 0; x < _cache(direction).extent(1); ++x)
              {
                srcPos(dims(1)) = x;
                if (srcPos(dims(1)) >= 0 &&
                    srcPos(dims(1)) < data->extent(dims(1)))
                    _cache(direction)(y, x) =
                        alpha * cmap.color((*data)(srcPos));
                else _cache(direction)(y, x) = blitz::TinyVector<float,3>(0.0f);
              }
            }
            else std::memset(&_cache(direction)(y, 0), 0,
                             _cache(direction).extent(1) * 3 * sizeof(float));
          }
        }
        else std::memset(_cache(direction).data(), 0,
                         _cache(direction).size() * 3 * sizeof(float));
      }
      else if (blitz::all(translation == 0.0))
      {
        // Scaled copy
#ifdef DEBUG
        std::cerr << "VisualizationChannelSpecsOrthoViewRenderer::updateCache("
                  << direction << "): Pure scaling transform detected"
                  << std::endl;
#endif
        double zUm =
            scaling(direction) * viewPosUm(direction) /
            dataElSizeUm(direction);
        double s0 = scaling(dims(0)) * modelElSizeUm(dims(0)) /
            dataElSizeUm(dims(0));
        double s1 = scaling(dims(1)) * modelElSizeUm(dims(1)) /
            dataElSizeUm(dims(1));
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (atb::BlitzIndexT y = 0; y < _cache(direction).extent(0); ++y)
        {
          blitz::TinyVector<double,3> srcPosPx;
          srcPosPx(direction) = zUm;
          srcPosPx(dims(0)) = s0 * y;
          for (atb::BlitzIndexT x = 0; x < _cache(direction).extent(1); ++x)
          {
            srcPosPx(dims(1)) = s1 * x;
            _cache(direction)(y, x) =
                alpha * cmap.color(data->interpolator().get(*data, srcPosPx));
          }
        }
      }
      else
      {
        // Scaled copy with subpixel translation
#ifdef DEBUG
        std::cerr << "VisualizationChannelSpecsOrthoViewRenderer::updateCache("
                  << direction << "): Scaling and translation detected: "
                  << "scaling = " << scaling << ", translation = "
                  << translation << std::endl;
#endif
        double zUm =
            (scaling(direction) * viewPosUm(direction) +
             translation(direction)) / dataElSizeUm(direction);
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (atb::BlitzIndexT y = 0; y < _cache(direction).extent(0); ++y)
        {
          blitz::TinyVector<double,3> srcPosPx;
          srcPosPx(direction) = zUm;
          srcPosPx(dims(0)) =
              (scaling(dims(0)) *
               (y * modelElSizeUm(dims(0)) + channelLbUm(dims(0))) +
               translation(dims(0))) / dataElSizeUm(dims(0));
          for (atb::BlitzIndexT x = 0; x < _cache(direction).extent(1); ++x)
          {
            srcPosPx(dims(1)) =
                (scaling(dims(1)) *
                 (x * modelElSizeUm(dims(1)) + channelLbUm(dims(1))) +
                 translation(dims(1))) / dataElSizeUm(dims(1));
            _cache(direction)(y, x) =
                alpha * cmap.color(data->interpolator().get(*data, srcPosPx));
          }
        }
      }
    }
    else
    {
      // Slanted plane copy
#ifdef DEBUG
      std::cerr << "VisualizationChannelSpecsOrthoViewRenderer::updateCache("
                << direction << "): General transform detected"
                << std::endl;
#endif
      double zUm = viewPosUm(direction);
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(_cache(direction).size()); ++i)
      {
        blitz::TinyVector<atb::BlitzIndexT,2> pos(
            i / _cache(direction).extent(1), i % _cache(direction).extent(1));
        blitz::TinyVector<double,3> posUm;
        posUm(dims(0)) = pos(0) * modelElSizeUm(dims(0)) + channelLbUm(dims(0));
        posUm(dims(1)) = pos(1) * modelElSizeUm(dims(1)) + channelLbUm(dims(1));
        posUm(direction) = zUm;
        _cache(direction).data()[i] = alpha * cmap.color(data->valueAt(posUm));
      }
    }
  }
  else std::memset(_cache(direction).data(), 0,
                   _cache(direction).size() * 3 * sizeof(float));

  emit cacheUpdated(this, direction);
  p_view->update();
}
