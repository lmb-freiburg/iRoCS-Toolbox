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

#include "FloatColormap.hh"

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "FloatColormapObserver.hh"

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

namespace iRoCS {

#ifdef DEBUG_VERBOSE_XML
  std::string FloatColormap::printState() const {
    std::stringstream stateStream;
    stateStream << "_observers=\"";
    if (_observers.size() != 0) {
      std::set<FloatColormapObserver*>::const_iterator it = _observers.begin();
      stateStream << *it;
      ++it;
      for (; it != _observers.end(); ++it) stateStream << "," << *it;
    }
    stateStream << "\"";
    return stateStream.str();
  }
#endif

  FloatColormap::FloatColormap(float start, float end)
    : Observable(), _observers(), _start(start), _end(end), _gamma(1.0),
    _startColor(0, 0, 0), _endColor(255, 255, 255),
    _monochromeColor(255, 255, 255), _intermediateColors(),
    _rangeIndicator(false), _gammaLUT(65536) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatColormap@" << this << "::FloatColormap start=\""
      << start << "\" end=\"" << end << "\" " << printState() << ">"
      << std::endl;
#endif
    _updateGammaLUT();
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</FloatColormap@" << this << "::FloatColormap>" << std::endl;
#endif
  }

  FloatColormap::FloatColormap(FloatColormap const &colorMap)
    : Observable(), _observers(), _start(colorMap._start),
    _end(colorMap._end), _gamma(colorMap._gamma),
    _startColor(colorMap._startColor),
    _endColor(colorMap._endColor),
    _monochromeColor(colorMap._monochromeColor),
    _intermediateColors(colorMap._intermediateColors),
    _rangeIndicator(colorMap._rangeIndicator),
    _gammaLUT(colorMap._gammaLUT) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatColormap@" << this << "::FloatColormap colorMap=\""
      << &colorMap << "\" " << printState() << " />" << std::endl;
#endif
  }

  FloatColormap::~FloatColormap() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatColormap@" << this << "::~FloatColormap "
      << printState() << ">" << std::endl;
    while (_observers.size() > 0) delete *_observers.begin();
    std::cerr << "</FloatColormap@" << this << "::~FloatColormap>" << std::endl;
#endif
  }

  FloatColormap &FloatColormap::operator=(FloatColormap const &colorMap) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatColormap@" << this << "::assignOperator colorMap=\""
      << &colorMap << "\" " << printState() << ">" << std::endl;
#endif
    _start = colorMap._start;
    _end = colorMap._end;
    _gamma = colorMap._gamma;
    _startColor = colorMap._startColor;
    _endColor = colorMap._endColor;
    _monochromeColor = colorMap._monochromeColor;
    _intermediateColors = colorMap._intermediateColors;
    _rangeIndicator = colorMap._rangeIndicator;
    _gammaLUT = colorMap._gammaLUT;
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateColormap();
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</FloatColormap@" << this << "::assignOperator>" << std::endl;
#endif
    return *this;
  }

  void FloatColormap::reset() {
    _start = 0;
    _end = 65535;
    _gamma = 1.0;
    _startColor = ColorT(0, 0, 0);
    _endColor = ColorT(255, 255, 255);
    _monochromeColor = _endColor;
    _intermediateColors.clear();
    _rangeIndicator = false;
    _updateGammaLUT();
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateColormap();
  }

  bool FloatColormap::isUnsigned8BitRange() const {
    return _start == std::numeric_limits<unsigned char>::min() &&
      _end == std::numeric_limits<unsigned char>::max();
  }

  void FloatColormap::setUnsigned8BitRange() {
    if (isUnsigned8BitRange()) return;
    _start = std::numeric_limits<unsigned char>::min();
    _end = std::numeric_limits<unsigned char>::max();
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIndexRange();
  }

  bool FloatColormap::isSigned8BitRange() const {
    return _start == std::numeric_limits<char>::min() &&
      _end == std::numeric_limits<char>::max();
  }

  void FloatColormap::setSigned8BitRange() {
    if (isSigned8BitRange()) return;
    _start = std::numeric_limits<char>::min();
    _end = std::numeric_limits<char>::max();
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIndexRange();
  }

  bool FloatColormap::isUnsigned12BitRange() const {
    return _start == 0.0f && _end == 4095.0f;
  }

  void FloatColormap::setUnsigned12BitRange() {
    if (isUnsigned12BitRange()) return;
    _start = 0.0f;
    _end = 4095.0f;
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIndexRange();
  }

  bool FloatColormap::isUnsigned16BitRange() const {
    return _start == std::numeric_limits<unsigned short>::min() &&
      _end == std::numeric_limits<unsigned short>::max();
  }

  void FloatColormap::setUnsigned16BitRange() {
    if (isUnsigned16BitRange()) return;
    _start = std::numeric_limits<unsigned short>::min();
    _end = std::numeric_limits<unsigned short>::max();
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIndexRange();
  }

  bool FloatColormap::isSigned16BitRange() const {
    return _start == std::numeric_limits<short>::min() &&
      _end == std::numeric_limits<short>::max();
  }

  void FloatColormap::setSigned16BitRange() {
    if (isSigned16BitRange()) return;
    _start = std::numeric_limits<short>::min();
    _end = std::numeric_limits<short>::max();
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIndexRange();
  }

  void FloatColormap::setIndexRange(float startIndex, float endIndex) {
    if ((startIndex == _start && endIndex == _end) || startIndex >= endIndex)
      return;
    _start = startIndex;
    _end = endIndex;
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIndexRange();
  }

  float FloatColormap::setStartIndex(float index) {
    if (_start == index || index >= _end) return _start;
    _start = index;
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIndexRange();
    return _start;
  }

  float FloatColormap::startIndex() const {
    return _start;
  }

  float FloatColormap::setEndIndex(float index) {
    if (_end == index || index <= _start) return _end;
    _end = index;
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIndexRange();
    return _end;
  }

  float FloatColormap::endIndex() const {
    return _end;
  }

  void FloatColormap::setStartColor(ColorT const &color) {
    if (blitz::all(_startColor == color)) return;
    _startColor = color;
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateStartColor();
  }

  FloatColormap::ColorT FloatColormap::startColor() const {
    return _startColor;
  }

  void FloatColormap::setEndColor(ColorT const &color) {
    if (blitz::all(_endColor == color)) return;
    _endColor = color;
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateEndColor();
  }

  FloatColormap::ColorT FloatColormap::endColor() const {
    return _endColor;
  }

  void FloatColormap::setIntermediateColor(
    double position, ColorT const &color) {
    if (position <= 0.0 || position >= 1.0) return;
    std::map<double, ColorT>::iterator it = _intermediateColors.find(position);
    if (it != _intermediateColors.end()) {
      if (blitz::all(it->second == color)) return;
      it->second = color;
    }
    else _intermediateColors[position] = color;
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIntermediateColor(position);
  }

  double FloatColormap::moveIntermediateColor(
    double oldPosition, double newPosition) {
    std::map<double, ColorT>::iterator itOld =
      _intermediateColors.find(oldPosition);
    if (itOld == _intermediateColors.end()) return oldPosition;
    if (newPosition <= 0.0) newPosition = 0.001;
    if (newPosition >= 1.0) newPosition = 0.999;
    std::map<double, ColorT>::iterator itNew =
      _intermediateColors.find(newPosition);
    if (itNew != _intermediateColors.end()) return oldPosition;
    _intermediateColors[newPosition] = itOld->second;
    _intermediateColors.erase(itOld);
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateMoveIntermediateColor(oldPosition, newPosition);
    return newPosition;
  }

  void FloatColormap::removeIntermediateColor(double position) {
    std::map<double, ColorT>::iterator it = _intermediateColors.find(position);
    if (it == _intermediateColors.end()) return;
    _intermediateColors.erase(it);
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateRemoveIntermediateColor(position);
  }

  void FloatColormap::clearIntermediateColors() {
    if (_intermediateColors.size() == 0) return;
    _intermediateColors.clear();
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateClearIntermediateColors();
  }

  std::map<double, FloatColormap::ColorT> const
    &FloatColormap::intermediateColors() const {
    return _intermediateColors;
  }

  bool FloatColormap::isGray() const {
    return _intermediateColors.size() == 0 &&
      blitz::all(_startColor == 0 && _endColor == 255);
  }

  void FloatColormap::setGray() {
    if (isGray()) return;
    _intermediateColors.clear();
    _startColor = ColorT(0, 0, 0);
    _endColor = ColorT(255, 255, 255);
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateColormap();
  }

  bool FloatColormap::isHot() const {
    return blitz::all(_startColor == 0) &&
      _intermediateColors.size() == 2 &&
      _intermediateColors.find(0.333) != _intermediateColors.end() &&
      blitz::all(
        _intermediateColors.find(0.333)->second == ColorT(255, 0, 0)) &&
      _intermediateColors.find(0.667) != _intermediateColors.end() &&
      blitz::all(
        _intermediateColors.find(0.667)->second == ColorT(255, 255, 0)) &&
      blitz::all(_endColor == 255);
  }

  void FloatColormap::setHot() {
    if (isHot()) return;
    _intermediateColors.clear();
    _startColor = ColorT(0, 0, 0);
    _intermediateColors[0.333] = ColorT(255, 0, 0);
    _intermediateColors[0.667] = ColorT(255, 255, 0);
    _endColor = ColorT(255);
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateColormap();
  }

  bool FloatColormap::isJet() const {
    return blitz::all(_startColor == ColorT(0, 0, 127)) &&
      _intermediateColors.size() == 4 &&
      _intermediateColors.find(0.125) != _intermediateColors.end() &&
      blitz::all(
        _intermediateColors.find(0.125)->second == ColorT(0, 0, 255)) &&
      _intermediateColors.find(0.375) != _intermediateColors.end() &&
      blitz::all(
        _intermediateColors.find(0.375)->second == ColorT(0, 255, 255)) &&
      _intermediateColors.find(0.625) != _intermediateColors.end() &&
      blitz::all(
        _intermediateColors.find(0.625)->second == ColorT(255, 255, 0)) &&
      _intermediateColors.find(0.875) != _intermediateColors.end() &&
      blitz::all(
        _intermediateColors.find(0.875)->second == ColorT(255, 0, 0)) &&
      blitz::all(_endColor == ColorT(127, 0, 0));
  }

  void FloatColormap::setJet() {
    if (isJet()) return;
    _intermediateColors.clear();
    _startColor = ColorT(0, 0, 127);
    _intermediateColors[0.125] = ColorT(0, 0, 255);
    _intermediateColors[0.375] = ColorT(0, 255, 255);
    _intermediateColors[0.625] = ColorT(255, 255, 0);
    _intermediateColors[0.875] = ColorT(255, 0, 0);
    _endColor = ColorT(127, 0, 0);
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateColormap();
  }

  bool FloatColormap::isHSV() const {
    return blitz::all(_startColor == ColorT(255, 0, 0)) &&
      _intermediateColors.size() == 5 &&
      _intermediateColors.find(0.167) != _intermediateColors.end() &&
      blitz::all(
        _intermediateColors.find(0.167)->second == ColorT(255, 255, 0)) &&
      _intermediateColors.find(0.333) != _intermediateColors.end() &&
      blitz::all(
        _intermediateColors.find(0.333)->second == ColorT(0, 255, 0)) &&
      _intermediateColors.find(0.5) != _intermediateColors.end() &&
      blitz::all(
        _intermediateColors.find(0.5)->second == ColorT(0, 255, 255)) &&
      _intermediateColors.find(0.667) != _intermediateColors.end() &&
      blitz::all(
        _intermediateColors.find(0.667)->second == ColorT(0, 0, 255)) &&
      _intermediateColors.find(0.833) != _intermediateColors.end() &&
      blitz::all(
        _intermediateColors.find(0.833)->second == ColorT(255, 0, 255)) &&
      blitz::all(_endColor == ColorT(255, 0, 0));
  }

  void FloatColormap::setHSV() {
    if (isHSV()) return;
    _intermediateColors.clear();
    _startColor = ColorT(255, 0, 0);
    _intermediateColors[0.167] = ColorT(255, 255, 0);
    _intermediateColors[0.333] = ColorT(0, 255, 0);
    _intermediateColors[0.5] = ColorT(0, 255, 255);
    _intermediateColors[0.667] = ColorT(0, 0, 255);
    _intermediateColors[0.833] = ColorT(255, 0, 255);
    _endColor = ColorT(255, 0, 0);
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateColormap();
  }

  void FloatColormap::setMonochromeColor(ColorT const &color) {
    if (blitz::all(_monochromeColor == color)) return;
    _monochromeColor = color;
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateMonochromeColor();
  }

  FloatColormap::ColorT FloatColormap::monochromeColor() const {
    return _monochromeColor;
  }

  void FloatColormap::setRangeIndicatorEnabled(bool enable) {
    if (_rangeIndicator == enable) return;
    _rangeIndicator = enable;
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateRangeIndicator();
  }

  bool FloatColormap::rangeIndicatorEnabled() const {
    return _rangeIndicator;
  }

  void FloatColormap::setGamma(double gamma) {
    if (_gamma == gamma) return;
    _gamma = gamma;
    _updateGammaLUT();
    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateGamma();
  }

  double FloatColormap::gamma() const {
    return _gamma;
  }

  FloatColormap::Type FloatColormap::type() const {
    return (_intermediateColors.size() == 0 &&
      blitz::all(_startColor == 0 && _endColor == 255)) ?
      Monochrome : RGB;
  }

  FloatColormap::ColorT FloatColormap::color(float index) const {
    // Out of bounds?
    if (index < _start) return color(_start);
    if (index > _end) return color(_end);

    // Range indicator?
    if (_rangeIndicator) {
      if (index == _start) return ColorT(0, 0, 255);
      if (index == _end) return ColorT(255, 0, 0);
    }

    // Compute float position and new index in colormap after gamma correction
    double lambda = static_cast<double>(index - _start) /
      static_cast<double>(_end - _start);
    if (_gamma != 1.0) {
      lambda = _gammaLUT[static_cast<size_t>(
        lambda * static_cast<double>(_gammaLUT.size() - 1))];
      index = static_cast<int>(
        lambda * static_cast<double>(_end - _start) +
        static_cast<double>(_start));
    }

    ColorT c1, c2;

    // No intermediate colors ==> interpolate between _start and _end
    if (_intermediateColors.size() == 0) {
      if (type() == Monochrome) {
        c1 = _monochromeColor * static_cast<float>(_startColor(0)) / 255.0f;
        c2 = _monochromeColor * static_cast<float>(_endColor(0)) / 255.0f;
      }
      else {
        c1 = _startColor;
        c2 = _endColor;
      }
    }
    else {
      // Find The color interval for interpolation
      std::map<double, ColorT>::const_iterator it =
        _intermediateColors.lower_bound(lambda);
      if (it == _intermediateColors.begin()) {
        lambda = std::min(1.0, std::max(0.0, lambda / it->first));
        if (type() == Monochrome) {
          c1 = _monochromeColor * static_cast<float>(_startColor(0)) / 255.0f;
          c2 = _monochromeColor * static_cast<float>(it->second(0)) / 255.0f;
        }
        else {
          c1 = _startColor;
          c2 = it->second;
        }
      }
      else if (it == _intermediateColors.end()) {
        --it;
        lambda = std::min(
          1.0, std::max(0.0, (lambda - it->first) / (1.0 - it->first)));
        if (type() == Monochrome) {
          c1 = _monochromeColor * static_cast<float>(it->second(0)) / 255.0f;
          c2 = _monochromeColor * static_cast<float>(_endColor(0)) / 255.0f;
        }
        else {
          c1 = it->second;
          c2 = _endColor;
        }
      }
      else {
        std::map<double, ColorT>::const_iterator it2 = it;
        --it;
        lambda = std::min(
          1.0, std::max(
            0.0, (lambda - it->first) / (it2->first - it->first)));
        if (type() == Monochrome) {
          c1 = _monochromeColor * static_cast<float>(it->second(0)) / 255.0f;
          c2 = _monochromeColor * static_cast<float>(it2->second(0)) / 255.0f;
        }
        else {
          c1 = it->second;
          c2 = it2->second;
        }
      }
    }
    return ColorT((1.0 - lambda) * c1 + lambda * c2);
  }

  void FloatColormap::load(
    BlitzH5File const &infile, std::string const &group) {
    std::string id;
    infile.readAttribute(id, "id", group);
    if (id != "FloatColormap")
      throw BlitzH5Error("'FloatColormap' id not found");
    infile.readAttribute(_start, "startIndex", group);
    infile.readAttribute(_end, "endIndex", group);
    infile.readAttribute(_gamma, "gamma", group);
    _updateGammaLUT();
    blitz::TinyVector<float, 3> tmp;
    infile.readAttribute(tmp, "startColor", group);
    _startColor = ColorT(255.0f * tmp);
    infile.readAttribute(tmp, "endColor", group);
    _endColor = ColorT(255.0f * tmp);
    infile.readAttribute(tmp, "monochromeColor", group);
    _monochromeColor = ColorT(255.0f * tmp);
    blitz::Array<int, 1> indices;
    blitz::Array<double, 1> positions;
    blitz::Array<blitz::TinyVector<float, 3>, 1> colors;
    _intermediateColors.clear();
    try {
      infile.readDataset(positions, group + "/intermediatePercentages");
      infile.readDataset(colors, group + "/intermediateColors");
      for (size_t i = 0; i < positions.size(); ++i)
        _intermediateColors[positions(i)] = ColorT(255.0f * colors(i));
    }
    catch (BlitzH5Error &) {
    }
    int range;
    infile.readAttribute(range, "rangeIndicatorEnabled", group);
    _rangeIndicator = (range == 1);

    for (std::set<FloatColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateColormap();
  }

  void FloatColormap::save(
    BlitzH5File &outfile, std::string const &group) const {
    std::string id = "FloatColormap";
    outfile.writeAttribute(id, "id", group);
    outfile.writeAttribute(_start, "startIndex", group);
    outfile.writeAttribute(_end, "endIndex", group);
    outfile.writeAttribute(_gamma, "gamma", group);
    blitz::TinyVector<float, 3> tmp(
      blitz::TinyVector<float, 3>(_startColor) / 255.0f);
    outfile.writeAttribute(tmp, "startColor", group);
    tmp = blitz::TinyVector<float, 3>(_endColor) / 255.0f;
    outfile.writeAttribute(tmp, "endColor", group);
    tmp = blitz::TinyVector<float, 3>(_monochromeColor) / 255.0f;
    outfile.writeAttribute(tmp, "monochromeColor", group);
    if (_intermediateColors.size() != 0) {
      blitz::Array<double, 1> positions(_intermediateColors.size());
      blitz::Array<blitz::TinyVector<float, 3>, 1> colors(
        _intermediateColors.size());
      ptrdiff_t i = 0;
      for (std::map< double, ColorT >::const_iterator it =
        _intermediateColors.begin(); it != _intermediateColors.end();
        ++it, ++i) {
        positions(i) = it->first;
        colors(i) = blitz::TinyVector<float, 3>(it->second) / 255.0f;
      }
      outfile.writeDataset(
        positions, group + "/intermediatePercentages");
      outfile.writeDataset(colors, group + "/intermediateColors");
    }
    int range = (_rangeIndicator) ? 1 : 0;
    outfile.writeAttribute(range, "rangeIndicatorEnabled", group);
  }

  void FloatColormap::_updateGammaLUT() {
    for (int i = 0; i < static_cast<int>(_gammaLUT.size()); ++i)
      _gammaLUT[i] = std::pow(
        static_cast<double>(i) / static_cast<double>(_gammaLUT.size() - 1),
        1.0 / _gamma);
  }

}
