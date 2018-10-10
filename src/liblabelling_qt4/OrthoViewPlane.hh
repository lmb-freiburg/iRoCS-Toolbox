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

#ifndef ORTHOVIEWPLANE_HH
#define ORTHOVIEWPLANE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QWidget>

#include <blitz/array.h>

#include <map>
#include <list>

#include <libProgressReporter/ProgressReporter.hh>

class QWheelEvent;
class QMouseEvent;
class QXmlStreamWriter;

class OrthoViewWidget;
class OrthoViewOverlay;
class ChannelSpecs;
class ChannelSpecsOrthoViewRenderer;

class OrthoViewPlane : public QWidget
{

  Q_OBJECT

  public:

  OrthoViewPlane(
      OrthoViewWidget* orthoView, int orthogonalDimension,
      QWidget* parent = NULL);
  ~OrthoViewPlane();

  void addOverlay(OrthoViewOverlay* overlay);
  void removeOverlay(OrthoViewOverlay* overlay);
  blitz::TinyVector<double,3> mousePositionUm(int x, int y) const;
  blitz::TinyVector<double,3> mousePositionPx(int x, int y) const;

  OrthoViewWidget *orthoView();
  blitz::TinyVector<int,2> const &dimensions() const;
  int orthogonalDimension() const;
  std::string viewId() const;

  void paint(QPainter &painter);
  void paint(
      QXmlStreamWriter &svgStream,
      blitz::TinyVector<double,3> const &shapePx,
      blitz::TinyVector<double,3> const &lowerBoundUm,
      blitz::TinyVector<double,3> const &upperBoundUm,
      std::string const &font, double fontSizePt,
      iRoCS::ProgressReporter *pr = NULL);

  blitz::Array<blitz::TinyVector<unsigned char,4>,2> const &image() const;

public slots:

  void adjustSize();

/*======================================================================*/
/*!
 *   Repaint the RGB Array containing the fused information of all Data,
 *   Visualization and RGBChannels. This must be called before a view update,
 *   if the cache of one of those channels changed.
 */
/*======================================================================*/
  void updateData();

private:

  void paintEvent(QPaintEvent*);
  void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  void wheelEvent(QWheelEvent* e);

  OrthoViewWidget* p_orthoView;
  blitz::Array<blitz::TinyVector<unsigned char,4>,2> _data;
  blitz::TinyVector<int,2> _dimensions;
  int _orthogonalDimension;

  bool _dragVertical, _dragHorizontal, _isDragging;

  std::list<OrthoViewOverlay*> _overlays;

};

#endif
