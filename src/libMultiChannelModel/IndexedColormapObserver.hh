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

#ifndef IROCS_INDEXEDCOLORMAPOBSERVER_HH
#define IROCS_INDEXEDCOLORMAPOBSERVER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <cstddef>

namespace iRoCS {

  class IndexedColormap;

  /*======================================================================*/
  /*!
   *  \class IndexedColormapObserver IndexedColormapObserver.hh <liblabelling_qt4/IndexedColormapObserver.hh>
   *  \brief The IndexedColormapObserver class specifies the interface
   *    that must be implemented by classes that shall be notified at indexed
   *    color map changes.
   *
   *  This class interfaces the pure C++ IndexedColormap class to QT
   *  view- and control widgets. The class is pure virtual, therefore it
   *  must be sub-classed and corresponding functionality implemented.
   */
   /*======================================================================*/
  class IndexedColormapObserver {

  public:

    /*======================================================================*/
    /*!
     *   Create a new IndexedColormapObserver object.
     */
     /*======================================================================*/
    IndexedColormapObserver(IndexedColormap &colormap);

    /*======================================================================*/
    /*!
     *   Destroy this IndexedColormapObserver.
     *
     *   If the observer is registered to a IndexedColormap at time of
     *   destruction, it will be unregistered first.
     */
     /*======================================================================*/
    virtual ~IndexedColormapObserver();

    /*======================================================================*/
    /*!
     *   This method is called when the color index range of the color map
     *   changes. Only colors between the smallest and largest color index
     *   are properly handled by this color map. Values exceeding the range
     *   are clipped.
     */
     /*======================================================================*/
    virtual void updateIndexRange() = 0;

    /*======================================================================*/
    /*!
     *   This method is called when the color for the smallest index changed.
     */
     /*======================================================================*/
    virtual void updateStartColor() = 0;

    /*======================================================================*/
    /*!
     *   This method is called when the color for the greatest index changed.
     */
     /*======================================================================*/
    virtual void updateEndColor() = 0;

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
    virtual void updateIntermediateColor(double position) = 0;

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
    virtual void updateMoveIntermediateColor(
      double oldPosition, double newPosition) = 0;

    /*======================================================================*/
    /*!
     *   This method is called when the intermediate interpolation color at
     *   the given position is removed from this color map.
     *
     *   \param position The position of the intermedaite interpolation color
     *     in (0, 1) (0 = start, 1 = end)
     */
     /*======================================================================*/
    virtual void updateRemoveIntermediateColor(double position) = 0;

    /*======================================================================*/
    /*!
     *   This method is called when all intermediate interpolation colors
     *   are have been removed from this color map.
     */
     /*======================================================================*/
    virtual void updateClearIntermediateColors() = 0;

    /*======================================================================*/
    /*!
     *   This method is called whenever an entry is set in the override color
     *   map. If currently no entry at the given index exists it will be created.
     *
     *   \param index The override color index to update
     */
     /*======================================================================*/
    virtual void updateOverrideColor(int index) = 0;

    /*======================================================================*/
    /*!
     *   This method is called whenever an entry in the override color
     *   map is moved.
     *
     *   \param oldIndex The old override color index
     *   \param newIndex The new override color index
     */
     /*======================================================================*/
    virtual void updateMoveOverrideColor(int oldIndex, int newIndex) = 0;

    /*======================================================================*/
    /*!
     *   This method is called whenever an entry is removed from the override
     *   color map.
     *
     *   \param index The override color index to remove
     */
     /*======================================================================*/
    virtual void updateRemoveOverrideColor(int index) = 0;

    /*======================================================================*/
    /*!
     *   This method is called whenever the override color map is cleared.
     */
     /*======================================================================*/
    virtual void updateClearOverrideColors() = 0;

    /*======================================================================*/
    /*!
     *   This method is called when the monochrome color of the Colormap changes.
     *   The monochrome color is only used if the color map is monochrome,
     *   otherwise the colors are defined by the start, end and intermediate color
     *   mapping.
     */
     /*======================================================================*/
    virtual void updateMonochromeColor() = 0;

    /*======================================================================*/
    /*!
     *   This method is called whenever the range indicator is enabled or
     *   disabled.
     */
     /*======================================================================*/
    virtual void updateRangeIndicator() = 0;

    /*======================================================================*/
    /*!
     *   This method is called whenever the gamma correction value for the
     *   colormap changes.
     */
     /*======================================================================*/
    virtual void updateGamma() = 0;

    /*======================================================================*/
    /*!
     *   This method forces an update of the full Observer. It is mainly
     *   intended for initialization. If the observer is a QWidget it should
     *   just call the update() method of the QWidget.
     */
     /*======================================================================*/
    virtual void updateColormap() = 0;

  protected:

    IndexedColormap &_colormap;

  };

}

#endif
