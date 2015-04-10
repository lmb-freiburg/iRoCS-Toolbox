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

#ifndef COLORMAPEDITORWIDGET_HH
#define COLORMAPEDITORWIDGET_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QWidget>

#include "IColorMapEditor.hh"

class QLabel;
class QTreeWidget;
class QPushButton;
class QToolButton;

class StringSelectionControlElement;
class IntControlElement;
class DoubleControlElement;
class BoolControlElement;
class ColorControlElement;

class ColorMapEditorWidget : public QWidget, public IColorMapEditor
{

Q_OBJECT

public:
  
  ColorMapEditorWidget(ColorMap *colorMap = NULL, QWidget *parent = NULL);
  ~ColorMapEditorWidget();

  void updateIndexRange(bool updatePreview = true);
  void updateIntermediateColors(bool updatePreview = true);
  void updateOverrideColors(bool updatePreview = true);
  void updateMonochromeColor(bool updatePreview = true);
  void updateRangeIndicator(bool updatePreview = true);
  void updateGamma(bool updatePreview = true);

  void updateControls();

signals:
        
  void colorMapChanged();

private slots:
  
  void processPresetColorMapSelection();
  void processColorMapLoadAction();
  void processColorMapSaveAction();
  void processPresetRangeSelection();
  void processColorMapStartIndexChange();
  void processColorMapEndIndexChange();
  void processGammaChange();
  void processRangeIndicatorChange();
  void processStartColorChange();
  void processEndColorChange();
  void processMonochromeColorChange();
  void processIntermediateColorAddition();
  void processIntermediateColorDeletion();
  void processIntermediateColorPercentageChange();
  void processIntermediateColorChange();
  void processIntermediateColorSelectionChange();
  void processOverrideColorAddition();
  void processOverrideColorDeletion();
  void processOverrideColorIndexChange();
  void processOverrideColorChange();
  void processOverrideColorSelectionChange();
  void toggleAdvanced(bool toggle);
  void updateColorMapPreview();

  void resizeEvent(QResizeEvent *e);

private:
  
  void setConnectionsEnabled(bool enable);

  QLabel *p_preview;
  StringSelectionControlElement *p_preset;
  QAction *p_load, *p_save;
  StringSelectionControlElement *p_dataType;
  IntControlElement *p_startIndex, *p_endIndex;
  DoubleControlElement *p_gamma;
  BoolControlElement *p_rangeIndicator;
  ColorControlElement *p_monochromeColor;
  QTreeWidget *p_intermediateColors;
  QPushButton *p_addIntermediateColor, *p_removeIntermediateColor;
  QTreeWidget *p_overrideColors;
  QPushButton *p_addOverrideColor, *p_removeOverrideColor;
  QToolButton *p_toggleAdvancedButton;
  QWidget *p_advancedWidget;
  bool _connectionsEnabled;

};

#endif
