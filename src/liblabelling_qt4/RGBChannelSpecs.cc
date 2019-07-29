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

#include "RGBChannelSpecs.hh"

#include <QtGui/QFormLayout>

#include "DoubleControlElement.hh"
#include "BoolControlElement.hh"

#include "OrthoViewWidget.hh"
#include "RGBChannelSpecsOrthoViewRenderer.hh"
#include "RGBChannelSpecsOpenGlRenderingViewRenderer.hh"
#include "MultiChannelModel.hh"

// #include <blitz/tinyvec-et.h>

#include <iostream>

RGBChannelSpecs::RGBChannelSpecs(
    atb::Array<blitz::TinyVector<float,3>,3> *data, MultiChannelModel *model)
        : ChannelSpecs(model), p_data(data), _minValue(0.0f), _maxValue(1.0f)
{
  this->_lowerBoundUm = data->lowerBoundUm();
  this->_upperBoundUm = data->upperBoundUm();
  this->_elementSizeUm = data->elementSizeUm();
  this->_originalTransformation = data->transformation();
  setTransformation(this->_originalTransformation);
  _gammaLUT.resize(65536);

  p_gammaControl = new DoubleControlElement(tr("Gamma:"), 1.0);
  p_gammaControl->setSliderViewEnabled(true);
  p_gammaControl->setRange(0.01, 10.0);
  p_gammaControl->setSingleStep(0.01);
  connect(p_gammaControl, SIGNAL(valueChanged()), SLOT(updateGammaLUT()));
  p_channelControlLayout->addRow(p_gammaControl->labelWidget(),
                                 p_gammaControl->controlWidget());

  p_normalizeControl = new BoolControlElement(tr("Normalize:"), false);
  connect(p_normalizeControl, SIGNAL(valueChanged(bool)),
          SLOT(updateNorm(bool)));
  p_channelControlLayout->addRow(p_normalizeControl->labelWidget(),
                                 p_normalizeControl->controlWidget());

  p_showExposureProblemsControl = new BoolControlElement(
      tr("Show Exposure Problems:"), false);
  connect(p_showExposureProblemsControl, SIGNAL(valueChanged()),
          SLOT(emitUpdateRequest()));
  p_channelControlLayout->addRow(
      p_showExposureProblemsControl->labelWidget(),
      p_showExposureProblemsControl->controlWidget());

  // p_channelControlLayout->addStretch(1);
}

RGBChannelSpecs::~RGBChannelSpecs()
{
  delete p_data;
}

ChannelSpecs::ChannelType RGBChannelSpecs::channelType() const
{
  return ChannelSpecs::RGB;
}

float RGBChannelSpecs::gamma() const
{
  return static_cast<float>(p_gammaControl->value());
}

void RGBChannelSpecs::setGamma(float gamma)
{
  if (this->gamma() == gamma) return;
  p_gammaControl->setValue(static_cast<double>(gamma));
  if (p_model != NULL) p_model->setModified(true);
}

bool RGBChannelSpecs::normalize() const
{
  return p_normalizeControl->value();
}

void RGBChannelSpecs::setNormalize(bool normalize)
{
  if (this->normalize() == normalize) return;
  p_normalizeControl->setValue(normalize);
  if (p_model != NULL) p_model->setModified(true);
}

float RGBChannelSpecs::minValue() const
{
  return _minValue;
}

float RGBChannelSpecs::maxValue() const
{
  return _maxValue;
}

float RGBChannelSpecs::gammaLUT(size_t index) const
{
  if (index >= _gammaLUT.size()) return 1.0f;
  return _gammaLUT[index];
}

bool RGBChannelSpecs::showExposureProblems() const
{
  return p_showExposureProblemsControl->value();
}

void RGBChannelSpecs::setShowExposureProblems(bool exposure)
{
  if (this->showExposureProblems() == exposure) return;
  p_showExposureProblemsControl->setValue(exposure);
  if (p_model != NULL) p_model->setModified(true);
}

atb::Array<blitz::TinyVector<float,3>,3> const *RGBChannelSpecs::data() const
{
  return p_data;
}

atb::Array<blitz::TinyVector<float,3>,3> *RGBChannelSpecs::data()
{
  return p_data;
}

float RGBChannelSpecs::valueAt(
    blitz::TinyVector<double,3> const &positionUm) const
{
  blitz::TinyVector<float,3> value = p_data->valueAt(positionUm);
  return 0.3f * value(0) + 0.59f * value(1) + 0.11f * value(2);
}

std::string RGBChannelSpecs::stringValueAt(
    blitz::TinyVector<double,3> const &positionUm) const
{
  blitz::TinyVector<float,3> value = p_data->valueAt(positionUm);
  std::stringstream os;
  os << "(" << value(0) << "," << value(1) << "," << value(2) << ")";
  return os.str();
}

void RGBChannelSpecs::setTransformation(
    blitz::TinyMatrix<double,4,4> const &transformation)
{
  if (blitz::all(_transformation == transformation)) return;
  p_data->setTransformation(transformation);
  _lowerBoundUm = p_data->lowerBoundUm();
  _upperBoundUm = p_data->upperBoundUm();
  ChannelSpecs::setTransformation(transformation);
}

void RGBChannelSpecs::revalidate()
{
  this->_lowerBoundUm = p_data->lowerBoundUm();
  this->_upperBoundUm = p_data->upperBoundUm();
  this->_elementSizeUm = p_data->elementSizeUm();
  this->_originalTransformation = p_data->transformation();
  setTransformation(this->_originalTransformation);
  ChannelSpecs::revalidate();
}

ChannelSpecsRenderer *RGBChannelSpecs::addRenderer(ViewWidget *view)
{
  if (_renderers.find(view) != _renderers.end())
  {
    std::cout << "RGBChannelSpecs::addRenderer(): WARNING: The channel "
              << "already has an associated renderer" << std::endl;
    return _renderers[view];
  }
  switch (view->viewType())
  {
  case ViewWidget::OrthoView:
    _renderers[view] = new RGBChannelSpecsOrthoViewRenderer(
        this, static_cast<OrthoViewWidget*>(view));
    break;
  case ViewWidget::OpenGlRenderingView:
    _renderers[view] = new RGBChannelSpecsOpenGlRenderingViewRenderer(
        this, reinterpret_cast<OpenGlRenderingViewWidget*>(view));
    break;
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Implementation missing"
              << std::endl;
    break;
  }
  return _renderers[view];
}

void RGBChannelSpecs::updateGammaLUT()
{
  for (int i = 0; i < 65536; ++i)
      _gammaLUT[i] = static_cast<float>(
          pow(static_cast<double>(i) / 65535.0, 1.0 / this->gamma()));
  if (p_model != NULL) p_model->setModified(true);
  emitUpdateRequest();
}

void RGBChannelSpecs::updateNorm(bool normalize)
{
  if (normalize)
  {
    _minValue = std::numeric_limits<float>::infinity();
    _maxValue = -std::numeric_limits<float>::infinity();
    for (size_t i = 0; i < p_data->size(); ++i)
    {
      for (int d = 0; d < 3; ++d)
      {
        float value = p_data->data()[i](d);
        if (value < _minValue) _minValue = value;
        if (value > _maxValue) _maxValue = value;
      }
    }
  }
  emitUpdateRequest();
}
