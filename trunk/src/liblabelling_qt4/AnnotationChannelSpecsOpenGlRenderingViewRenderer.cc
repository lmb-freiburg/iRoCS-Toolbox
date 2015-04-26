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

#include "AnnotationChannelSpecsOpenGlRenderingViewRenderer.hh"

#include <QtGui/QPainter>

#include "MarkerRenderer.hh"

#include "MultiChannelModel.hh"
#include "AnnotationChannelSpecs.hh"
#include "OpenGlRenderingViewWidget.hh"

AnnotationChannelSpecsOpenGlRenderingViewRenderer::
AnnotationChannelSpecsOpenGlRenderingViewRenderer(
    AnnotationChannelSpecs* channel, OpenGlRenderingViewWidget* view)
        : ChannelSpecsOpenGlRenderingViewRenderer(channel, view)
{}
  
AnnotationChannelSpecsOpenGlRenderingViewRenderer::
~AnnotationChannelSpecsOpenGlRenderingViewRenderer()
{}

void AnnotationChannelSpecsOpenGlRenderingViewRenderer::render(
    QPainter* painter) const
{
  if (painter == NULL || !painter->isActive()) return;
  if (p_view != NULL && !p_view->isVisible()) return;
  if (p_channel != NULL && !p_channel->visible()) return;

  AnnotationChannelSpecs* channel =
      static_cast<AnnotationChannelSpecs*>(p_channel);
  
  for (std::vector<Marker*>::const_iterator mIt = channel->begin();
       mIt != channel->end(); ++mIt)
  {
    if ((*mIt)->renderer(p_view) != NULL)
    {
      if (static_cast<OpenGlRenderingViewWidget*>(p_view)->positionVisible(
              (*mIt)->positionUm())) (*mIt)->renderer(p_view)->render(painter);
    }
    else 
        std::cerr << "Warning: Marker at " << (*mIt)->positionUm()
                  << " could not be rendered. No associated renderer found"
                  << std::endl;
  }
}

