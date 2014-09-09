#include "AssignLayersToCellSegmentationParametersDialog.hh"

#include <QtGui/QDialogButtonBox>
#include <QtGui/QMessageBox>
#include <QtCore/QSettings>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>

#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/VisualizationChannelSpecs.hh>

AssignLayersToCellSegmentationParametersDialog::
AssignLayersToCellSegmentationParametersDialog(
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

  p_iRoCSChannelSelector = new ChannelSelectionControlElement(
      tr("iRoCS channel"), mainWidget->multiChannelModel(),
      ChannelSpecs::IRoCSSCT);
  p_iRoCSChannelSelector->setToolTip(
      tr("Select the channel containing the iRoCS shell coordinate transform."));
  parameterLayout->addWidget(p_iRoCSChannelSelector);

  p_outFileControlElement = new FileNameSelectionControlElement(
      tr("Output file"), "", false, "*.h5");
  p_outFileControlElement->setToolTip(
      tr("Set the name of the output file to store the layer labels to."));
  parameterLayout->addWidget(p_outFileControlElement);  

  p_labelNameControlElement = new StringControlElement(
      tr("Label dataset"), "/predicted_labels");
  p_labelNameControlElement->setToolTip(
      tr("Set the name of the dataset to store the layer labels to."));
  parameterLayout->addWidget(p_labelNameControlElement);  

  p_modelFileControlElement = new FileNameSelectionControlElement(
      tr("Model file"), "", true, "*.h5");
  p_modelFileControlElement->setToolTip(
      tr("Select the random forest model for layer label prediction."));
  parameterLayout->addWidget(p_modelFileControlElement);  

  p_featureFileControlElement = new FileNameSelectionControlElement(
      tr("Feature file"), "", false, "*.h5");
  p_featureFileControlElement->setToolTip(
      tr("Set the name of the hdf5 file to store the computed features to."));
  parameterLayout->addWidget(p_featureFileControlElement);  

  p_featureGroupControlElement = new StringControlElement(
      tr("Feature group"), "/features_SCT");
  p_featureGroupControlElement->setToolTip(
      tr("Set the name of the hdf5 group to store the computed features to."));
  parameterLayout->addWidget(p_featureGroupControlElement);  

  QGroupBox *classificationParameters = new QGroupBox(
      tr("Classification parameters"));
  QVBoxLayout *classificationParametersLayout = new QVBoxLayout;
  classificationParameters->setLayout(classificationParametersLayout);
  
  p_volumeThresholdUm3Control = new DoubleControlElement(
      tr("Minimum segment volume [um^3]"), 60.0);
  p_volumeThresholdUm3Control->setToolTip(
      tr("Segments with volumes below this value will be treated as "
         "backgound."));
  p_volumeThresholdUm3Control->setRange(0.0, 1000000.0);
  p_volumeThresholdUm3Control->setSingleStep(1.0);
  classificationParametersLayout->addWidget(p_volumeThresholdUm3Control);
  
  p_backgroundLabelControl = new IntControlElement(tr("Background label"), 0);
  p_backgroundLabelControl->setToolTip(
      tr("Pixels with this value in the segmentation will be regarded as "
         "background. Voxels with label <=0 will always be treated as "
         "background independent of this choice."));
  p_backgroundLabelControl->setRange(0, INT_MAX);
  classificationParametersLayout->addWidget(p_backgroundLabelControl);
  
  parameterLayout->addWidget(classificationParameters);
  
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

  QString segmentationName, iRoCSName, outFileName, labelName, modelFileName;
  QString featureFileName, featureGroup;
  double volumeThresholdUm3;
  int backgroundLabel;

  if (p_mainWidget->fileName() == "")
  {
    segmentationName = settings.value(
        "iRoCSPipeline/segmentationChannel",
        p_segmentationChannelSelector->toStdString().c_str()).toString();
    iRoCSName = settings.value(
        "iRoCSPipeline/iRoCSSCTChannel",
        p_iRoCSChannelSelector->toStdString().c_str()).toString();
    outFileName = settings.value("iRoCSPipeline/outFileName", "").toString();
    labelName = settings.value(
        "iRoCSPipeline/labelName", "/predicted_labels").toString();
    modelFileName = settings.value(
        "iRoCSPipeline/modelFileName", "").toString();
    featureFileName = settings.value(
        "iRoCSPipeline/featureFileName", "").toString();
    featureGroup = settings.value(
        "iRoCSPipeline/featureGroup", "/features_SCT").toString();
    volumeThresholdUm3 =
        settings.value("iRoCSPipeline/volumeThresholdUm3", 60.0).toDouble();
    backgroundLabel =
        settings.value("iRoCSPipeline/backgroundLabel", 1).toInt();
  }
  else
  {
    segmentationName = settings.value(
        ("iRoCSPipeline/segmentationChannel/" +
         p_mainWidget->fileName()).c_str(),
        p_segmentationChannelSelector->toStdString().c_str()).toString();
    iRoCSName = settings.value(
        ("iRoCSPipeline/iRoCSSCTChannel/" +
         p_mainWidget->fileName()).c_str(),
        p_iRoCSChannelSelector->toStdString().c_str()).toString();
    outFileName = settings.value(
        ("iRoCSPipeline/outFileName/" +
         p_mainWidget->fileName()).c_str(),
        p_mainWidget->fileName().c_str()).toString();
    labelName = settings.value(
        ("iRoCSPipeline/labelName/" +
         p_mainWidget->fileName()).c_str(), "/predicted_labels").toString();
    modelFileName = settings.value(
        ("iRoCSPipeline/modelFileName/" +
         p_mainWidget->fileName()).c_str(), "").toString();
    featureFileName = settings.value(
        ("iRoCSPipeline/featureFileName/" +
         p_mainWidget->fileName()).c_str(),
        p_mainWidget->fileName().c_str()).toString();
    featureGroup = settings.value(
        ("iRoCSPipeline/featureGroup/" +
         p_mainWidget->fileName()).c_str(), "/features_SCT").toString();
    volumeThresholdUm3 = settings.value(
        ("iRoCSPipeline/volumeThresholdUm3/" +
         p_mainWidget->fileName()).c_str(), 60.0).toDouble();
    backgroundLabel = settings.value(
        ("iRoCSPipeline/backgroundLabel/" +
         p_mainWidget->fileName()).c_str(), 1).toInt();
  }
  p_segmentationChannelSelector->setValue(segmentationName.toStdString());
  p_iRoCSChannelSelector->setValue(iRoCSName.toStdString());
  p_outFileControlElement->setValue(outFileName.toStdString());
  p_labelNameControlElement->setValue(labelName.toStdString());
  p_modelFileControlElement->setValue(modelFileName.toStdString());
  p_featureFileControlElement->setValue(featureFileName.toStdString());
  p_volumeThresholdUm3Control->setValue(volumeThresholdUm3);
  p_backgroundLabelControl->setValue(backgroundLabel);
}

AssignLayersToCellSegmentationParametersDialog::
~AssignLayersToCellSegmentationParametersDialog()
{}

VisualizationChannelSpecs*
AssignLayersToCellSegmentationParametersDialog::segmentationChannel() const
{
  return static_cast<VisualizationChannelSpecs*>(
      p_segmentationChannelSelector->selectedChannel());
}

void AssignLayersToCellSegmentationParametersDialog::setSegmentationChannel(
    VisualizationChannelSpecs *channel)
{
  p_segmentationChannelSelector->setValue(channel);
}

IRoCSSCTChannelSpecs*
AssignLayersToCellSegmentationParametersDialog::iRoCSChannel() const
{
  return static_cast<IRoCSSCTChannelSpecs*>(
      p_iRoCSChannelSelector->selectedChannel());
}

void AssignLayersToCellSegmentationParametersDialog::setIRoCSChannel(
    IRoCSSCTChannelSpecs *channel)
{
  p_iRoCSChannelSelector->setValue(channel);
}

std::string AssignLayersToCellSegmentationParametersDialog::outFileName() const
{
  return p_outFileControlElement->value();
}

void AssignLayersToCellSegmentationParametersDialog::setOutFileName(
    std::string const &outFileName)
{
  p_outFileControlElement->setValue(outFileName);
}

std::string AssignLayersToCellSegmentationParametersDialog::labelName() const
{
  return p_labelNameControlElement->value();
}

void AssignLayersToCellSegmentationParametersDialog::setLabelName(
    std::string const &labelName)
{
  p_labelNameControlElement->setValue(labelName);
}

std::string AssignLayersToCellSegmentationParametersDialog::modelFileName() const
{
  return p_modelFileControlElement->value();
}

void AssignLayersToCellSegmentationParametersDialog::setModelFileName(
    std::string const &modelFileName)
{
  p_modelFileControlElement->setValue(modelFileName);
}

std::string AssignLayersToCellSegmentationParametersDialog::featureFileName()
    const
{
  return p_featureFileControlElement->value();
}

void AssignLayersToCellSegmentationParametersDialog::setFeatureFileName(
    std::string const &featureFileName)
{
  p_featureFileControlElement->setValue(featureFileName);
}

std::string AssignLayersToCellSegmentationParametersDialog::featureGroup()
    const
{
  return p_featureGroupControlElement->value();
}

void AssignLayersToCellSegmentationParametersDialog::setFeatureGroup(
    std::string const &featureGroup)
{
  p_featureGroupControlElement->setValue(featureGroup);
}

double AssignLayersToCellSegmentationParametersDialog::volumeThresholdUm3()
    const
{
  return p_volumeThresholdUm3Control->value();
}

void AssignLayersToCellSegmentationParametersDialog::setVolumeThresholdUm3(
    double volumeThresholdUm3)
{
  p_volumeThresholdUm3Control->setValue(volumeThresholdUm3);
}

int AssignLayersToCellSegmentationParametersDialog::backgroundLabel() const
{
  return p_backgroundLabelControl->value();
}

void AssignLayersToCellSegmentationParametersDialog::setBackgroundLabel(
    int backgroundLabel)
{
  p_backgroundLabelControl->setValue(backgroundLabel);
}

std::string AssignLayersToCellSegmentationParametersDialog::check()
{
  try
  {
    if (outFileName() == "") return "No output file specified";
    if (labelName() == "") return "No label name specified";
    if (modelFileName() == "") return "No model file selected";
    if (featureFileName() == "") return "No feature file name selected";
  }
  catch (std::exception& e)
  {
    return std::string("Error while checking the parameters: ") + e.what();
  }
  return "";
}

void AssignLayersToCellSegmentationParametersDialog::checkAndAccept()
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
        "iRoCSPipeline/segmentationChannel",
        segmentationChannel()->name().c_str());
    settings.setValue(
        "iRoCSPipeline/iRoCSSCTChannel", iRoCSChannel()->name().c_str());
    settings.setValue("iRoCSPipeline/outFileName", outFileName().c_str());
    settings.setValue("iRoCSPipeline/labelName", labelName().c_str());
    settings.setValue("iRoCSPipeline/modelFileName", modelFileName().c_str());
    settings.setValue(
        "iRoCSPipeline/featureFileName", featureFileName().c_str());
    settings.setValue("iRoCSPipeline/featureGroup", featureGroup().c_str());
    settings.setValue("iRoCSPipeline/volumeThresholdUm3", volumeThresholdUm3());
    settings.setValue("iRoCSPipeline/backgroundLabel", backgroundLabel());
  }
  else
  {
    settings.setValue(
        ("iRoCSPipeline/segmentationChannel/" +
         p_mainWidget->fileName()).c_str(),
        segmentationChannel()->name().c_str());
    settings.setValue(
        ("iRoCSPipeline/iRoCSSCTChannel/" + p_mainWidget->fileName()).c_str(),
        iRoCSChannel()->name().c_str());
    settings.setValue(
        ("iRoCSPipeline/outFileName/" + p_mainWidget->fileName()).c_str(),
        outFileName().c_str());
    settings.setValue(
        ("iRoCSPipeline/labelName/" + p_mainWidget->fileName()).c_str(),
        labelName().c_str());
    settings.setValue(
        ("iRoCSPipeline/modelFileName/" + p_mainWidget->fileName()).c_str(),
         modelFileName().c_str());
    settings.setValue(
        ("iRoCSPipeline/featureFileName/" + p_mainWidget->fileName()).c_str(),
        featureFileName().c_str());
    settings.setValue(
        ("iRoCSPipeline/featureGroup/" + p_mainWidget->fileName()).c_str(),
        featureGroup().c_str());
    settings.setValue(
        ("iRoCSPipeline/volumeThresholdUm3/" + p_mainWidget->fileName()).c_str(),
        volumeThresholdUm3());
    settings.setValue(
        ("iRoCSPipeline/backgroundLabel/" + p_mainWidget->fileName()).c_str(),
        backgroundLabel());
  }
  
  accept();
}
