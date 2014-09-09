#ifndef ORTHOVIEWUSERINTERACTIONEVENT_HH
#define ORTHOVIEWUSERINTERACTIONEVENT_HH

#include "UserInteractionEvent.hh"

class OrthoViewPlane;

class OrthoViewUserInteractionEvent : public UserInteractionEvent
{

public:

  OrthoViewUserInteractionEvent(OrthoViewPlane* plane,
                                QMouseEvent* mouseEvent = NULL,
                                QWheelEvent* wheelEvent = NULL);
  ~OrthoViewUserInteractionEvent();

  OrthoViewPlane* orthoViewPlane() const;

private:

  OrthoViewPlane* p_plane;

};

#endif
