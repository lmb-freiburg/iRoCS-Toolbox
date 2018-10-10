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

#ifndef ORTHOVIEWWIDGET_HH
#define ORTHOVIEWWIDGET_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ViewWidget.hh"

#include <blitz/array.h>

#include <libProgressReporter/ProgressReporter.hh>

class MultiChannelModel;
class OrthoViewPlane;
class ChannelSpecsOrthoViewRenderer;

class QScrollArea;
class QSplitter;
class QComboBox;
class QXmlStreamWriter;

class Double3ControlElement;
class IntControlElement;

class OrthoViewWidget : public ViewWidget
{

  Q_OBJECT

public:

  OrthoViewWidget(MultiChannelModel* model, QWidget* parent = 0,
                  Qt::WindowFlags f = 0);
  ~OrthoViewWidget();

  ViewWidget::ViewType viewType() const;

  void addChannel(ChannelSpecs *channel);
  void removeChannel(ChannelSpecs* channel);

  OrthoViewPlane* xyView();
  OrthoViewPlane* xzView();
  OrthoViewPlane* zyView();

  OrthoViewPlane *orthoViewPlane(int orthogonalDimension);

  bool interpolation() const;
  double zoom() const;
  int fontSize() const;

/*======================================================================*/
/*!
 *   Get the pixel position of the given model position in micrometers in
 *   the view widget. This transformation respects the model bounding box
 *   and the currently set zoom of the view.
 *
 *   \param positionUm The micrometer position to locate in the view widget
 *
 *   \return The corresponding pixel position in the view widget.
 */
/*======================================================================*/
  blitz::TinyVector<float,3> um2Px(
      blitz::TinyVector<float,3> const &positionUm);

/*======================================================================*/
/*!
 *   Get the pixel distance in the view for the given micrometer distance.
 *
 *   \param distanceUm The micrometer distance
 *
 *   \return The same distance in pixels.
 */
/*======================================================================*/
  double scaleToPx(double distanceUm);

/*======================================================================*/
/*!
 *   Get the pixel position of the given model position in micrometers in
 *   the view widget for the specified dimension. This transformation
 *   respects the model bounding box and the currently set zoom of the view.
 *
 *   \param positionUm The micrometer position to locate in the view widget
 *   \param dimension  The vector component of the position to evaluate
 *
 *   \return The corresponding pixel position along the given dimension
 *     in the view widget.
 */
/*======================================================================*/
  double um2Px(double positionUm, int dimension);

/*======================================================================*/
/*!
 *   Get the model micrometer position of the given pixel position.
 *   This transformation respects the model bounding box
 *   and the currently set zoom of the view.
 *
 *   \param positionPx The pixel position in the view
 *
 *   \return The corresponding micrometer position in the model.
 */
/*======================================================================*/
  blitz::TinyVector<float,3> px2Um(
      blitz::TinyVector<float,3> const &positionPx);

/*======================================================================*/
/*!
 *   Get the micrometer distance for the given pixel distance in the view.
 *
 *   \param distancePx The pixel distance
 *
 *   \return The same distance in micrometers.
 */
/*======================================================================*/
  double scaleToUm(double distancePx);

/*======================================================================*/
/*!
 *   Get the model micrometer position of the given pixel position along
 *   the specified dimension. This transformation respects the model
 *   bounding box and the currently set zoom of the view.
 *
 *   \param positionPx The pixel position in the view
 *   \param dimension  The vector component of the position to evaluate
 *
 *   \return The corresponding micrometer position in the model.
 */
/*======================================================================*/
  double px2Um(double positionPx, int dimension);

  void paint(QPainter &painter);
  void paint(
      QXmlStreamWriter &svgStream,
      blitz::TinyVector<double,3> const &shapePx, double marginPx,
      blitz::TinyVector<double,3> const &lowerBoundUm,
      blitz::TinyVector<double,3> const &upperBoundUm,
      double scaleBarLengthUm, std::string const &font, double fontSizePt,
      iRoCS::ProgressReporter *pr = NULL);

signals:

  void zoomChanged(double zoom);

public slots:

  virtual void redraw();
  void updateShape();
  void setInterpolation(bool interpolation);
  void setZoom(double zoom);
  void updatePlaneData(
      ChannelSpecsOrthoViewRenderer const *renderer, int direction);

private slots:

  void resetZoom();
  void zoomIn();
  void zoomOut();
  void updateZoomFromComboBox();
  void updateZoomFromComboBox(QString const &str);
  void setZoomInComboBox(double zoom);
  void updatePositionUmFromControl();
  void setPositionUmControlRangeFromModel();
  void setPositionUmInControl(blitz::TinyVector<double,3> const &positionUm);
  void synchronizeSplitters(int pos, int);
  void updateCacheFontSize();

private:

  blitz::TinyVector<double,3> _cachedPosition;
  bool _interpolation;
  double _zoom;

  QSplitter* p_vSplitter;
  QSplitter* p_uhSplitter;
  QSplitter* p_lhSplitter;
  QScrollArea* p_xyScrollArea;
  OrthoViewPlane* p_xyView;
  QScrollArea* p_xzScrollArea;
  OrthoViewPlane* p_xzView;
  QScrollArea* p_zyScrollArea;
  OrthoViewPlane* p_zyView;

  QAction *p_resetZoomAction;
  QAction *p_zoomInAction;
  QAction *p_zoomOutAction;
  QComboBox *p_zoomControl;

  Double3ControlElement *p_crosshairPositionControl;
  IntControlElement *p_fontSizeControl;

  friend class OrthoViewPlane;

};

#endif
