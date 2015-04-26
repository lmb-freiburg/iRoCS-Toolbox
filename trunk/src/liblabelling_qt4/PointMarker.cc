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

#include "PointMarker.hh"

#include "AnnotationChannelSpecs.hh"

#include "OrthoViewWidget.hh"
#include "PointMarkerOrthoViewRenderer.hh"

#include "OpenGlRenderingViewWidget.hh"
#include "PointMarkerOpenGlRenderingViewRenderer.hh"

// #include <blitz/tinyvec-et.h>

PointMarkerPresetWidget::PointMarkerPresetWidget(
    AnnotationChannelSpecs* channel, QWidget* parent) 
        : MarkerPresetWidget(channel, parent)
{}

PointMarkerPresetWidget::~PointMarkerPresetWidget()
{}


PointMarkerControlWidget::PointMarkerControlWidget(
    AnnotationChannelSpecs* channel, QWidget* parent)
        : MarkerControlWidget(channel, parent)
{}

PointMarkerControlWidget::~PointMarkerControlWidget()
{}


PointMarker::PointMarker(const blitz::TinyVector<double,3>& position,
                         AnnotationChannelSpecs* channel)
        : Marker(position, channel)
{}
  
PointMarker::PointMarker(const PointMarker& marker)
        : Marker(marker)
{}
  
PointMarker::~PointMarker() 
{}

PointMarker &PointMarker::operator=(const PointMarker& marker)
{
  Marker::operator=(static_cast<Marker const &>(marker));
  return *this;
}

Marker::MarkerType PointMarker::markerType() const 
{
  return Marker::Point;
}

bool PointMarker::inherits(Marker::MarkerType type) const 
{
  return type == Marker::Point;
}

MarkerRenderer *PointMarker::addRenderer(ViewWidget* view)
{ 
  if (_renderers.find(view) != _renderers.end()) return _renderers[view];
  
  switch (view->viewType())
  {
  case ViewWidget::OrthoView:
    _renderers[view] = new PointMarkerOrthoViewRenderer(
        this, static_cast<OrthoViewWidget*>(view));
    return _renderers[view];
  case ViewWidget::OpenGlRenderingView:
    _renderers[view] = new PointMarkerOpenGlRenderingViewRenderer(
        this, static_cast<OpenGlRenderingViewWidget*>(view));
    return _renderers[view];
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Implementation missing"
              << std::endl;
    break;
  }
  return NULL;
}

void PointMarker::save(
    AnnotationChannelSpecs const *channel,
    BlitzH5File &outFile, std::string const &group,
    iRoCS::ProgressReporter *pr)
{
  Marker::save(channel, outFile, group, pr);
}

void PointMarker::load(
    AnnotationChannelSpecs *channel, BlitzH5File const &inFile,
    std::string const &group, iRoCS::ProgressReporter *pr)
{
  Marker::load(channel, inFile, group, pr);
}

void PointMarker::saveCSV(std::ofstream &out) const
{
  Marker::saveCSV(out);
}

void PointMarker::loadFromMap(
    std::map<std::string,std::string> const &values)
{
  Marker::loadFromMap(values);
}

bool PointMarker::occupiesPositionUm(
    blitz::TinyVector<double,3> const &positionUm) const
{
  return blitz::all(positionUm > boundingBoxLowerBoundUm() - 5.0 &&
                    positionUm < boundingBoxUpperBoundUm() + 5.0);
}

void PointMarker::_updateBoundingBox() const
{
  _boundingBoxLowerBoundUm = 0.0;
  _boundingBoxUpperBoundUm = 0.0;
  _boundingBoxUpToDate = true;
}
