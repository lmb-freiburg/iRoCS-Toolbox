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

#ifndef MARKERRENDERER_HH
#define MARKERRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtCore/QObject>

#include <blitz/array.h>

#include <cstddef>

class Marker;
class ViewWidget;
class UserInteractionEvent;

class QPainter;
class QXmlStreamWriter;

class MarkerRenderer : public QObject
{

Q_OBJECT

public:
  
  MarkerRenderer(Marker* marker, ViewWidget* view);
  MarkerRenderer(MarkerRenderer const &renderer);
  virtual ~MarkerRenderer();
  
  Marker* marker() const;
  ViewWidget* view() const;

  bool isDragging() const;
  void setDragging(bool drag);

  virtual MarkerRenderer *clone() const = 0;

/*======================================================================*/
/*! 
 *   Check whether cache updates are enabled.
 *
 *   \return true if the cache is updated by changes in Marker or View
 *     false otherwise
 *
 *   \see setCacheUpdatesEnabled(bool)
 */
/*======================================================================*/
  bool cacheUpdatesEnabled() const;

/*======================================================================*/
/*! 
 *   Main rendering routine which is called whenever a paintEvent() on the
 *   Marker occurs. Within this function the actual rendering of the
 *   associated Marker into the associated view is performed. This may
 *   imply to use the passed QPainter but may be also done by other means
 *   as e.g. by directly imposing OpenGL commands. This function must be
 *   re-implemented and should be as fast as possible since it is called at
 *   every widget repaint
 *
 *   \param painter The active QPainter or NULL if no painter is currently
 *     active or the view update is not QPainter-based, like e.g. for OpenGL
 *     rendering
 */
/*======================================================================*/
  virtual void render(QPainter* painter = NULL) const = 0;

  virtual void userInteractionEvent(UserInteractionEvent *event);

public slots:

/*======================================================================*/
/*! 
 *   Enables or disables the updates of the rendering cache. This usually
 *   means that changes in the Marker or the View position don't
 *   lead to an update of the view. This is useful, if you have to
 *   programmatically perform many small operations on the marker of which
 *   not each should trigger a repaint.
 *
 *   \param enable true to enable, false to disable cache updates
 */
/*======================================================================*/
  void setCacheUpdatesEnabled(bool enable);

/*======================================================================*/
/*! 
 *   Update the rendering cache. This function is called by the
 *   associated Marker if its content changes or by the ViewWidget
 *   if the ViewPosition changes such that an cache update might be
 *   necessary. Reimplement this function if you have expensive computations
 *   that should not be done at every repaint. The default implementation only
 *   calls the update() function of the view. You can disable the cache updates
 *   using the setCacheUpdatesEnabled() function. This is important if you
 *   programmatically perform many operations on the Marker, of which
 *   each would need a cache update. After the sequence of operations, you
 *   have to call updateCache() explicitely to apply the changes. After cache
 *   update update() of the view is called to visualize the changes. So if
 *   many markers are involved you should call setUpdatesEnabled(false)
 *   to the view before these changes.
 */
/*======================================================================*/
  virtual void updateCache() const;

protected:
  
  Marker* p_marker;
  ViewWidget* p_view;

  bool _dragging;

private:

  bool _cacheUpdatesEnabled;

};

#endif
