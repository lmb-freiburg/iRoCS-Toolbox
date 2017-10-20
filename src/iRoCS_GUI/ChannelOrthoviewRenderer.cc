/**************************************************************************
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

#include "ChannelOrthoviewRenderer.hh"
#include "FloatChannelOrthoviewRenderer.hh"
 // #include "IndexedChanelOrthoviewRenderer.hh"
 // #include "RGBChanelOrthoviewRenderer.hh"
 // #include "AnnotationChanelOrthoviewRenderer.hh"

#include <libMultiChannelModel/Channel.hh>
#include <libMultiChannelModel/FloatChannel.hh>
#include "OrthoviewWidget.hh"

#include <stdexcept>

namespace iRoCS {

  ChannelOrthoviewRenderer::ChannelOrthoviewRenderer(
    OrthoviewWidget &view, Channel &channel)
    : OrthoviewRenderer(view), ChannelObserver(channel) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<ChannelOrthoviewRenderer@" << this
      << "::ChannelOrthoviewRenderer view=\"" << &view
      << "\" channel=\"" << &channel << "\" />" << std::endl;
#endif
  }

  ChannelOrthoviewRenderer::~ChannelOrthoviewRenderer() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<ChannelOrthoviewRenderer@" << this
      << "::~ChannelOrthoviewRenderer />" << std::endl;
#endif
  }

  ChannelOrthoviewRenderer *ChannelOrthoviewRenderer::instance(
    OrthoviewWidget &view, Channel &channel) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<ChannelOrthoviewRenderer::instance view=\"" << &view
      << "\" channel=\"" << typeid(channel).name() << "@" << &channel
      << "(" << channel.name() << ")\">" << std::endl;
#endif
    try {
      FloatChannelOrthoviewRenderer *renderer =
        FloatChannelOrthoviewRenderer::instance(
          view, dynamic_cast<FloatChannel&>(channel));
#ifdef DEBUG_VERBOSE_XML
      std::cerr << "return " << renderer << std::endl;
      std::cerr << "</ChannelOrthoviewRenderer::instance>" << std::endl;
#endif
      return renderer;
    }
    catch (std::bad_cast &) {
    }

    // try
    // {
    //   return IndexedChannelOrthoviewRenderer::instance(
    //       view, dynamic_cast<IndexedChannel&>(channel));
    // }
    // catch (std::bad_cast &)
    // {}

    // try
    // {
    //   return RGBChannelOrthoviewRenderer::instance(
    //       view, dynamic_cast<RGBChannel&>(channel));
    // }
    // catch (std::bad_cast &)
    // {}

    // try
    // {
    //   return AnnotationChannelOrthoviewRenderer::instance(
    //       view, dynamic_cast<AnnotationChannel&>(channel));
    // }
    // catch (std::bad_cast &)
    // {}

    std::stringstream msg;
    msg << "Could not create a ChannelOrthoviewRenderer for the given channel "
      << "with address " << &channel << ". The given Channel type cannot "
      << "be rendered into an Orthoview. Is the implementation missing?";
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "throw std::runtime_error \"" << msg.str() << "\""
      << std::endl;
    std::cerr << "</ChannelOrthoviewRenderer::instance>" << std::endl;
#endif
    throw std::runtime_error(msg.str().c_str());
  }

}
