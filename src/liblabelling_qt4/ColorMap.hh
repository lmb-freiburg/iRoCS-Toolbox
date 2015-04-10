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

#ifndef COLORMAP_HH
#define COLORMAP_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <blitz/array.h>
#include <map>
#include <vector>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

class IColorMapEditor;

class ColorMap
{

public:
  
  enum ColorMapType { Monochrome, RGB };

  ColorMap(long long start = 0, long long end = 65535);
  ColorMap(ColorMap const &colorMap);
  ~ColorMap();
  
  ColorMap &operator=(ColorMap const &colorMap);

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
  bool isUnsigned32BitRange() const;
  void setUnsigned32BitRange();
  bool isSigned32BitRange() const;
  void setSigned32BitRange();
  bool isSigned64BitRange() const;
  void setSigned64BitRange();

  // Custom index ranges
  void setStartIndex(long long index);
  void setEndIndex(long long index);
  long long startIndex() const;
  long long endIndex() const;

  // Custom colors at the ends of the color transition
  void setStartColor(blitz::TinyVector<float,3> const &color);
  void setEndColor(blitz::TinyVector<float,3> const &color);
  blitz::TinyVector<float,3> startColor() const;
  blitz::TinyVector<float,3> endColor() const;

  // Intermediate colors for smooth interpolation
  void addIntermediateColor(
      double percentage, blitz::TinyVector<float,3> const &color);
  void removeIntermediateColor(double percentage);
  void clearIntermediateColors();
  std::map< double,blitz::TinyVector<float,3> > const
  &intermediateColors() const;

  // Intermediate colors replacing the interpolated ones
  void addOverrideColor(
      long long index, blitz::TinyVector<float,3> const &color);
  void removeOverrideColor(long long index);
  void clearOverrideColors();
  std::map< long long,blitz::TinyVector<float,3> > const
  &overrideColors() const;
  
  // Default colormaps (Overrides will be preserved)
  bool isGray() const;
  void setGray();
  bool isHot() const;
  void setHot();
  bool isJet() const;
  void setJet();
  bool isHSV() const;
  void setHSV();
  bool isRandom() const;
  void setRandom();

  // Monochrome color
  void setMonochromeColor(blitz::TinyVector<float,3> const &color);
  blitz::TinyVector<float,3> monochromeColor() const;

  // Toggle range indicator
  void setRangeIndicatorEnabled(bool enable);
  bool rangeIndicatorEnabled() const;
  
  // gamma correction
  void setGamma(double gamma);
  double gamma() const;

  // Type is either Monochrome or RGB
  ColorMapType type() const;

  // Get the color at index
  blitz::TinyVector<float,3> color(long long index) const;

  // Load and save from/to HDF5
  void load(BlitzH5File const &infile, std::string const &group);
  void load(std::string const &fileName, std::string const &group);
  void save(BlitzH5File &outfile, std::string const &group) const;
  void save(std::string const &fileName, std::string const &group) const;

  // Attach and get the attached color map editor
  void setColorMapEditor(IColorMapEditor* colorMapEditor);
  IColorMapEditor* colorMapEditor() const;

  static blitz::TinyVector<float,3> generateRandomColor();

private:
  
  void _updateGammaLUT();
  void _updateColorMapType();

  long long _start, _end;
  double _gamma;
  blitz::TinyVector<float,3> _startColor, _endColor;
  blitz::TinyVector<float,3> _monochromeColor;
  bool _isRandom;
  std::map< double,blitz::TinyVector<float,3> > _intermediateColors;
  std::map< long long,blitz::TinyVector<float,3> > _overrideColors;
  std::vector< blitz::TinyVector<float,3> > _fixedRandomColors;
  mutable std::map< long long,blitz::TinyVector<float,3> >
  _additionalRandomColors;
  ColorMapType _colorMapType;
  bool _rangeIndicator;

  std::vector<double> _gammaLUT;

  IColorMapEditor *p_colorMapEditor;

};

#endif

  
