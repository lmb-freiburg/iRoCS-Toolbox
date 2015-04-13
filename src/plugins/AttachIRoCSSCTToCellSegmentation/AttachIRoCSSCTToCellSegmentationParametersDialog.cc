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

#include "AttachIRoCSSCTToCellSegmentationParametersDialog.hh"

#include <QtGui/QDialogButtonBox>
#include <QtGui/QMessageBox>
#include <QtCore/QSettings>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>

#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/VisualizationChannelSpecs.hh>

AttachIRoCSSCTToCellSegmentationParametersDialog::
AttachIRoCSSCTToCellSegmentationParametersDialog(
    LabellingMainWidget* mainWidget, QWidget* parent, Qt::WindowFlags f)
        : QDialog(parent, f), p_mainWidget(mainWidget)
{
  QVBoxLayout* mainLayout = new QVBoxLayout;
  setLayout(mainLayout);

  QFrame* parameterPanel = new QFrame;
  parameterPanel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  QVBoxLayout* parameterLayout = new QVBoxLayout;
  parameterPanel->setLayout(parameterLayout);
  
  p_segmentationChannelSelector = new ChannelSelectionControlElement(
      tr("Segmentation channel"), mainWidget->multiChannelModel(),
      ChannelSpecs::Visualization);
  p_segmentationChannelSelector->setToolTip(
      tr("Select the channel containing the cellular segmentation"));
  parameterLayout->addWidget(p_segmentationChannelSelector);
  p_qcChannelSelector = new ChannelSelectionControlElement(
      tr("QC channel"), mainWidget->multiChannelModel(),
      ChannelSpecs::Annotation);
  p_qcChannelSelector->setToolTip(
      tr("Select the channel containing the QC marker. Only the first marker "
         "in this channel will be used. At best it only contains one marker."));
  for (std::vector<ChannelSpecs*>::const_iterator it =
           p_mainWidget->multiChannelModel()->begin();
       it != p_mainWidget->multiChannelModel()->end(); ++it)
  {
    if ((*it)->name().find("qc") != std::string::npos ||
        (*it)->name().find("QC") != std::string::npos)
    {
      p_qcChannelSelector->setValue((*it)->name());
      break;
    }
  }
  parameterLayout->addWidget(p_qcChannelSelector);

  p_iRoCSChannelNameControlElement = new StringControlElement(
      tr("Axis channel"), "/annotation/axis");
  p_iRoCSChannelNameControlElement->setToolTip(
      tr("Set the name of the output channel the fitted iRoCS model will be "
         "showed in. You can change the name later using the channel control "
         "dock."));
  parameterLayout->addWidget(p_iRoCSChannelNameControlElement);  

  QGroupBox *iRoCSParameters = new QGroupBox(tr("iRoCS parameters"));
  QVBoxLayout *iRoCSParametersLayout = new QVBoxLayout;
  iRoCSParameters->setLayout(iRoCSParametersLayout);
  
  p_backgroundLabelControl = new IntControlElement(tr("Background label"), 0);
  p_backgroundLabelControl->setToolTip(
      tr("Pixels with this value in the segmentation will be regarded as "
         "background. Voxels with label <=0 will always be treated as "
         "background independent of this choice."));
  p_backgroundLabelControl->setRange(0, INT_MAX);
  iRoCSParametersLayout->addWidget(p_backgroundLabelControl);
  
  p_segmentLengthUmControl = new DoubleControlElement(
      tr("Segment length [um]"), 10.0);
  p_segmentLengthUmControl->setToolTip(
      tr("The axis will be sampled with line segments of this length."));
  p_segmentLengthUmControl->setRange(1.0, 10000.0);
  p_segmentLengthUmControl->setSingleStep(0.1);
  iRoCSParametersLayout->addWidget(p_segmentLengthUmControl);
  
  p_marginUmControl = new DoubleControlElement(tr("Margin [um]"), 30.0);
  p_marginUmControl->setToolTip(
      tr("Voxels closer to the boundary as margin will be ignored."));
  p_marginUmControl->setRange(0.0, 10000.0);
  p_marginUmControl->setSingleStep(0.1);
  iRoCSParametersLayout->addWidget(p_marginUmControl);
  
  p_downsampleRatioControl = new IntControlElement(tr("Downsample ratio"), 4);
  p_downsampleRatioControl->setToolTip(
      tr("Only every downsample ratio's boundary voxel will be used during the "
         "fit."));
  p_downsampleRatioControl->setRange(1, INT_MAX);
  iRoCSParametersLayout->addWidget(p_downsampleRatioControl);

  parameterLayout->addWidget(iRoCSParameters);
  
  mainLayout->addWidget(parameterPanel);
  
  QDialogButtonBox* buttonBox = new QDialogButtonBox(
      QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
  connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
          SLOT(checkAndAccept()));
  buttonBox->button(QDialogButtonBox::Cancel)->setAutoDefault(false);
  connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
          SLOT(reject()));
  mainLayout->addWidget(buttonBox);

  // Load field values from settings
  QSettings settings;

  QString segmentationName;
  QString qcName;
  QString iRoCSName;
  int backgroundLabel, downsampleRatio;
  double segmentLengthUm, marginUm;

  if (p_mainWidget->fileName() == "")
  {
    segmentationName = settings.value(
        "iRoCSPipeline/SegmentationChannel",
        p_segmentationChannelSelector->toStdString().c_str()).toString();
    qcName = settings.value(
        "iRoCSPipeline/QcChannel",
        p_qcChannelSelector->toStdString().c_str()).toString();
    iRoCSName = settings.value(
        "iRoCSPipeline/AxisChannel", "/annotation/axis").toString();
    backgroundLabel =
        settings.value("iRoCSPipeline/backgroundLabel", 1).toInt();
    downsampleRatio =
        settings.value("iRoCSPipeline/downsampleRatio", 4).toInt();
    segmentLengthUm =
        settings.value("iRoCSPipeline/segmentLengthUm", 10.0).toDouble();
    marginUm = settings.value("iRoCSPipeline/marginUm", 30.0).toDouble();
  }
  else
  {
    segmentationName = settings.value(
        ("iRoCSPipeline/" + p_mainWidget->fileName() +
         "/SegmentationChannel").c_str(),
        p_segmentationChannelSelector->toStdString().c_str()).toString();
    qcName = settings.value(
        ("iRoCSPipeline/" + p_mainWidget->fileName() + "/QcChannel").c_str(),
        p_qcChannelSelector->toStdString().c_str()).toString();
    iRoCSName = settings.value(
        ("iRoCSPipeline/" + p_mainWidget->fileName() + "/AxisChannel").c_str(),
        "/annotation/axis").toString();
    backgroundLabel = settings.value(
        ("iRoCSPipeline/" + p_mainWidget->fileName() +
         "/backgroundLabel").c_str(), 1).toInt();
    downsampleRatio = settings.value(
        ("iRoCSPipeline/" + p_mainWidget->fileName() +
         "/downsampleRatio").c_str(), 4).toInt();
    segmentLengthUm = settings.value(
        ("iRoCSPipeline/" + p_mainWidget->fileName() +
         "/segmentLengthUm").c_str(), 10.0).toDouble();
    marginUm = settings.value(
        ("iRoCSPipeline/" + p_mainWidget->fileName() + "/marginUm").c_str(),
        30.0).toDouble();
  }
  if (p_segmentationChannelSelector->contains(segmentationName.toStdString()))
      p_segmentationChannelSelector->setValue(segmentationName.toStdString());
  if (p_qcChannelSelector->contains(qcName.toStdString()))
      p_qcChannelSelector->setValue(qcName.toStdString());
  p_iRoCSChannelNameControlElement->setValue(iRoCSName.toStdString());
  p_backgroundLabelControl->setValue(backgroundLabel);
  p_downsampleRatioControl->setValue(downsampleRatio);
  p_segmentLengthUmControl->setValue(segmentLengthUm);
  p_marginUmControl->setValue(marginUm);
}

AttachIRoCSSCTToCellSegmentationParametersDialog::
~AttachIRoCSSCTToCellSegmentationParametersDialog()
{}

VisualizationChannelSpecs*
AttachIRoCSSCTToCellSegmentationParametersDialog::segmentationChannel() const
{
  return static_cast<VisualizationChannelSpecs*>(
      p_segmentationChannelSelector->selectedChannel());
}

void AttachIRoCSSCTToCellSegmentationParametersDialog::setSegmentationChannel(
    VisualizationChannelSpecs *channel)
{
  if (p_segmentationChannelSelector->contains(channel))
      p_segmentationChannelSelector->setValue(channel);
}

AnnotationChannelSpecs*
AttachIRoCSSCTToCellSegmentationParametersDialog::qcChannel() const
{
  return static_cast<AnnotationChannelSpecs*>(
      p_qcChannelSelector->selectedChannel());
}

void AttachIRoCSSCTToCellSegmentationParametersDialog::setQcChannel(
    AnnotationChannelSpecs *channel)
{
  if (p_qcChannelSelector->contains(channel))
      p_qcChannelSelector->setValue(channel);
}

std::string
AttachIRoCSSCTToCellSegmentationParametersDialog::iRoCSChannelName() const
{
  return p_iRoCSChannelNameControlElement->value();
}

void AttachIRoCSSCTToCellSegmentationParametersDialog::setIRoCSChannelName(
    std::string const &name)
{
  p_iRoCSChannelNameControlElement->setValue(name);
}

int AttachIRoCSSCTToCellSegmentationParametersDialog::backgroundLabel() const
{
  return p_backgroundLabelControl->value();
}

void AttachIRoCSSCTToCellSegmentationParametersDialog::setBackgroundLabel(
    int backgroundLabel)
{
  p_backgroundLabelControl->setValue(backgroundLabel);
}

double AttachIRoCSSCTToCellSegmentationParametersDialog::segmentLengthUm() const
{
  return p_segmentLengthUmControl->value();
}

void AttachIRoCSSCTToCellSegmentationParametersDialog::setSegmentLengthUm(
    double segmentLengthUm)
{
  p_segmentLengthUmControl->setValue(segmentLengthUm);
}

double AttachIRoCSSCTToCellSegmentationParametersDialog::marginUm() const
{
  return p_marginUmControl->value();
}

void AttachIRoCSSCTToCellSegmentationParametersDialog::setMarginUm(
    double marginUm)
{
  p_marginUmControl->setValue(marginUm);
}

int AttachIRoCSSCTToCellSegmentationParametersDialog::downsampleRatio() const
{
  return p_downsampleRatioControl->value();
}

void AttachIRoCSSCTToCellSegmentationParametersDialog::setDownsampleRatio(
    int downsampleRatio)
{
  p_downsampleRatioControl->setValue(downsampleRatio);
}

std::string AttachIRoCSSCTToCellSegmentationParametersDialog::check()
{
  try
  {
    // Check whether the selected QC channel contains markers
    if (qcChannel()->markers().size() == 0) return "No QC marker found";

    if (iRoCSChannelName() == "") return "No axis channel name specified";
  }
  catch (std::exception& e)
  {
    return std::string("Error while checking the parameters: ") + e.what();
  }
  return "";
}

void AttachIRoCSSCTToCellSegmentationParametersDialog::checkAndAccept()
{
  std::string res = check();
  
  if (res != "")
  {
    QMessageBox::warning(
        p_mainWidget, tr("Plugin prerequisites not met"), res.c_str());
    return;
  }
  
  QSettings settings;

  if (p_mainWidget->fileName() == "")
  {
    settings.setValue(
        "iRoCSPipeline/SegmentationChannel",
        p_segmentationChannelSelector->toStdString().c_str());
    settings.setValue(
        "iRoCSPipeline/QcChannel",
        p_qcChannelSelector->toStdString().c_str());
    settings.setValue(
        "iRoCSPipeline/AxisChannel",
        p_iRoCSChannelNameControlElement->toStdString().c_str());
    settings.setValue(
        "iRoCSPipeline/backgroundLabel", p_backgroundLabelControl->value());
    settings.setValue(
        "iRoCSPipeline/segmentLengthUm", p_segmentLengthUmControl->value());
    settings.setValue("iRoCSPipeline/marginUm", p_marginUmControl->value());
    settings.setValue("iRoCSPipeline/downsampleRatio",
                      p_downsampleRatioControl->value());
  }
  else
  {
    settings.setValue(
        ("iRoCSPipeline/" + p_mainWidget->fileName() +
         "/SegmentationChannel").c_str(),
        p_segmentationChannelSelector->toStdString().c_str());
    settings.setValue(
        ("iRoCSPipeline/" + p_mainWidget->fileName() + "/QcChannel").c_str(),
        p_qcChannelSelector->toStdString().c_str());
    settings.setValue(
        ("iRoCSPipeline/" + p_mainWidget->fileName() + "/AxisChannel").c_str(),
        p_iRoCSChannelNameControlElement->toStdString().c_str());
    settings.setValue(
        ("iRoCSPipeline/" + p_mainWidget->fileName() +
         "/backgroundLabel").c_str(), p_backgroundLabelControl->value());
    settings.setValue(
        ("iRoCSPipeline/" + p_mainWidget->fileName() +
         "/segmentLengthUm").c_str(), p_segmentLengthUmControl->value());
    settings.setValue(
        ("iRoCSPipeline/" + p_mainWidget->fileName() +
         "/marginUm").c_str(), p_marginUmControl->value());
    settings.setValue(
        ("iRoCSPipeline/" + p_mainWidget->fileName() +
         "/downsampleRatio").c_str(), p_downsampleRatioControl->value());
  }
  
  accept();
}
