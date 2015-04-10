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

#include "UserInteractionEvent.hh"

#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>

#include <sstream>

UserInteractionEvent::UserInteractionEvent(
    QMouseEvent* mouseEvent, QWheelEvent* wheelEvent)
        : QEvent(Type), p_mouseEvent(mouseEvent), p_wheelEvent(wheelEvent)
{}

UserInteractionEvent::~UserInteractionEvent()
{}

QMouseEvent* UserInteractionEvent::mouseEvent() const
{
  return p_mouseEvent;
}

QWheelEvent* UserInteractionEvent::wheelEvent() const
{
  return p_wheelEvent;
}

QEvent::Type UserInteractionEvent::type() const
{
  return Type;
}

QEvent::Type const UserInteractionEvent::Type = QEvent::User;

std::ostream &operator<<(std::ostream &os, UserInteractionEvent const &e)
{
  std::stringstream mouseEventStream;
  if (e.mouseEvent() != NULL)
      mouseEventStream << " { type = " << e.mouseEvent()->type()
                       << ", button = " << e.mouseEvent()->button()
                       << ", buttons = " << e.mouseEvent()->buttons() << " }";
  std::stringstream wheelEventStream;
  if (e.wheelEvent() != NULL)
      wheelEventStream << " { delta = " << e.wheelEvent()->delta() << " }";
  os << "UserInteractionEvent (" << &e << ") { QMouseEvent ("
     << e.mouseEvent() << ")" << mouseEventStream.str() << ", WheelEvent ("
     << e.wheelEvent() << ")" << wheelEventStream.str() << " }";
  return os;
}
