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

