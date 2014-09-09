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
