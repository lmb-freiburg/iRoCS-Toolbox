/**************************************************************************
**       Title: Ortho-View Annotation-Channel Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Channel
**    types. For each Channel type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for AnnotationChannelSpecs <=> OrthoView
**
**************************************************************************/

#include "AnnotationChannelSpecsOrthoViewRenderer.hh"

#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtCore/QThread>
#include <QtXml/QXmlStreamWriter>

#include "MarkerOrthoViewRenderer.hh"
#include "OrthoViewUserInteractionEvent.hh"
#include "NucleusMarker.hh"
#include "MultiChannelModel.hh"
#include "OrthoViewWidget.hh"
#include "AnnotationChannelSpecs.hh"
#include "OrthoViewPlane.hh"

AnnotationChannelSpecsOrthoViewRenderer::
AnnotationChannelSpecsOrthoViewRenderer(
    AnnotationChannelSpecs* channel, OrthoViewWidget* view)
        : ChannelSpecsOrthoViewRenderer(channel, view), p_marker(NULL)
{}
  
AnnotationChannelSpecsOrthoViewRenderer::
~AnnotationChannelSpecsOrthoViewRenderer()
{}

void AnnotationChannelSpecsOrthoViewRenderer::userInteractionEvent(
    UserInteractionEvent *event)
{
  // std::cerr << "Channel " << p_channel->name() << " (" << p_channel
  //           << ") received UserInteractionEvent " << event << std::endl;

  if (!p_channel->visible())
  {
    if (p_marker != NULL)
    {
      p_marker->renderer(p_view)->setDragging(false);
      p_marker = NULL;
    }
    event->ignore();
    return;
  }

  // If no marker is dragged ignore mouse move events
  if (p_marker == NULL && event->mouseEvent() != NULL &&
      event->mouseEvent()->type() == QEvent::MouseMove)
  {
    event->ignore();
    return;
  }
  
  AnnotationChannelSpecs* channel =
      reinterpret_cast<AnnotationChannelSpecs*>(p_channel);
  OrthoViewUserInteractionEvent* e =
      reinterpret_cast<OrthoViewUserInteractionEvent*>(event);
  
  // If mouse is moved while the left mouse button is pressed and a marker
  // is available for dragging, drag it and tell it, that it is currently
  // dragged
  if (e->mouseEvent() != NULL &&
      e->mouseEvent()->type() == QEvent::MouseMove &&
      e->mouseEvent()->buttons().testFlag(Qt::LeftButton) &&
      p_marker != NULL && channel->selectedOperation() !=
      AnnotationChannelSpecs::LabelMarker)
  {
    blitz::TinyVector<double,3> posUm(
        e->orthoViewPlane()->mousePositionUm(
            e->mouseEvent()->x(), e->mouseEvent()->y()));
    posUm(e->orthoViewPlane()->orthogonalDimension()) =
        p_marker->positionUm()(e->orthoViewPlane()->orthogonalDimension());
    p_marker->setPositionUm(posUm);
    channel->selectMarker(p_marker);
    p_marker->renderer(p_view)->setDragging(true);
    e->accept();
    return;
  }
  
  // If left mouse button is released while a marker is dragged, release it
  if (p_marker != NULL && p_marker->renderer(p_view)->isDragging() &&
      e->mouseEvent() != NULL &&
      e->mouseEvent()->type() == QEvent::MouseButtonRelease &&
      e->mouseEvent()->button() == Qt::LeftButton)
  {
    p_marker->renderer(p_view)->setDragging(false);
    p_marker = NULL;
    e->accept();
    return;
  }
  
  // If left mouse button is released and we are in Add marker mode,
  // add a marker
  if (e->mouseEvent() != NULL &&
      e->mouseEvent()->button() == Qt::LeftButton &&
      e->mouseEvent()->type() == QEvent::MouseButtonRelease &&
      channel->selectedOperation() == AnnotationChannelSpecs::AddMarker)
  {
    Marker *marker = channel->addMarker(
        e->orthoViewPlane()->mousePositionUm(
            e->mouseEvent()->x(), e->mouseEvent()->y()));
    if (marker != NULL)
    {
      marker->setManual(true);
      channel->selectMarker(marker);
    }
    p_marker = NULL;
    e->accept();
    return;
  }
  
  // Get current mouse position in volume
  blitz::TinyVector<float,3> posUm;
  if (e->mouseEvent() != NULL) 
  {
    posUm = e->orthoViewPlane()->mousePositionUm(
        e->mouseEvent()->x(), e->mouseEvent()->y());
  }
  else if (e->wheelEvent() != NULL)
  {
    posUm = e->orthoViewPlane()->mousePositionUm(
        e->wheelEvent()->x(), e->wheelEvent()->y());
  }
  else
  {
    e->ignore();
    return;
  }
  
  // Find the marker under the cursor
  Marker *marker = channel->closestMarker(posUm, true);

  // If there is no marker under the cursor ignore the event
  if (marker == NULL)
  {
    e->ignore();
    return;
  }

  // If right mouse button is released and we are in Add marker mode,
  // or left mouse button was released and we are in Delete marker mode,
  // delete the marker under the cursor
  if (e->mouseEvent() != NULL &&
      e->mouseEvent()->type() == QEvent::MouseButtonRelease &&
      ((e->mouseEvent()->button() == Qt::LeftButton &&
        channel->selectedOperation() == AnnotationChannelSpecs::DeleteMarker) ||
       (e->mouseEvent()->button() == Qt::RightButton &&
        channel->selectedOperation() == AnnotationChannelSpecs::AddMarker)))
  {
    channel->removeMarker(marker);
    p_marker = NULL;
    e->accept();
    return;
  }

  // If left mouse button is released, select marker
  if (e->mouseEvent() != NULL &&
      e->mouseEvent()->type() == QEvent::MouseButtonRelease &&
      e->mouseEvent()->button() == Qt::LeftButton)
  {
    p_marker = marker;
    channel->selectMarker(marker);
  }

  // If the left mouse button is pressed, remember marker for potential drag
  if (e->mouseEvent() != NULL &&
      e->mouseEvent()->type() == QEvent::MouseButtonPress &&
      e->mouseEvent()->button() == Qt::LeftButton &&
      channel->selectedOperation() != AnnotationChannelSpecs::LabelMarker)
  {
    p_marker = marker;
    e->accept();
    return;
  }
  
  // Send all other events directly to the marker
  marker->renderer(p_view)->userInteractionEvent(e);
  if (e->isAccepted())
  {
    p_marker = NULL;
    return;
  }
}

void AnnotationChannelSpecsOrthoViewRenderer::render(QPainter* painter) const
{
  if (painter == NULL || !painter->isActive()) return;
  if (p_channel == NULL) return;

  std::vector<Marker*> const &markers =
      static_cast<AnnotationChannelSpecs*>(p_channel)->markers();

  for (size_t i = 0; i < markers.size(); ++i)
  {
    MarkerOrthoViewRenderer *renderer =
        static_cast<MarkerOrthoViewRenderer*>(markers[i]->renderer(p_view));
    if (renderer != NULL)
    {
      renderer->setOrthogonalDimension(_orthogonalDimension);
      renderer->render(painter);
    }
    else 
    {
      std::cerr << "Warning: Marker at " << markers[i]->positionUm()
                << " could not be rendered. No associated renderer found"
                << std::endl;
    }
  }
}

void AnnotationChannelSpecsOrthoViewRenderer::render(
    QXmlStreamWriter &svgStream,
    blitz::TinyVector<double,3> const &shapePx,
    blitz::TinyVector<double,3> const &lowerBoundUm,
    blitz::TinyVector<double,3> const &upperBoundUm,
    std::string const &font, double fontSizePt,
    iRoCS::ProgressReporter *pr) const
{
  AnnotationChannelSpecs* channel =
      static_cast<AnnotationChannelSpecs*>(p_channel);
  
  std::string viewId = static_cast<OrthoViewWidget*>(p_view)->orthoViewPlane(
      _orthogonalDimension)->viewId();

  int pMin = (pr != NULL) ? pr->taskProgressMin() : 0;
  int pScale = (pr != NULL) ? pr->taskProgressMax() - pMin : 100;
  if (pr != NULL && !pr->updateProgress(pMin)) return;

  svgStream.writeStartElement("g");
  svgStream.writeAttribute("id", (viewId + ": " + channel->name()).c_str());

  int m = 0;
  for (std::vector<Marker*>::const_iterator mIt = channel->begin();
       mIt != channel->end(); ++mIt, ++m)
  {
    if (pr != NULL)
    {
      if (pr->isAborted()) continue;
      pr->setTaskProgressMin(
          static_cast<int>(
              pMin + static_cast<double>(m) /
              static_cast<double>(channel->markers().size()) * pScale));
      pr->setTaskProgressMax(
          static_cast<int>(
              pMin + static_cast<double>(m + 1) /
              static_cast<double>(channel->markers().size()) * pScale));
    }
    if ((*mIt)->renderer(p_view) != NULL &&
        blitz::all((*mIt)->positionUm() >= lowerBoundUm &&
                   (*mIt)->positionUm() <= upperBoundUm))
    {
      static_cast<MarkerOrthoViewRenderer*>((*mIt)->renderer(p_view))->render(
          svgStream, shapePx, lowerBoundUm, upperBoundUm, font, fontSizePt, pr);
    }
  }

  svgStream.writeEndElement(); // g id=<channelname>
  if (pr != NULL) pr->updateProgress(pMin + pScale);
}

void AnnotationChannelSpecsOrthoViewRenderer::setCacheUpdatesEnabled(
    bool enable)
{
  if (_cacheUpdatesEnabled == enable) return;
  
  _cacheUpdatesEnabled = enable;

  std::vector<Marker*> const &markers =
      static_cast<AnnotationChannelSpecs*>(p_channel)->markers();
  
  for (size_t i = 0; i < markers.size(); ++i)
      markers[i]->setUpdatesEnabled(enable);
}

void AnnotationChannelSpecsOrthoViewRenderer::updateCache(int direction) const
{
  if (p_channel == NULL || p_view == NULL || !_cacheUpdatesEnabled) return;

  bool oldUpdatesEnabledState = p_view->updatesEnabled();
  p_view->setUpdatesEnabled(false);

  std::vector<Marker*> const &markers =
      static_cast<AnnotationChannelSpecs*>(p_channel)->markers();
  
  for (size_t i = 0; i < markers.size(); ++i)
  {
      MarkerOrthoViewRenderer* renderer =
          static_cast<MarkerOrthoViewRenderer*>(markers[i]->renderer(p_view));
      if (renderer != NULL) renderer->updateCache(direction);
      else std::cerr << "Warning: Marker at " << markers[i]->positionUm()
                     << " could not be rendered. No associated renderer found"
                     << std::endl;
  }
  p_view->setUpdatesEnabled(oldUpdatesEnabledState);
  p_view->update();
}

