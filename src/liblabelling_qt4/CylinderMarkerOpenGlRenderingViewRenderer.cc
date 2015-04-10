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

#include "CylinderMarkerOpenGlRenderingViewRenderer.hh"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

#include "MultiChannelModel.hh"
#include "AnnotationChannelSpecs.hh"

#include "CylinderMarker.hh"
#include "OpenGlRenderingViewWidget.hh"

#include "UserInteractionEvent.hh"

CylinderMarkerOpenGlRenderingViewRenderer::
CylinderMarkerOpenGlRenderingViewRenderer(
    CylinderMarker* marker, OpenGlRenderingViewWidget* view)
        : MarkerOpenGlRenderingViewRenderer(marker, view)
{}

CylinderMarkerOpenGlRenderingViewRenderer::
CylinderMarkerOpenGlRenderingViewRenderer(
    CylinderMarkerOpenGlRenderingViewRenderer const &renderer)
        : MarkerOpenGlRenderingViewRenderer(renderer)
{}

CylinderMarkerOpenGlRenderingViewRenderer::
~CylinderMarkerOpenGlRenderingViewRenderer()
{}
  
MarkerRenderer *CylinderMarkerOpenGlRenderingViewRenderer::clone() const
{
  return new CylinderMarkerOpenGlRenderingViewRenderer(*this);
}

void CylinderMarkerOpenGlRenderingViewRenderer::render(QPainter*) const
{
  if (p_marker->channel() == NULL)
  {
    std::cerr << "CylinderMarkerOpenGlRenderingViewRenderer::render(): "
              << "ERROR: Marker at position " << p_marker->positionUm()
              << " has no asociated channel!" << std::endl;
    return;
  }

  // To be implemented
  std::cerr << __FILE__ << ":" << __LINE__ << ": Implement me" << std::endl;
}

void CylinderMarkerOpenGlRenderingViewRenderer::userInteractionEvent(
    UserInteractionEvent *)
{}
