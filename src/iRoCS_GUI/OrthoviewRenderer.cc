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

#include "OrthoviewRenderer.hh"
#include "OrthoviewWidget.hh"
#include "ChannelOrthoviewRenderer.hh"
 // #include "MarkerOrthoviewRenderer.hh"
#include <libMultiChannelModel/Observable.hh>
#include <libMultiChannelModel/Channel.hh>

#include <stdexcept>

namespace iRoCS {

  OrthoviewRenderer::OrthoviewRenderer(OrthoviewWidget &orthoview)
    : QObject(), _orthoview(orthoview) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<OrthoviewRenderer@" << this << "::OrthoviewRenderer "
      << "orthoview=\"" << &_orthoview << " _orthoview=\""
      << &_orthoview << "\"" << "\">" << std::endl;
#endif
    _orthoview._renderers.insert(this);
    _orthoview.update();
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</OrthoviewRenderer@" << this << "::OrthoviewRenderer>"
      << std::endl;
#endif
  }

  OrthoviewRenderer::~OrthoviewRenderer() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<OrthoviewRenderer@" << this << "::~OrthoviewRenderer "
      << "_orthoview=\"" << &_orthoview << "\">" << std::endl;
#endif
    _orthoview._renderers.erase(this);
    _orthoview.update();
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</OrthoviewRenderer@" << this << "::~OrthoviewRenderer>"
      << std::endl;
#endif
  }

  OrthoviewWidget const &OrthoviewRenderer::orthoview() const {
    return _orthoview;
  }

  OrthoviewWidget &OrthoviewRenderer::orthoview() {
    return _orthoview;
  }

  OrthoviewRenderer *OrthoviewRenderer::instance(
    OrthoviewWidget &view, Observable &observable) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<OrthoviewRenderer::instance "
      << "view=\"" << &view << "\" observable=\"" << &observable
      << "\">" << std::endl;
#endif
    try {
      ChannelOrthoviewRenderer *renderer = ChannelOrthoviewRenderer::instance(
        view, dynamic_cast<Channel&>(observable));
#ifdef DEBUG_VERBOSE_XML
      std::cerr << "return " << renderer << std::endl;
      std::cerr << "</OrthoviewRenderer::instance>" << std::endl;
#endif
      return renderer;
    }
    catch (std::bad_cast &) {
    }

    // try
    // {
    //   return MarkerOrthoviewRenderer::instance(
    //       view, dynamic_cast<Marker&>(observable));
    // }
    // catch (std::bad_cast &)
    // {}

    std::stringstream msg;
    msg << "Could not create an OrthoviewRenderer for the given observable "
      << "with address " << &observable << ". The given Observable cannot "
      << "be rendered into an Orthoview. Is the implementation missing?";
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "throw std::runtime_error msg=\"" << msg.str() << "\""
      << std::endl;
    std::cerr << "</OrthoviewRenderer::instance>" << std::endl;
#endif
    throw std::runtime_error(msg.str().c_str());
  }

}
