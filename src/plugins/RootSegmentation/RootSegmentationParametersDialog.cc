#include "RootSegmentationParametersDialog.hh"

#include <QtGui/QDialogButtonBox>
#include <QtGui/QMessageBox>
#include <QtCore/QSettings>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QGroupBox>

#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/FileNameSelectionControlElement.hh>
#include <liblabelling_qt4/StringControlElement.hh>
#include <liblabelling_qt4/DoubleControlElement.hh>
#include <liblabelling_qt4/StringSelectionControlElement.hh>
#include <liblabelling_qt4/IntControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/MultiChannelModel.hh>

#include <libsvmtl/StDataHdf5.hh>

#include <fstream>

RootSegmentationParametersDialog::RootSegmentationParametersDialog(
    LabellingMainWidget* mainWidget, QWidget* parent, Qt::WindowFlags f)
        : QDialog(parent, f), p_mainWidget(mainWidget)
{
  QVBoxLayout* mainLayout = new QVBoxLayout;

  QFrame* parameterPanel = new QFrame;
  parameterPanel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  QVBoxLayout* parameterLayout = new QVBoxLayout;
  
  p_dataChannelSelector = new ChannelSelectionControlElement(
      tr("Data channel:"), mainWidget->multiChannelModel(),
      ChannelSpecs::Data | ChannelSpecs::Visualization);
  connect(p_dataChannelSelector, SIGNAL(activated(int)),
          SLOT(setDataChannel(int)));
  parameterLayout->addWidget(p_dataChannelSelector);

  p_segmentationChannelNameControl = new StringControlElement(
      tr("Segmentation channel:"), "/segmentation/cellularMasks");
  connect(p_segmentationChannelNameControl,
          SIGNAL(valueChanged(std::string const &)),
          SLOT(setSegmentationChannelName(std::string const &)));
  parameterLayout->addWidget(p_segmentationChannelNameControl);

  blitz::TinyVector<double,3> elSize(
      p_mainWidget->multiChannelModel()->elementSizeUm());
  double minElSize = std::min(std::min(elSize(0), elSize(1)), elSize(2));
  p_processingElementSizeUmControl = new DoubleControlElement(
      tr("Processing element size (µm):"), minElSize);
  p_processingElementSizeUmControl->setRange(0.01, 100.0);
  p_processingElementSizeUmControl->setSingleStep(0.01);
  connect(p_processingElementSizeUmControl, SIGNAL(valueChanged(double)),
          SLOT(setProcessingElementSizeUm(double)));
  parameterLayout->addWidget(p_processingElementSizeUmControl);

  p_debugFileNameControl = new FileNameSelectionControlElement(
      tr("Debug File:"), "", false, "HDF5 (*.h5)");
  connect(p_debugFileNameControl, SIGNAL(valueChanged(std::string const &)),
          SLOT(setDebugFileName(std::string const &)));
  parameterLayout->addWidget(p_debugFileNameControl);  
  
  QGroupBox *preprocessingGroup = new QGroupBox(tr("Pre-processing"));
  QVBoxLayout *preprocessingLayout = new QVBoxLayout;

  QStringList typeStrings;
  typeStrings << tr("None") << tr("Minimum / Maximum")
              << tr("Standard deviation")
              << tr("Zero Mean / Unit Standard Deviation");
  p_normalizationTypeControl = new StringSelectionControlElement(
      tr("Normalization Type:"), typeStrings);
  connect(p_normalizationTypeControl, SIGNAL(valueChanged(int)),
          SLOT(setNormalizationType(int)));
  preprocessingLayout->addWidget(p_normalizationTypeControl);
  
  p_gammaControl = new DoubleControlElement(tr("Gamma correction:"), 1.0);
  p_gammaControl->setRange(0.01, 100.0);
  p_gammaControl->setSingleStep(0.01);
  connect(p_gammaControl, SIGNAL(valueChanged(double)), SLOT(setGamma(double)));
  preprocessingLayout->addWidget(p_gammaControl);
  
  p_medianWidthPxControl = new IntControlElement(
      tr("Median filter radius (px):"), 1);
  p_medianWidthPxControl->setSwitchable(true);
  p_medianWidthPxControl->setActive(false);
  p_medianWidthPxControl->setRange(1, 100);
  connect(p_medianWidthPxControl, SIGNAL(activationStateChanged(bool)),
          SLOT(setApplyMedian(bool)));
  connect(p_medianWidthPxControl, SIGNAL(valueChanged(int)),
          SLOT(setMedianWidthPx(int)));
  preprocessingLayout->addWidget(p_medianWidthPxControl);
  
  preprocessingGroup->setLayout(preprocessingLayout);
  parameterLayout->addWidget(preprocessingGroup);

  p_varianceNormalizationGroup = new QGroupBox(tr("Variance Normalization"));
  p_varianceNormalizationGroup->setCheckable(true);
  p_varianceNormalizationGroup->setChecked(true);
  connect(p_varianceNormalizationGroup, SIGNAL(toggled(bool)),
          SLOT(setApplyVarianceNormalization(bool)));
  QVBoxLayout *varianceNormalizationLayout = new QVBoxLayout;
  
  p_varSigmaUmControl = new DoubleControlElement(tr("sigma (micron):"), 20.0);
  p_varSigmaUmControl->setRange(0.01, 1000.0);
  p_varSigmaUmControl->setSingleStep(0.01);
  connect(p_varSigmaUmControl, SIGNAL(valueChanged(double)),
          SLOT(setVarSigmaUm(double)));
  varianceNormalizationLayout->addWidget(p_varSigmaUmControl);

  p_epsilonControl = new DoubleControlElement(tr("epsilon:"), 1.0);
  p_epsilonControl->setRange(0.000001, 100.0);
  p_epsilonControl->setSingleStep(0.000001);
  connect(p_epsilonControl, SIGNAL(valueChanged(double)),
          SLOT(setEpsilon(double)));
  varianceNormalizationLayout->addWidget(p_epsilonControl);

  p_varianceNormalizationGroup->setLayout(varianceNormalizationLayout);
  parameterLayout->addWidget(p_varianceNormalizationGroup);

  p_diffusionGroup = new QGroupBox(tr("Anisotropic Diffusion"));
  p_diffusionGroup->setCheckable(true);
  p_diffusionGroup->setChecked(true);
  connect(p_diffusionGroup, SIGNAL(toggled(bool)),
          SLOT(setApplyDiffusion(bool)));
  QVBoxLayout *diffusionLayout = new QVBoxLayout;
  
  p_kappaControl = new DoubleControlElement(
      tr("Diffusion strength (kappa):"), 0.2);
  p_kappaControl->setRange(0.0001, 1000.0);
  p_kappaControl->setSingleStep(0.01);
  connect(p_kappaControl, SIGNAL(valueChanged(double)), SLOT(setKappa(double)));
  diffusionLayout->addWidget(p_kappaControl);
  
  p_tauControl = new DoubleControlElement(tr("Timestep (tau):"), 0.0625);
  p_tauControl->setRange(0.0001, 1.0);
  p_tauControl->setSingleStep(0.01);
  connect(p_tauControl, SIGNAL(valueChanged(double)), SLOT(setTau(double)));
  diffusionLayout->addWidget(p_tauControl);
  
  p_zCompensationFactorControl = new DoubleControlElement(
      tr("z Compensation factor:"), 1.0);
  p_zCompensationFactorControl->setRange(
      0.0, std::numeric_limits<double>::infinity());
  p_zCompensationFactorControl->setSingleStep(0.1);
  connect(p_zCompensationFactorControl, SIGNAL(valueChanged(double)),
          SLOT(setZCompensationFactor(double)));
  diffusionLayout->addWidget(p_zCompensationFactorControl);

  p_nDiffusionIterationsControl = new IntControlElement(
      tr("# iterations:"), 10);
  p_nDiffusionIterationsControl->setRange(1, 100000);
  connect(p_nDiffusionIterationsControl, SIGNAL(valueChanged(int)),
          SLOT(setNDiffusionIterations(int)));
  diffusionLayout->addWidget(p_nDiffusionIterationsControl);  
  
  p_diffusionGroup->setLayout(diffusionLayout);
  parameterLayout->addWidget(p_diffusionGroup);

  QGroupBox *watershedGroup = new QGroupBox("Region growing");
  QVBoxLayout *watershedLayout = new QVBoxLayout;

  p_hessianSigmaUmControl = new DoubleControlElement(
      tr("sigma (micron):"), minElSize);
  p_hessianSigmaUmControl->setRange(0.01, 1000.0);
  p_hessianSigmaUmControl->setSingleStep(0.01);
  connect(p_hessianSigmaUmControl, SIGNAL(valueChanged(double)),
          SLOT(setHessianSigmaUm(double)));
  watershedLayout->addWidget(p_hessianSigmaUmControl);

  p_edgeThresholdControl = new DoubleControlElement(
      tr("Edge threshold:"), -0.2);
  p_edgeThresholdControl->setRange(-10.0, 0.0);
  p_edgeThresholdControl->setSingleStep(0.01);
  connect(p_edgeThresholdControl, SIGNAL(valueChanged(double)),
          SLOT(setEdgeThreshold(double)));
  watershedLayout->addWidget(p_edgeThresholdControl);

  p_boundaryThicknessPxControl = new IntControlElement(
      tr("Boundary width (px):"), 0);
  p_boundaryThicknessPxControl->setRange(0, INT_MAX);
  connect(p_boundaryThicknessPxControl, SIGNAL(valueChanged(int)),
          SLOT(setBoundaryThicknessPx(int)));
  watershedLayout->addWidget(p_boundaryThicknessPxControl);
  
  p_minimumCellVolumeUm3Control = new DoubleControlElement(
      tr("Minimum cell volume (micron cube):"), 60.0);
  p_minimumCellVolumeUm3Control->setRange(
      0.0, std::numeric_limits<double>::infinity());
  p_minimumCellVolumeUm3Control->setSingleStep(0.1);
  connect(p_minimumCellVolumeUm3Control, SIGNAL(valueChanged(double)),
          SLOT(setMinimumCellVolumeUm3(double)));
  watershedLayout->addWidget(p_minimumCellVolumeUm3Control);
  
  watershedGroup->setLayout(watershedLayout);
  parameterLayout->addWidget(watershedGroup);

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

  // Load field values from settings
  QSettings settings;

  if (p_mainWidget->fileName() == "")
  {
    std::string dataChannelName = settings.value(
        "iRoCSPipeline/cellBoundaryChannel", "").toString().toStdString();
    p_dataChannelSelector->setValue(dataChannelName);
    setSegmentationChannelName(
        settings.value(
            "iRoCSPipeline/segmentationChannel",
            "/segmentation/cellularMasks").toString().toStdString());
    setProcessingElementSizeUm(
        settings.value(
            "iRoCSPipeline/processingElementSizeUm", minElSize).toDouble());
    setNormalizationType(
        settings.value("iRoCSPipeline/normalizationType", 0).toInt());
    setGamma(
        settings.value("iRoCSPipeline/gamma", 1.0).toDouble());
    setApplyMedian(settings.value("iRoCSPipeline/applyMedian", false).toBool());
    setMedianWidthPx(
        settings.value("iRoCSPipeline/medianWidthPx", 1).toInt());
    setApplyVarianceNormalization(
        settings.value(
            "iRoCSPipeline/applyVarianceNormalization", true).toBool());
    setVarSigmaUm(settings.value("iRoCSPipeline/varSigmaUm", 20.0).toDouble());
    setEpsilon(settings.value("iRoCSPipeline/varEpsilon", 1.0).toDouble());
    setApplyDiffusion(
        settings.value("iRoCSPipeline/applyDiffusion", true).toBool());
    setKappa(settings.value("iRoCSPipeline/kappa", 0.2).toDouble());
    setTau(settings.value("iRoCSPipeline/tau", 0.0625).toDouble());
    setZCompensationFactor(
        settings.value("iRoCSPipeline/zCompensationFactor", 1.0).toDouble());
    setNDiffusionIterations(
        settings.value("iRoCSPipeline/nDiffusionIterations", 10).toInt());
    setHessianSigmaUm(
        settings.value("iRoCSPipeline/hessianSigmaUm", minElSize).toDouble());
    setEdgeThreshold(
        settings.value("iRoCSPipeline/edgeThreshold", -0.2).toDouble());
    setBoundaryThicknessPx(
        settings.value("iRoCSPipeline/boundaryThicknessPx", 0).toInt());
    setMinimumCellVolumeUm3(
        settings.value("iRoCSPipeline/minimumCellVolumeUm3", 60.0).toDouble());
  }
  else
  {
    std::string dataChannelName = settings.value(
        ("iRoCSPipeline/cellBoundaryChannel/" +
         p_mainWidget->fileName()).c_str(), "").toString().toStdString();
    p_dataChannelSelector->setValue(dataChannelName);
    setSegmentationChannelName(
        settings.value(
            ("iRoCSPipeline/segmentationChannel/" +
             p_mainWidget->fileName()).c_str(),
            "/segmentation/cellularMasks").toString().toStdString());
    setProcessingElementSizeUm(
        settings.value(
            ("iRoCSPipeline/processingElementSizeUm/" +
             p_mainWidget->fileName()).c_str(), minElSize).toDouble());
    setDebugFileName(
        settings.value(
            ("iRoCSPipeline/debugFileName/" + p_mainWidget->fileName()).c_str(),
            "").toString().toStdString());
    setNormalizationType(
        settings.value(
            ("iRoCSPipeline/normalizationType/" +
             p_mainWidget->fileName()).c_str(), 0).toInt());
    setGamma(
        settings.value(
            ("iRoCSPipeline/gamma/" + p_mainWidget->fileName()).c_str(),
            1.0).toDouble());
    setApplyMedian(
        settings.value(
            ("iRoCSPipeline/applyMedian/" + p_mainWidget->fileName()).c_str(),
            false).toBool());
    setMedianWidthPx(
        settings.value(
            ("iRoCSPipeline/medianWidthPx/" + p_mainWidget->fileName()).c_str(),
            1).toInt());
    setApplyVarianceNormalization(
        settings.value(
            ("iRoCSPipeline/applyVarianceNormalization/" +
             p_mainWidget->fileName()).c_str(), true).toBool());
    setVarSigmaUm(
        settings.value(
            ("iRoCSPipeline/varSigmaUm/" + p_mainWidget->fileName()).c_str(),
            20.0).toDouble());
    setEpsilon(
        settings.value(
            ("iRoCSPipeline/varEpsilon/" + p_mainWidget->fileName()).c_str(),
            1.0).toDouble());
    setApplyDiffusion(
        settings.value(
            ("iRoCSPipeline/applyDiffusion/" +
             p_mainWidget->fileName()).c_str(), true).toBool());
    setKappa(
        settings.value(
            ("iRoCSPipeline/kappa/" + p_mainWidget->fileName()).c_str(),
            0.2).toDouble());
    setTau(
        settings.value(
            ("iRoCSPipeline/tau/" + p_mainWidget->fileName()).c_str(),
            0.0625).toDouble());
    setZCompensationFactor(
        settings.value(
            ("iRoCSPipeline/zCompensationFactor/" +
             p_mainWidget->fileName()).c_str(), 1.0).toDouble());
    setNDiffusionIterations(
        settings.value(
            ("iRoCSPipeline/nDiffusionIterations/" +
             p_mainWidget->fileName()).c_str(), 10).toInt());
    setHessianSigmaUm(
        settings.value(
            ("iRoCSPipeline/hessianSigmaUm/" +
             p_mainWidget->fileName()).c_str(), minElSize).toDouble());
    setEdgeThreshold(
        settings.value(
            ("iRoCSPipeline/edgeThreshold/" + p_mainWidget->fileName()).c_str(),
            -0.2).toDouble());
    setBoundaryThicknessPx(
        settings.value(
            ("iRoCSPipeline/boundaryThicknessPx/" +
             p_mainWidget->fileName()).c_str(), 0).toInt());
    setMinimumCellVolumeUm3(
        settings.value(
            ("iRoCSPipeline/minimumCellVolumeUm3/" +
             p_mainWidget->fileName()).c_str(), 60.0).toDouble());
  }

  setLayout(mainLayout);
}

RootSegmentationParametersDialog::~RootSegmentationParametersDialog()
{}

ChannelSpecs *RootSegmentationParametersDialog::dataChannel() const
{
  return p_dataChannelSelector->selectedChannel();
}

void RootSegmentationParametersDialog::setDataChannel(ChannelSpecs *channel)
{
  p_dataChannelSelector->setValue(channel);
}

void RootSegmentationParametersDialog::setDataChannel(int channelIndex)
{
  p_dataChannelSelector->setValue(channelIndex);
}

std::string RootSegmentationParametersDialog::segmentationChannelName() const
{
  return p_segmentationChannelNameControl->value();
}

void RootSegmentationParametersDialog::setSegmentationChannelName(
    std::string const &name)
{
  p_segmentationChannelNameControl->setValue(name);
}

double RootSegmentationParametersDialog::processingElementSizeUm() const
{
  return p_processingElementSizeUmControl->value();
}

void RootSegmentationParametersDialog::setProcessingElementSizeUm(
    double elementSizeUm)
{
  p_processingElementSizeUmControl->setValue(elementSizeUm);
}

std::string RootSegmentationParametersDialog::debugFileName() const
{
  return p_debugFileNameControl->value();
}

void RootSegmentationParametersDialog::setDebugFileName(
    std::string const &debugFile)
{
  p_debugFileNameControl->setValue(debugFile);
}

int RootSegmentationParametersDialog::normalizationType() const
{
  return p_normalizationTypeControl->value();
}

void RootSegmentationParametersDialog::setNormalizationType(
    int normalizationType)
{
  p_normalizationTypeControl->setValue(normalizationType);
}

double RootSegmentationParametersDialog::gamma() const
{
  return p_gammaControl->value();
}

void RootSegmentationParametersDialog::setGamma(double gamma)
{
  p_gammaControl->setValue(gamma);
}

bool RootSegmentationParametersDialog::applyMedian() const
{
  return p_medianWidthPxControl->isActive();
}

void RootSegmentationParametersDialog::setApplyMedian(bool applyMedian)
{
  p_medianWidthPxControl->setActive(applyMedian);
}

int RootSegmentationParametersDialog::medianWidthPx() const
{
  return p_medianWidthPxControl->value();
}

void RootSegmentationParametersDialog::setMedianWidthPx(int widthPx)
{
  p_medianWidthPxControl->setValue(widthPx);
}

bool RootSegmentationParametersDialog::applyVarianceNormalization() const
{
  return p_varianceNormalizationGroup->isChecked();
}

void RootSegmentationParametersDialog::setApplyVarianceNormalization(
    bool varianceNormalization)
{
  p_varianceNormalizationGroup->setChecked(varianceNormalization);
}

double RootSegmentationParametersDialog::varSigmaUm() const
{
  return p_varSigmaUmControl->value();
}

void RootSegmentationParametersDialog::setVarSigmaUm(double sigmaUm)
{
  p_varSigmaUmControl->setValue(sigmaUm);
}

double RootSegmentationParametersDialog::epsilon() const
{
  return p_epsilonControl->value();
}

void RootSegmentationParametersDialog::setEpsilon(double epsilon)
{
  p_epsilonControl->setValue(epsilon);
}

bool RootSegmentationParametersDialog::applyDiffusion() const
{
  return p_diffusionGroup->isChecked();
}

void RootSegmentationParametersDialog::setApplyDiffusion(bool applyDiffusion)
{
  p_diffusionGroup->setChecked(applyDiffusion);
}

double RootSegmentationParametersDialog::kappa() const
{
  return p_kappaControl->value();
}

void RootSegmentationParametersDialog::setKappa(double kappa)
{
  p_kappaControl->setValue(kappa);
}

double RootSegmentationParametersDialog::tau() const
{
  return p_tauControl->value();
}

void RootSegmentationParametersDialog::setTau(double tau)
{
  p_tauControl->setValue(tau);
}

double RootSegmentationParametersDialog::zCompensationFactor() const
{
  return p_zCompensationFactorControl->value();
}

void RootSegmentationParametersDialog::setZCompensationFactor(
    double zCompensation)
{
  p_zCompensationFactorControl->setValue(zCompensation);
}

int RootSegmentationParametersDialog::nDiffusionIterations() const
{
  return p_nDiffusionIterationsControl->value();
}

void RootSegmentationParametersDialog::setNDiffusionIterations(
    int nDiffusionIterations)
{
  p_nDiffusionIterationsControl->setValue(nDiffusionIterations);
}

double RootSegmentationParametersDialog::hessianSigmaUm() const
{
  return p_hessianSigmaUmControl->value();
}

void RootSegmentationParametersDialog::setHessianSigmaUm(double sigmaUm)
{
  p_hessianSigmaUmControl->setValue(sigmaUm);
}

double RootSegmentationParametersDialog::edgeThreshold() const
{
  return p_edgeThresholdControl->value();
}

void RootSegmentationParametersDialog::setEdgeThreshold(double threshold)
{
  p_edgeThresholdControl->setValue(threshold);
}

int RootSegmentationParametersDialog::boundaryThicknessPx() const
{
  return p_boundaryThicknessPxControl->value();
}

void RootSegmentationParametersDialog::setBoundaryThicknessPx(
    int boundaryThicknessPx)
{
  p_boundaryThicknessPxControl->setValue(boundaryThicknessPx);
}

double RootSegmentationParametersDialog::minimumCellVolumeUm3() const
{
  return p_minimumCellVolumeUm3Control->value();
}

void RootSegmentationParametersDialog::setMinimumCellVolumeUm3(double volumeUm3)
{
  p_minimumCellVolumeUm3Control->setValue(volumeUm3);
}

std::string RootSegmentationParametersDialog::check()
{
  if (dataChannel() == NULL) return "The data channel has not been set.";

  try
  {
    // Check syntax of SegmentationChannel string
    try 
    {
      std::string segmentationDataset =
          BlitzH5File::simplifyGroupDescriptor(segmentationChannelName());
      setSegmentationChannelName(segmentationDataset);
    }
    catch (BlitzH5Error&)
    {
      return "The segmentation channel name you entered is no valid hdf5 group "
          "descriptor.";
    }
    
    // Check whether debug file can be created or opened for writing
    if (debugFileName() != "")
    {
      try
      {
        BlitzH5File debugFile(debugFileName(), BlitzH5File::WriteOrNew);
      }
      catch (BlitzH5Error&)
      {
        return "The debug file '" + debugFileName() +
            "' cannot be created or opened for writing.";
      }
    }
  }
  catch (std::exception& e)
  {
    return std::string("The supplied parameters contained an error: ") +
        e.what();
  }

  return "";
}

void RootSegmentationParametersDialog::checkAndAccept()
{
  std::string res = check();
  
  if (res != "")
  {
    QMessageBox::warning(p_mainWidget, tr("Wrong Parameters"), res.c_str());
    return;
  }
  
  QSettings settings;

  if (p_mainWidget->fileName() == "")
  {
    settings.setValue(
        "iRoCSPipeline/cellBoundaryChannel", dataChannel()->name().c_str());
    settings.setValue(
        "iRoCSPipeline/segmentationChannel", segmentationChannelName().c_str());
    settings.setValue(
        "iRoCSPipeline/processingElementSizeUm", processingElementSizeUm());
    settings.setValue("iRoCSPipeline/normalizationType", normalizationType());
    settings.setValue("iRoCSPipeline/gamma", gamma());
    settings.setValue("iRoCSPipeline/applyMedian", applyMedian());
    settings.setValue("iRoCSPipeline/medianWidthPx", medianWidthPx());
    settings.setValue(
        "iRoCSPipeline/applyVarianceNormalization",
        applyVarianceNormalization());
    settings.setValue("iRoCSPipeline/varSigmaUm", varSigmaUm());
    settings.setValue("iRoCSPipeline/varEpsilon", epsilon());
    settings.setValue("iRoCSPipeline/applyDiffusion", applyDiffusion());
    settings.setValue("iRoCSPipeline/kappa", kappa());
    settings.setValue("iRoCSPipeline/tau", tau());
    settings.setValue(
        "iRoCSPipeline/zCompensationFactor", zCompensationFactor());
    settings.setValue(
        "iRoCSPipeline/nDiffusionIterations", nDiffusionIterations());
    settings.setValue("iRoCSPipeline/hessianSigmaUm", hessianSigmaUm());
    settings.setValue("iRoCSPipeline/edgeThreshold", edgeThreshold());
    settings.setValue(
        "iRoCSPipeline/boundaryThicknessPx", boundaryThicknessPx());
    settings.setValue(
        "iRoCSPipeline/minimumCellVolumeUm3", minimumCellVolumeUm3());
  }
  else
  {
    settings.setValue(
        ("iRoCSPipeline/cellBoundaryChannel/" +
         p_mainWidget->fileName()).c_str(), dataChannel()->name().c_str());
    settings.setValue(
        ("iRoCSPipeline/segmentationChannel/" +
         p_mainWidget->fileName()).c_str(), segmentationChannelName().c_str());
    settings.setValue(
        ("iRoCSPipeline/processingElementSizeUm/" +
         p_mainWidget->fileName()).c_str(), processingElementSizeUm());
    settings.setValue(
        ("iRoCSPipeline/debugFileName/" + p_mainWidget->fileName()).c_str(),
        debugFileName().c_str());
    settings.setValue(
        ("iRoCSPipeline/normalizationType/" +
         p_mainWidget->fileName()).c_str(), normalizationType());
    settings.setValue(
        ("iRoCSPipeline/gamma/" + p_mainWidget->fileName()).c_str(), gamma());
    settings.setValue(
        ("iRoCSPipeline/applyMedian/" + p_mainWidget->fileName()).c_str(),
        applyMedian());
    settings.setValue(
        ("iRoCSPipeline/medianWidthPx/" + p_mainWidget->fileName()).c_str(),
        medianWidthPx());
    settings.setValue(
        ("iRoCSPipeline/applyVarianceNormalization/" +
         p_mainWidget->fileName()).c_str(), applyVarianceNormalization());
    settings.setValue(
        ("iRoCSPipeline/varSigmaUm/" + p_mainWidget->fileName()).c_str(),
        varSigmaUm());
    settings.setValue(
        ("iRoCSPipeline/varEpsilon/" + p_mainWidget->fileName()).c_str(),
        epsilon());
    settings.setValue(
        ("iRoCSPipeline/applyDiffusion/" + p_mainWidget->fileName()).c_str(),
        applyDiffusion());
    settings.setValue(
        ("iRoCSPipeline/kappa/" + p_mainWidget->fileName()).c_str(), kappa());
    settings.setValue(
        ("iRoCSPipeline/tau/" + p_mainWidget->fileName()).c_str(), tau());
    settings.setValue(
        ("iRoCSPipeline/zCompensationFactor/" +
         p_mainWidget->fileName()).c_str(), zCompensationFactor());
    settings.setValue(
        ("iRoCSPipeline/nDiffusionIterations/" +
         p_mainWidget->fileName()).c_str(), nDiffusionIterations());
    settings.setValue(
        ("iRoCSPipeline/hessianSigmaUm/" + p_mainWidget->fileName()).c_str(),
        hessianSigmaUm());
    settings.setValue(
        ("iRoCSPipeline/edgeThreshold/" + p_mainWidget->fileName()).c_str(),
        edgeThreshold());
    settings.setValue(
        ("iRoCSPipeline/boundaryThicknessPx/" +
         p_mainWidget->fileName()).c_str(), boundaryThicknessPx());
    settings.setValue(
        ("iRoCSPipeline/minimumCellVolumeUm3/" +
         p_mainWidget->fileName()).c_str(), minimumCellVolumeUm3());
  }

  accept();
}
