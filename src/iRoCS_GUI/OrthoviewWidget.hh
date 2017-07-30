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

#ifndef IROCS_ORTHOVIEWWIDGET_HH
#define IROCS_ORTHOVIEWWIDGET_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QWidget>
#include "MultichannelModelObserver.hh"

#include <blitz/array.h>

#include <set>

class QSplitter;
class QComboBox;

namespace iRoCS {

  class OrthoviewPlane;
  class OrthoviewRenderer;

  /*======================================================================*/
  /*!
   *  \class OrthoviewWidget OrthoviewWidget.hh <liblabelling_qt4/OrthoviewWidget.hh>
   *  \brief The OrthoviewWidget class shows three orthogonal slices through
   *    a 3D MultichannelModel at a specified and adjustable crosshair position.
   *
   *  \see OrthoviewPlane
   */
   /*======================================================================*/
  class OrthoviewWidget : public QWidget, public MultichannelModelObserver {

    Q_OBJECT

#ifdef DEBUG_VERBOSE_XML
      std::string printState() const;
#endif

  public:

    /*======================================================================*/
    /*!
     *   Create a new OrthoviewWidget with given parent and window flags.
     *   An OrthoviewWidget consists of three OrthoviewPlane widgets that
     *   show the xy-, xz- and zy- cuts through the associated 3D
     *   MultiChannelModel at the current crosshair position.
     *
     *   The proportions of the shown OrthoviewPlanes can be manually adjusted
     *   and will be stored in the settings of the application this widget
     *   belongs to and restored on application startup.
     *
     *   Additionally to the OrthoviewPlanes it contains a toolbar to set
     *   crosshair position, zoom and font size.
     *
     *   \param parent The parent widget to associate this OrthoviewWidget to.
     *     If set this widget will be deleted when its parent is deleted. If set
     *     to NULL this widget will be a top level widget and the user is
     *     responsible for deleting it when quitting the application. Adding
     *     this widget to a layout will automatically reparent it to belong to
     *     the layouted widget.
     *  \param f You can pass additional window flags that control the behaviour
     *     of the widget. See the Qt QWidget documentation for further details.
     */
     /*======================================================================*/
    OrthoviewWidget(
      MultichannelModel &model, QWidget *parent = NULL,
      Qt::WindowFlags f = 0);

    /*======================================================================*/
    /*!
     *   Delete this OrthoviewWidget and all children.
     */
     /*======================================================================*/
    ~OrthoviewWidget();

    /*======================================================================*/
    /*!
     *   This method is called <b>after</b> a channel was added to the
     *   MultichannelModel.
     *
     *   An implementation should insert a corresponding entry to the Observer's
     *   view.
     *
     *   If updates for this observer are disabled, calling this method is a
     *   noop.
     *
     *   \param index A pointer to the Channel that was added to the model
     */
     /*======================================================================*/
    void updateAddChannel(Channel *channel);

    /*======================================================================*/
    /*!
     *   This method is called <b>before</b> a channel is deleted from the
     *   MultichannelModel.
     *
     *   An implementation must remove the corresponding entry from the
     *   Observer's view. Immediately after execution of this method the
     *   Channel is deleted, so make sure that the Observer is not able to
     *   accidentally access it later.
     *
     *   If updates for this observer are disabled, calling this method is a
     *   noop.
     *
     *   \param index A pointer to the Channel that will be removed from the
     *     model
     */
     /*======================================================================*/
    void updateRemoveChannel(Channel *channel);

    /*======================================================================*/
    /*!
     *   This method is called when the modified flag of the observed
     *   MultichannelModel changes. Corresponding views should be updated
     *   accordingly.
     */
     /*======================================================================*/
    void updateModified(bool modified);

    /*======================================================================*/
    /*!
     *   This method updates the complete view. Call it for intialization or
     *   after extended model changes.
     */
     /*======================================================================*/
    void updateModel();

    /*======================================================================*/
    /*!
     *   Get the current crosshair position in micrometers according to the
     *   underlying model.
     *
     *   \return The current crosshair position in micrometers
     */
     /*======================================================================*/
    blitz::TinyVector<double, 3> const &crossPositionUm() const;

    /*======================================================================*/
    /*!
     *   Get the position of the central pixel of this view in micrometers.
     *
     *   \return The voxel position in micrometers this view is centered at
     */
     /*======================================================================*/
    blitz::TinyVector<double, 3> const &centerPositionUm() const;

    /*======================================================================*/
    /*!
     *   Get the extents of a pixel of this view in micrometers.
     *
     *   \return The voxel extents in micrometers
     */
     /*======================================================================*/
    blitz::TinyVector<double, 3> const &pixelSizeUm() const;

    /*======================================================================*/
    /*!
     *   Get the pointer to the OrthoviewPlane for the specified dimension.
     *
     *   \return A pointer to the OrthoviewPlane perpendicular to the
     *     given orthogonal dimension
     */
     /*======================================================================*/
    OrthoviewPlane *plane(int orthogonalDimension) const;

    /*======================================================================*/
    /*!
     *   Get the curent magnification setting of this OrthoviewWidget.
     *
     *   \return The currently set magnification (1 = original size)
     */
     /*======================================================================*/
    double magnification() const;

    /*======================================================================*/
    /*!
     *   Get the current visibility state of the scale bar that is shown in
     *   the top left OrthoviewPlane.
     *
     *   \return \c true if the scale bar is currently visible, \c false
     *     otherwise
     */
     /*======================================================================*/
    bool scalebarVisible() const;

    /*======================================================================*/
    /*!
     *   Get the current interpolation state.
     *
     *   \return \c true : linear interpolation, \c false : nearest interpolation
     */
     /*======================================================================*/
    bool interpolationEnabled() const;

    /*======================================================================*/
    /*!
     *   Get a read-only reference to the list of renderers associated
     *   with this OrthoviewWidget.
     *
     *   \return A reference to the list of OrthoviewRenderers
     */
     /*======================================================================*/
    std::set<OrthoviewRenderer*> const &renderers() const;

    public slots:

    /*======================================================================*/
    /*!
     *   Show/Hide the scale bar.
     *
     *   \param show The new visibility state of the scale bar
     */
     /*======================================================================*/
    void setScalebarVisible(bool show);

    /*======================================================================*/
    /*!
     *   Enable/Disable linear interpolation in the data Array. If disabled,
     *   nearest interpolation is used.
     *
     *   \param enable The new linear interpolation state
     */
     /*======================================================================*/
    void setInterpolationEnabled(bool enable);

    /*======================================================================*/
    /*!
     *   Get the current crosshair position in micrometers according to the
     *   underlying model.
     *
     *   \return The current crosshair position in micrometers
     */
     /*======================================================================*/
    void setCrossPositionUm(blitz::TinyVector<double, 3> const &positionUm);

    /*======================================================================*/
    /*!
     *   Get the position of the central pixel of this view in micrometers.
     *
     *   \return The voxel position in micrometers this view is centered at
     */
     /*======================================================================*/
    void setCenterPositionUm(blitz::TinyVector<double, 3> const &positionUm);

    /*======================================================================*/
    /*!
     *   Get the extents of a pixel of this view in micrometers.
     *
     *   \return The voxel extents in micrometers
     */
     /*======================================================================*/
    void setPixelSizeUm(blitz::TinyVector<double, 3> const &pizelSizeUm);

    /*======================================================================*/
    /*!
     *   Set the magnification setting of this OrthoviewWidget.
     *
     *   \param magnification The currently set magnification (1 = original size)
     */
     /*======================================================================*/
    void setMagnification(double magnification);

  signals:

    /*======================================================================*/
    /*!
     *   This signal is emitted whenever the mouse cursor position changes
     *   within an OrthoviewPlane belonging to this OrthoviewWidget. Connect
     *   this signal to continuously updated status information widgets that
     *   show current cursor position in micrometers, values under the cursor
     *   or the like.
     *
     *   \param positionUm The current cursor position in the OrthoViewWidget
     *     in micrometers relative to the origin of the MultiChannelModel
     */
     /*======================================================================*/
    void mousePositionChanged(blitz::TinyVector<double, 3> const &positionUm);

    /*======================================================================*/
    /*!
     *  This signal is emitted whenever the magnification of the view changes
     *
     *   \param magnification The new magnification
     */
     /*======================================================================*/
    void magnificationChanged(double magnification);

    private slots:

    void synchronizeTopSplitter();
    void synchronizeBottomSplitter();

    void resetMagnification();
    void magnifyIn();
    void magnifyOut();
    void updateMagnification();
    void setMagnificationInComboBox(double magnification);

    void emitMousePositionChanged(
      int orthogonalDimension, QPoint const &mousePositionPx);

  private:

    OrthoviewPlane *p_ovPlanes[3];
    blitz::TinyVector<double, 3> _centerPositionUm;
    blitz::TinyVector<double, 3> _pixelSizeUm;

    blitz::TinyVector<double, 3> _crossPositionUm;

    QSplitter *p_vSplitter, *p_hTopSplitter, *p_hBottomSplitter;

    double _magnification;
    QComboBox *p_magnificationComboBox;

    QAction *p_showScalebarAction;
    QAction *p_enableInterpolationAction;

    std::set<OrthoviewRenderer*> _renderers;

    friend class OrthoviewRenderer;

  };

}

#endif
