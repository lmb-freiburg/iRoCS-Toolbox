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

#ifndef IROCS_ORTHOVIEWRENDERER_HH
#define IROCS_ORTHOVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QObject>

class QPainter;
class QRegion;

namespace iRoCS {

  class Observable;
  class OrthoviewWidget;

  class OrthoviewRenderer : public QObject {

  protected:

    OrthoviewRenderer(OrthoviewWidget &orthoview);

  public:

    virtual ~OrthoviewRenderer();

    OrthoviewWidget const &orthoview() const;
    OrthoviewWidget &orthoview();

    virtual Observable const &observable() const = 0;
    virtual Observable &observable() = 0;

    /*======================================================================*/
    /*!
     *   Check whether this OrthoviewRenderer renders voxel data.
     *   Return true if this renderer implements the renderImageData() member.
     *
     *   \return \c true if the renderImageData() method is implemented and
     *     has to be called on repaint of the Orthoview
     */
     /*======================================================================*/
    virtual bool rendersImageData() const = 0;

    /*======================================================================*/
    /*!
     *   Check whether this OrthoviewRenderer renders vectorial data.
     *   Return true if this renderer implements the renderVectorData() member.
     *
     *   \return \c true if the renderVectorData() method is implemented and
     *     has to be called on repaint of the Orthoview
     */
     /*======================================================================*/
    virtual bool rendersVectorData() const = 0;

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
    virtual void renderImageData(
      int orthogonalDimension, QRegion const &region) = 0;

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
    virtual void renderVectorData(
      int orthogonalDimension, QPainter &painter) = 0;

    /*======================================================================*/
    /*!
     *   Create a new concrete OrthoviewRenderer for the given
     *   Channel/OrthoviewWidget combination.
     *
     *   \param view The Orthoview to create a renderer for
     *   \param observable The observable to create a renderer for
     *
     *   \return The new OrthoviewRenderer
     *
     *   \exception std::runtime_error is thrown if no OrthoviewRenderer could
     *     be created. This may indicate missing implementation for the
     *     type of observable.
     */
     /*======================================================================*/
    static OrthoviewRenderer *instance(
      OrthoviewWidget &view, Observable &observable);

  protected:

    OrthoviewWidget &_orthoview;

  };

}

#endif
