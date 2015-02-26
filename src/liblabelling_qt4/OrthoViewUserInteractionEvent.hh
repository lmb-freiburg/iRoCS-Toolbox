#ifndef ORTHOVIEWUSERINTERACTIONEVENT_HH
#define ORTHOVIEWUSERINTERACTIONEVENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
