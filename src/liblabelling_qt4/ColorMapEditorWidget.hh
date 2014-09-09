/**************************************************************************
**       Title: Widget for editing Colormaps
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef COLORMAPEDITORWIDGET_HH
#define COLORMAPEDITORWIDGET_HH

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
