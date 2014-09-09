#include "TrainDetectorParametersDialog.hh"

#include <QtGui/QMessageBox>
#include <QtGui/QDialogButtonBox>
#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <QtGui/QFileDialog>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>

#include "TrainDetectorTrainfileParametersItem.hh"

TrainDetectorParametersDialog::TrainDetectorParametersDialog(
    LabellingMainWidget* mainWidget, QWidget* parent, Qt::WindowFlags f)
        : QDialog(parent, f), TrainingParameters(),
          p_mainWidget(mainWidget)
{
  assert(mainWidget != NULL);
  QVBoxLayout* mainLayout = new QVBoxLayout;

  QFrame* parameterPanel = new QFrame;
  parameterPanel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  QVBoxLayout* parameterLayout = new QVBoxLayout;
  
  p_trainFileList = new QTreeWidget;
  p_trainFileList->setColumnCount(4);
  QTreeWidgetItem *trainFileListHeader = new QTreeWidgetItem;
  trainFileListHeader->setText(0, tr("File"));
  trainFileListHeader->setText(1, tr("Data channel"));
  trainFileListHeader->setText(2, tr("Annotation Channel"));
  trainFileListHeader->setText(3, tr("Cache file"));
  p_trainFileList->setHeaderItem(trainFileListHeader);
  p_trainFileList->setSelectionMode(QAbstractItemView::NoSelection);
  parameterLayout->addWidget(p_trainFileList);

  p_addTrainFileButton = new QPushButton(tr("Add train file")); 
  QObject::connect(p_addTrainFileButton, SIGNAL(clicked()),
                   SLOT(addTrainFile()));
  parameterLayout->addWidget(p_addTrainFileButton);

  p_featureGroupControlElement =
      new StringControlElement(tr("Feature Group:"), "/features");
  parameterLayout->addWidget(p_featureGroupControlElement);

  QStringList options;
  options << iRoCS::Features::normalizationTypeToString(
      iRoCS::Features::None).c_str()
          << iRoCS::Features::normalizationTypeToString(
              iRoCS::Features::FeatureZeroMeanStddev).c_str()
          << iRoCS::Features::normalizationTypeToString(
              iRoCS::Features::UnitNorm).c_str()
          << iRoCS::Features::normalizationTypeToString(
              iRoCS::Features::Combined).c_str();
  p_sdNormalizationControl = new StringSelectionControlElement(
      tr("Gauss-Laguerre feature normalization"), options);
  parameterLayout->addWidget(p_sdNormalizationControl);
  
  p_houghNormalizationControl = new StringSelectionControlElement(
      tr("Hough feature normalization"), options);
  parameterLayout->addWidget(p_houghNormalizationControl);
  
  p_randomSamplesGroup = new QGroupBox(
      tr("Generate Random Background Samples"));
  p_randomSamplesGroup->setCheckable(true);
  p_randomSamplesGroup->setChecked(false);
  QVBoxLayout* randomSamplesGroupLayout = new QVBoxLayout;
  p_randomSamplesGroup->setLayout(randomSamplesGroupLayout);
  p_nInRootSamplesControlElement = new IntControlElement(
      tr("# in-root samples:"), 0);
  p_nInRootSamplesControlElement->setRange(0, INT_MAX);
  randomSamplesGroupLayout->addWidget(p_nInRootSamplesControlElement);
  p_nOutRootSamplesControlElement = new IntControlElement(
      tr("# out-of-root samples:"), 0);
  p_nOutRootSamplesControlElement->setRange(0, INT_MAX);
  randomSamplesGroupLayout->addWidget(p_nOutRootSamplesControlElement);
  parameterLayout->addWidget(p_randomSamplesGroup);
  
  QGroupBox* kernelParameterGroup = new QGroupBox(tr("SVM parameters"));
  QVBoxLayout* kernelParameterGroupLayout = new QVBoxLayout;
  kernelParameterGroup->setLayout(kernelParameterGroupLayout);
  p_modelFileNameControlElement = new FileNameSelectionControlElement(
      tr("Model File:"), "", false, "HDF5 (*.h5)");
  p_modelFileNameControlElement->setValue("detector_model.h5");
  kernelParameterGroupLayout->addWidget(p_modelFileNameControlElement);
  p_costControlElement = new DoubleControlElement(tr("C:"), 10.0);
  p_costControlElement->setRange(0.0, std::numeric_limits<double>::infinity());
  kernelParameterGroupLayout->addWidget(p_costControlElement);
  p_gammaControlElement = new DoubleControlElement(tr("gamma:"), 0.01);
  p_gammaControlElement->setRange(0.0, std::numeric_limits<double>::infinity());
  kernelParameterGroupLayout->addWidget(p_gammaControlElement);
  parameterLayout->addWidget(kernelParameterGroup);

  parameterPanel->setLayout(parameterLayout);
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

  QSettings settings;
  p_featureGroupControlElement->setValue(
      settings.value("iRoCSPipeline/featureGroup",
                     "/features").toString().toStdString());
  p_randomSamplesGroup->setChecked(
      settings.value(
          "iRoCSPipeline/TrainDetector/generateRandomSamples", false).toBool());
  p_nInRootSamplesControlElement->setValue(
      settings.value("iRoCSPipeline/TrainDetector/nInRootSamples", 0).toInt());
  p_nOutRootSamplesControlElement->setValue(
      settings.value("iRoCSPipeline/TrainDetector/nOutRootSamples", 0).toInt());
  p_modelFileNameControlElement->setValue(
      settings.value("iRoCSPipeline/DetectorModel",
                     "detector_model.h5").toString().toStdString());
  p_sdNormalizationControl->setValue(
      static_cast<iRoCS::Features::NormalizationType>(
          settings.value(
              "iRoCSPipeline/TrainDetector/sdNormalization", 3).toInt()));
  p_houghNormalizationControl->setValue(
      static_cast<iRoCS::Features::NormalizationType>(
          settings.value(
              "iRoCSPipeline/TrainDetector/houghNormalization", 1).toInt()));
  p_costControlElement->setValue(
      settings.value("iRoCSPipeline/TrainDetector/cost", 10.0).toDouble());
  p_gammaControlElement->setValue(
      settings.value("iRoCSPipeline/TrainDetector/gamma", 0.01).toDouble());  

  setLayout(mainLayout);
}

TrainDetectorParametersDialog::~TrainDetectorParametersDialog()
{}

std::vector<TrainfileParameters*>
TrainDetectorParametersDialog::trainFiles() const
{
  std::vector<TrainfileParameters*> res;
  for (int i = 0; i < p_trainFileList->topLevelItemCount(); ++i)
      res.push_back(static_cast<TrainDetectorTrainfileParametersItem*>(
                        p_trainFileList->topLevelItem(i)));
  return res;
}

void TrainDetectorParametersDialog::setFeatureGroup(std::string const &group)
{
  p_featureGroupControlElement->setValue(group);
}

std::string TrainDetectorParametersDialog::featureGroup() const
{
  return p_featureGroupControlElement->value();
}

void TrainDetectorParametersDialog::setGenerateRandomSamples(bool generate)
{
  p_randomSamplesGroup->setChecked(generate);
}

bool TrainDetectorParametersDialog::generateRandomSamples() const
{
  return p_randomSamplesGroup->isChecked();
}

void TrainDetectorParametersDialog::setNInRootSamples(int samples)
{
  p_nInRootSamplesControlElement->setValue(samples);
}

int TrainDetectorParametersDialog::nInRootSamples() const
{
  return p_nInRootSamplesControlElement->value();
}

void TrainDetectorParametersDialog::setNOutRootSamples(int samples)
{
  p_nOutRootSamplesControlElement->setValue(samples);
}

int TrainDetectorParametersDialog::nOutRootSamples() const
{
  return p_nOutRootSamplesControlElement->value();
}

void TrainDetectorParametersDialog::setModelFileName(std::string const &name)
{
  p_modelFileNameControlElement->setValue(name);
}

std::string TrainDetectorParametersDialog::modelFileName() const
{
  return p_modelFileNameControlElement->value();
}

void TrainDetectorParametersDialog::setSdFeatureNormalization(
    iRoCS::Features::NormalizationType normType)
{
  p_sdNormalizationControl->setValue(int(normType));
}

iRoCS::Features::NormalizationType
TrainDetectorParametersDialog::sdFeatureNormalization() const
{
  return static_cast<iRoCS::Features::NormalizationType>(
      p_sdNormalizationControl->value());
}

void TrainDetectorParametersDialog::setHoughFeatureNormalization(
    iRoCS::Features::NormalizationType normType)
{
   p_houghNormalizationControl->setValue(int(normType)); 
}

iRoCS::Features::NormalizationType
TrainDetectorParametersDialog::houghFeatureNormalization() const
{
  return static_cast<iRoCS::Features::NormalizationType>(
      p_houghNormalizationControl->value());
}

void TrainDetectorParametersDialog::setCost(double cost)
{
  p_costControlElement->setValue(cost);
}

double TrainDetectorParametersDialog::cost() const
{
  return p_costControlElement->value();
}

void TrainDetectorParametersDialog::setGamma(double gamma)
{
  p_gammaControlElement->setValue(gamma);
}

double TrainDetectorParametersDialog::gamma() const
{
  return p_gammaControlElement->value();
}

void TrainDetectorParametersDialog::checkAndAccept()
{
  // Check input files
  std::vector<TrainfileParameters*> files(trainFiles());
  if (files.size() == 0)
  {
    QMessageBox::information(
        p_mainWidget, tr("No input files selected"),
        tr("Please select at least one input file for training"));
    return;
  }

  int nPositiveSamples = 0, nSamples = 0;
  for (size_t i = 0; i < files.size(); ++i)
  {
    std::string res = files[i]->check(nPositiveSamples, nSamples);
    if (res != "")
    {
      QMessageBox::information(
          p_mainWidget, tr("Train file error"), res.c_str());
      return;
    }
  }

  if (nSamples == 0)
  {
    QMessageBox::critical(
        p_mainWidget, tr("No samples found"),
        tr("No training samples found in the given files"));
    return;
  }

  if (nPositiveSamples == 0)
  {
    QMessageBox::critical(
        p_mainWidget, tr("No positive samples found"),
        tr("No positive training samples found in the given files"));
    return;
  }

  if (nPositiveSamples == nSamples && nInRootSamples() + nOutRootSamples() == 0)
  {
    QMessageBox::critical(
        p_mainWidget, tr("No negative samples"),
        tr("No negative samples available for discriminative training. "
           "You need to generate random negative samples."));
    return;
  }
  
  // Check, whether model file can be created
  if (QFile::exists(modelFileName().c_str()))
  {
    QMessageBox::StandardButton res = QMessageBox::question(
        p_mainWidget, tr("Overwrite existing model"),
        tr("A model with name %1 already exists. Do you want to "
           "replace it?").arg(modelFileName().c_str()),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (res != QMessageBox::Yes) return;
    QFile file(modelFileName().c_str());
    bool fileOpen = file.open(QIODevice::ReadWrite);
    if (!fileOpen)
    {
      QMessageBox::critical(
          p_mainWidget, tr("Model file cannot be written"),
          tr("The file %1 cannot be opened in write-mode. Please make "
             "sure, that you have sufficient permissions.").arg(
                 modelFileName().c_str()));
      return;
    }
    file.close();
  }
  else
  {
    QFile file(modelFileName().c_str());
    bool fileOpen = file.open(QIODevice::ReadWrite);
    if (!fileOpen)
    {
      QMessageBox::critical(
          p_mainWidget, tr("Model file cannot be written"),
          tr("The file %1 cannot be created. Please make sure, that "
             "you have sufficient permissions.").arg(
                 modelFileName().c_str()));
      return;
    }
    file.close();
    file.remove();
  }

  QSettings settings;
  settings.setValue("iRoCSPipeline/featureGroup", featureGroup().c_str());
  settings.setValue(
      "iRoCSPipeline/TrainDetector/generateRandomSamples",
      generateRandomSamples());
  settings.setValue(
      "iRoCSPipeline/TrainDetector/nInRootSamples", nInRootSamples());
  settings.setValue(
      "iRoCSPipeline/TrainDetector/nOutRootSamples", nOutRootSamples());
  settings.setValue("iRoCSPipeline/DetectorModel", modelFileName().c_str());
  settings.setValue(
      "iRoCSPipeline/TrainDetector/sdNormalization",
      int(sdFeatureNormalization()));
  settings.setValue(
      "iRoCSPipeline/TrainDetector/houghNormalization",
      int(houghFeatureNormalization()));  
  settings.setValue("iRoCSPipeline/TrainDetector/cost", cost());
  settings.setValue("iRoCSPipeline/TrainDetector/gamma", gamma());  

  accept();
}

void TrainDetectorParametersDialog::addTrainFile()
{
  QSettings settings;
  QString startPath = settings.value(
      "FileDialog/startpath", getenv("HOME")).toString();
  QString filename = QFileDialog::getOpenFileName(
      p_mainWidget, tr("Select train file"), startPath, tr("hdf5 (*.h5)"));
  if (filename == QString::Null()) return;
  TrainDetectorTrainfileParametersItem *trainFile =
      new TrainDetectorTrainfileParametersItem(
          p_mainWidget, filename.toStdString());
  p_trainFileList->addTopLevelItem(trainFile);
  p_trainFileList->setItemWidget(
      trainFile, 0, trainFile->p_trainFileSelector);
  p_trainFileList->setItemWidget(
      trainFile, 1, trainFile->p_dataChannelControlElement);
  p_trainFileList->setItemWidget(
      trainFile, 2, trainFile->p_annotationChannelControlElement);
  p_trainFileList->setItemWidget(
      trainFile, 3, trainFile->p_cacheFileSelector);
}

