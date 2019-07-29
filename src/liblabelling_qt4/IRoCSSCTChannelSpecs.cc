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

#include <QtGui/QFormLayout>

#include "IRoCSSCTChannelSpecs.hh"

#include "IRoCSSCTChannelSpecsOrthoViewRenderer.hh"
#include "IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer.hh"
#include "MultiChannelModel.hh"
#include "OrthoViewWidget.hh"

#include "ColorControlElement.hh"

IRoCSSCTChannelSpecs::IRoCSSCTChannelSpecs(
    ShellCoordinateTransform *sct, MultiChannelModel *model)
        : ChannelSpecs(model), p_sct(sct)
{
  p_colorControl = new ColorControlElement(
      tr("Color:"), blitz::TinyVector<unsigned char,3>(255, 255, 255));
  connect(p_colorControl, SIGNAL(valueChanged()), SLOT(updateChannelColor()));
  p_channelControlLayout->addRow(p_colorControl->labelWidget(),
                                 p_colorControl->controlWidget());

  // p_channelControlLayout->addStretch(1);
}

IRoCSSCTChannelSpecs::~IRoCSSCTChannelSpecs()
{
  delete p_sct;
}

ChannelSpecs::ChannelType IRoCSSCTChannelSpecs::channelType() const
{
  return ChannelSpecs::IRoCSSCT;
}

blitz::TinyVector<float,3> IRoCSSCTChannelSpecs::color() const
{
  return blitz::TinyVector<float,3>(p_colorControl->value()) / 255.0f;
}

float IRoCSSCTChannelSpecs::valueAt(
    blitz::TinyVector<double,3> const &positionUm) const
{
  return static_cast<float>(p_sct->getCoordinates(positionUm)(0));
}

std::string IRoCSSCTChannelSpecs::stringValueAt(
    blitz::TinyVector<double,3> const &positionUm) const
{
  blitz::TinyVector<double,3> p(p_sct->getCoordinates(positionUm));
  std::stringstream os;
  os << "(" << p(0) << "," << p(1) << "," << p(2) << ")";
  return os.str();
}

ChannelSpecsRenderer *IRoCSSCTChannelSpecs::addRenderer(ViewWidget *view)
{
  if (_renderers.find(view) != _renderers.end())
  {
    std::cerr << "IRoCSSCTChannelSpecs::addRenderer(): WARNING: The channel "
              << "already has an associated renderer" << std::endl;
    return _renderers[view];
  }
  switch (view->viewType())
  {
  case ViewWidget::OrthoView:
    _renderers[view] = new IRoCSSCTChannelSpecsOrthoViewRenderer(
        this, static_cast<OrthoViewWidget*>(view));
    break;
  case ViewWidget::OpenGlRenderingView:
    _renderers[view] = new IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer(
        this, reinterpret_cast<OpenGlRenderingViewWidget*>(view));
    break;
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Implementation missing"
              << std::endl;
    break;
  }
  return _renderers[view];
}

blitz::TinyVector<double,3> const &IRoCSSCTChannelSpecs::lowerBoundUm() const
{
  if (p_model != NULL) return p_model->lowerBoundUm();
  return this->_lowerBoundUm;
}

blitz::TinyVector<double,3> const &IRoCSSCTChannelSpecs::upperBoundUm() const
{
  if (p_model != NULL) return p_model->upperBoundUm();
  return this->_upperBoundUm;
}

blitz::TinyVector<double,3> const &IRoCSSCTChannelSpecs::elementSizeUm() const
{
  if (p_model != NULL) return p_model->elementSizeUm();
  return this->_elementSizeUm;
}

void IRoCSSCTChannelSpecs::load(
    std::string const &fileName, std::string const &group)
{
  if (p_sct == NULL) p_sct = new ShellCoordinateTransform();
  p_sct->load(fileName, group);
  if (p_model != NULL) p_model->setModified(true);
}

void IRoCSSCTChannelSpecs::save(
    std::string const &fileName, std::string const &group)
{
  p_sct->save(fileName, group);
}

ShellCoordinateTransform *IRoCSSCTChannelSpecs::sct()
{
  return p_sct;
}

void IRoCSSCTChannelSpecs::setColor(
    blitz::TinyVector<float,3> const &color)
{
  if (blitz::all(this->color() == color)) return;
  p_colorControl->setValue(blitz::TinyVector<unsigned char,3>(color * 255.0f));
  if (p_model != NULL) p_model->setModified(true);
}

void IRoCSSCTChannelSpecs::updateChannelColor()
{
  QPixmap icon(16, 16);
  icon.fill(QColor(
                static_cast<int>(color()(0) * 255),
                static_cast<int>(color()(1) * 255),
                static_cast<int>(color()(2) * 255)));
  setIcon(QIcon(icon));
  emitUpdateRequest();
}

QColor IRoCSSCTChannelSpecs::qColor()
{
  return QColor(
      static_cast<int>(color()(0) * 255),
      static_cast<int>(color()(1) * 255),
      static_cast<int>(color()(2) * 255),
      static_cast<int>(alpha() * 255));
}
