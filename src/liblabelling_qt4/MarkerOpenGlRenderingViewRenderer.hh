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

#ifndef MARKEROPENGLRENDERINGVIEWRENDERER_HH
#define MARKEROPENGLRENDERINGVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "MarkerRenderer.hh"

#include <blitz/array.h>

class OpenGlRenderingViewWidget;

class MarkerOpenGlRenderingViewRenderer : public MarkerRenderer
{

Q_OBJECT

public:
  
  MarkerOpenGlRenderingViewRenderer(
      Marker* marker, OpenGlRenderingViewWidget* view);
  MarkerOpenGlRenderingViewRenderer(
      MarkerOpenGlRenderingViewRenderer const &renderer);
  virtual ~MarkerOpenGlRenderingViewRenderer();

  virtual MarkerRenderer *clone() const = 0;

protected:
  
/*======================================================================*/
/*! 
 *   Set the pen according to the PresetWidget
 *
 *   \param hilighted  If the marker is selected set this bool to true
 */
/*======================================================================*/
  void _setPenColor(bool hilighted = false) const;

};

#endif
