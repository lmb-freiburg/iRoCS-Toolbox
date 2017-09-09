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

#ifndef IROCS_FLOATCOLORMAP_HH
#define IROCS_FLOATCOLORMAP_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "Observable.hh"

#include <blitz/array.h>
#include <set>
#include <map>
#include <vector>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

namespace iRoCS {

  class FloatColormapObserver;

  class FloatColormap : virtual public Observable {

  public:

    enum Type { Monochrome = 0x0001, RGB = 0x0002 };

    typedef blitz::TinyVector<unsigned char, 3> ColorT;

#ifdef DEBUG_VERBOSE_XML
    std::string printState() const;
#endif

    FloatColormap(float start = 0.0f, float end = 1.0f);
    FloatColormap(FloatColormap const &colorMap);
    ~FloatColormap();

    FloatColormap &operator=(FloatColormap const &colorMap);

    void reset();

    // Index ranges for default types
    bool isUnsigned8BitRange() const;
    void setUnsigned8BitRange();
    bool isSigned8BitRange() const;
    void setSigned8BitRange();
    bool isUnsigned12BitRange() const;
    void setUnsigned12BitRange();
    bool isUnsigned16BitRange() const;
    void setUnsigned16BitRange();
    bool isSigned16BitRange() const;
    void setSigned16BitRange();

    // Custom index ranges
    void setIndexRange(float startIndex, float endIndex);

    /*======================================================================*/
    /*!
     *   Sets the start index of this FloatColormap. The start index is the
     *   intensity value that will be mapped to the first color of this
     *   FloatColormap. If the given start index is greater than the current
     *   end index it will not be set. In any case the finally set start index
     *   is returned.
     *
     *   \param index The wanted new start index
     *
     *   \return The actually set start index
     */
     /*======================================================================*/
    float setStartIndex(float index);
    float startIndex() const;

    /*======================================================================*/
    /*!
     *   Sets the end index of this FloatColormap. The end index is the intensity
     *   value that will be mapped to the last color of this FloatColormap.
     *   If the given end index is less than the current start index it will
     *   not be set. In any case the finally set end index is returned.
     *
     *   \param index The wanted new end index
     *
     *   \return The actually set end index
     */
     /*======================================================================*/
    float setEndIndex(float index);
    float endIndex() const;

    // Custom colors at the ends of the color transition
    void setStartColor(ColorT const &color);
    ColorT startColor() const;
    void setEndColor(ColorT const &color);
    ColorT endColor() const;

    // Intermediate colors for smooth interpolation
    /*======================================================================*/
    /*!
     *   Set the intermediate color at the given position in (0, 1) relative to
     *   the colormap range, i.e. 0 = startIndex, 1 = endIndex. If a position
     *   outside the (0, 1) range is requested, this method is a noop. If there
     *   is no intermediate color at the given position, it will be added to
     *   the intermediate color map.
     *
     *   \param position The position relative to the colormap range
     *   \param color    The intermediate color to set
     */
     /*======================================================================*/
    void setIntermediateColor(double position, ColorT const &color);

    /*======================================================================*/
    /*!
     *   Move the intermediate color at the given old position to the given
     *   new position. If no intermediate color at the specified old position
     *   exists, or the new position is already occupied, this method is a noop.
     *   If the position exceeds the (0, 1) range, it will be clipped to the
     *   [0.001 0.999] interval. In any case the final position of the color is
     *   returned, i.e. the (clipped) new position if the operation succeeded or
     *   the old position otherwise.
     *
     *   \param oldPosition The current position of the color in the intermediate
     *     color map to move
     *   \param newPosition The new position the color will be moved to if
     *     possible
     *
     *   \return The actual new position of the intermediate color
     */
     /*======================================================================*/
    double moveIntermediateColor(double oldPosition, double newPosition);
    void removeIntermediateColor(double position);
    void clearIntermediateColors();
    std::map<double, ColorT> const &intermediateColors() const;

    // Default colormaps
    bool isGray() const;
    void setGray();
    bool isHot() const;
    void setHot();
    bool isJet() const;
    void setJet();
    bool isHSV() const;
    void setHSV();

    // Monochrome color
    void setMonochromeColor(ColorT const &color);
    ColorT monochromeColor() const;

    // Toggle range indicator
    void setRangeIndicatorEnabled(bool enable);
    bool rangeIndicatorEnabled() const;

    // gamma correction
    void setGamma(double gamma);
    double gamma() const;

    // Type is either Monochrome or RGB
    Type type() const;

    // Get the color at index
    ColorT color(float index) const;

    // Load and save from/to HDF5
    void load(BlitzH5File const &infile, std::string const &group);
    void save(BlitzH5File &outfile, std::string const &group) const;

  private:

    void _updateGammaLUT();

    std::set<FloatColormapObserver*> _observers;

    float _start, _end;
    double _gamma;
    ColorT _startColor, _endColor;
    ColorT _monochromeColor;
    std::map<double, ColorT> _intermediateColors;
    bool _rangeIndicator;

    std::vector<double> _gammaLUT;

    friend class FloatColormapObserver;

  };

}

#endif
