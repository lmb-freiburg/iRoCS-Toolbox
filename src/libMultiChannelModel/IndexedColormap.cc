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

#include "IndexedColormap.hh"

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "IndexedColormapObserver.hh"

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

namespace iRoCS {

#ifdef DEBUG_VERBOSE_XML
  std::string IndexedColormap::printState() const {
    std::stringstream stateStream;
    stateStream << "_observers=\"";
    if (_observers.size() != 0) {
      std::set<IndexedColormapObserver*>::const_iterator it =
        _observers.begin();
      stateStream << *it;
      ++it;
      for (; it != _observers.end(); ++it) stateStream << "," << *it;
    }
    stateStream << "\"";
    return stateStream.str();
  }
#endif

  IndexedColormap::IndexedColormap(int start, int end)
    : Observable(), _observers(), _start(start), _end(end), _gamma(1.0),
    _startColor(0, 0, 0), _endColor(255, 255, 255),
    _monochromeColor(255, 255, 255), _isRandom(false),
    _intermediateColors(), _overrideColors(), _fixedRandomColors(65536),
    _additionalRandomColors(), _rangeIndicator(false),
    _gammaLUT(65536) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<IndexedColormap@" << this << "::IndexedColormap start=\""
      << start << "\" end=\"" << end << "\" " << printState() << ">"
      << std::endl;
#endif
    for (size_t i = 0; i < _fixedRandomColors.size(); ++i)
      _fixedRandomColors[i] = generateRandomColor();
    _updateGammaLUT();
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</IndexedColormap@" << this << "::IndexedColormap>"
      << std::endl;
#endif
  }

  IndexedColormap::IndexedColormap(IndexedColormap const &colorMap)
    : Observable(), _observers(), _start(colorMap._start),
    _end(colorMap._end), _gamma(colorMap._gamma),
    _startColor(colorMap._startColor),
    _endColor(colorMap._endColor),
    _monochromeColor(colorMap._monochromeColor),
    _isRandom(colorMap._isRandom),
    _intermediateColors(colorMap._intermediateColors),
    _overrideColors(colorMap._overrideColors),
    _fixedRandomColors(colorMap._fixedRandomColors),
    _additionalRandomColors(colorMap._additionalRandomColors),
    _rangeIndicator(colorMap._rangeIndicator),
    _gammaLUT(colorMap._gammaLUT) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<IndexedColormap@" << this
      << "::IndexedColormap colorMap=\"" << &colorMap
      << "\" " << printState() << " />" << std::endl;
#endif
  }

  IndexedColormap::~IndexedColormap() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<IndexedColormap@" << this << "::~IndexedColormap "
      << printState() << ">" << std::endl;
    while (_observers.size() > 0) delete *_observers.begin();
    std::cerr << "</IndexedColormap" << this << "::~IndexedColormap>"
      << std::endl;
#endif
  }

  IndexedColormap &IndexedColormap::operator=(IndexedColormap const &colorMap) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<IndexedColormap@" << this
      << "::assignOperator colorMap=\"" << &colorMap << "\" "
      << printState() << ">" << std::endl;
#endif
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
    _rangeIndicator = colorMap._rangeIndicator;
    _gammaLUT = colorMap._gammaLUT;
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateColormap();
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</IndexedColormap@" << this << "::assignOperator>"
      << std::endl;
#endif
    return *this;
  }

  void IndexedColormap::reset() {
    _start = 0;
    _end = 65535;
    _gamma = 1.0;
    _startColor = ColorT(0, 0, 0);
    _endColor = ColorT(255, 255, 255);
    _monochromeColor = _endColor;
    _isRandom = false;
    _intermediateColors.clear();
    _overrideColors.clear();
    _additionalRandomColors.clear();
    _rangeIndicator = false;
    _updateGammaLUT();
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateColormap();
  }

  bool IndexedColormap::isUnsigned8BitRange() const {
    return _start == std::numeric_limits<unsigned char>::min() &&
      _end == std::numeric_limits<unsigned char>::max();
  }

  void IndexedColormap::setUnsigned8BitRange() {
    if (isUnsigned8BitRange()) return;
    _start = std::numeric_limits<unsigned char>::min();
    _end = std::numeric_limits<unsigned char>::max();
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIndexRange();
  }

  bool IndexedColormap::isSigned8BitRange() const {
    return _start == std::numeric_limits<char>::min() &&
      _end == std::numeric_limits<char>::max();
  }

  void IndexedColormap::setSigned8BitRange() {
    if (isSigned8BitRange()) return;
    _start = std::numeric_limits<char>::min();
    _end = std::numeric_limits<char>::max();
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIndexRange();
  }

  bool IndexedColormap::isUnsigned12BitRange() const {
    return _start == 0 && _end == 4095;
  }

  void IndexedColormap::setUnsigned12BitRange() {
    if (isUnsigned12BitRange()) return;
    _start = 0;
    _end = 4095;
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIndexRange();
  }

  bool IndexedColormap::isUnsigned16BitRange() const {
    return _start == std::numeric_limits<unsigned short>::min() &&
      _end == std::numeric_limits<unsigned short>::max();
  }

  void IndexedColormap::setUnsigned16BitRange() {
    if (isUnsigned16BitRange()) return;
    _start = std::numeric_limits<unsigned short>::min();
    _end = std::numeric_limits<unsigned short>::max();
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIndexRange();
  }

  bool IndexedColormap::isSigned16BitRange() const {
    return _start == std::numeric_limits<short>::min() &&
      _end == std::numeric_limits<short>::max();
  }

  void IndexedColormap::setSigned16BitRange() {
    if (isSigned16BitRange()) return;
    _start = std::numeric_limits<short>::min();
    _end = std::numeric_limits<short>::max();
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIndexRange();
  }

  void IndexedColormap::setIndexRange(int startIndex, int endIndex) {
    if ((startIndex == _start && endIndex == _end) || startIndex >= endIndex)
      return;
    _start = startIndex;
    _end = endIndex;
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIndexRange();
  }

  int IndexedColormap::setStartIndex(int index) {
    if (_start == index || index >= _end) return _start;
    _start = index;
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIndexRange();
    return _start;
  }

  int IndexedColormap::startIndex() const {
    return _start;
  }

  int IndexedColormap::setEndIndex(int index) {
    if (_end == index || index <= _start) return _end;
    _end = index;
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIndexRange();
    return _end;
  }

  int IndexedColormap::endIndex() const {
    return _end;
  }

  void IndexedColormap::setStartColor(ColorT const &color) {
    if (blitz::all(_startColor == color)) return;
    _startColor = color;
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateStartColor();
  }

  IndexedColormap::ColorT IndexedColormap::startColor() const {
    return _startColor;
  }

  void IndexedColormap::setEndColor(ColorT const &color) {
    if (blitz::all(_endColor == color)) return;
    _endColor = color;
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateEndColor();
  }

  IndexedColormap::ColorT IndexedColormap::endColor() const {
    return _endColor;
  }

  void IndexedColormap::setIntermediateColor(
    double position, ColorT const &color) {
    if (position <= 0.0 || position >= 1.0) return;
    std::map<double, ColorT>::iterator it = _intermediateColors.find(position);
    if (it != _intermediateColors.end()) {
      if (blitz::all(it->second == color)) return;
      it->second = color;
    }
    else _intermediateColors[position] = color;
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateIntermediateColor(position);
  }

  double IndexedColormap::moveIntermediateColor(
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
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateMoveIntermediateColor(oldPosition, newPosition);
    return newPosition;
  }

  void IndexedColormap::removeIntermediateColor(double position) {
    std::map<double, ColorT>::iterator it = _intermediateColors.find(position);
    if (it == _intermediateColors.end()) return;
    _intermediateColors.erase(it);
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateRemoveIntermediateColor(position);
  }

  void IndexedColormap::clearIntermediateColors() {
    if (_intermediateColors.size() == 0) return;
    _intermediateColors.clear();
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateClearIntermediateColors();
  }

  std::map<double, IndexedColormap::ColorT> const
    &IndexedColormap::intermediateColors() const {
    return _intermediateColors;
  }

  void IndexedColormap::setOverrideColor(int index, ColorT const &color) {
    std::map<int, ColorT>::iterator it = _overrideColors.find(index);
    if (it != _overrideColors.end()) {
      if (blitz::all(it->second == color)) return;
      it->second = color;
    }
    else _overrideColors[index] = color;
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateOverrideColor(index);
  }

  int IndexedColormap::moveOverrideColor(int oldIndex, int newIndex) {
    std::map<int, ColorT>::iterator itOld =
      _overrideColors.find(oldIndex);
    if (itOld == _overrideColors.end()) return oldIndex;
    std::map<int, ColorT>::iterator itNew =
      _overrideColors.find(newIndex);
    if (itNew != _overrideColors.end()) return oldIndex;
    _overrideColors[newIndex] = itOld->second;
    _overrideColors.erase(itOld);
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateMoveOverrideColor(oldIndex, newIndex);
    return newIndex;
  }

  void IndexedColormap::removeOverrideColor(int index) {
    std::map<int, ColorT>::iterator it = _overrideColors.find(index);
    if (it == _overrideColors.end()) return;
    _overrideColors.erase(it);
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateRemoveOverrideColor(index);
  }

  void IndexedColormap::clearOverrideColors() {
    if (_overrideColors.size() == 0) return;
    _overrideColors.clear();
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateClearOverrideColors();
  }

  std::map<int, IndexedColormap::ColorT> const
    &IndexedColormap::overrideColors() const {
    return _overrideColors;
  }

  bool IndexedColormap::isGray() const {
    return !isRandom() && _intermediateColors.size() == 0 &&
      blitz::all(_startColor == 0 && _endColor == 255);
  }

  void IndexedColormap::setGray() {
    if (isGray()) return;
    _isRandom = false;
    _additionalRandomColors.clear();
    _intermediateColors.clear();
    _startColor = ColorT(0, 0, 0);
    _endColor = ColorT(255, 255, 255);
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateColormap();
  }

  bool IndexedColormap::isHot() const {
    return !isRandom() && blitz::all(_startColor == 0) &&
      _intermediateColors.size() == 2 &&
      _intermediateColors.find(0.333) != _intermediateColors.end() &&
      blitz::all(
        _intermediateColors.find(0.333)->second == ColorT(255, 0, 0)) &&
      _intermediateColors.find(0.667) != _intermediateColors.end() &&
      blitz::all(
        _intermediateColors.find(0.667)->second == ColorT(255, 255, 0)) &&
      blitz::all(_endColor == 255);
  }

  void IndexedColormap::setHot() {
    if (isHot()) return;
    _isRandom = false;
    _additionalRandomColors.clear();
    _intermediateColors.clear();
    _startColor = ColorT(0, 0, 0);
    _intermediateColors[0.333] = ColorT(255, 0, 0);
    _intermediateColors[0.667] = ColorT(255, 255, 0);
    _endColor = ColorT(255);
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateColormap();
  }

  bool IndexedColormap::isJet() const {
    return !isRandom() && blitz::all(_startColor == ColorT(0, 0, 127)) &&
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

  void IndexedColormap::setJet() {
    if (isJet()) return;
    _isRandom = false;
    _additionalRandomColors.clear();
    _intermediateColors.clear();
    _startColor = ColorT(0, 0, 127);
    _intermediateColors[0.125] = ColorT(0, 0, 255);
    _intermediateColors[0.375] = ColorT(0, 255, 255);
    _intermediateColors[0.625] = ColorT(255, 255, 0);
    _intermediateColors[0.875] = ColorT(255, 0, 0);
    _endColor = ColorT(127, 0, 0);
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateColormap();
  }

  bool IndexedColormap::isHSV() const {
    return !isRandom() && blitz::all(
      _startColor == ColorT(255, 0, 0)) &&
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

  void IndexedColormap::setHSV() {
    if (isHSV()) return;
    _isRandom = false;
    _additionalRandomColors.clear();
    _intermediateColors.clear();
    _startColor = ColorT(255, 0, 0);
    _intermediateColors[0.167] = ColorT(255, 255, 0);
    _intermediateColors[0.333] = ColorT(0, 255, 0);
    _intermediateColors[0.5] = ColorT(0, 255, 255);
    _intermediateColors[0.667] = ColorT(0, 0, 255);
    _intermediateColors[0.833] = ColorT(255, 0, 255);
    _endColor = ColorT(255, 0, 0);
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateColormap();
  }

  bool IndexedColormap::isRandom() const {
    return _isRandom;
  }

  void IndexedColormap::setRandom() {
    if (isRandom()) return;
    _isRandom = true;
    _intermediateColors.clear();
    _startColor = ColorT(0, 0, 0);
    _endColor = ColorT(255, 255, 255);
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateColormap();
  }

  void IndexedColormap::setMonochromeColor(ColorT const &color) {
    if (blitz::all(_monochromeColor == color)) return;
    _monochromeColor = color;
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateMonochromeColor();
  }

  IndexedColormap::ColorT IndexedColormap::monochromeColor() const {
    return _monochromeColor;
  }

  void IndexedColormap::setRangeIndicatorEnabled(bool enable) {
    if (_rangeIndicator == enable) return;
    _rangeIndicator = enable;
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateRangeIndicator();
  }

  bool IndexedColormap::rangeIndicatorEnabled() const {
    return _rangeIndicator;
  }

  void IndexedColormap::setGamma(double gamma) {
    if (_gamma == gamma) return;
    _gamma = gamma;
    _updateGammaLUT();
    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateGamma();
  }

  double IndexedColormap::gamma() const {
    return _gamma;
  }

  IndexedColormap::Type IndexedColormap::type() const {
    return (_intermediateColors.size() == 0 &&
      blitz::all(_startColor == 0 && _endColor == 255)) ?
      Monochrome : RGB;
  }

  IndexedColormap::ColorT IndexedColormap::color(int index) const {
    // Out of bounds?
    if (index < _start) return color(_start);
    if (index > _end) return color(_end);

    // Range indicator?
    if (_rangeIndicator) {
      if (index == _start) return ColorT(0, 0, 255);
      if (index == _end) return ColorT(255, 0, 0);
    }

    // Check whether the index has an explicit override
    std::map<int, ColorT>::const_iterator itO = _overrideColors.find(index);
    if (itO != _overrideColors.end()) return itO->second;

    // Is the colormap set to random? If so check random color table and on
    // demand create new entry
    if (isRandom()) {
      if (index >= 0 && index < static_cast<int>(_fixedRandomColors.size()))
        return _fixedRandomColors[index];

      ColorT color;
#ifdef _OPENMP
#pragma omp critical
#endif
      {
        std::map<int, ColorT>::iterator it = _additionalRandomColors.find(index);
        if (it != _additionalRandomColors.end()) color = it->second;
        else {
          color = generateRandomColor();
          _additionalRandomColors[index] = color;
        }
      }
      return color;
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
        int uBoundIdx = static_cast<int>(
          it->first * static_cast<double>(_end - _start) + 0.5) + _start;
        lambda = std::min(
          1.0, std::max(
            0.0, static_cast<double>(index - _start) /
            static_cast<double>(uBoundIdx - _start)));
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
        int lBoundIdx = static_cast<int>(
          it->first * static_cast<double>(_end - _start) + 0.5) + _start;
        lambda = std::min(
          1.0, std::max(
            0.0, static_cast<double>(index - lBoundIdx) /
            static_cast<double>(_end - lBoundIdx)));
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
        int lBoundIdx = static_cast<int>(
          it->first * static_cast<double>(_end - _start) + 0.5) + _start;
        int uBoundIdx = static_cast<int>(
          it2->first * static_cast<double>(_end - _start) + 0.5) + _start;
        lambda = std::min(
          1.0, std::max(
            0.0, static_cast<double>(index - lBoundIdx) /
            static_cast<double>(uBoundIdx - lBoundIdx)));
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

  void IndexedColormap::load(
    BlitzH5File const &infile, std::string const &group) {
    std::string id;
    infile.readAttribute(id, "id", group);
    if (id != "IndexedColormap")
      throw BlitzH5Error("'IndexedColormap' id not found");
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
    _overrideColors.clear();
    try {
      infile.readDataset(indices, group + "/overrideIndices");
      infile.readDataset(colors, group + "/overrideColors");
      for (size_t i = 0; i < indices.size(); ++i)
        _overrideColors[indices(i)] = ColorT(255.0f * colors(i));
    }
    catch (BlitzH5Error &) {
    }
    int range;
    infile.readAttribute(range, "rangeIndicatorEnabled", group);
    _rangeIndicator = (range == 1);
    try {
      int random;
      infile.readAttribute(random, "random", group);
      if (random == 1) setRandom();
    }
    catch (BlitzH5Error &) {
    }

    for (std::set<IndexedColormapObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateColormap();
  }

  void IndexedColormap::save(
    BlitzH5File &outfile, std::string const &group) const {
    std::string id = "IndexedColormap";
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
    int random = (isRandom()) ? 1 : 0;
    outfile.writeAttribute(random, "random", group);
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
    if (_overrideColors.size() != 0) {
      blitz::Array<int, 1> indices(_overrideColors.size());
      blitz::Array<blitz::TinyVector<float, 3>, 1> colors(
        _overrideColors.size());
      ptrdiff_t i = 0;
      for (std::map<int, ColorT>::const_iterator
        it = _overrideColors.begin(); it != _overrideColors.end();
        ++it, ++i) {
        indices(i) = it->first;
        colors(i) = blitz::TinyVector<float, 3>(it->second) / 255.0f;
      }
      outfile.writeDataset(indices, group + "/overrideIndices");
      outfile.writeDataset(colors, group + "/overrideColors");
    }
    int range = (_rangeIndicator) ? 1 : 0;
    outfile.writeAttribute(range, "rangeIndicatorEnabled", group);
  }

  IndexedColormap::ColorT IndexedColormap::generateRandomColor() {
    return ColorT(
      255.0f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
      255.0f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
      255.0f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
  }

  void IndexedColormap::_updateGammaLUT() {
    for (int i = 0; i < static_cast<int>(_gammaLUT.size()); ++i)
      _gammaLUT[i] = std::pow(
        static_cast<double>(i) / static_cast<double>(_gammaLUT.size() - 1),
        1.0 / _gamma);
  }

}
