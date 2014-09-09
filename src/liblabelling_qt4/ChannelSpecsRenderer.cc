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

#include "ChannelSpecsRenderer.hh"

#include "ChannelSpecs.hh"
#include "ViewWidget.hh"

#include "UserInteractionEvent.hh"

ChannelSpecsRenderer::ChannelSpecsRenderer(
    ChannelSpecs* channel, ViewWidget* view)
        : QObject(), p_channel(channel), p_view(view),
          _cacheUpdatesEnabled(true)
{
  // std::cerr << __FILE__ << ":" << __LINE__ << ": ChannelSpecsRenderer("
  //           << channel << "," << view << ")" << std::endl;
}

ChannelSpecsRenderer::~ChannelSpecsRenderer()
{}

ChannelSpecs* ChannelSpecsRenderer::channel() const
{
  return p_channel;
}

ViewWidget* ChannelSpecsRenderer::view() const
{
  return p_view;
}

bool ChannelSpecsRenderer::cacheUpdatesEnabled() const
{
  return _cacheUpdatesEnabled;
}

bool ChannelSpecsRenderer::event(QEvent *e)
{
  if (e->type() == UserInteractionEvent::Type)
  {
    userInteractionEvent(reinterpret_cast<UserInteractionEvent*>(e));
    if (e->isAccepted()) return true;
  }
  
  return QObject::event(e);
}

void ChannelSpecsRenderer::setCacheUpdatesEnabled(bool enable)
{
  _cacheUpdatesEnabled = enable;
}

void ChannelSpecsRenderer::updateCache() const
{
  if (!_cacheUpdatesEnabled) return;
  p_view->update();
}

void ChannelSpecsRenderer::userInteractionEvent(UserInteractionEvent *e)
{
  e->ignore();
}

