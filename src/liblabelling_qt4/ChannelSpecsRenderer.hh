/**************************************************************************
**       Title: Abstract Channel Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Channel
**    types. For each Channel type and each View type one such interface
**    class needs to be implemented.
**
**************************************************************************/

#ifndef CHANNELSPECSRENDERER_HH
#define CHANNELSPECSRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtCore/QObject>

#include <blitz/array.h>

class QPainter;
class QXmlStreamWriter;

class UserInteractionEvent;

class ChannelSpecs;
class ViewWidget;

class ChannelSpecsRenderer : public QObject
{

Q_OBJECT

public:
  
  ChannelSpecsRenderer(ChannelSpecs* channel, ViewWidget* view);
  virtual ~ChannelSpecsRenderer();

  ChannelSpecs* channel() const;
  ViewWidget* view() const;

/*======================================================================*/
/*! 
 *   Check whether cache updates are enabled.
 *
 *   \return true if the cache is updated by changes in channel or view
 *     false otherwise
 *
 *   \see setCacheUpdatesEnabled(bool)
 */
/*======================================================================*/
  bool cacheUpdatesEnabled() const;

/*======================================================================*/
/*! 
 *   Main rendering routine which is called whenever a paintEvent() on the
 *   ViewWidget occurs. Within this function the actual rendering of the
 *   associated channel into the associated view is performed. This may
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

/*======================================================================*/
/*! 
 *   Interprets and distributes the given event. This function checks,
 *   whether the event is a UserInteractionEvent and if so passes the
 *   event to the specialized function
 *   userInteractionEvent(UserInteractionEvent*). If you want to enable
 *   mouse interaction with the view associated with this renderer you
 *   should reimplement the specialized function, for everything beyond
 *   mouse interaction reimplement this function.
 *
 *   \param event The event sent to this ChannelSpecsRenderer
 *
 *   \return true if the event was accepted, false otherwise
 */
/*======================================================================*/
  virtual bool event(QEvent *event);

public slots:

/*======================================================================*/
/*! 
 *   Enables or disables the updates of the rendering cache. This usually
 *   means that changes in the ChannelSpecs or the view position don't
 *   lead to an update of the view. This is useful, if you have to
 *   programmatically perform many small operations on the channel of which
 *   not each should trigger a repaint.
 *
 *   \param enable true to enable, false to disable cache updates
 */
/*======================================================================*/
  virtual void setCacheUpdatesEnabled(bool enable);

/*======================================================================*/
/*! 
 *   Update the rendering cache. This function is called by the
 *   associated ChannelSpecs if its content changes or by the ViewWidget
 *   if the ViewPosition changes such that an cache update might be
 *   neccessary. Reimplement this function if you have expensive computations
 *   that should not be done at every repaint. The default implementation only
 *   calls the update() function of the view. You can disable the cacheUpdates
 *   using the setCacheUpdatesEnabled() function. This is important if you
 *   programmatically perform many operations on the ChannelSpecs, of which
 *   each would need a cache update. After the sequence of operations, you
 *   have to call updateCache() explicitely to apply the changes. After cache
 *   update update() of the view is called to visualize the changes. So if
 *   many channels are involved you should call setUpdatesEnabled(false)
 *   to the view before these changes.
 */
/*======================================================================*/
  virtual void updateCache() const;

protected:
  
/*======================================================================*/
/*! 
 *   Reimplement this function if your specialized ChannelSpecsRenderer
 *   should respond to a passed UserInteractionEvent.
 *
 *   \param e The UserInteractionEvent containing the position, type and
 *     other parameters of the event
 */
/*======================================================================*/
  virtual void userInteractionEvent(UserInteractionEvent* e);

  ChannelSpecs* p_channel;
  ViewWidget* p_view;

  bool _cacheUpdatesEnabled;

};

#endif
