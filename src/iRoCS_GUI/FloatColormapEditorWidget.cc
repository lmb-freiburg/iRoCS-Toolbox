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

#include "FloatColormapEditorWidget.hh"
#include <libMultiChannelModel/FloatColormap.hh>
#include "FloatColormapBarWidget.hh"
#include "ColorControlElement.hh"
#include "DoubleControlElement.hh"
#include "BoolControlElement.hh"

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QFormLayout>
#include <QtGui/QComboBox>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QToolButton>
#include <QtGui/QColorDialog>

namespace iRoCS {

  FloatColormapEditorWidget::FloatColormapEditorWidget(
    FloatColormap &colormap, QWidget *parent)
    : FloatColormapObserver(colormap), QWidget(parent),
    p_presetSelector(NULL), p_indexRangeSelector(NULL),
    p_normalizeButton(NULL), p_startIndexControl(NULL),
    p_endIndexControl(NULL), p_colorbar(NULL),
    p_monochromeColorControl(NULL), p_gammaControl(NULL),
    p_rangeIndicatorControl(NULL) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatColormapEditorWidget@" << this
      << "::FloatColormapEditorWidget colormap=\"" << &colormap
      << "\" parent=\"" << parent << "\">" << std::endl;
#endif
    QFormLayout *layout = new QFormLayout(this);

    QStringList presetStrings;
    presetStrings << tr("Gray") << tr("Hot") << tr("Jet") << tr("HSV")
      << tr("Custom");
    p_presetSelector = new QComboBox();
    p_presetSelector->addItems(presetStrings);
    _updatePresetSelector();
    connect(p_presetSelector, SIGNAL(currentIndexChanged(int)),
      SLOT(setPresetInColormap()));
    layout->addRow(tr("Preset"), p_presetSelector);

    QHBoxLayout *rangeLayout = new QHBoxLayout;
    QStringList indexRangeStrings;
    indexRangeStrings
      << tr("Unsigned 8-Bit") << tr("Signed 8-Bit") << tr("Unsigned 12-Bit")
      << tr("Unsigned 16-Bit") << tr("Signed 16-Bit")
      << tr("Custom");
    p_indexRangeSelector = new QComboBox();
    p_indexRangeSelector->addItems(indexRangeStrings);
    _updateIndexRangeSelector();
    connect(p_indexRangeSelector, SIGNAL(currentIndexChanged(int)),
      SLOT(setIndexRangeInColormap()));
    rangeLayout->addWidget(p_indexRangeSelector);
    p_normalizeButton = new QPushButton(tr("Normalize"));
    connect(p_normalizeButton, SIGNAL(clicked()), SIGNAL(normalizeRequest()));
    rangeLayout->addWidget(p_normalizeButton);
    layout->addRow(tr("Data type"), rangeLayout);

    QHBoxLayout *colorbarLayout = new QHBoxLayout;
    p_startIndexControl = new QDoubleSpinBox;
    p_startIndexControl->setMinimum(-std::numeric_limits<double>::infinity());
    p_startIndexControl->setMaximum(
        _colormap.endIndex() - std::numeric_limits<double>::epsilon());
    p_startIndexControl->setValue(_colormap.startIndex());
    p_startIndexControl->setSingleStep(0.1);
    p_startIndexControl->setDecimals(6);
    connect(p_startIndexControl, SIGNAL(valueChanged(double)),
      SLOT(setStartIndexInColormap()));
    colorbarLayout->addWidget(p_startIndexControl);

    p_startColorButton = new QToolButton;
    QPixmap startColorPixmap(15, 15);
    startColorPixmap.fill(
        QColor(_colormap.startColor()(0), _colormap.startColor()(1),
               _colormap.startColor()(2)));
    p_startColorButton->setIcon(QIcon(startColorPixmap));
    connect(p_startColorButton, SIGNAL(clicked()), SLOT(selectStartColor()));
    colorbarLayout->addWidget(p_startColorButton);

    p_colorbar = new FloatColormapBarWidget(_colormap);
    colorbarLayout->addWidget(p_colorbar);

    p_endColorButton = new QToolButton;
    QPixmap endColorPixmap(15, 15);
    endColorPixmap.fill(
        QColor(_colormap.endColor()(0), _colormap.endColor()(1),
               _colormap.endColor()(2)));
    p_endColorButton->setIcon(QIcon(endColorPixmap));
    connect(p_endColorButton, SIGNAL(clicked()), SLOT(selectEndColor()));
    colorbarLayout->addWidget(p_endColorButton);

    p_endIndexControl = new QDoubleSpinBox;
    p_endIndexControl->setMinimum(
        _colormap.startIndex() + std::numeric_limits<double>::epsilon());
    p_endIndexControl->setMaximum(std::numeric_limits<double>::infinity());
    p_endIndexControl->setValue(_colormap.endIndex());
    p_endIndexControl->setSingleStep(0.1);
    p_endIndexControl->setDecimals(6);
    connect(p_endIndexControl, SIGNAL(valueChanged(double)),
      SLOT(setEndIndexInColormap()));
    colorbarLayout->addWidget(p_endIndexControl);

    layout->addRow(colorbarLayout);

    p_monochromeColorControl = new ColorControlElement(tr("Monochrome Color"));
    p_monochromeColorControl->setValue(_colormap.monochromeColor());
    p_monochromeColorControl->controlWidget()->setEnabled(
      _colormap.type() == FloatColormap::Monochrome);
    connect(p_monochromeColorControl, SIGNAL(valueChanged()),
      SLOT(setMonochromeColorInColormap()));
    layout->addRow(p_monochromeColorControl->label(),
      p_monochromeColorControl->controlWidget());

    p_gammaControl = new DoubleControlElement(tr("Gamma"));
    p_gammaControl->setSliderVisible(true);
    p_gammaControl->setRange(0.001, 10.0);
    p_gammaControl->setValue(_colormap.gamma());
    p_gammaControl->setSingleStep(0.1);
    p_gammaControl->setDecimals(3);
    connect(p_gammaControl, SIGNAL(valueChanged()), SLOT(setGammaInColormap()));
    layout->addRow(p_gammaControl->label(), p_gammaControl->controlWidget());

    p_rangeIndicatorControl = new BoolControlElement(tr("Range Indicator"));
    p_rangeIndicatorControl->setValue(_colormap.rangeIndicatorEnabled());
    connect(p_rangeIndicatorControl, SIGNAL(valueChanged()),
      SLOT(setRangeIndicatorFlagInColormap()));
    layout->addRow(p_rangeIndicatorControl->label(),
      p_rangeIndicatorControl->controlWidget());
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</FloatColormapEditorWidget@" << this
      << "::FloatColormapEditorWidget>" << std::endl;
#endif
  }

  FloatColormapEditorWidget::~FloatColormapEditorWidget() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatColormapEditorWidget@" << this
      << "::~FloatColormapEditorWidget _colormap=\"" << &_colormap
      << "\" />" << std::endl;
#endif
  }

  void FloatColormapEditorWidget::updateIndexRange() {
    _updateIndexRangeSelector();
    disconnect(p_startIndexControl, SIGNAL(valueChanged(double)),
      this, SLOT(setStartIndexInColormap()));
    disconnect(p_endIndexControl, SIGNAL(valueChanged(double)),
      this, SLOT(setEndIndexInColormap()));
    p_startIndexControl->setMaximum(
      _colormap.endIndex() - std::numeric_limits<double>::epsilon());
    p_endIndexControl->setMinimum(
      _colormap.startIndex() + std::numeric_limits<double>::epsilon());
    if (p_startIndexControl->value() == _colormap.startIndex() &&
      p_endIndexControl->value() == _colormap.endIndex()) {
      connect(p_startIndexControl, SIGNAL(valueChanged(double)),
        SLOT(setStartIndexInColormap()));
      connect(p_endIndexControl, SIGNAL(valueChanged(double)),
        SLOT(setEndIndexInColormap()));
      return;
    }
    p_startIndexControl->setValue(_colormap.startIndex());
    p_endIndexControl->setValue(_colormap.endIndex());
    connect(p_startIndexControl, SIGNAL(valueChanged(double)),
      SLOT(setStartIndexInColormap()));
    connect(p_endIndexControl, SIGNAL(valueChanged(double)),
      SLOT(setEndIndexInColormap()));
  }

  void FloatColormapEditorWidget::updateStartColor() {
    p_monochromeColorControl->controlWidget()->setEnabled(
      _colormap.type() == FloatColormap::Monochrome);
    _updatePresetSelector();
    QPixmap startColorPixmap(15, 15);
    startColorPixmap.fill(
      QColor(_colormap.startColor()(0),
        _colormap.startColor()(1),
        _colormap.startColor()(2)));
    p_startColorButton->setIcon(QIcon(startColorPixmap));
  }

  void FloatColormapEditorWidget::updateEndColor() {
    p_monochromeColorControl->controlWidget()->setEnabled(
      _colormap.type() == FloatColormap::Monochrome);
    _updatePresetSelector();
    QPixmap endColorPixmap(15, 15);
    endColorPixmap.fill(
      QColor(_colormap.endColor()(0),
        _colormap.endColor()(1),
        _colormap.endColor()(2)));
    p_endColorButton->setIcon(QIcon(endColorPixmap));
  }

  void FloatColormapEditorWidget::updateIntermediateColor(double) {
    p_monochromeColorControl->controlWidget()->setEnabled(
      _colormap.type() == FloatColormap::Monochrome);
    _updatePresetSelector();
  }

  void FloatColormapEditorWidget::updateMoveIntermediateColor(double, double) {
    _updatePresetSelector();
  }

  void FloatColormapEditorWidget::updateRemoveIntermediateColor(double) {
    p_monochromeColorControl->controlWidget()->setEnabled(
      _colormap.type() == FloatColormap::Monochrome);
    _updatePresetSelector();
  }

  void FloatColormapEditorWidget::updateClearIntermediateColors() {
    p_monochromeColorControl->controlWidget()->setEnabled(
      _colormap.type() == FloatColormap::Monochrome);
    _updatePresetSelector();
  }

  void FloatColormapEditorWidget::updateMonochromeColor() {
    if (blitz::all(p_monochromeColorControl->value() ==
      _colormap.monochromeColor())) return;
    p_monochromeColorControl->setValue(_colormap.monochromeColor());
  }

  void FloatColormapEditorWidget::updateRangeIndicator() {
    if (_colormap.rangeIndicatorEnabled() ==
      p_rangeIndicatorControl->value()) return;
    p_rangeIndicatorControl->setValue(_colormap.rangeIndicatorEnabled());
  }

  void FloatColormapEditorWidget::updateGamma() {
    if (_colormap.gamma() == p_gammaControl->value()) return;
    p_gammaControl->setValue(_colormap.gamma());
  }

  void FloatColormapEditorWidget::updateColormap() {
    updateIndexRange();
    updateStartColor();
    updateEndColor();
    updateMonochromeColor();
    updateRangeIndicator();
    updateGamma();

    _updatePresetSelector();
    _updateIndexRangeSelector();

    p_monochromeColorControl->controlWidget()->setEnabled(
      _colormap.type() == FloatColormap::Monochrome);
  }

  void FloatColormapEditorWidget::setStartIndexInColormap() {
    _colormap.setStartIndex(p_startIndexControl->value());
  }

  void FloatColormapEditorWidget::setEndIndexInColormap() {
    _colormap.setEndIndex(p_endIndexControl->value());
  }

  void FloatColormapEditorWidget::setPresetInColormap() {
    switch (p_presetSelector->currentIndex()) {
      case 0:
        _colormap.setGray();
        break;
      case 1:
        _colormap.setHot();
        break;
      case 2:
        _colormap.setJet();
        break;
      case 3:
        _colormap.setHSV();
        break;
      default:
        break;
    }
  }

  void FloatColormapEditorWidget::setIndexRangeInColormap() {
    switch (p_indexRangeSelector->currentIndex()) {
      case 0:
        _colormap.setUnsigned8BitRange();
        break;
      case 1:
        _colormap.setSigned8BitRange();
        break;
      case 2:
        _colormap.setUnsigned12BitRange();
        break;
      case 3:
        _colormap.setUnsigned16BitRange();
        break;
      case 4:
        _colormap.setSigned16BitRange();
        break;
      default:
        break;
    }
  }

  void FloatColormapEditorWidget::setMonochromeColorInColormap() {
    _colormap.setMonochromeColor(p_monochromeColorControl->value());
  }

  void FloatColormapEditorWidget::setGammaInColormap() {
    _colormap.setGamma(p_gammaControl->value());
  }

  void FloatColormapEditorWidget::setRangeIndicatorFlagInColormap() {
    _colormap.setRangeIndicatorEnabled(p_rangeIndicatorControl->value());
  }

  void FloatColormapEditorWidget::selectStartColor() {
    FloatColormap::ColorT col(_colormap.startColor());
    QColor color = QColorDialog::getColor(QColor(col(0), col(1), col(2)), this);
    if (color.isValid())
      _colormap.setStartColor(
        FloatColormap::ColorT(
          static_cast<unsigned char>(color.red()),
          static_cast<unsigned char>(color.green()),
          static_cast<unsigned char>(color.blue())));
  }

  void FloatColormapEditorWidget::selectEndColor() {
    FloatColormap::ColorT col(_colormap.endColor());
    QColor color = QColorDialog::getColor(QColor(col(0), col(1), col(2)), this);
    if (color.isValid())
      _colormap.setEndColor(
        FloatColormap::ColorT(
          static_cast<unsigned char>(color.red()),
          static_cast<unsigned char>(color.green()),
          static_cast<unsigned char>(color.blue())));
  }

  void FloatColormapEditorWidget::_updateIndexRangeSelector() {
    if (_colormap.isUnsigned8BitRange())
      p_indexRangeSelector->setCurrentIndex(0);
    else if (_colormap.isSigned8BitRange())
      p_indexRangeSelector->setCurrentIndex(1);
    else if (_colormap.isUnsigned12BitRange())
      p_indexRangeSelector->setCurrentIndex(2);
    else if (_colormap.isUnsigned16BitRange())
      p_indexRangeSelector->setCurrentIndex(3);
    else if (_colormap.isSigned16BitRange())
      p_indexRangeSelector->setCurrentIndex(4);
    else p_indexRangeSelector->setCurrentIndex(5);
  }

  void FloatColormapEditorWidget::_updatePresetSelector() {
    if (_colormap.isGray()) p_presetSelector->setCurrentIndex(0);
    else if (_colormap.isHot()) p_presetSelector->setCurrentIndex(1);
    else if (_colormap.isJet()) p_presetSelector->setCurrentIndex(2);
    else if (_colormap.isHSV()) p_presetSelector->setCurrentIndex(3);
    else p_presetSelector->setCurrentIndex(4);
  }

}

