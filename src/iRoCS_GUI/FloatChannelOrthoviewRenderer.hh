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

#ifndef IROCS_FLOATCHANNELORTHOVIEWRENDERER_HH
#define IROCS_FLOATCHANNELORTHOVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelOrthoviewRenderer.hh"
#include <libMultiChannelModel/FloatColormapObserver.hh>

#include <vector>

namespace iRoCS {

  class FloatChannel;

  class FloatChannelOrthoviewRenderer
    : public ChannelOrthoviewRenderer, public FloatColormapObserver {

    FloatChannelOrthoviewRenderer(OrthoviewWidget &view, FloatChannel &channel);

  public:

    ~FloatChannelOrthoviewRenderer();

    /*======================================================================*/
    /*!
     *   Get the Channel this ChannelObserver observes.
     *
     *   \return A read only reference to the observed Channel
     */
     /*======================================================================*/
    Observable const &observable() const;

    /*======================================================================*/
    /*!
     *   Get the Channel this ChannelObserver observes.
     *
     *   \return A random access reference to the observed Channel
     */
     /*======================================================================*/
    Observable &observable();

    /*======================================================================*/
    /*!
     *   Check whether this ChannelOrthoviewRenderer renders voxel data.
     *
     *   \return \c true
     */
     /*======================================================================*/
    bool rendersImageData() const;

    /*======================================================================*/
    /*!
     *   Check whether this ChannelOrthoviewRenderer renders vectorial data.
     *
     *   \return \c false
     */
     /*======================================================================*/
    bool rendersVectorData() const;

    /*======================================================================*/
    /*!
     *   This method is called when the name of the observed Channel changes.
     *   For an orthoview it is a noop.
     */
     /*======================================================================*/
    void updateName();

    /*======================================================================*/
    /*!
     *   This method is called when the filename of the observed Channel changes.
     *   For an orthoview it is a noop.
     */
     /*======================================================================*/
    void updateFilename();

    /*======================================================================*/
    /*!
     *   This method is called when the transparency of the observed Channel
     *   changes. It requests a full update of the parent orthoview.
     *
     *   If updates for this observer are disabled, calling this method is a
     *   noop.
     */
     /*======================================================================*/
    void updateAlpha();

    /*======================================================================*/
    /*!
     *   This method is called when the visibility of the observed Channel
     *   changes. It requests a full update of the parent orthoview.
     *
     *   If updates for this observer are disabled, calling this method is a
     *   noop.
     */
     /*======================================================================*/
    void updateVisibilityFlag();

    /*======================================================================*/
    /*!
     *   This method is called when the modified flag of the observed Channel
     *   changes. For an orthoview it is a noop.
     */
     /*======================================================================*/
    void updateModifiedFlag();

    /*======================================================================*/
    /*!
     *   This method is called when the voxel extents in micrometers of the
     *   observed Channel change. This method triggers OrthoViewWidget::update().
     */
     /*======================================================================*/
    void updateElementSizeUm();

    /*======================================================================*/
    /*!
     *   This method is called when the homogeneous backwarp transformation of
     *   the observed Channel changes. It requests a full update of the parent
     *   orthoview.
     */
     /*======================================================================*/
    void updateTransformation();

    /*======================================================================*/
    /*!
     *   This method is called when the color index range of the color map
     *   changes. Only colors between the smallest and largest color index
     *   are properly handled by this color map. Values exceeding the range
     *   are clipped.
     */
     /*======================================================================*/
    void updateIndexRange();

    /*======================================================================*/
    /*!
     *   This method is called when the color for the smallest index changed.
     */
     /*======================================================================*/
    void updateStartColor();

    /*======================================================================*/
    /*!
     *   This method is called when the color for the greatest index changed.
     */
     /*======================================================================*/
    void updateEndColor();

    /*======================================================================*/
    /*!
     *   This method is called when an intermediate interpolation color at
     *   the given position is changed in this color map. If there is no
     *   intermediate color at the given position it will be created.
     *
     *   \param position The position of the intermediate interpolation color
     *     in this color map in (0, 1) (0 = start, 1 = end)
     */
     /*======================================================================*/
    void updateIntermediateColor(double position);

    /*======================================================================*/
    /*!
     *   This method is called when an intermediate interpolation color is moved
     *   from the given old position to the given new position.
     *
     *   \param oldPosition The old position of the intermediate interpolation
     *     color in this color map in (0, 1) (0 = start, 1 = end)
     *   \param newPosition The new position of the interpolation color
     */
     /*======================================================================*/
    void updateMoveIntermediateColor(
      double oldPosition, double newPosition);

    /*======================================================================*/
    /*!
     *   This method is called when the intermediate interpolation color at
     *   the given position is removed from this color map.
     *
     *   \param position The position of the intermedaite interpolation color
     *     in (0, 1) (0 = start, 1 = end)
     */
     /*======================================================================*/
    void updateRemoveIntermediateColor(double position);

    /*======================================================================*/
    /*!
     *   This method is called when all intermediate interpolation colors
     *   are have been removed from this color map.
     */
     /*======================================================================*/
    void updateClearIntermediateColors();

    /*======================================================================*/
    /*!
     *   This method is called when the monochrome color of the Colormap changes.
     *   The monochrome color is only used if the color map is monochrome,
     *   otherwise the colors are defined by the start, end and intermediate color
     *   mapping.
     */
     /*======================================================================*/
    void updateMonochromeColor();

    /*======================================================================*/
    /*!
     *   This method is called whenever the range indicator is enabled or
     *   disabled.
     */
     /*======================================================================*/
    void updateRangeIndicator();

    /*======================================================================*/
    /*!
     *   This method is called whenever the gamma correction value for the
     *   colormap changes.
     */
     /*======================================================================*/
    void updateGamma();

    /*======================================================================*/
    /*!
     *   This method forces an update of the full Observer. It is mainly
     *   intended for initialization. If the observer is a QWidget it should
     *   just call the update() method of the QWidget.
     */
     /*======================================================================*/
    void updateColormap();

    /*======================================================================*/
    /*!
     *   This method is called when the data Array of the observed FloatChannel
     *   changes. This method triggers OrthoViewWidget::update().
     */
     /*======================================================================*/
    void updateData();

    /*======================================================================*/
    /*!
     *   Render the given QRegion of pixel image data of the specified
     *   orthogonal plane of the Channel into the image data Array of the
     *   OrthoviewPlane. This function is called by the
     *   paintEvent() method of the OrthoviewPlane. Plane position and
     *   transformation are read from the parent OrthoviewWidget.
     *
     *   \param orthogonalDimension The orthogonal dimension of the
     *     plane to render
     *   \param region The region to render
     */
     /*======================================================================*/
    void renderImageData(int orthogonalDimension, QRegion const &region);

    /*======================================================================*/
    /*!
     *   Render the vectorial data of the specified orthogonal plane of the
     *   Channel into the OrthoviewPlane using the given QPainter. This function
     *   is called by the paintEvent() method of the OrthoviewPlane. Plane
     *   position and transformation are read from the parent OrthoviewWidget.
     *
     *   \param orthogonalDimension The orthogonal dimension of the
     *     plane to render
     *   \param painter The QPainter to use for rendering. Only render parts
     *     within the clipping region defined by the painter.
     */
     /*======================================================================*/
    void renderVectorData(
      int orthogonalDimension, QPainter &painter);

    /*======================================================================*/
    /*!
     *   Create a new FloatChannelOrthoviewRenderer for the given
     *   FloatChannel/OrthoviewWidget combination if it does not exist yet. If
     *   it already exists get the pointer to the existing instance.
     *
     *   \param view The OrthoviewWidget to create a renderer for
     *   \param channel The FloatChannel to create a renderer for
     *
     *   \return The new FloatChannelOrthoviewRenderer
     */
     /*======================================================================*/
    static FloatChannelOrthoviewRenderer *instance(
      OrthoviewWidget &view, FloatChannel &channel);

  };

}

#endif
