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

#include "MarkerOpenGlRenderingViewRenderer.hh"

#include "OpenGlRenderingViewWidget.hh"
#include "AnnotationChannelSpecs.hh"
#include "NucleusMarker.hh"
#include "CellMarker.hh"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

MarkerOpenGlRenderingViewRenderer::MarkerOpenGlRenderingViewRenderer(
    Marker* marker, OpenGlRenderingViewWidget* view)
        : MarkerRenderer(marker, view)
{}

MarkerOpenGlRenderingViewRenderer::MarkerOpenGlRenderingViewRenderer(
   MarkerOpenGlRenderingViewRenderer const &renderer)
        : MarkerRenderer(renderer)
{}

MarkerOpenGlRenderingViewRenderer::~MarkerOpenGlRenderingViewRenderer()
{}

void MarkerOpenGlRenderingViewRenderer::_setPenColor(bool hilighted) const
{
  AnnotationChannelSpecs *channel = p_marker->channel();
  blitz::TinyVector<float,4> diffuseColor, ambientColor, specularColor;
  switch (channel->selectedViewType())
  {
  case 0 : // Label
  {
    blitz::TinyVector<float,3> c(channel->colorForLabel(p_marker->label()));

    int sublabel = -1;
    if (p_marker->inherits(Marker::Nucleus))
        sublabel = static_cast<NucleusMarker*>(p_marker)->subtype();
    else if (p_marker->inherits(Marker::Cell))
        sublabel = static_cast<CellMarker*>(p_marker)->subtype();
    if (sublabel > 0) c += static_cast<float>(sublabel) * 0.2f;
    for (int d = 0; d < 3; ++d) if (c(d) > 1.0f) c(d) = 1.0f;
    
    diffuseColor = c(0), c(1), c(2),
        channel->alpha() * channel->alphaForLabel(p_marker->label());
    ambientColor = 0.1f * diffuseColor(0), 0.1f * diffuseColor(1),
        0.1f * diffuseColor(2), diffuseColor(3);
    specularColor = 1.0f, 1.0f, 1.0f, diffuseColor(3);
    break;
  }
  case 1 : // Predicted Label
  {
    blitz::TinyVector<float,3> c(
        channel->colorForLabel(p_marker->predictedLabel()));

    int sublabel = -1;
    if (p_marker->inherits(Marker::Nucleus))
        sublabel = static_cast<NucleusMarker*>(p_marker)->predictedSubtype();
    else if (p_marker->inherits(Marker::Cell))
        sublabel = static_cast<CellMarker*>(p_marker)->predictedSubtype();
    if (sublabel > 0) c += static_cast<float>(sublabel) * 0.2f;
    for (int d = 0; d < 3; ++d) if (c(d) > 1.0f) c(d) = 1.0f;
    
    diffuseColor = c(0), c(1), c(2),
        channel->alpha() * channel->alphaForLabel(p_marker->predictedLabel());
    ambientColor = 0.1f * diffuseColor(0), 0.1f * diffuseColor(1),
        0.1f * diffuseColor(2), diffuseColor(3);
    specularColor = 1.0f, 1.0f, 1.0f, diffuseColor(3);
    break;
  }
  case 2 : // Confusion
  {
    blitz::TinyVector<float,3> col;
    if (p_marker->manual() && p_marker->predictedLabel() != -1)
    {
      if (p_marker->label() == p_marker->predictedLabel())
          col = 0.0f, 1.0f, 0.0f;
      else col = 1.0f, 0.0f, 0.0f;
    }
    else col = 1.0f, 1.0f, 0.0f;
    diffuseColor = col(0), col(1), col(2), channel->alpha();
    ambientColor = 0.1f * diffuseColor(0), 0.1f * diffuseColor(1),
        0.1f * diffuseColor(2), diffuseColor(3);
    specularColor = 1.0f, 1.0f, 1.0f, diffuseColor(3);
    break;
  }
  case 3: // Random
  {
    blitz::TinyVector<float,3> const &col = p_marker->randomColor();
    diffuseColor = col(0), col(1), col(2), channel->alpha();
    ambientColor = 0.1f * diffuseColor(0), 0.1f * diffuseColor(1),
        0.1f * diffuseColor(2), diffuseColor(3);
    specularColor = 1.0f, 1.0f, 1.0f, diffuseColor(3);
    break;    
  }
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Missing implementation"
              << std::endl;
    exit(-1);
  }
  
  if (hilighted)
  {
    ambientColor(0) = 0.7f * diffuseColor(0);
    ambientColor(1) = 0.7f * diffuseColor(1);
    ambientColor(2) = 0.7f * diffuseColor(2);
  }

  // Unlit case
  glColor4f(diffuseColor(0), diffuseColor(1), diffuseColor(2), diffuseColor(3));
  
  // With lighting enabled
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientColor.data());
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseColor.data());
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor.data());
}

