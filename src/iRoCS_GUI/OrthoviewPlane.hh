/**************************************************************************
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

#ifndef IROCS_ORTHOVIEWPLANE_HH
#define IROCS_ORTHOVIEWPLANE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QWidget>
#include <QtGui/QColor>

#include <blitz/array.h>

namespace iRoCS {

  class OrthoviewWidget;

  /*======================================================================*/
  /*!
   *  \class OrthoviewPlane OrthoviewPlane.hh <liblabelling_qt4/OrthoviewPlane.hh>
   *  \brief The OrthoviewPlane class shows a main axis-aligned 2-D slice of a
   *    3-D dataset.
   *
   *  OrthoviewPlanes are part of OrthoviewWidgets and are automatically created
   *  when creating an OrthoviewWidget, there is no way of creating them without.
   *
   *  \see OrthoviewWidget
   */
   /*======================================================================*/
  class OrthoviewPlane : public QWidget {

    Q_OBJECT

  public:

    /*======================================================================*/
    /*!
     *   Create a new OrthoviewPlane object and associate it to the
     *   selected orthogonal dimension of the given OrthoviewWidget.
     *
     *   \param orthogonalDimension The orthogonal dimension this OrthoviewPlane
     *     will show
     *   \param orthoview The Orthoview this OrthoviewPlane belongs to
     *   \param parent    The parent QWidget of this OrthoviewPlane
     *   \param f         The window flags to apply to this OrthoviewPlane
     */
     /*======================================================================*/
    OrthoviewPlane(
      int orthogonalDimension, OrthoviewWidget *orthoview,
      QWidget *parent = NULL, Qt::WindowFlags f = 0);

    /*======================================================================*/
    /*!
     *   Delete this OrthoviewPlane.
     */
     /*======================================================================*/
    ~OrthoviewPlane();

    /*======================================================================*/
    /*!
     *   Get the 3-D dimensions corresponding to the y- and x-dimensions of
     *   this OrthoviewPlane widget.
     *
     *   \return (3-D dimension corresponding to plane's y-dimension,
     *            3-D dimension corresponding to plane's x-dimension)
     */
     /*======================================================================*/
    blitz::TinyVector<int, 2> const &planeDimensions() const;

    /*======================================================================*/
    /*!
     *   Get a read-only pointer to the Array containing the raw image data
     *   to show.
     *
     *   \return The raw image data pointer
     */
     /*======================================================================*/
    QRgb const *imageData() const;

    /*======================================================================*/
    /*!
     *   Get a random-access pointer to the Array containing the raw image data
     *   to show.
     *
     *   \return The raw image data pointer
     */
     /*======================================================================*/
    QRgb *imageData();

    /*======================================================================*/
    /*!
     *   Get the pixel position corresponding to the given absolute model
     *   position in micrometers projected to this OrthoviewPlane. The
     *   orthogonal dimension will be ignored. The projection does not respect
     *   the shape of the OrthoviewPlane, i.e. this function ignores the widget
     *   shape.
     *
     *   \param positionUm The micrometer position in model coordinates
     *
     *   \return The corresponding pixel position projected to the
     *     OrthoviewPlane
     */
     /*======================================================================*/
    QPointF absoluteUmToPx(
      blitz::TinyVector<double, 3> const &positionUm) const;

    /*======================================================================*/
    /*!
     *   Get the pixel offset corresponding to the given micrometer offset
     *   vector in model coordinates projected to this OrthoviewPlane. The
     *   orthogonal dimension will be ignored.
     *
     *   \param offsetUm The micrometer offset vector in model coordinates
     *
     *   \return The corresponding pixel offset projected to the
     *     OrthoviewPlane
     */
     /*======================================================================*/
    QPointF relativeUmToPx(
      blitz::TinyVector<double, 3> const &offsetUm) const;

    /*======================================================================*/
    /*!
     *   Get the absolute model position in micrometers for the given canvas
     *   pixel of this OrthoViewPlane. The orthogonal dimension will be set
     *   to the corresponding cross position, i.e. the shown slice position in
     *   micrometers.
     *
     *   \param x The x component of the pixel in the canvas
     *   \param y The y component of the pixel in the canvas
     *
     *   \return The absolute model coordinates of the given pixel coordinates
     *     in micrometers
     */
     /*======================================================================*/
    blitz::TinyVector<double, 3> absolutePxToUm(int x, int y) const;

    /*======================================================================*/
    /*!
     *   Get a relative offset vector in micrometers for the given canvas
     *   pixel offset w.r.t. this OrthoViewPlane. The orthogonal dimension will
     *   be set to zero.
     *
     *   \param x The x component of the pixel offset in the canvas
     *   \param y The y component of the pixel offset in the canvas
     *
     *   \return The relative offset vector in model coordinates of the
     *     given pixel offset in micrometers
     */
     /*======================================================================*/
    blitz::TinyVector<double, 3> relativePxToUm(int dx, int dy) const;

  signals:

    /*======================================================================*/
    /*!
     *   This signal is emitted whenever the mouse cursor position changes
     *   within the OrthoviewPlane. This signal is connected to the
     *   OrthoviewWidget::emitMousePositionChanged() slot to continuously emit
     *   the current micrometer position in the volume for visualization in the
     *   status bar or similar information widgets.
     *
     *   \param orthogonalDimension The orthogonal dimension index of this
     *     OrthoviewPlane object
     *   \param mousePositionPx The current mouse position in screen coordinates
     *     relative to the upper left widget corner
     */
     /*======================================================================*/
    void mousePositionChanged(
      int orthogonalDimension, QPoint const &mousePositionPx);

  private:

    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);

  private:

    OrthoviewWidget *p_orthoview;

    int _orthogonalDimension;
    blitz::TinyVector<int, 2> _planeDimensions;

    QRgb *p_imageData;

    enum DragState { None, View, Horizontal, Vertical, Cross } _dragState;
    QPoint _previousGlobalMousePositionPx;

  };

}

#endif
