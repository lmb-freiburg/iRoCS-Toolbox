/**************************************************************************
**       Title: Abstract Channel OrthoView Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the OrthoView and different Channel
**    types. For each Channel type one such interface class needs to be
**    implemented.
**
**************************************************************************/

#include "ChannelSpecsOrthoViewRenderer.hh"

#include "OrthoViewWidget.hh"
#include "OrthoViewPlane.hh"
#include "MarkerOrthoViewRenderer.hh"
#include "AnnotationChannelSpecs.hh"

ChannelSpecsOrthoViewRenderer::ChannelSpecsOrthoViewRenderer(
    ChannelSpecs* channel, OrthoViewWidget* view)
        : ChannelSpecsRenderer(channel, view),
          _orthogonalDimension(0)
{
  if (view != NULL)
  {
    for (int d = 0; d < 3; ++d) view->orthoViewPlane(d)->adjustSize();
    connect(
        this, SIGNAL(cacheUpdated(ChannelSpecsOrthoViewRenderer const*,int)),
        view, SLOT(updatePlaneData(ChannelSpecsOrthoViewRenderer const*,int)));
  }
}

ChannelSpecsOrthoViewRenderer::~ChannelSpecsOrthoViewRenderer()
{}

void ChannelSpecsOrthoViewRenderer::setOrthogonalDimension(
    int orthogonalDimension)
{
  _orthogonalDimension = orthogonalDimension;
  if (p_channel->channelType() == ChannelSpecs::Annotation)
  {
    AnnotationChannelSpecs *channel =
        static_cast<AnnotationChannelSpecs*>(p_channel);
    for (std::vector<Marker*>::iterator it = channel->begin();
         it != channel->end(); ++it)
        static_cast<MarkerOrthoViewRenderer*>(
            (*it)->renderer(p_view))->setOrthogonalDimension(
                orthogonalDimension);
  }
}

void ChannelSpecsOrthoViewRenderer::updateCache() const
{
  if (!cacheUpdatesEnabled()) return;
  p_view->setUpdatesEnabled(false);
  for (int d = 0; d < 3; ++d) updateCache(d);
  p_view->setUpdatesEnabled(true);
  p_view->update();
}

void ChannelSpecsOrthoViewRenderer::updateCache(int) const
{
  if (!cacheUpdatesEnabled()) return;
  p_view->update();
}
