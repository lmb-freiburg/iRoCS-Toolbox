#ifndef USERINTERACTIONEVENT_HH
#define USERINTERACTIONEVENT_HH

#include <QtCore/QEvent>

#include <iostream>

class QMouseEvent;
class QWheelEvent;

class UserInteractionEvent : public QEvent
{

public:

  UserInteractionEvent(QMouseEvent* mouseEvent = NULL,
                       QWheelEvent* wheelEvent = NULL);
  virtual ~UserInteractionEvent();

  QMouseEvent *mouseEvent() const;
  QWheelEvent *wheelEvent() const;

  QEvent::Type type() const;

  static QEvent::Type const Type;

protected:

  QMouseEvent* p_mouseEvent;
  QWheelEvent* p_wheelEvent;

};

std::ostream &operator<<(std::ostream &os, UserInteractionEvent const &e);

#endif
