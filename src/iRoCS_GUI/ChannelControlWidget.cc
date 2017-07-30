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

#include "ChannelControlWidget.hh"

#include <QtGui/QFormLayout>
#include <QtGui/QLabel>

#include <libMultiChannelModel/Channel.hh>
#include <libMultiChannelModel/FloatChannel.hh>
#include "FloatChannelControlWidget.hh"
 // #include "IndexedChannelControlWidget.hh"
 // #include "RGBChannelControlWidget.hh"
 // #include "AnnotationChannelControlWidget.hh"

#include "MultichannelModelControlWidgetStack.hh"

#include "StringControlElement.hh"
#include "DoubleControlElement.hh"
#include "BoolControlElement.hh"
#include "Double3ControlElement.hh"
#include "Double4x4ControlElement.hh"

#include <stdexcept>

#ifdef HAVE_BLITZ_V9
#include <libArrayToolbox/TinyMatrixOperators.hh>
#endif

namespace iRoCS {

  ChannelControlWidget::ChannelControlWidget(
    Channel &channel, MultichannelModelControlWidgetStack *controlStack,
    bool advancedMode)
    : QWidget(), ChannelObserver(channel), _advancedMode(advancedMode),
    p_nameControl(NULL), p_filenameControl(NULL), p_alphaControl(NULL),
    p_visibleControl(NULL), p_modifiedControl(NULL),
    p_elementSizeUmControl(NULL), p_transformationControl(NULL) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<ChannelControlWidget@" << this
      << "::ChannelControlWidget channel=\"" << &channel
      << "\" controlStack=\"" << controlStack << "\" advancedMode=\""
      << advancedMode << "\">" << std::endl;
#endif
    setAttribute(Qt::WA_DeleteOnClose);

    QFormLayout *layout = new QFormLayout(this);
    layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    p_nameControl = new StringControlElement(tr("Name"), _channel.name());
    connect(p_nameControl, SIGNAL(valueChanged(std::string const &)),
      SLOT(setName(std::string const &)));
    layout->addRow(p_nameControl->label(), p_nameControl->controlWidget());

    p_filenameControl = new StringControlElement(
      tr("Filename"), _channel.filename());
    p_filenameControl->setActive(false);
    if (_advancedMode)
      layout->addRow(
        p_filenameControl->label(), p_filenameControl->controlWidget());

    p_alphaControl = new DoubleControlElement(tr("Alpha"), _channel.alpha());
    p_alphaControl->setRange(0.0, 1.0);
    p_alphaControl->setDecimals(2);
    p_alphaControl->setSingleStep(0.01);
    p_alphaControl->setSliderVisible(true);
    connect(p_alphaControl, SIGNAL(valueChanged(double)),
      SLOT(setAlpha(double)));
    layout->addRow(p_alphaControl->label(), p_alphaControl->controlWidget());

    p_visibleControl = new BoolControlElement(
      tr("Visible"), _channel.visible());
    connect(p_visibleControl, SIGNAL(valueChanged(bool)),
      SLOT(setVisibility(bool)));
    layout->addRow(
      p_visibleControl->label(), p_visibleControl->controlWidget());

    p_modifiedControl = new BoolControlElement(
      tr("Modified"), _channel.modified());
    p_modifiedControl->setActive(false);
    if (_advancedMode)
      layout->addRow(
        p_modifiedControl->label(), p_modifiedControl->controlWidget());

    p_elementSizeUmControl = new Double3ControlElement(
      tr("Element size [µm]"), _channel.elementSizeUm());
    connect(p_elementSizeUmControl,
      SIGNAL(valueChanged(blitz::TinyVector<double, 3> const &)),
      SLOT(setElementSizeUm(blitz::TinyVector<double, 3> const &)));
    if (_advancedMode)
      layout->addRow(
        p_elementSizeUmControl->label(),
        p_elementSizeUmControl->controlWidget());

    p_transformationControl = new Double4x4ControlElement(
      tr("Transformation"), _channel.transformation());
    connect(p_transformationControl,
      SIGNAL(valueChanged(blitz::TinyMatrix<double, 4, 4> const &)),
      SLOT(setTransformation(blitz::TinyMatrix<double, 4, 4> const &)));
    if (_advancedMode)
      layout->addRow(
        p_transformationControl->label(),
        p_transformationControl->controlWidget());

    if (controlStack != NULL) controlStack->addWidget(this);
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</ChannelControlWidget@" << this
      << "::ChannelControlWidget>" << std::endl;
#endif
  }

  ChannelControlWidget::~ChannelControlWidget() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<ChannelControlWidget@" << this
      << "::~ChannelControlWidget>" << std::endl;
#endif
    delete p_nameControl;
    delete p_filenameControl;
    delete p_alphaControl;
    delete p_visibleControl;
    delete p_modifiedControl;
    delete p_elementSizeUmControl;
    delete p_transformationControl;
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</ChannelControlWidget@" << this
      << "::~ChannelControlWidget>" << std::endl;
#endif
  }

  bool ChannelControlWidget::advancedModeEnabled() const {
    return _advancedMode;
  }

  void ChannelControlWidget::updateName() {
    if (p_nameControl->value() == _channel.name()) return;
    p_nameControl->setValue(_channel.name());
  }

  void ChannelControlWidget::updateFilename() {
    if (p_filenameControl->value() == _channel.filename()) return;
    p_filenameControl->setValue(_channel.filename());
  }

  void ChannelControlWidget::updateAlpha() {
    if (p_alphaControl->value() == _channel.alpha()) return;
    p_alphaControl->setValue(_channel.alpha());
  }

  void ChannelControlWidget::updateVisibilityFlag() {
    if (p_visibleControl->value() == _channel.visible()) return;
    p_visibleControl->setValue(_channel.visible());
  }

  void ChannelControlWidget::updateModifiedFlag() {
    if (p_modifiedControl->value() == _channel.modified()) return;
    p_modifiedControl->setValue(_channel.modified());
  }

  void ChannelControlWidget::updateElementSizeUm() {
    if (blitz::all(p_elementSizeUmControl->value() ==
      _channel.elementSizeUm())) return;
    p_elementSizeUmControl->setValue(_channel.elementSizeUm());
  }

  void ChannelControlWidget::updateTransformation() {
    if (blitz::all(p_transformationControl->value() ==
      _channel.transformation())) return;
    p_transformationControl->setValue(_channel.transformation());
  }

  ChannelControlWidget *ChannelControlWidget::instance(
    Channel &channel, MultichannelModelControlWidgetStack *controlStack,
    bool advancedMode) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<ChannelControlWidget::instance channel=\"" << &channel
      << "\" controlStack=\"" << controlStack << "\">" << std::endl;
#endif
    try {
      FloatChannelControlWidget *controlWidget =
        FloatChannelControlWidget::instance(
          dynamic_cast<FloatChannel&>(channel), controlStack, advancedMode);
#ifdef DEBUG_VERBOSE_XML
      std::cerr << "return " << controlWidget << std::endl;
      std::cerr << "</ChannelControlWidget::instance>" << std::endl;
#endif
      return controlWidget;
    }
    catch (std::bad_cast &) {
    }
    std::stringstream msg;
    msg << "Could not create a ChannelControlWidget for the given channel "
      << "with address " << &channel << ". The given Channel type has no "
      << "ControlWidget implementation.";
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "throw std::runtime_error \"" << msg.str() << "\""
      << std::endl;
    std::cerr << "</ChannelControlWidget::instance>" << std::endl;
#endif
    throw std::runtime_error(msg.str().c_str());
  }

  void ChannelControlWidget::setName(std::string const &name) const {
    if (_channel.name() != name) _channel.setName(name);
  }

  void ChannelControlWidget::setAlpha(double alpha) const {
    if (_channel.alpha() != alpha) _channel.setAlpha(alpha);
  }

  void ChannelControlWidget::setVisibility(bool visible) const {
    if (_channel.visible() != visible) _channel.setVisible(visible);
  }

  void ChannelControlWidget::setElementSizeUm(
    blitz::TinyVector<double, 3> const &elementSizeUm) const {
    if (blitz::any(_channel.elementSizeUm() != elementSizeUm))
      _channel.setElementSizeUm(elementSizeUm);
  }

  void ChannelControlWidget::setTransformation(
    blitz::TinyMatrix<double, 4, 4> const &transformation) const {
    if (blitz::any(_channel.transformation() != transformation))
      _channel.setTransformation(transformation);
  }

}
