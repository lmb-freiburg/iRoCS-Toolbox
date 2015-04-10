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

#ifndef SURFACEMARKEROPENGLRENDERINGVIEWRENDERER_HH
#define SURFACEMARKEROPENGLRENDERINGVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "MarkerOpenGlRenderingViewRenderer.hh"

class SurfaceMarker;
class OpenGlRenderingViewWidget;

class SurfaceMarkerOpenGlRenderingViewRenderer :
    public MarkerOpenGlRenderingViewRenderer
{

Q_OBJECT

public:
  
  SurfaceMarkerOpenGlRenderingViewRenderer(
      SurfaceMarker* marker, OpenGlRenderingViewWidget* view);
  SurfaceMarkerOpenGlRenderingViewRenderer(
      SurfaceMarkerOpenGlRenderingViewRenderer const &renderer);
  virtual ~SurfaceMarkerOpenGlRenderingViewRenderer();
  
  virtual MarkerRenderer *clone() const;

  virtual void render(QPainter* painter = NULL) const;

  virtual void userInteractionEvent(UserInteractionEvent *event);

};

#endif
