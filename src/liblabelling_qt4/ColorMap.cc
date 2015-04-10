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

#include "ColorMap.hh"

#include "IColorMapEditor.hh"

#if defined HAVE_CONFIG_H && !defined PACKAGE
#include <config.hh>
#endif

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

ColorMap::ColorMap(long long start, long long end)
        : _start(start), _end(end), _gamma(1.0), _startColor(0.0f),
          _endColor(1.0f), _monochromeColor(1.0f), _isRandom(false),
          _colorMapType(Monochrome), _rangeIndicator(false),
          p_colorMapEditor(NULL)
{
  _fixedRandomColors.resize(65535);
  for (size_t i = 0; i < _fixedRandomColors.size(); ++i)
      _fixedRandomColors[i] = generateRandomColor();

  _gammaLUT.resize(65536);
#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (int i = 0; i < static_cast<int>(_gammaLUT.size()); ++i)
      _gammaLUT[i] = static_cast<double>(i) /
          static_cast<double>(_gammaLUT.size() - 1);
}

ColorMap::ColorMap(ColorMap const &colorMap)
        : _start(colorMap._start), _end(colorMap._end), _gamma(colorMap._gamma),
          _startColor(colorMap._startColor), _endColor(colorMap._endColor),
          _monochromeColor(colorMap._monochromeColor),
          _isRandom(colorMap._isRandom),
          _intermediateColors(colorMap._intermediateColors),
          _overrideColors(colorMap._overrideColors),
          _fixedRandomColors(colorMap._fixedRandomColors),
          _additionalRandomColors(colorMap._additionalRandomColors),
          _colorMapType(colorMap._colorMapType),
          _rangeIndicator(colorMap._rangeIndicator),
          _gammaLUT(colorMap._gammaLUT),
          p_colorMapEditor(colorMap.p_colorMapEditor)
{}

ColorMap::~ColorMap()
{}
  
ColorMap &ColorMap::operator=(ColorMap const &colorMap)
{
  _start = colorMap._start;
  _end = colorMap._end;
  _gamma = colorMap._gamma;
  _startColor = colorMap._startColor;
  _endColor = colorMap._endColor;
  _monochromeColor = colorMap._monochromeColor;
  _isRandom = colorMap._isRandom;
  _intermediateColors = colorMap._intermediateColors;
  _overrideColors = colorMap._overrideColors;
  _fixedRandomColors = colorMap._fixedRandomColors;
  _additionalRandomColors = colorMap._additionalRandomColors;
  _colorMapType = colorMap._colorMapType;
  _rangeIndicator = colorMap._rangeIndicator;
  _gammaLUT = colorMap._gammaLUT;
  p_colorMapEditor = colorMap.p_colorMapEditor;
  return *this;
}

bool ColorMap::isUnsigned8BitRange() const
{
  return _start == std::numeric_limits<unsigned char>::min() &&
      _end == std::numeric_limits<unsigned char>::max();
}

void ColorMap::setUnsigned8BitRange()
{
  if (isUnsigned8BitRange()) return;
  _start = std::numeric_limits<unsigned char>::min();
  _end = std::numeric_limits<unsigned char>::max();
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIndexRange();
}

bool ColorMap::isSigned8BitRange() const
{
  return _start == std::numeric_limits<char>::min() &&
      _end == std::numeric_limits<char>::max();
}

void ColorMap::setSigned8BitRange()
{
  if (isSigned8BitRange()) return;
  _start = std::numeric_limits<char>::min();
  _end = std::numeric_limits<char>::max();
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIndexRange();
}

bool ColorMap::isUnsigned12BitRange() const
{
  return _start == 0 && _end == 4095;
}

void ColorMap::setUnsigned12BitRange()
{
  if (isUnsigned12BitRange()) return;
  _start = 0;
  _end = 4095;
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIndexRange();
}

bool ColorMap::isUnsigned16BitRange() const
{
  return _start == std::numeric_limits<unsigned short>::min() &&
      _end == std::numeric_limits<unsigned short>::max();
}

void ColorMap::setUnsigned16BitRange()
{
  if (isUnsigned16BitRange()) return;
  _start = std::numeric_limits<unsigned short>::min();
  _end = std::numeric_limits<unsigned short>::max();
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIndexRange();
}

bool ColorMap::isSigned16BitRange() const
{
  return _start == std::numeric_limits<short>::min() &&
      _end == std::numeric_limits<short>::max();
}

void ColorMap::setSigned16BitRange()
{
  if (isSigned16BitRange()) return;
  _start = std::numeric_limits<short>::min();
  _end = std::numeric_limits<short>::max();
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIndexRange();
}

bool ColorMap::isUnsigned32BitRange() const
{
  return _start == std::numeric_limits<unsigned int>::min() &&
      _end == std::numeric_limits<unsigned int>::max();
}

void ColorMap::setUnsigned32BitRange()
{
  if (isUnsigned32BitRange()) return;
  _start = std::numeric_limits<unsigned int>::min();
  _end = std::numeric_limits<unsigned int>::max();
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIndexRange();
}

bool ColorMap::isSigned32BitRange() const
{
  return _start == std::numeric_limits<int>::min() &&
      _end == std::numeric_limits<int>::max();
}

void ColorMap::setSigned32BitRange()
{
  if (isSigned32BitRange()) return;
  _start = std::numeric_limits<int>::min();
  _end = std::numeric_limits<int>::max();
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIndexRange();
}

bool ColorMap::isSigned64BitRange() const
{
  return _start == std::numeric_limits<long long>::min() &&
      _end == std::numeric_limits<long long>::max();
}

void ColorMap::setSigned64BitRange()
{
  if (isSigned64BitRange()) return;
  _start = std::numeric_limits<long long>::min();
  _end = std::numeric_limits<long long>::max();
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIndexRange();
}

void ColorMap::setStartIndex(long long index)
{
  if (_start == index) return;
  _start = index;
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIndexRange();
}

void ColorMap::setEndIndex(long long index)
{
  if (_end == index) return;
  _end = index;
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIndexRange();
}

long long ColorMap::startIndex() const
{
  return _start;
}

long long ColorMap::endIndex() const
{
  return _end;
}

void ColorMap::setStartColor(blitz::TinyVector<float,3> const &color)
{
  if (blitz::all(_startColor == color)) return;
  _startColor = color;
  _updateColorMapType();
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIntermediateColors();
}

void ColorMap::setEndColor(blitz::TinyVector<float,3> const &color)
{
  if (blitz::all(_endColor == color)) return;
  _endColor = color;
  _updateColorMapType();
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIntermediateColors();
}

blitz::TinyVector<float,3> ColorMap::startColor() const
{
  return _startColor;
}

blitz::TinyVector<float,3> ColorMap::endColor() const
{
  return _endColor;
}

void ColorMap::addIntermediateColor(
    double percentage, blitz::TinyVector<float,3> const &color)
{
  std::map< double,blitz::TinyVector<float,3> >::iterator it =
      _intermediateColors.find(percentage);
  if (it != _intermediateColors.end())
  {
    if (blitz::all(it->second == color)) return;
    it->second = color;
  }
  else _intermediateColors[percentage] = color;
  _updateColorMapType();
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIntermediateColors();
}

void ColorMap::removeIntermediateColor(double percentage)
{
  std::map< double,blitz::TinyVector<float,3> >::iterator it =
      _intermediateColors.find(percentage);
  if (it == _intermediateColors.end()) return;
  _intermediateColors.erase(it);
  _updateColorMapType();
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIntermediateColors();
}

void ColorMap::clearIntermediateColors()
{
  if (_intermediateColors.size() == 0) return;
  _intermediateColors.clear();
  _updateColorMapType();
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIntermediateColors();
}

std::map< double,blitz::TinyVector<float,3> > const
&ColorMap::intermediateColors() const
{
  return _intermediateColors;
}

void ColorMap::addOverrideColor(
    long long index, blitz::TinyVector<float,3> const &color)
{
  std::map< long long,blitz::TinyVector<float,3> >::iterator it =
      _overrideColors.find(index);
  if (it != _overrideColors.end())
  {
    if (blitz::all(it->second == color)) return;
    it->second = color;
  }
  else _overrideColors[index] = color;
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateOverrideColors();
}

void ColorMap::removeOverrideColor(long long index)
{
  std::map< long long,blitz::TinyVector<float,3> >::iterator it =
      _overrideColors.find(index);
  if (it == _overrideColors.end()) return;
  _overrideColors.erase(it);
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateOverrideColors();
}

void ColorMap::clearOverrideColors()
{
  if (_intermediateColors.size() == 0) return;
  _overrideColors.clear();
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateOverrideColors();
}
  
std::map< long long,blitz::TinyVector<float,3> > const
&ColorMap::overrideColors() const
{
  return _overrideColors;
}

bool ColorMap::isGray() const
{
  return !isRandom() && _intermediateColors.size() == 0 &&
      blitz::all(_startColor == 0.0f && _endColor == 1.0f);
}

void ColorMap::setGray()
{
  if (isGray()) return;
  IColorMapEditor *tmp = p_colorMapEditor;
  p_colorMapEditor = NULL;
  _isRandom = false;
  _additionalRandomColors.clear();
  _intermediateColors.clear();
  _startColor = blitz::TinyVector<float,3>(0.0f);
  _endColor = blitz::TinyVector<float,3>(1.0f);
  _colorMapType = Monochrome;
  p_colorMapEditor = tmp;
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIntermediateColors();
}

bool ColorMap::isHot() const
{
  return !isRandom() && blitz::all(_startColor == 0.0f) &&
      _intermediateColors.size() == 2 &&
      _intermediateColors.find(0.333) != _intermediateColors.end() &&
      blitz::all(_intermediateColors.find(0.333)->second ==
                 blitz::TinyVector<float,3>(1.0f, 0.0f, 0.0f)) &&
      _intermediateColors.find(0.667) != _intermediateColors.end() &&
      blitz::all(_intermediateColors.find(0.667)->second ==
                 blitz::TinyVector<float,3>(1.0f, 1.0f, 0.0f)) &&
      blitz::all(_endColor == 1.0f);
}

void ColorMap::setHot()
{
  if (isHot()) return;
  IColorMapEditor *tmp = p_colorMapEditor;
  p_colorMapEditor = NULL;
  _isRandom = false;
  _additionalRandomColors.clear();
  _intermediateColors.clear();
  _startColor = blitz::TinyVector<float,3>(0.0f);
  addIntermediateColor(0.333, blitz::TinyVector<float,3>(1.0f, 0.0f, 0.0f));
  addIntermediateColor(0.667, blitz::TinyVector<float,3>(1.0f, 1.0f, 0.0f));
  _endColor = blitz::TinyVector<float,3>(1.0f);
  _colorMapType = RGB;
  p_colorMapEditor = tmp;
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIntermediateColors();
}

bool ColorMap::isJet() const
{
  return !isRandom() && blitz::all(
      _startColor == blitz::TinyVector<float,3>(0.0f, 0.0f, 0.5f)) &&
      _intermediateColors.size() == 4 &&
      _intermediateColors.find(0.125) != _intermediateColors.end() &&
      blitz::all(_intermediateColors.find(0.125)->second ==
                 blitz::TinyVector<float,3>(0.0f, 0.0f, 1.0f)) &&
      _intermediateColors.find(0.375) != _intermediateColors.end() &&
      blitz::all(_intermediateColors.find(0.375)->second ==
                 blitz::TinyVector<float,3>(0.0f, 1.0f, 1.0f)) &&
      _intermediateColors.find(0.625) != _intermediateColors.end() &&
      blitz::all(_intermediateColors.find(0.625)->second ==
                 blitz::TinyVector<float,3>(1.0f, 1.0f, 0.0f)) &&
      _intermediateColors.find(0.875) != _intermediateColors.end() &&
      blitz::all(_intermediateColors.find(0.875)->second ==
                 blitz::TinyVector<float,3>(1.0f, 0.0f, 0.0f)) &&
      blitz::all(_endColor == blitz::TinyVector<float,3>(0.5f, 0.0f, 0.0f));
}

void ColorMap::setJet()
{
  if (isJet()) return;
  IColorMapEditor *tmp = p_colorMapEditor;
  p_colorMapEditor = NULL;
  _isRandom = false;
  _additionalRandomColors.clear();
  _intermediateColors.clear();
  _startColor = blitz::TinyVector<float,3>(0.0f, 0.0f, 0.5f);
  addIntermediateColor(0.125, blitz::TinyVector<float,3>(0.0f, 0.0f, 1.0f));
  addIntermediateColor(0.375, blitz::TinyVector<float,3>(0.0f, 1.0f, 1.0f));
  addIntermediateColor(0.625, blitz::TinyVector<float,3>(1.0f, 1.0f, 0.0f));
  addIntermediateColor(0.875, blitz::TinyVector<float,3>(1.0f, 0.0f, 0.0f));
  _endColor = blitz::TinyVector<float,3>(0.5f, 0.0f, 0.0f);  
  _colorMapType = RGB;
  p_colorMapEditor = tmp;
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIntermediateColors();
}

bool ColorMap::isHSV() const
{
  return !isRandom() && blitz::all(
      _startColor == blitz::TinyVector<float,3>(1.0f, 0.0f, 0.0f)) &&
      _intermediateColors.size() == 5 &&
      _intermediateColors.find(0.167) != _intermediateColors.end() &&
      blitz::all(_intermediateColors.find(0.167)->second ==
                 blitz::TinyVector<float,3>(1.0f, 1.0f, 0.0f)) &&
      _intermediateColors.find(0.333) != _intermediateColors.end() &&
      blitz::all(_intermediateColors.find(0.333)->second ==
                 blitz::TinyVector<float,3>(0.0f, 1.0f, 0.0f)) &&
      _intermediateColors.find(0.5) != _intermediateColors.end() &&
      blitz::all(_intermediateColors.find(0.5)->second ==
                 blitz::TinyVector<float,3>(0.0f, 1.0f, 1.0f)) &&
      _intermediateColors.find(0.667) != _intermediateColors.end() &&
      blitz::all(_intermediateColors.find(0.667)->second ==
                 blitz::TinyVector<float,3>(0.0f, 0.0f, 1.0f)) &&
      _intermediateColors.find(0.833) != _intermediateColors.end() &&
      blitz::all(_intermediateColors.find(0.833)->second ==
                 blitz::TinyVector<float,3>(1.0f, 0.0f, 1.0f)) &&
      blitz::all(_endColor == blitz::TinyVector<float,3>(1.0f, 0.0f, 0.0f));
}

void ColorMap::setHSV()
{
  if (isHSV()) return;
  IColorMapEditor *tmp = p_colorMapEditor;
  p_colorMapEditor = NULL;
  _isRandom = false;
  _additionalRandomColors.clear();
  _intermediateColors.clear();
  _startColor = blitz::TinyVector<float,3>(1.0f, 0.0f, 0.0f);
  addIntermediateColor(0.167, blitz::TinyVector<float,3>(1.0f, 1.0f, 0.0f));
  addIntermediateColor(0.333, blitz::TinyVector<float,3>(0.0f, 1.0f, 0.0f));
  addIntermediateColor(0.5, blitz::TinyVector<float,3>(0.0f, 1.0f, 1.0f));
  addIntermediateColor(0.667, blitz::TinyVector<float,3>(0.0f, 0.0f, 1.0f));
  addIntermediateColor(0.833, blitz::TinyVector<float,3>(1.0f, 0.0f, 1.0f));
  _endColor = blitz::TinyVector<float,3>(1.0f, 0.0f, 0.0f);
  _colorMapType = RGB;
  p_colorMapEditor = tmp;
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIntermediateColors();
}

bool ColorMap::isRandom() const
{
  return _isRandom;
}

void ColorMap::setRandom()
{
  if (isRandom()) return;
  IColorMapEditor *tmp = p_colorMapEditor;
  p_colorMapEditor = NULL;
  _isRandom = true;
  _intermediateColors.clear();
  _startColor = blitz::TinyVector<float,3>(0.0f, 0.0f, 0.0f);
  _endColor = blitz::TinyVector<float,3>(1.0f, 1.0f, 1.0f);
  _colorMapType = RGB;
  p_colorMapEditor = tmp;
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateIntermediateColors();
}

void ColorMap::setMonochromeColor(blitz::TinyVector<float,3> const &color)
{
  if (blitz::all(_monochromeColor == color)) return;
  _monochromeColor = color;
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateMonochromeColor();
}

blitz::TinyVector<float,3> ColorMap::monochromeColor() const
{
  return _monochromeColor;
}

void ColorMap::setRangeIndicatorEnabled(bool enable)
{
  if (_rangeIndicator == enable) return;
  _rangeIndicator = enable;
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateRangeIndicator();
}

bool ColorMap::rangeIndicatorEnabled() const
{
  return _rangeIndicator;
}

void ColorMap::setGamma(double gamma)
{
  if (_gamma == gamma) return;
  _gamma = gamma;
  _updateGammaLUT();
  if (p_colorMapEditor != NULL) p_colorMapEditor->updateGamma();
}

double ColorMap::gamma() const
{
  return _gamma;
}

ColorMap::ColorMapType ColorMap::type() const
{
  return _colorMapType;
}

blitz::TinyVector<float,3> ColorMap::color(long long index) const
{
  // Out of bounds?
  if (index < _start) return color(_start);
  if (index > _end) return color(_end);

  // Range indicator?
  if (_rangeIndicator)
  {
    if (index == _start) return blitz::TinyVector<float,3>(0.0f, 0.0f, 1.0f);
    if (index == _end) return blitz::TinyVector<float,3>(1.0f, 0.0f, 0.0f);
  }
  
  // Check whether the index has an explicit override
  std::map< long long,blitz::TinyVector<float,3> >::const_iterator itO =
      _overrideColors.find(index);
  if (itO != _overrideColors.end()) return itO->second;
  
  // Is the colormap set to random? If so check random color table and on
  // demand create new entry
  if (isRandom())
  {
    if (index >= 0 && index < static_cast<int>(_fixedRandomColors.size()))
        return _fixedRandomColors[index];
    
    blitz::TinyVector<float,3> color;
#ifdef _OPENMP
#pragma omp critical
#endif
    {
      std::map< long long,blitz::TinyVector<float,3> >::iterator it =
          _additionalRandomColors.find(index);    
      if (it != _additionalRandomColors.end()) color = it->second;
      else
      {
        color = generateRandomColor();
        _additionalRandomColors[index] = color;
      }
    }
    return color;
  }

  // Compute float position and new index in colormap after gamma correction
  double lambda = static_cast<double>(index - _start) /
      static_cast<double>(_end - _start);
  if (_gamma != 1.0)
  {
    lambda = _gammaLUT[static_cast<size_t>(
          lambda * static_cast<double>(_gammaLUT.size() - 1))];
    index = static_cast<long long>(
        lambda * static_cast<double>(_end - _start) +
        static_cast<double>(_start));
  }

  // No intermediate colors ==> interpolate between _start and _end
  if (_intermediateColors.size() == 0)
  {
    if (_colorMapType == Monochrome)
        return _monochromeColor * (
            (1.0 - lambda) * _startColor + lambda * _endColor);
    else
        return (1.0 - lambda) * _startColor + lambda * _endColor;
  }
  
  // Find The color interval for interpolation
  std::map< double,blitz::TinyVector<float,3> >::const_iterator it =
      _intermediateColors.lower_bound(lambda);
  if (it == _intermediateColors.begin())
  {
    long long uBoundIdx =
        static_cast<long long>(it->first * static_cast<double>(_end - _start) +
                               0.5) + _start;
    lambda = static_cast<double>(index - _start) /
        static_cast<double>(uBoundIdx - _start);
    if (lambda < 0.0) lambda = 0.0;
    if (lambda > 1.0) lambda = 1.0;
    if (_colorMapType == Monochrome)
        return _monochromeColor * (
            (1.0 - lambda) * _startColor + lambda * it->second);
    else
        return (1.0 - lambda) * _startColor + lambda * it->second;
  }
  
  if (it == _intermediateColors.end())
  {
    --it;
    long long lBoundIdx =
        static_cast<long long>(
            it->first * static_cast<double>(_end - _start) + 0.5) + _start;
    lambda = static_cast<double>(index - lBoundIdx) /
        static_cast<double>(_end - lBoundIdx);
    if (lambda < 0.0) lambda = 0.0;
    if (lambda > 1.0) lambda = 1.0;
    if (_colorMapType == Monochrome)
        return _monochromeColor * (
            (1.0 - lambda) * it->second + lambda * _endColor);
    else
        return (1.0 - lambda) * it->second + lambda * _endColor;
  }
  
  std::map< double,blitz::TinyVector<float,3> >::const_iterator it2 = it;
  --it;
  long long lBoundIdx =
      static_cast<long long>(
          it->first * static_cast<double>(_end - _start) + 0.5) + _start;
  long long uBoundIdx =
      static_cast<long long>(
          it2->first * static_cast<double>(_end - _start) + 0.5) + _start;
  lambda = static_cast<double>(index - lBoundIdx) /
      static_cast<double>(uBoundIdx - lBoundIdx);  
  if (lambda < 0.0) lambda = 0.0;
  if (lambda > 1.0) lambda = 1.0;
  if (_colorMapType == Monochrome)
      return _monochromeColor * (
          (1.0 - lambda) * it->second + lambda * it2->second);
  else
      return (1.0 - lambda) * it->second + lambda * it2->second;
}

void ColorMap::load(BlitzH5File const &infile, std::string const &group)
{
  std::string id;
  infile.readAttribute(id, "id", group);
  if (id != "ColorMap") throw BlitzH5Error("'ColorMap' id not found");
  infile.readAttribute(_start, "startIndex", group);
  infile.readAttribute(_end, "endIndex", group);
  infile.readAttribute(_gamma, "gamma", group);
  _updateGammaLUT();
  infile.readAttribute(_startColor, "startColor", group);
  infile.readAttribute(_endColor, "endColor", group);
  infile.readAttribute(_monochromeColor, "monochromeColor", group);
  blitz::Array<long long,1> indices;
  blitz::Array<double,1> percentages;
  blitz::Array<blitz::TinyVector<float,3>,1> colors;
  _intermediateColors.clear();
  try
  {
    infile.readDataset(percentages, group + "/intermediatePercentages");
    infile.readDataset(colors, group + "/intermediateColors");
    for (size_t i = 0; i < percentages.size(); ++i)
        _intermediateColors[percentages(i)] = colors(i);
    _updateColorMapType();
  }
  catch (BlitzH5Error &)
  {}
  _overrideColors.clear();
  try
  {
    infile.readDataset(indices, group + "/overrideIndices");
    infile.readDataset(colors, group + "/overrideColors");
    for (size_t i = 0; i < indices.size(); ++i)
        _overrideColors[indices(i)] = colors(i);
  }
  catch (BlitzH5Error &)
  {}
  int range;
  infile.readAttribute(range, "rangeIndicatorEnabled", group);
  _rangeIndicator = (range == 1);
  try
  {
    int random;
    infile.readAttribute(random, "random", group);
    if (random == 1) setRandom();
  }
  catch (BlitzH5Error &)
  {}

  if (p_colorMapEditor != NULL) p_colorMapEditor->updateControls();
}

void ColorMap::load(std::string const &fileName, std::string const &group)
{
  try
  {
    BlitzH5File inFile(fileName);
    load(inFile, group);
  }
  catch (BlitzH5Error &e)
  {
   std::cerr << "Error while reading color map from '" << fileName << ":"
              << group << "': " << e.what() << std::endl;
  }
}

void ColorMap::save(BlitzH5File &outfile, std::string const &group) const
{
  std::string id = "ColorMap";
  outfile.writeAttribute(id, "id", group);
  outfile.writeAttribute(_start, "startIndex", group);
  outfile.writeAttribute(_end, "endIndex", group);
  outfile.writeAttribute(_gamma, "gamma", group);
  outfile.writeAttribute(_startColor, "startColor", group);
  outfile.writeAttribute(_endColor, "endColor", group);
  outfile.writeAttribute(_monochromeColor, "monochromeColor", group);
  int random = (isRandom()) ? 1 : 0;
  outfile.writeAttribute(random, "random", group);
  if (_intermediateColors.size() != 0)
  {
    blitz::Array<double,1> percentages(_intermediateColors.size());
    blitz::Array<blitz::TinyVector<float,3>,1> colors(
        _intermediateColors.size());
    ptrdiff_t i = 0;
    for (std::map< double,blitz::TinyVector<float,3> >::const_iterator it =
             _intermediateColors.begin(); it != _intermediateColors.end();
         ++it, ++i)
    {
      percentages(i) = it->first;
      colors(i) = it->second;
    }
    outfile.writeDataset(
        percentages, group + "/intermediatePercentages");
    outfile.writeDataset(colors, group + "/intermediateColors");
  }
  if (_overrideColors.size() != 0)
  {
    blitz::Array<long long,1> indices(_overrideColors.size());
    blitz::Array<blitz::TinyVector<float,3>,1> colors(
        _overrideColors.size());
    ptrdiff_t i = 0;
    for (std::map< long long,blitz::TinyVector<float,3> >::const_iterator
             it = _overrideColors.begin(); it != _overrideColors.end();
         ++it, ++i)
    {
      indices(i) = it->first;
      colors(i) = it->second;
    }
    outfile.writeDataset(indices, group + "/overrideIndices");
    outfile.writeDataset(colors, group + "/overrideColors");
  }
  int range = (_rangeIndicator) ? 1 : 0;
  outfile.writeAttribute(range, "rangeIndicatorEnabled", group);
}

void ColorMap::save(std::string const &fileName, std::string const &group) const
{
  try
  {
    BlitzH5File outFile(fileName, BlitzH5File::WriteOrNew);
    save(outFile, group);
  }
  catch (BlitzH5Error &e)
  {
    std::cerr << "Error while writing color map to '" << fileName << ":"
              << group << "': " << e.what() << std::endl;
  }
}

void ColorMap::setColorMapEditor(IColorMapEditor *colorMapEditor)
{
  if (colorMapEditor == p_colorMapEditor) return;
  p_colorMapEditor = colorMapEditor;
  if (p_colorMapEditor != NULL) p_colorMapEditor->setColorMap(this);
}

IColorMapEditor *ColorMap::colorMapEditor() const
{
  return p_colorMapEditor;
}

void ColorMap::_updateGammaLUT()
{
#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (int i = 0; i < static_cast<int>(_gammaLUT.size()); ++i)
      _gammaLUT[i] = std::pow(
          static_cast<double>(i) / static_cast<double>(_gammaLUT.size() - 1),
          1.0 / _gamma);  
}

void ColorMap::_updateColorMapType()
{
  if (blitz::any(_startColor != _startColor(0)))
  {
    _colorMapType = RGB;
    return;
  }

  if (blitz::any(_endColor != _endColor(0)))
  {
    _colorMapType = RGB;
    return;
  }

  for (std::map< double, blitz::TinyVector<float,3> >::const_iterator it =
           _intermediateColors.begin(); it != _intermediateColors.end(); ++it)
  {
    if (blitz::any(it->second != it->second(0)))
    {
      _colorMapType = RGB;
      return;
    }
  }
}

blitz::TinyVector<float,3> ColorMap::generateRandomColor()
{
  return blitz::TinyVector<float,3>(
      static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
      static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
      static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}
