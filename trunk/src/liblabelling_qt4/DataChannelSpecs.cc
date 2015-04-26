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

#include "DataChannelSpecs.hh"

#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>

#include "ColorControlElement.hh"
#include "DoubleControlElement.hh"
#include "BoolControlElement.hh"
#include "DoubleRangeControlElement.hh"

#include "OrthoViewWidget.hh"
#include "DataChannelSpecsOrthoViewRenderer.hh"
#include "DataChannelSpecsOpenGlRenderingViewRenderer.hh"
#include "MultiChannelModel.hh"

// #include <blitz/tinyvec-et.h>

#include <iostream>

DataChannelSpecs::DataChannelSpecs(
    atb::Array<float,3> *data, MultiChannelModel *model) 
        : ChannelSpecs(model), p_data(data)
{
  this->_lowerBoundUm = data->lowerBoundUm();
  this->_upperBoundUm = data->upperBoundUm();
  this->_elementSizeUm = data->elementSizeUm();
  this->_originalTransformation = data->transformation();
  setTransformation(this->_originalTransformation);
  _gammaLUT.resize(65536);

  p_colorControl = new ColorControlElement(
      tr("Color:"), blitz::TinyVector<unsigned char,3>(255, 255, 255));
  connect(p_colorControl, SIGNAL(valueChanged()), SLOT(updateIcon()));
  connect(p_colorControl, SIGNAL(valueChanged()), SLOT(emitUpdateRequest()));
  p_channelControlLayout->addWidget(p_colorControl);

  p_gammaControl = new DoubleControlElement(tr("Gamma:"), 1.0);
  p_gammaControl->setSliderViewEnabled(true);
  p_gammaControl->setRange(0.01, 10.0);
  p_gammaControl->setSingleStep(0.01);
  connect(p_gammaControl, SIGNAL(valueChanged()), SLOT(updateGammaLUT()));
  p_channelControlLayout->addWidget(p_gammaControl);

  QHBoxLayout *rangeLayout = new QHBoxLayout;
  p_displayRangeControl = new DoubleRangeControlElement(
      tr("Display Range"), 0.0, 1.0);
  connect(p_displayRangeControl, SIGNAL(valueChanged()),
          SLOT(emitUpdateRequest()));
  rangeLayout->addWidget(p_displayRangeControl);
  p_normalizeButton = new QPushButton(tr("Normalize"));
  connect(p_normalizeButton, SIGNAL(clicked()), SLOT(normalize()));
  rangeLayout->addWidget(p_normalizeButton);

  p_channelControlLayout->addLayout(rangeLayout);

  p_showExposureProblemsControl = new BoolControlElement(
      tr("Show Exposure Problems:"), false);
  connect(p_showExposureProblemsControl, SIGNAL(valueChanged()),
          SLOT(emitUpdateRequest()));
  p_channelControlLayout->addWidget(p_showExposureProblemsControl);

  p_channelControlLayout->addStretch(1);

  updateIcon();
}

DataChannelSpecs::~DataChannelSpecs()
{
  delete p_data;
}

ChannelSpecs::ChannelType DataChannelSpecs::channelType() const
{
  return ChannelSpecs::Data;
}

blitz::TinyVector<float,3> DataChannelSpecs::color() const 
{
  return blitz::TinyVector<float,3>(p_colorControl->value()) / 255.0f;
}

void DataChannelSpecs::setColor(blitz::TinyVector<float,3> const &color) 
{
  if (blitz::all(this->color() == color)) return;
  p_colorControl->setValue(blitz::TinyVector<unsigned char,3>(color * 255.0f));
  if (p_model != NULL) p_model->setModified(true);
}

float DataChannelSpecs::gamma() const
{ 
  return static_cast<float>(p_gammaControl->value());
}

void DataChannelSpecs::setGamma(float gamma) 
{
  if (this->gamma() == gamma) return;
  p_gammaControl->setValue(static_cast<double>(gamma));
  if (p_model != NULL) p_model->setModified(true);
}

float DataChannelSpecs::displayMin() const
{
  return static_cast<float>(p_displayRangeControl->lowerBound());
}

float DataChannelSpecs::displayMax() const
{
  return static_cast<float>(p_displayRangeControl->upperBound());  
}

float DataChannelSpecs::gammaLUT(size_t index) const
{
  if (index >= _gammaLUT.size()) return 1.0f;
  return _gammaLUT[index];
}

bool DataChannelSpecs::showExposureProblems() const 
{
  return p_showExposureProblemsControl->value();
}

void DataChannelSpecs::setShowExposureProblems(bool exposure) 
{
  if (this->showExposureProblems() == exposure) return;
  p_showExposureProblemsControl->setValue(exposure);
  if (p_model != NULL) p_model->setModified(true);
}

atb::Array<float,3> const *DataChannelSpecs::data() const
{
  return p_data;
}

atb::Array<float,3> *DataChannelSpecs::data() 
{
  return p_data;
}

float DataChannelSpecs::valueAt(
    blitz::TinyVector<double,3> const &positionUm) const 
{
  return p_data->valueAt(positionUm);
}
  
std::string DataChannelSpecs::stringValueAt(
    blitz::TinyVector<double,3> const &positionUm) const
{
  std::stringstream os;
  os << p_data->valueAt(positionUm);
  return os.str();
}

void DataChannelSpecs::setTransformation(
    blitz::TinyMatrix<double,4,4> const &transformation)
{
  if (blitz::all(_transformation == transformation)) return;
  p_data->setTransformation(transformation);
  _lowerBoundUm = p_data->lowerBoundUm();
  _upperBoundUm = p_data->upperBoundUm();
  ChannelSpecs::setTransformation(transformation);
}

void DataChannelSpecs::revalidate()
{
  this->_lowerBoundUm = p_data->lowerBoundUm();
  this->_upperBoundUm = p_data->upperBoundUm();
  this->_elementSizeUm = p_data->elementSizeUm();
  this->_originalTransformation = p_data->transformation();
  setTransformation(this->_originalTransformation);
  ChannelSpecs::revalidate();
}

ChannelSpecsRenderer *DataChannelSpecs::addRenderer(ViewWidget *view)
{
  if (_renderers.find(view) != _renderers.end())
  {
    std::cout << "DataChannelSpecs::addRenderer(): WARNING: The channel "
              << "already has an associated renderer" << std::endl;
    return _renderers[view];
  }
  switch (view->viewType())
  {
  case ViewWidget::OrthoView:
    _renderers[view] = new DataChannelSpecsOrthoViewRenderer(
        this, static_cast<OrthoViewWidget*>(view));
    break;
  case ViewWidget::OpenGlRenderingView:
    _renderers[view] = new DataChannelSpecsOpenGlRenderingViewRenderer(
        this, reinterpret_cast<OpenGlRenderingViewWidget*>(view));
    break;
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Implementation missing"
              << std::endl;
    break;
  }
  return _renderers[view];
}

void DataChannelSpecs::updateIcon()
{
  QPixmap icon(16, 16);
  icon.fill(QColor(
                static_cast<int>(color()(0) * 255),
                static_cast<int>(color()(1) * 255),
                static_cast<int>(color()(2) * 255)));
  setIcon(QIcon(icon));
}

void DataChannelSpecs::updateGammaLUT() 
{
  for (int i = 0; i < 65536; ++i) 
      _gammaLUT[i] = static_cast<float>(
          pow(static_cast<double>(i) / 65535.0, 1.0 / this->gamma()));
  if (p_model != NULL) p_model->setModified(true);
  emitUpdateRequest();
}

void DataChannelSpecs::normalize() 
{
  p_displayRangeControl->setValue(
      static_cast<double>(blitz::min(*p_data)),
      static_cast<double>(blitz::max(*p_data)));
  if (p_model != NULL) p_model->setModified(true);
}

void DataChannelSpecs::setDisplayRange(float displayMin, float displayMax)
{
  p_displayRangeControl->setValue(static_cast<double>(displayMin),
                                  static_cast<double>(displayMax));
  if (p_model != NULL) p_model->setModified(true);
}

void DataChannelSpecs::setDisplayMin(float displayMin)
{
  p_displayRangeControl->setLowerBound(static_cast<double>(displayMin));
  if (p_model != NULL) p_model->setModified(true);
}

void DataChannelSpecs::setDisplayMax(float displayMax)
{
  p_displayRangeControl->setUpperBound(static_cast<double>(displayMax));
  if (p_model != NULL) p_model->setModified(true);
}

