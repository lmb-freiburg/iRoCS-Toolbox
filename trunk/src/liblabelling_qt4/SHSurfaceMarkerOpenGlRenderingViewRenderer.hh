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

#ifndef SHSURFACEMARKEROPENGLRENDERINGVIEWRENDERER_HH
#define SHSURFACEMARKEROPENGLRENDERINGVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "SphereMarkerOpenGlRenderingViewRenderer.hh"

class SHSurfaceMarker;
class OpenGlRenderingViewWidget;

class SHSurfaceMarkerOpenGlRenderingViewRenderer :
    public SphereMarkerOpenGlRenderingViewRenderer
{

Q_OBJECT

public:
  
  SHSurfaceMarkerOpenGlRenderingViewRenderer(
      SHSurfaceMarker* marker, OpenGlRenderingViewWidget* view);
  SHSurfaceMarkerOpenGlRenderingViewRenderer(
      SHSurfaceMarkerOpenGlRenderingViewRenderer const &renderer);
  ~SHSurfaceMarkerOpenGlRenderingViewRenderer();
  
  MarkerRenderer *clone() const;

  void render(QPainter* painter = NULL) const;

  void userInteractionEvent(UserInteractionEvent *event);

};

#endif
