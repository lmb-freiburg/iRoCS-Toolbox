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

#include "VisualizationChannelSpecs.hh"

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QIcon>
#include <QtGui/QColorDialog>
#include <QtGui/QComboBox>
#include <QtGui/QVBoxLayout>

#include "MultiChannelModel.hh"
#include "OrthoViewWidget.hh"
#include "VisualizationChannelSpecsOrthoViewRenderer.hh"
#include "VisualizationChannelSpecsOpenGlRenderingViewRenderer.hh"
#include "ColorMap.hh"
#include "ColorMapEditorWidget.hh"

#include <iostream>
#include <stdexcept>

VisualizationChannelSpecs::VisualizationChannelSpecs(
  atb::Array<int,3> *data, MultiChannelModel *model, bool sign, int bitDepth)
        : ChannelSpecs(model), p_data(data), _fillValue(0), _sign(sign),
          _bitDepth(bitDepth), _dataChanged(false)
{
  this->_lowerBoundUm = data->lowerBoundUm();
  this->_upperBoundUm = data->upperBoundUm();
  this->_elementSizeUm = data->elementSizeUm();
  this->_originalTransformation = data->transformation();
  setTransformation(this->_originalTransformation);
  p_colorMap = new ColorMap(0, 1 << (_bitDepth - 1));

  QPushButton *normalizeButton = new QPushButton(tr("normalize"));
  connect(normalizeButton, SIGNAL(clicked()), SLOT(normalizeIndexRange()));
  p_channelControlLayout->addWidget(normalizeButton);

  p_colorMapEditor = new ColorMapEditorWidget(p_colorMap);
  p_colorMap->setColorMapEditor(p_colorMapEditor);
  p_channelControlLayout->addWidget(p_colorMapEditor);
  connect(p_colorMapEditor, SIGNAL(colorMapChanged()),
          SLOT(emitUpdateRequest()));
  connect(p_colorMapEditor, SIGNAL(colorMapChanged()), SLOT(updateIcon()));

  p_channelControlLayout->addStretch(1);

  updateIcon();
}

VisualizationChannelSpecs::~VisualizationChannelSpecs()
{
  delete p_data;
  p_colorMapEditor->setColorMap(NULL);
  delete p_colorMap;
}

ChannelSpecs::ChannelType VisualizationChannelSpecs::channelType() const
{
  return ChannelSpecs::Visualization;
}

void VisualizationChannelSpecs::setHasNewData()
{
  _hasNewData = true;
}

bool VisualizationChannelSpecs::dataChanged() const {
  return _dataChanged;
}

void VisualizationChannelSpecs::setDataChanged(bool dataChanged) {
  _dataChanged = dataChanged;
}

atb::Array<int,3> const *VisualizationChannelSpecs::data() const
{
  return p_data;
}

atb::Array<int,3> *VisualizationChannelSpecs::data()
{
  _dataChanged = true;
  return p_data;
}

float VisualizationChannelSpecs::valueAt(
    blitz::TinyVector<double,3> const &positionUm) const
{
  return static_cast<float>(p_data->valueAt(positionUm));
}

std::string VisualizationChannelSpecs::stringValueAt(
    blitz::TinyVector<double,3> const &positionUm) const
{
  std::stringstream os;
  os << p_data->valueAt(positionUm);
  return os.str();
}

int &VisualizationChannelSpecs::labelAt(
    blitz::TinyVector<double,3> const &positionUm)
{
  blitz::TinyVector<ptrdiff_t, 3> p =
      blitz::TinyVector<ptrdiff_t, 3>(
          positionUm / p_data->elementSizeUm() + 0.5);
  return labelAtVoxel(p);
}

int &VisualizationChannelSpecs::labelAtVoxel(
  blitz::TinyVector<ptrdiff_t,3> const &pos)
{
  if (blitz::all(pos >= 0) && blitz::all(pos < p_data->shape()))
      return (*p_data)(pos);
  else
      throw std::out_of_range(
          "VisualizationChannelSpecs: Called labelAtVoxel with position out of "
          "bounds");
}

void VisualizationChannelSpecs::setTransformation(
    blitz::TinyMatrix<double,4,4> const &transformation)
{
  if (blitz::all(_transformation == transformation)) return;
  p_data->setTransformation(transformation);
  _lowerBoundUm = p_data->lowerBoundUm();
  _upperBoundUm = p_data->upperBoundUm();
  ChannelSpecs::setTransformation(transformation);
}

void VisualizationChannelSpecs::revalidate()
{
  this->_lowerBoundUm = p_data->lowerBoundUm();
  this->_upperBoundUm = p_data->upperBoundUm();
  this->_elementSizeUm = p_data->elementSizeUm();
  this->_originalTransformation = p_data->transformation();
  setTransformation(this->_originalTransformation);
  ChannelSpecs::revalidate();
}

ChannelSpecsRenderer *VisualizationChannelSpecs::addRenderer(ViewWidget *view)
{
  if (_renderers.find(view) != _renderers.end())
  {
    std::cout << "VisualizationChannelSpecs::addRenderer(): WARNING: "
              << "The channel already has an associated renderer" << std::endl;
    return _renderers[view];
  }
  switch (view->viewType())
  {
  case ViewWidget::OrthoView:
    _renderers[view] = new VisualizationChannelSpecsOrthoViewRenderer(
        this, static_cast<OrthoViewWidget*>(view));
    break;
  case ViewWidget::OpenGlRenderingView:
    _renderers[view] = new VisualizationChannelSpecsOpenGlRenderingViewRenderer(
        this, reinterpret_cast<OpenGlRenderingViewWidget*>(view));
    break;
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Implementation missing"
              << std::endl;
    break;
  }
  return _renderers[view];
}

ColorMap const &VisualizationChannelSpecs::colorMap() const
{
  return *p_colorMap;
}

ColorMap &VisualizationChannelSpecs::colorMap()
{
  return *p_colorMap;
}

void VisualizationChannelSpecs::setFillValue(int value) {
  std::cout << "Setting fill value to " << value << std::endl;
  _fillValue = value;
}

void VisualizationChannelSpecs::pickFillValue(
    blitz::TinyVector<double,3> const &positionUm) {
  blitz::TinyVector<atb::BlitzIndexT,3> posPx{
    blitz::floor(
        atb::homogeneousToEuclidean(
            p_data->transformation() *
            atb::euclideanToHomogeneous(positionUm)) /
        p_data->elementSizeUm() + 0.5)};

  if (blitz::any(posPx < 0 || posPx >= p_data->shape())) return;

  setFillValue((*p_data)(posPx));
}

int VisualizationChannelSpecs::fillValue() const {
  return _fillValue;
}

void VisualizationChannelSpecs::floodFill(
    blitz::TinyVector<double,3> const &positionUm,
    bool mergeNeighboringSegments) {
  blitz::TinyVector<atb::BlitzIndexT,3> posPx{
    blitz::floor(
        atb::homogeneousToEuclidean(
            p_data->transformation() *
            atb::euclideanToHomogeneous(positionUm)) /
        p_data->elementSizeUm() + 0.5)};

  if (blitz::any(posPx < 0 or posPx >= p_data->shape()) or
      (*p_data)(posPx) == _fillValue ||
      blitz::all(p_colorMap->color((*p_data)(posPx)) == 0)) return;

  // In merge segment mode identify the boundary pixels to fill with _fillValue
  // and mark them with INT_MIN as "must be filled"
  if (mergeNeighboringSegments) {
    // First grassfire pass: Fill segment with INT_MIN and get boundary pixels
    PixelSet boundarySet{_grassfire(posPx, INT_MIN, true)};

    // Fill gap pixels with INT_MIN if a segment with label _fillValue touches
    // the boundary
    atb::Neighborhood<3> nh(atb::Neighborhood<3>::Complex);
    for (PixelSet::const_iterator it = boundarySet.begin();
         it != boundarySet.end(); ++it) {
      bool merge{false};
      int const value{(*p_data)(*it)};
      for (typename atb::Neighborhood<3>::const_iterator it2 = nh.begin();
           it2 != nh.end(); ++it2) {
        blitz::TinyVector<atb::BlitzIndexT,3> const nbPos{*it + *it2};
        int const nbValue{(*p_data)(nbPos)};
        if (nbValue == INT_MIN or nbValue == value) continue;
        if (nbValue == _fillValue) merge = true;
        else {
          merge = false;
          break;
        }
      }
      if (merge) (*p_data)(*it) = INT_MIN;
    }
  }

  // The actual flood fill
  _grassfire(posPx, _fillValue, false);

  _dataChanged = true;
  p_model->setModified(true);
  emitUpdateRequest();
}

void VisualizationChannelSpecs::setSign(bool sign)
{
  _sign = sign;
  if (_sign)
  {
    switch (_bitDepth)
    {
    case 8:
      p_colorMap->setSigned8BitRange();
      break;
    case 16:
      p_colorMap->setSigned16BitRange();
      break;
    case 32:
      p_colorMap->setSigned32BitRange();
      break;
    default:
      p_colorMap->setSigned64BitRange();
    }
  }
  else
  {
    switch (_bitDepth)
    {
    case 8:
      p_colorMap->setUnsigned8BitRange();
      break;
    case 12:
      p_colorMap->setUnsigned12BitRange();
      break;
    case 16:
      p_colorMap->setUnsigned16BitRange();
      break;
    default:
      p_colorMap->setUnsigned32BitRange();
    }
  }
}

bool VisualizationChannelSpecs::sign() const
{
  return _sign;
}

void VisualizationChannelSpecs::setBitDepth(int bitDepth)
{
  _bitDepth = bitDepth;
  if (_sign)
  {
    switch (_bitDepth)
    {
    case 8:
      p_colorMap->setSigned8BitRange();
      break;
    case 16:
      p_colorMap->setSigned16BitRange();
      break;
    case 32:
      p_colorMap->setSigned32BitRange();
      break;
    default:
      p_colorMap->setSigned64BitRange();
    }
  }
  else
  {
    switch (_bitDepth)
    {
    case 8:
      p_colorMap->setUnsigned8BitRange();
      break;
    case 12:
      p_colorMap->setUnsigned12BitRange();
      break;
    case 16:
      p_colorMap->setUnsigned16BitRange();
      break;
    default:
      p_colorMap->setUnsigned32BitRange();
    }
  }
}

int VisualizationChannelSpecs::bitDepth() const
{
  return _bitDepth;
}

void VisualizationChannelSpecs::updateIcon()
{
  QImage image(16, 16, QImage::Format_RGB32);
  for (int y = 0; y < 16; ++y)
  {
    for (int x = 0; x < 16; ++x)
    {
      blitz::TinyVector<float,3> color = p_colorMap->color(
          static_cast<long long>(
              static_cast<double>(x) / 15.0 *
              static_cast<double>(
                  p_colorMap->endIndex() - p_colorMap->startIndex()) + 0.5) +
          p_colorMap->startIndex());
      QRgb col =
          (static_cast<int>(255.0f * color(0)) << 16) +
          (static_cast<int>(255.0f * color(1)) << 8) +
          static_cast<int>(255.0f * color(2));
      image.setPixel(x, y, col);
    }
  }
  setIcon(QIcon(QPixmap::fromImage(image)));
}

void VisualizationChannelSpecs::normalizeIndexRange()
{
  p_colorMap->setStartIndex(blitz::min(*p_data));
  p_colorMap->setEndIndex(blitz::max(*p_data));
  emitUpdateRequest();
}

VisualizationChannelSpecs::PixelSet VisualizationChannelSpecs::_grassfire(
    blitz::TinyVector<atb::BlitzIndexT,3> const &posPx,
    int value, bool getBoundary) {
  int oldValue = (*p_data)(posPx);
  atb::Neighborhood<3> const nh{
    getBoundary ? atb::Neighborhood<3>::Complex : atb::Neighborhood<3>::Simple};
  PixelSet activeSet{};
  PixelSet boundarySet{};
  activeSet.insert(posPx);
  while (!activeSet.empty()) {
    blitz::TinyVector<atb::BlitzIndexT,3> p{*activeSet.begin()};
    activeSet.erase(activeSet.begin());
    (*p_data)(p) = value;
    for (typename atb::Neighborhood<3>::const_iterator it = nh.begin();
         it != nh.end(); ++it) {
      blitz::TinyVector<atb::BlitzIndexT,3> const nbPos{p + *it};
      if (blitz::all(nbPos >= 0) and blitz::all(nbPos < p_data->shape())) {
        if ((*p_data)(nbPos) == oldValue) activeSet.insert(nbPos);
        else if (getBoundary) boundarySet.insert(nbPos);
      }
    }
  }
  return boundarySet;
}
