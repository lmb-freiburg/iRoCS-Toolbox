/**************************************************************************
**       Title: Widget for editing Colormaps
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#include "ColorMapEditorWidget.hh"

#include <QtGui/QLabel>
#include <QtGui/QTreeWidget>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>
#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <QtGui/QPixmap>
#include <QtCore/QSettings>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QInputDialog>
#include <QtGui/QVBoxLayout>

#include "StringSelectionControlElement.hh"
#include "IntControlElement.hh"
#include "DoubleControlElement.hh"
#include "BoolControlElement.hh"
#include "ColorControlElement.hh"
#include "ColorMap.hh"
#include "HDF5SelectionDialog.hh"

#if defined HAVE_CONFIG_H && !defined PACKAGE
#include <config.hh>
#endif

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

ColorMapEditorWidget::ColorMapEditorWidget(ColorMap *colorMap, QWidget *parent)
        : QWidget(parent), IColorMapEditor(colorMap), _connectionsEnabled(false)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
  QVBoxLayout *layout = new QVBoxLayout;
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);

  QWidget *basicControlWidget = new QWidget;
  basicControlWidget->setContentsMargins(0, 0, 0, 0);
  basicControlWidget->setSizePolicy(
      QSizePolicy::Expanding, QSizePolicy::Maximum);
  QVBoxLayout *basicLayout = new QVBoxLayout;
  basicLayout->setContentsMargins(0, 0, 0, 0);
  basicControlWidget->setLayout(basicLayout);

  p_preview = new QLabel;
  p_preview->setFixedHeight(30);
  p_preview->setMinimumWidth(100);
  p_preview->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  p_preview->setBackgroundRole(QPalette::Base);
  basicLayout->addWidget(p_preview);
  
  QToolBar *colorMapToolBar = new QToolBar;

  QStringList presetsList;
  presetsList << "Gray" << "Hot" << "Jet" << "HSV" << "Random" << "User";
  p_preset = new StringSelectionControlElement(tr("Preset:"), presetsList);
  colorMapToolBar->addWidget(p_preset);

  colorMapToolBar->setMovable(false);
  colorMapToolBar->setFloatable(false);
  colorMapToolBar->setOrientation(Qt::Horizontal);
  p_load = new QAction(
      QIcon::fromTheme("document-open"), tr("Load Color Map"), this);
  colorMapToolBar->addAction(p_load);
  p_save = new QAction(
      QIcon::fromTheme("document-save"), tr("Save Color Map"), this);
  colorMapToolBar->addAction(p_save);

  basicLayout->addWidget(colorMapToolBar);

  QStringList dataRangeList;
  dataRangeList << "Unsigned 8 Bit" << "Signed 8 Bit" << "Unsigned 12 Bit"
                << "Unsigned 16 Bit" << "Signed 16 Bit" << "Signed 32 Bit"
                << "User";
  p_dataType = new StringSelectionControlElement(
      tr("Index Range:"), dataRangeList);
  basicLayout->addWidget(p_dataType);

  QHBoxLayout *gammaLayout = new QHBoxLayout;

  p_gamma = new DoubleControlElement(tr("Gamma:"), 1.0);
  p_gamma->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  p_gamma->setRange(0.1, 10.0);
  p_gamma->setSingleStep(0.01);
  p_gamma->setSliderViewEnabled(true);
  gammaLayout->addWidget(p_gamma);
  
  p_rangeIndicator = new BoolControlElement(tr("Range indicator:"), false);
  gammaLayout->addWidget(p_rangeIndicator);
  
  basicLayout->addLayout(gammaLayout);

  p_monochromeColor = new ColorControlElement(
      tr("Monochrome Color:"), blitz::TinyVector<unsigned char,3>(
          255, 255, 255));
  basicLayout->addWidget(p_monochromeColor);
  
  p_toggleAdvancedButton = new QToolButton;
  p_toggleAdvancedButton->setFixedHeight(15);
  p_toggleAdvancedButton->setArrowType(Qt::DownArrow);
  p_toggleAdvancedButton->setSizePolicy(
      QSizePolicy::Expanding, QSizePolicy::Fixed);
  p_toggleAdvancedButton->setCheckable(true);
  connect(p_toggleAdvancedButton, SIGNAL(toggled(bool)),
          SLOT(toggleAdvanced(bool)));
  basicLayout->addWidget(p_toggleAdvancedButton);
  
  layout->addWidget(basicControlWidget);

  p_advancedWidget = new QWidget;
  p_advancedWidget->setContentsMargins(0, 0, 0, 0);
  p_advancedWidget->setSizePolicy(
      QSizePolicy::Expanding, QSizePolicy::Expanding);
  QVBoxLayout *advancedLayout = new QVBoxLayout;
  advancedLayout->setContentsMargins(0, 0, 0, 0);
  p_advancedWidget->setLayout(advancedLayout);

  QHBoxLayout *rangeLayout = new QHBoxLayout;

  p_startIndex = new IntControlElement(tr("Start:"), 0);
  rangeLayout->addWidget(p_startIndex);
  
  p_endIndex = new IntControlElement(tr("End:"), 255);
  rangeLayout->addWidget(p_endIndex);

  advancedLayout->addLayout(rangeLayout);

  QVBoxLayout *listLayout = new QVBoxLayout;

  QVBoxLayout *intermediateColorLayout = new QVBoxLayout;

  p_intermediateColors = new QTreeWidget;
  p_intermediateColors->setSelectionMode(QAbstractItemView::ExtendedSelection);
  p_intermediateColors->setColumnCount(2);

  QTreeWidgetItem *intermediateColorHeaderItem = new QTreeWidgetItem;
  intermediateColorHeaderItem->setText(0, tr("Stop [%]"));
  intermediateColorHeaderItem->setText(1, tr("Color [RGB]"));
  p_intermediateColors->setHeaderItem(intermediateColorHeaderItem);
  
  QTreeWidgetItem *startColorItem = new QTreeWidgetItem;
  startColorItem->setText(0, "0");
  p_intermediateColors->addTopLevelItem(startColorItem);
  ColorControlElement *startColor = new ColorControlElement(
      "", blitz::TinyVector<unsigned char,3>(0, 0, 0));
  p_intermediateColors->setItemWidget(
      p_intermediateColors->topLevelItem(0), 1, startColor);

  QTreeWidgetItem *endColorItem = new QTreeWidgetItem;
  endColorItem->setText(0, "100");
  p_intermediateColors->addTopLevelItem(endColorItem);
  ColorControlElement *endColor = new ColorControlElement(
      "", blitz::TinyVector<int,3>(255, 255, 255));
  p_intermediateColors->setItemWidget(
      p_intermediateColors->topLevelItem(1), 1, endColor);
  
  intermediateColorLayout->addWidget(p_intermediateColors);
  
  QHBoxLayout *intermediateColorButtonLayout = new QHBoxLayout;
  
  p_addIntermediateColor = new QPushButton("+");
  intermediateColorButtonLayout->addWidget(p_addIntermediateColor);
  
  p_removeIntermediateColor = new QPushButton("-");
  p_removeIntermediateColor->setEnabled(false);
  intermediateColorButtonLayout->addWidget(p_removeIntermediateColor);
  
  intermediateColorButtonLayout->addStretch(1);

  intermediateColorLayout->addLayout(intermediateColorButtonLayout);

  listLayout->addLayout(intermediateColorLayout);

  QVBoxLayout *overrideColorLayout = new QVBoxLayout;

  p_overrideColors = new QTreeWidget;
  p_overrideColors->setSelectionMode(QAbstractItemView::ExtendedSelection);
  p_overrideColors->setColumnCount(2);

  QTreeWidgetItem *overrideColorHeaderItem = new QTreeWidgetItem;
  overrideColorHeaderItem->setText(0, tr("Index"));
  overrideColorHeaderItem->setText(1, tr("Color [RGB]"));
  p_overrideColors->setHeaderItem(overrideColorHeaderItem);
  
  overrideColorLayout->addWidget(p_overrideColors);
  
  QHBoxLayout *overrideColorButtonLayout = new QHBoxLayout;
  
  p_addOverrideColor = new QPushButton("+");
  overrideColorButtonLayout->addWidget(p_addOverrideColor);
  
  p_removeOverrideColor = new QPushButton("-");
  p_removeOverrideColor->setEnabled(false);
  overrideColorButtonLayout->addWidget(p_removeOverrideColor);
  
  overrideColorButtonLayout->addStretch(1);

  overrideColorLayout->addLayout(overrideColorButtonLayout);

  listLayout->addLayout(overrideColorLayout);

  advancedLayout->addLayout(listLayout);
  layout->addWidget(p_advancedWidget);

  p_advancedWidget->setVisible(false);
  p_toggleAdvancedButton->setChecked(false);

  updateControls();
}

ColorMapEditorWidget::~ColorMapEditorWidget()
{}

void ColorMapEditorWidget::updateIndexRange(bool updatePreview)
{
  if (p_colorMap == NULL)
  {
    p_dataType->setValue("User");
    p_startIndex->setValue(0);
    p_endIndex->setValue(0);
    return;
  }

  bool oldConnectionState = _connectionsEnabled;
  setConnectionsEnabled(false);

  if (p_colorMap->isUnsigned8BitRange())
      p_dataType->setValue("Unsigned 8 Bit");
  else if (p_colorMap->isSigned8BitRange())
      p_dataType->setValue("Signed 8 Bit");
  else if (p_colorMap->isUnsigned12BitRange())
      p_dataType->setValue("Unsigned 12 Bit");
  else if (p_colorMap->isUnsigned16BitRange())
      p_dataType->setValue("Unsigned 16 Bit");
  else if (p_colorMap->isSigned16BitRange())
      p_dataType->setValue("Signed 16 Bit");
  else if (p_colorMap->isSigned32BitRange())
      p_dataType->setValue("Signed 32 Bit");
  else p_dataType->setValue("User");

  p_startIndex->setValue(static_cast<int>(p_colorMap->startIndex()));
  p_endIndex->setValue(static_cast<int>(p_colorMap->endIndex()));

  if (updatePreview) updateColorMapPreview();
  setConnectionsEnabled(oldConnectionState);
}

void ColorMapEditorWidget::updateIntermediateColors(bool updatePreview)
{
  if (p_colorMap == NULL)
  {
    p_preset->setValue("User");
    p_intermediateColors->clear();

    QTreeWidgetItem *startColorItem = new QTreeWidgetItem;
    startColorItem->setText(0, "0");
    p_intermediateColors->addTopLevelItem(startColorItem);
    ColorControlElement *startColor = new ColorControlElement(
        "", blitz::TinyVector<unsigned char,3>(0, 0, 0));
    p_intermediateColors->setItemWidget(
        p_intermediateColors->topLevelItem(0), 1, startColor);
    
    QTreeWidgetItem *endColorItem = new QTreeWidgetItem;
    endColorItem->setText(0, "100");
    p_intermediateColors->addTopLevelItem(endColorItem);
    ColorControlElement *endColor = new ColorControlElement(
        "", blitz::TinyVector<int,3>(255, 255, 255));
    p_intermediateColors->setItemWidget(
        p_intermediateColors->topLevelItem(1), 1, endColor);

    return;
  }

  bool oldConnectionState = _connectionsEnabled;
  setConnectionsEnabled(false);

  if (p_colorMap->isGray()) p_preset->setValue("Gray");
  else if (p_colorMap->isHot()) p_preset->setValue("Hot");
  else if (p_colorMap->isJet()) p_preset->setValue("Jet");
  else if (p_colorMap->isHSV()) p_preset->setValue("HSV");
  else if (p_colorMap->isRandom()) p_preset->setValue("Random");
  else p_preset->setValue("User");

  if (p_intermediateColors->topLevelItemCount() !=
      static_cast<int>(p_colorMap->intermediateColors().size()) + 2)
  {
    p_intermediateColors->clear();
    for (int i = 0;
         i < static_cast<int>(p_colorMap->intermediateColors().size()) + 2; ++i)
    {
      QTreeWidgetItem *intermediateColorItem = new QTreeWidgetItem;
      p_intermediateColors->addTopLevelItem(intermediateColorItem);
    }
  }

  int i = 0;
  p_intermediateColors->topLevelItem(i)->setText(0, "0");
  p_intermediateColors->setItemWidget(
      p_intermediateColors->topLevelItem(i), 1,
      new ColorControlElement("", blitz::TinyVector<unsigned char,3>(
                                  255.0f * p_colorMap->startColor())));
  ++i;
  for (std::map< double,blitz::TinyVector<float,3> >::const_iterator it =
           p_colorMap->intermediateColors().begin();
       it != p_colorMap->intermediateColors().end(); ++it, ++i)
  {
    DoubleControlElement *percentageControl =
        new DoubleControlElement("", 100.0 * it->first);
    percentageControl->setRange(0.1, 99.9);
    percentageControl->setSingleStep(0.1);
    p_intermediateColors->setItemWidget(
        p_intermediateColors->topLevelItem(i), 0, percentageControl);
    p_intermediateColors->setItemWidget(
        p_intermediateColors->topLevelItem(i), 1,
        new ColorControlElement(
            "", blitz::TinyVector<unsigned char,3>(255.0f * it->second)));
  }
  p_intermediateColors->topLevelItem(i)->setText(0, "100");
  p_intermediateColors->setItemWidget(
      p_intermediateColors->topLevelItem(i), 1,
      new ColorControlElement("", blitz::TinyVector<unsigned char,3>(
                                  255.0f * p_colorMap->endColor())));

  p_monochromeColor->setEnabled(p_colorMap->type() == ColorMap::Monochrome);

  if (updatePreview) updateColorMapPreview();
  setConnectionsEnabled(oldConnectionState);
}

void ColorMapEditorWidget::updateOverrideColors(bool updatePreview)
{
  if (p_colorMap == NULL)
  {
    p_overrideColors->clear();
    return;
  }

  bool oldConnectionState = _connectionsEnabled;
  setConnectionsEnabled(false);
  if (p_overrideColors->topLevelItemCount() !=
      static_cast<int>(p_colorMap->overrideColors().size()))
  {
    p_overrideColors->clear();
    for (int i = 0; i < static_cast<int>(p_colorMap->overrideColors().size());
         ++i)
    {
      QTreeWidgetItem *overrideColorItem = new QTreeWidgetItem;
      p_overrideColors->addTopLevelItem(overrideColorItem);
    }
  }

  int i = 0;
  for (std::map< long long,blitz::TinyVector<float,3> >::const_iterator it =
           p_colorMap->overrideColors().begin();
       it != p_colorMap->overrideColors().end(); ++it, ++i)
  {
    IntControlElement *indexControl = new IntControlElement(
        "", static_cast<int>(it->first));
    p_overrideColors->setItemWidget(
        p_overrideColors->topLevelItem(i), 0, indexControl);
    p_overrideColors->setItemWidget(
        p_overrideColors->topLevelItem(i), 1,
        new ColorControlElement(
            "", blitz::TinyVector<unsigned char,3>(255.0f * it->second)));
  }
  if (updatePreview) updateColorMapPreview();
  setConnectionsEnabled(oldConnectionState);
}

void ColorMapEditorWidget::updateMonochromeColor(bool updatePreview)
{
  if (p_colorMap == NULL)
  {
    p_monochromeColor->setValue(
        blitz::TinyVector<unsigned char,3>(255, 255, 255));
    return;
  }
  
  bool oldConnectionState = _connectionsEnabled;
  setConnectionsEnabled(false);

  p_monochromeColor->setValue(blitz::TinyVector<unsigned char,3>(
                                  255.0f * p_colorMap->monochromeColor()));

  if (updatePreview) updateColorMapPreview();
  setConnectionsEnabled(oldConnectionState);
}

void ColorMapEditorWidget::updateRangeIndicator(bool updatePreview)
{
  if (p_colorMap == NULL)
  {
    p_rangeIndicator->setValue(false);
    return;
  }
  
  bool oldConnectionState = _connectionsEnabled;
  setConnectionsEnabled(false);

  p_rangeIndicator->setValue(p_colorMap->rangeIndicatorEnabled());

  if (updatePreview) updateColorMapPreview();
  setConnectionsEnabled(oldConnectionState);
}

void ColorMapEditorWidget::updateGamma(bool updatePreview)
{
  if (p_colorMap == NULL)
  {
    p_gamma->setValue(1.0);
    return;
  }
  
  bool oldConnectionState = _connectionsEnabled;
  setConnectionsEnabled(false);

  p_gamma->setValue(p_colorMap->gamma());

  if (updatePreview) updateColorMapPreview();
  setConnectionsEnabled(oldConnectionState);
}

void ColorMapEditorWidget::updateControls()
{
  setConnectionsEnabled(false);

  updateIndexRange(false);
  updateIntermediateColors(false);
  updateOverrideColors(false);
  updateMonochromeColor(false);
  updateRangeIndicator(false);
  updateGamma(false);
  updateColorMapPreview();

  setConnectionsEnabled(true);
}

void ColorMapEditorWidget::processPresetColorMapSelection()
{
  if (p_preset->toStdString() == "User")
  {
    if (p_colorMap->isGray()) p_preset->setValue("Gray");
    else if (p_colorMap->isHot()) p_preset->setValue("Hot");
    else if (p_colorMap->isJet()) p_preset->setValue("Jet");
    else if (p_colorMap->isHSV()) p_preset->setValue("HSV");    
    else if (p_colorMap->isRandom()) p_preset->setValue("Random");    
    return;
  }
  
  if (p_preset->toStdString() == "Gray") p_colorMap->setGray();
  else if (p_preset->toStdString() == "Hot") p_colorMap->setHot();
  else if (p_preset->toStdString() == "Jet") p_colorMap->setJet();
  else if (p_preset->toStdString() == "HSV") p_colorMap->setHSV();
  else if (p_preset->toStdString() == "Random") p_colorMap->setRandom();

  emit colorMapChanged();
}

void ColorMapEditorWidget::processColorMapLoadAction()
{
  QSettings settings;
  QString startPath = settings.value(
      "FileDialog/startpath", getenv("HOME")).toString();
  QString selectedFilter;
  QString fileName = QFileDialog::getOpenFileName(
      this, tr("Load Color Map"), startPath,
      tr("HDF5 (*.h5)"), &selectedFilter);
  if (fileName == QString::Null()) return;
  settings.setValue("FileDialog/startpath", fileName);
  std::string selectedItem = 
      HDF5ColorMapSelectionDialog::selectColorMap(
          fileName.toStdString().c_str(), this);
  if (selectedItem == "") return;
  try
  {
    ColorMap cmap;
    cmap.load(fileName.toStdString().c_str(), selectedItem);
    *p_colorMap = cmap;
    p_colorMap->setColorMapEditor(this);
    updateControls();
    emit colorMapChanged();
  }
  catch (BlitzH5Error &)
  {
    QMessageBox::warning(
        this, tr("Invalid color map"),
        tr("The selected hdf5 group doesn't contain a valid color map"));
  }
}

void ColorMapEditorWidget::processColorMapSaveAction()
{
  QSettings settings;
  QString startPath = settings.value(
      "FileDialog/startpath", getenv("HOME")).toString();
  QString selectedFilter;
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save Color Map"), startPath,
      tr("HDF5 (*.h5)"), &selectedFilter, QFileDialog::DontConfirmOverwrite);
  if (fileName == QString::Null()) return;
  settings.setValue("FileDialog/startpath", fileName);
  bool ok;
  std::string groupName = QInputDialog::getText(
      this, tr("Enter hdf5 color map path"), tr("hdf5 group:"),
      QLineEdit::Normal, "", &ok).toStdString();
  if (!ok || groupName == "") return;
  try
  {
    try
    {
      BlitzH5File inFile(fileName.toStdString());
      if (inFile.existsAttribute("id", groupName))
      {
        std::string id;
        inFile.readAttribute(id, "id", groupName);
        if (id == "ColorMap")
        {
          QMessageBox::StandardButton button = QMessageBox::question(
              this, tr("Overwrite existing colormap?"),
              tr("The group name you entered already contains a color map, "
                 "do you want to replace it?"),
              QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
          if (button == QMessageBox::No) return;
        }
        else
        {
          QMessageBox::warning(
              this, tr("Group exists with incompatible content"),
              tr("The group you entered already contains data, please choose "
                 "another group."));
          return;
        }
      }
    }
    catch (BlitzH5Error &)
    {}
    p_colorMap->save(fileName.toStdString().c_str(), groupName);
  }
  catch (BlitzH5Error &e)
  {
    QMessageBox::critical(
        this, tr("Could not write color map"),
        tr("Error while writing color map: ") + e.what());
  }
} 

void ColorMapEditorWidget::processPresetRangeSelection()
{
  if (p_dataType->toStdString() == "User")
  {
    if (p_colorMap->isUnsigned8BitRange())
        p_dataType->setValue("Unsigned 8 Bit");
    else if (p_colorMap->isSigned8BitRange())
        p_dataType->setValue("Signed 8 Bit");
    else if (p_colorMap->isUnsigned12BitRange())
        p_dataType->setValue("Unsigned 12 Bit");
    else if (p_colorMap->isUnsigned16BitRange())
        p_dataType->setValue("Unsigned 16 Bit");
    else if (p_colorMap->isSigned16BitRange())
        p_dataType->setValue("Signed 16 Bit");
    else if (p_colorMap->isSigned32BitRange())
        p_dataType->setValue("Signed 32 Bit");
    return;
  }

  if (p_dataType->toStdString() == "Unsigned 8 Bit")
      p_colorMap->setUnsigned8BitRange();
  else if (p_dataType->toStdString() == "Signed 8 Bit")
      p_colorMap->setSigned8BitRange();
  else if (p_dataType->toStdString() == "Unsigned 12 Bit")
      p_colorMap->setUnsigned12BitRange();
  else if (p_dataType->toStdString() == "Unsigned 16 Bit")
      p_colorMap->setUnsigned16BitRange();
  else if (p_dataType->toStdString() == "Signed 16 Bit")
      p_colorMap->setSigned16BitRange();
  else if (p_dataType->toStdString() == "Signed 32 Bit")
      p_colorMap->setSigned32BitRange();

  emit colorMapChanged();
}

void ColorMapEditorWidget::processColorMapStartIndexChange()
{
  if (p_colorMap->startIndex() == p_startIndex->value()) return;
  if (p_startIndex->value() >= p_endIndex->value())
  {
    p_startIndex->setValue(p_endIndex->value() - 1);
    return;
  }
  p_colorMap->setStartIndex(p_startIndex->value());
  emit colorMapChanged();
}

void ColorMapEditorWidget::processColorMapEndIndexChange()
{
  if (p_colorMap->endIndex() == p_endIndex->value()) return;
  if (p_startIndex->value() >= p_endIndex->value())
  {
    p_endIndex->setValue(p_startIndex->value() + 1);
    return;
  }
  p_colorMap->setEndIndex(p_endIndex->value());
  emit colorMapChanged();
}

void ColorMapEditorWidget::processGammaChange()
{
  if (p_colorMap->gamma() == p_gamma->value()) return;
  p_colorMap->setGamma(p_gamma->value());
  emit colorMapChanged();
}

void ColorMapEditorWidget::processStartColorChange()
{
  if (blitz::all(
          p_colorMap->startColor() ==
          blitz::TinyVector<float,3>(
              reinterpret_cast<ColorControlElement*>(
                  p_intermediateColors->itemWidget(
                      p_intermediateColors->topLevelItem(0), 1))->value()) /
          255.0f))
      return;
  p_colorMap->setStartColor(
      blitz::TinyVector<float,3>(
          reinterpret_cast<ColorControlElement*>(
              p_intermediateColors->itemWidget(
                  p_intermediateColors->topLevelItem(0), 1))->value()) /
      255.0f);
  emit colorMapChanged();  
}

void ColorMapEditorWidget::processEndColorChange()
{
  if (blitz::all(
          p_colorMap->endColor() ==
          blitz::TinyVector<float,3>(
              reinterpret_cast<ColorControlElement*>(
                  p_intermediateColors->itemWidget(
                      p_intermediateColors->topLevelItem(
                          p_intermediateColors->topLevelItemCount() - 1),
                      1))->value()) /
          255.0f))
      return;
  p_colorMap->setEndColor(
      blitz::TinyVector<float,3>(
          reinterpret_cast<ColorControlElement*>(
              p_intermediateColors->itemWidget(
                  p_intermediateColors->topLevelItem(
                          p_intermediateColors->topLevelItemCount() - 1),
                  1))->value()) /
      255.0f);
  emit colorMapChanged();
}

void ColorMapEditorWidget::processMonochromeColorChange()
{
  if (blitz::all(
          blitz::TinyVector<float,3>(p_monochromeColor->value()) / 255.0f ==
          p_colorMap->monochromeColor())) return;
  p_colorMap->setMonochromeColor(
      blitz::TinyVector<float,3>(p_monochromeColor->value()) / 255.0f);
  emit colorMapChanged();
}

void ColorMapEditorWidget::processIntermediateColorAddition()
{
  double insertPercentage;
  blitz::TinyVector<float,3> insertColor;

  // No intermediate colors yet
  if (p_colorMap->intermediateColors().size() == 0)
  {
    insertPercentage = 0.5;
    insertColor = (p_colorMap->startColor() + p_colorMap->endColor()) / 2.0f;
    p_colorMap->addIntermediateColor(insertPercentage, insertColor);
    emit colorMapChanged();
    return;
  }
  
  // New intermediate color after start color
  if (p_intermediateColors->currentItem() == NULL ||
      p_intermediateColors->indexOfTopLevelItem(
          p_intermediateColors->currentItem()) <= 1)
  {
    insertPercentage = p_colorMap->intermediateColors().begin()->first / 2.0;
    insertColor = (p_colorMap->startColor() +
                   p_colorMap->intermediateColors().begin()->second) / 2.0f;
    p_colorMap->addIntermediateColor(insertPercentage, insertColor);
    emit colorMapChanged();
    return;
  }

  // New intermediate color before end color
  if (p_intermediateColors->indexOfTopLevelItem(
          p_intermediateColors->currentItem()) ==
      p_intermediateColors->topLevelItemCount() - 1)
  {
    insertPercentage = ((--p_colorMap->intermediateColors().end())->first +
                        1.0) / 2.0;
    insertColor = ((--p_colorMap->intermediateColors().end())->second +
                   p_colorMap->endColor()) / 2.0f;
    p_colorMap->addIntermediateColor(insertPercentage, insertColor);
    emit colorMapChanged();
    return;
  }
  
  // New intermediate color in-between two other intermediate colors
  int itemIndex = p_intermediateColors->indexOfTopLevelItem(
      p_intermediateColors->currentItem());
  insertPercentage =
      (reinterpret_cast<DoubleControlElement*>(
          p_intermediateColors->itemWidget(
              p_intermediateColors->topLevelItem(
                  itemIndex - 1), 0))->value() / 100.0 +
       reinterpret_cast<DoubleControlElement*>(
          p_intermediateColors->itemWidget(
              p_intermediateColors->topLevelItem(
                  itemIndex), 0))->value() / 100.0) / 2.0;
  insertColor = (blitz::TinyVector<float,3>(
                     reinterpret_cast<ColorControlElement*>(
                         p_intermediateColors->itemWidget(
                             p_intermediateColors->topLevelItem(
                                 itemIndex - 1), 1))->value()) / 255.0f +
                 blitz::TinyVector<float,3>(
                     reinterpret_cast<ColorControlElement*>(
                         p_intermediateColors->itemWidget(
                             p_intermediateColors->topLevelItem(
                                 itemIndex), 1))->value()) / 255.0f) / 2.0;
  p_colorMap->addIntermediateColor(insertPercentage, insertColor);
  emit colorMapChanged();
  return;
}

void ColorMapEditorWidget::processIntermediateColorDeletion()
{
  p_colorMap->setColorMapEditor(NULL);
  QList<QTreeWidgetItem*> items = p_intermediateColors->selectedItems();
  for (QList<QTreeWidgetItem*>::iterator it = items.begin();
       it != items.end(); ++it)
      p_colorMap->removeIntermediateColor(
          reinterpret_cast<DoubleControlElement*>(
              p_intermediateColors->itemWidget(*it, 0))->value() / 100.0);
  p_colorMap->setColorMapEditor(this);
  emit colorMapChanged();
}

void ColorMapEditorWidget::processIntermediateColorPercentageChange()
{
  p_colorMap->setColorMapEditor(NULL);
  std::map< double,blitz::TinyVector<float,3> >::const_iterator it =
      p_colorMap->intermediateColors().begin();
  for (int i = 1; i < p_intermediateColors->topLevelItemCount() - 1 &&
           it != p_colorMap->intermediateColors().end(); ++i, ++it)
  {
    double percentage = reinterpret_cast<DoubleControlElement*>(
        p_intermediateColors->itemWidget(
            p_intermediateColors->topLevelItem(i), 0))->value() / 100.0;
    if (it->first != percentage)
    {
      p_colorMap->addIntermediateColor(percentage, it->second);
      p_colorMap->removeIntermediateColor(it->first);
      break;
    }
  }
  p_colorMap->setColorMapEditor(this);
  emit colorMapChanged();  
}

void ColorMapEditorWidget::processIntermediateColorChange()
{
  p_colorMap->setColorMapEditor(NULL);
  p_colorMap->setStartColor(
      blitz::TinyVector<float,3>(
          reinterpret_cast<ColorControlElement*>(
              p_intermediateColors->itemWidget(
                  p_intermediateColors->topLevelItem(0), 1))->value()) /
      255.0f);
  std::map< double,blitz::TinyVector<float,3> >::const_iterator it =
      p_colorMap->intermediateColors().begin();
  for (int i = 1; i < p_intermediateColors->topLevelItemCount() - 1 &&
           it != p_colorMap->intermediateColors().end(); ++i, ++it)
  {
    p_colorMap->addIntermediateColor(
        it->first, blitz::TinyVector<float,3>(
            reinterpret_cast<ColorControlElement*>(
                p_intermediateColors->itemWidget(
                    p_intermediateColors->topLevelItem(i), 1))->value()) /
        255.0f);
  }
  p_colorMap->setEndColor(
      blitz::TinyVector<float,3>(
          reinterpret_cast<ColorControlElement*>(
              p_intermediateColors->itemWidget(
                  p_intermediateColors->topLevelItem(
                      p_intermediateColors->topLevelItemCount() - 1),
                  1))->value()) / 255.0f);
  p_colorMap->setColorMapEditor(this);
  emit colorMapChanged();  
}

void ColorMapEditorWidget::processIntermediateColorSelectionChange()
{
  p_intermediateColors->topLevelItem(0)->setSelected(false);
  p_intermediateColors->topLevelItem(
      p_intermediateColors->topLevelItemCount() - 1)->setSelected(false);
  p_removeIntermediateColor->setEnabled(
      p_overrideColors->topLevelItemCount() != 2 &&
      p_intermediateColors->selectedItems().size() != 0);
}

void ColorMapEditorWidget::processOverrideColorAddition()
{
  long long insertIndex;
  if (p_colorMap->overrideColors().size() == 0) insertIndex = 0ll;
  else insertIndex = (--p_colorMap->overrideColors().end())->first + 1;
  blitz::TinyVector<float,3> insertColor(0.0f, 0.0f, 0.0f);
  p_colorMap->addOverrideColor(insertIndex, insertColor);
  emit colorMapChanged();
}

void ColorMapEditorWidget::processOverrideColorDeletion()
{
  p_colorMap->setColorMapEditor(NULL);
  QList<QTreeWidgetItem*> items = p_overrideColors->selectedItems();
  for (QList<QTreeWidgetItem*>::iterator it = items.begin();
       it != items.end(); ++it)
      p_colorMap->removeOverrideColor(
          reinterpret_cast<IntControlElement*>(
              p_overrideColors->itemWidget(*it, 0))->value());
  p_colorMap->setColorMapEditor(this);
  emit colorMapChanged();
}

void ColorMapEditorWidget::processOverrideColorIndexChange()
{
  p_colorMap->setColorMapEditor(NULL);
  std::map< long long,blitz::TinyVector<float,3> >::const_iterator it =
      p_colorMap->overrideColors().begin();
  for (int i = 0; i < p_overrideColors->topLevelItemCount() &&
           it != p_colorMap->overrideColors().end(); ++i, ++it)
  {
    int index = reinterpret_cast<IntControlElement*>(
        p_overrideColors->itemWidget(
            p_overrideColors->topLevelItem(i), 0))->value();
    if (it->first != index)
    {
      p_colorMap->addOverrideColor(index, it->second);
      p_colorMap->removeOverrideColor(it->first);
      break;
    }
  }
  p_colorMap->setColorMapEditor(this);
  emit colorMapChanged();  
}

void ColorMapEditorWidget::processOverrideColorChange()
{
  p_colorMap->setColorMapEditor(NULL);
  std::map< long long,blitz::TinyVector<float,3> >::const_iterator it =
      p_colorMap->overrideColors().begin();
  for (int i = 0; i < p_overrideColors->topLevelItemCount() &&
           it != p_colorMap->overrideColors().end(); ++i, ++it)
  {
    p_colorMap->addOverrideColor(
        it->first, blitz::TinyVector<float,3>(
            reinterpret_cast<ColorControlElement*>(
                p_overrideColors->itemWidget(
                    p_overrideColors->topLevelItem(i), 1))->value()) /
        255.0f);
  }
  p_colorMap->setColorMapEditor(this);
  emit colorMapChanged();  
}

void ColorMapEditorWidget::processOverrideColorSelectionChange()
{
  p_removeOverrideColor->setEnabled(
      p_overrideColors->topLevelItemCount() != 0 &&
      p_overrideColors->selectedItems().size() != 0);
}

void ColorMapEditorWidget::processRangeIndicatorChange()
{
  if (p_rangeIndicator->value() == p_colorMap->rangeIndicatorEnabled()) return;
  p_colorMap->setRangeIndicatorEnabled(p_rangeIndicator->value());
  emit colorMapChanged();
}

void ColorMapEditorWidget::toggleAdvanced(bool toggle)
{
  if (toggle) p_toggleAdvancedButton->setArrowType(Qt::UpArrow);
  else p_toggleAdvancedButton->setArrowType(Qt::DownArrow);
  p_advancedWidget->setVisible(toggle);
}

void ColorMapEditorWidget::updateColorMapPreview()
{
  if (p_colorMap == NULL)
  {
    p_preview->clear();
    return;
  }
  
  QImage image(p_preview->width(), p_preview->height(), QImage::Format_RGB32);
#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (int y = 0; y < p_preview->height(); ++y)
  {
    for (int x = 0; x < p_preview->width(); ++x)
    {
      blitz::TinyVector<float,3> color = p_colorMap->color(
          static_cast<long long>(
              static_cast<double>(x) /
              static_cast<double>(p_preview->width() - 1) *
              static_cast<double>(p_colorMap->endIndex() -
                                  p_colorMap->startIndex()) + 0.5) +
              p_colorMap->startIndex());
      QRgb col =
          (static_cast<int>(255.0f * color(0)) << 16) +
          (static_cast<int>(255.0f * color(1)) << 8) +
          static_cast<int>(255.0f * color(2));
      image.setPixel(x, y, col);
    }
  }
  
  p_preview->setPixmap(QPixmap::fromImage(image));
}

void ColorMapEditorWidget::resizeEvent(QResizeEvent *)
{
  if (p_colorMap != NULL) updateColorMapPreview();
}

void ColorMapEditorWidget::setConnectionsEnabled(bool enable)
{
  if (enable == _connectionsEnabled) return;
  if (enable)
  {
    connect(p_preset, SIGNAL(activated(int)),
            SLOT(processPresetColorMapSelection()));
    connect(p_load, SIGNAL(triggered()), SLOT(processColorMapLoadAction()));
    connect(p_save, SIGNAL(triggered()), SLOT(processColorMapSaveAction()));
    connect(p_dataType, SIGNAL(activated(int)),
            SLOT(processPresetRangeSelection()));
    connect(p_startIndex, SIGNAL(valueChanged(int)),
            SLOT(processColorMapStartIndexChange()));
    connect(p_endIndex, SIGNAL(valueChanged(int)),
            SLOT(processColorMapEndIndexChange()));
    connect(p_gamma, SIGNAL(valueChanged(double)), SLOT(processGammaChange()));
    connect(p_rangeIndicator, SIGNAL(valueChanged()),
            SLOT(processRangeIndicatorChange()));
    connect(p_monochromeColor, SIGNAL(valueChanged()),
            SLOT(processMonochromeColorChange()));
    connect(p_intermediateColors->itemWidget(
                p_intermediateColors->topLevelItem(0), 1),
            SIGNAL(valueChanged()), SLOT(processStartColorChange()));
    for (int i = 1; i < p_intermediateColors->topLevelItemCount() - 1; ++i)
    {
      connect(p_intermediateColors->itemWidget(
                  p_intermediateColors->topLevelItem(i), 0),
              SIGNAL(valueChanged()),
              SLOT(processIntermediateColorPercentageChange()));
      connect(p_intermediateColors->itemWidget(
                  p_intermediateColors->topLevelItem(i), 1),
              SIGNAL(valueChanged()),
              SLOT(processIntermediateColorChange()));
    }
    connect(p_intermediateColors->itemWidget(
                p_intermediateColors->topLevelItem(
                    p_intermediateColors->topLevelItemCount() - 1), 1),
            SIGNAL(valueChanged()), SLOT(processEndColorChange()));
    connect(p_addIntermediateColor, SIGNAL(clicked()),
            SLOT(processIntermediateColorAddition()));
    connect(p_removeIntermediateColor, SIGNAL(clicked()),
          SLOT(processIntermediateColorDeletion()));
    connect(p_intermediateColors, SIGNAL(itemSelectionChanged()),
            SLOT(processIntermediateColorSelectionChange()));
    for (int i = 0; i < p_overrideColors->topLevelItemCount(); ++i)
    {
      connect(p_overrideColors->itemWidget(
                  p_overrideColors->topLevelItem(i), 0),
              SIGNAL(valueChanged()),
              SLOT(processOverrideColorIndexChange()));
      connect(p_overrideColors->itemWidget(
                  p_overrideColors->topLevelItem(i), 1),
              SIGNAL(valueChanged()),
              SLOT(processOverrideColorChange()));
    }
    connect(p_addOverrideColor, SIGNAL(clicked()),
            SLOT(processOverrideColorAddition()));
    connect(p_removeOverrideColor, SIGNAL(clicked()),
            SLOT(processOverrideColorDeletion()));
    connect(p_overrideColors, SIGNAL(itemSelectionChanged()),
            SLOT(processOverrideColorSelectionChange()));
  }
  else
  {
    disconnect(p_preset, SIGNAL(activated(int)),
               this, SLOT(processPresetColorMapSelection()));
    disconnect(p_load, SIGNAL(triggered()),
               this, SLOT(processColorMapLoadAction()));
    disconnect(p_save, SIGNAL(triggered()),
               this, SLOT(processColorMapSaveAction()));
    disconnect(p_dataType, SIGNAL(activated(int)),
               this, SLOT(processPresetRangeSelection()));
    disconnect(p_startIndex, SIGNAL(valueChanged(int)),
               this, SLOT(processColorMapStartIndexChange()));
    disconnect(p_endIndex, SIGNAL(valueChanged(int)),
               this, SLOT(processColorMapEndIndexChange()));
    disconnect(p_gamma, SIGNAL(valueChanged(double)),
               this, SLOT(processGammaChange()));
    disconnect(p_rangeIndicator, SIGNAL(valueChanged()),
               this, SLOT(processRangeIndicatorChange()));
    disconnect(p_monochromeColor, SIGNAL(valueChanged()),
               this, SLOT(processMonochromeColorChange()));
    disconnect(p_intermediateColors->itemWidget(
                   p_intermediateColors->topLevelItem(0), 1),
               SIGNAL(valueChanged()),
               this, SLOT(processStartColorChange()));
    for (int i = 1; i < p_intermediateColors->topLevelItemCount() - 1; ++i)
    {
      disconnect(p_intermediateColors->itemWidget(
                     p_intermediateColors->topLevelItem(i), 0),
                 SIGNAL(valueChanged()),
                 this, SLOT(processIntermediateColorPercentageChange()));
      disconnect(p_intermediateColors->itemWidget(
                     p_intermediateColors->topLevelItem(i), 1),
                 SIGNAL(valueChanged()),
                 this, SLOT(processIntermediateColorChange()));
    }
    disconnect(p_intermediateColors->itemWidget(
                   p_intermediateColors->topLevelItem(
                       p_intermediateColors->topLevelItemCount() - 1), 1),
               SIGNAL(valueChanged()),
               this, SLOT(processEndColorChange()));
    disconnect(p_addIntermediateColor, SIGNAL(clicked()),
               this, SLOT(processIntermediateColorAddition()));
    disconnect(p_removeIntermediateColor, SIGNAL(clicked()),
               this, SLOT(processIntermediateColorDeletion()));
    disconnect(p_intermediateColors, SIGNAL(itemSelectionChanged()),
               this, SLOT(processIntermediateColorSelectionChange()));
    for (int i = 0; i < p_overrideColors->topLevelItemCount(); ++i)
    {
      disconnect(p_overrideColors->itemWidget(
                     p_overrideColors->topLevelItem(i), 0),
                 SIGNAL(valueChanged()),
                 this, SLOT(processOverrideColorIndexChange()));
      disconnect(p_overrideColors->itemWidget(
                     p_overrideColors->topLevelItem(i), 1),
                 SIGNAL(valueChanged()),
                 this, SLOT(processOverrideColorChange()));
    }
    disconnect(p_addOverrideColor, SIGNAL(clicked()),
               this, SLOT(processOverrideColorAddition()));
    disconnect(p_removeOverrideColor, SIGNAL(clicked()),
               this, SLOT(processOverrideColorDeletion()));
    disconnect(p_overrideColors, SIGNAL(itemSelectionChanged()),
               this, SLOT(processOverrideColorSelectionChange()));
  }
  _connectionsEnabled = enable;
}
