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

#ifndef CYLINDERMARKERORTHOVIEWRENDERER_HH
#define CYLINDERMARKERORTHOVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "MarkerOrthoViewRenderer.hh"

class CylinderMarker;
class OrthoViewWidget;

class CylinderMarkerOrthoViewRenderer : public MarkerOrthoViewRenderer
{

Q_OBJECT

public:
  
  CylinderMarkerOrthoViewRenderer(
      CylinderMarker* marker, OrthoViewWidget* view);
  CylinderMarkerOrthoViewRenderer(
      CylinderMarkerOrthoViewRenderer const &renderer);
  virtual ~CylinderMarkerOrthoViewRenderer();
  
  virtual MarkerRenderer *clone() const;

  virtual void render(QPainter* painter = NULL) const;
  virtual void render(
      QXmlStreamWriter &svgStream,
      blitz::TinyVector<double,3> const &shapePx,
      blitz::TinyVector<double,3> const &lowerBoundUm,
      blitz::TinyVector<double,3> const &upperBoundUm,
      std::string const &font, double fontSizePt,
      iRoCS::ProgressReporter *pr = NULL) const;

  virtual void userInteractionEvent(UserInteractionEvent *event);

};

#endif
