#include "OrthoViewUserInteractionEvent.hh"

#include "OrthoViewPlane.hh"

OrthoViewUserInteractionEvent::OrthoViewUserInteractionEvent(
    OrthoViewPlane* plane, QMouseEvent* mouseEvent,
    QWheelEvent* wheelEvent)
        : UserInteractionEvent(mouseEvent, wheelEvent), p_plane(plane)
{}

OrthoViewUserInteractionEvent::~OrthoViewUserInteractionEvent()
{}

OrthoViewPlane* OrthoViewUserInteractionEvent::orthoViewPlane() const
{
  return p_plane;
}

