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

#include "FloatColormapObserver.hh"
#include "FloatColormap.hh"

namespace iRoCS {

  FloatColormapObserver::FloatColormapObserver(FloatColormap &colormap)
    : _colormap(colormap) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatColormapObserver@" << this
      << "::FloatColormapObserver colormap=\"" << &colormap
      << "\" _colormap=\"" << &_colormap << "\">"
      << std::endl;
    _colormap._observers.insert(this);
    std::cerr << "</FloatColormapObserver@" << this
      << "::FloatColormapObserver>" << std::endl;
#endif
  }

  FloatColormapObserver::~FloatColormapObserver() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatColormapObserver@" << this
      << "::~FloatColormapObserver _colormap=\"" << &_colormap << "\">"
      << std::endl;
    _colormap._observers.erase(this);
    std::cerr << "</FloatColormapObserver@" << this
      << "::~FloatColormapObserver>" << std::endl;
#endif
  }

}

