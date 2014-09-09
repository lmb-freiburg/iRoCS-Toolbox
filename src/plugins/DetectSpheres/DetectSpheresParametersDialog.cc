#include "DetectSpheresParametersDialog.hh"

#include <QtGui/QDialogButtonBox>
#include <QtGui/QMessageBox>
#include <QtCore/QSettings>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>

#include <liblabelling_qt4/MultiChannelModel.hh>

#include <fstream>

DetectSpheresParametersDialog::DetectSpheresParametersDialog(
    LabellingMainWidget* mainWidget, QWidget* parent, Qt::WindowFlags f)
        : QDialog(parent, f), p_mainWidget(mainWidget)
{
  QVBoxLayout* mainLayout = new QVBoxLayout;

  QFrame* parameterPanel = new QFrame;
  parameterPanel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  QVBoxLayout* parameterLayout = new QVBoxLayout;
  
  p_dataChannelSelector = new ChannelSelectionControlElement(
      tr("Data channel:"), p_mainWidget->multiChannelModel(),
      ChannelSpecs::Data | ChannelSpecs::Visualization);
  parameterLayout->addWidget(p_dataChannelSelector);

  p_annotationChannelControlElement = new StringControlElement(
      tr("Annotation channel:"));
  parameterLayout->addWidget(p_annotationChannelControlElement);

  blitz::TinyVector<double,3> elSize(1.0);
  if (p_mainWidget != NULL && p_mainWidget->multiChannelModel() != NULL)
      elSize = p_mainWidget->multiChannelModel()->elementSizeUm();

  p_radiusRangeControl = new DoubleRangeControlElement(
      tr("Radius range (micron):"));
  p_radiusRangeControl->setRange(0.0, std::numeric_limits<double>::infinity());
  p_radiusRangeControl->setSingleStep(0.1);
  parameterLayout->addWidget(p_radiusRangeControl);
  
  p_radiusStepControl = new DoubleControlElement(tr("Radius step (micron):"));
  p_radiusStepControl->setRange(0.0, std::numeric_limits<double>::infinity());
  p_radiusStepControl->setSingleStep(0.1);
  parameterLayout->addWidget(p_radiusStepControl);
  
  p_preSmoothingSigmaUmControl =
      new DoubleControlElement(tr("Pre-smoothing kernel width (micron):"));
  p_preSmoothingSigmaUmControl->setRange(
      0.0, std::numeric_limits<double>::infinity());
  p_preSmoothingSigmaUmControl->setSingleStep(0.1);
  parameterLayout->addWidget(p_preSmoothingSigmaUmControl);
  
  p_postSmoothingSigmaUmControl =
      new DoubleControlElement(tr("Post-smoothing kernel width (micron):"));
  p_postSmoothingSigmaUmControl->setRange(
      0.0, std::numeric_limits<double>::infinity());
  p_postSmoothingSigmaUmControl->setSingleStep(0.1);
  parameterLayout->addWidget(p_postSmoothingSigmaUmControl);
  
  p_minMagnitudeControl =
      new DoubleControlElement(tr("Minimum gradient magnitude:"));
  p_minMagnitudeControl->setRange(
      0.0, std::numeric_limits<double>::infinity());
  p_minMagnitudeControl->setSingleStep(0.01);
  parameterLayout->addWidget(p_minMagnitudeControl);
  
  p_invertGradientsControl = new BoolControlElement(
      tr("Search for dark spheres:"));
  parameterLayout->addWidget(p_invertGradientsControl);
  
  p_gammaControl =
      new DoubleControlElement(tr("Gamma correction:"));
  p_gammaControl->setRange(0.01, 10.0);
  p_gammaControl->setSingleStep(0.01);
  parameterLayout->addWidget(p_gammaControl);
  
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
  QString channelName;
  QString annotationName;
  double radiusMin, radiusMax, radiusStep;
  double preSmoothingSigma, postSmoothingSigma;
  double minMagnitude;
  bool invertGradients;
  double gamma;
  if (p_mainWidget->fileName() == "")
  {
    channelName = settings.value(
        "SphereDetector/DataChannel",
        p_dataChannelSelector->toStdString().c_str()).toString();
    annotationName = settings.value(
        "SphereDetector/AnnotationChannel", "/annotation/detector").toString();
    radiusMin = settings.value("SphereDetector/rMin", 0.0).toDouble();
    radiusMax = settings.value(
        "SphereDetector/rMax", 100.0 * elSize(1)).toDouble();
    radiusStep = settings.value("SphereDetector/rStep", elSize(1)).toDouble();
    preSmoothingSigma = settings.value(
        "SphereDetector/preSmoothingSigmaUm", 0.5 * elSize(0)).toDouble();
    postSmoothingSigma = settings.value(
        "SphereDetector/postSmoothingSigmaUm", 2.0 * elSize(0)).toDouble();
    minMagnitude = settings.value(
        "SphereDetector/minMagnitude", 0.0).toDouble();
    invertGradients = settings.value(
        "SphereDetector/invertGradient", false).toBool();
    gamma = settings.value(
        "SphereDetector/gamma", 1.0).toDouble();
  }
  else
  {
    channelName = settings.value(
        ("SphereDetector/DataChannel/" + p_mainWidget->fileName()).c_str(),
        p_dataChannelSelector->toStdString().c_str()).toString();
    annotationName = settings.value(
        ("SphereDetector/AnnotationChannel/" +
         p_mainWidget->fileName()).c_str(), "annotation/detector").toString();
    radiusMin = settings.value(
        ("SphereDetector/rMin/" + p_mainWidget->fileName()).c_str(),
        0.0).toDouble();
    radiusMax = settings.value(
        ("SphereDetector/rMax/" + p_mainWidget->fileName()).c_str(),
        100.0 * elSize(1)).toDouble();
    radiusStep = settings.value(
        ("SphereDetector/rStep/" + p_mainWidget->fileName()).c_str(),
        elSize(1)).toDouble();
    preSmoothingSigma = settings.value(
        ("SphereDetector/preSmoothingSigmaUm/" +
         p_mainWidget->fileName()).c_str(), 0.5 * elSize(0)).toDouble();
    postSmoothingSigma = settings.value(
        ("SphereDetector/postSmoothingSigmaUm/" +
         p_mainWidget->fileName()).c_str(), 2.0 * elSize(0)).toDouble();
    minMagnitude = settings.value(
        ("SphereDetector/minMagnitude/" + p_mainWidget->fileName()).c_str(),
        0.0).toDouble();
    invertGradients = settings.value(
        ("SphereDetector/invertGradient/" + p_mainWidget->fileName()).c_str(),
        false).toBool();
    gamma= settings.value(
        ("SphereDetector/gamma/" + p_mainWidget->fileName()).c_str(),
        1.0).toDouble();
  }
  if (p_dataChannelSelector->contains(channelName.toStdString()))
      p_dataChannelSelector->setValue(channelName.toStdString());
  p_annotationChannelControlElement->setValue(annotationName.toStdString());
  p_radiusRangeControl->setValue(radiusMin, radiusMax);
  p_radiusStepControl->setValue(radiusStep);
  p_preSmoothingSigmaUmControl->setValue(preSmoothingSigma);
  p_postSmoothingSigmaUmControl->setValue(postSmoothingSigma);
  p_minMagnitudeControl->setValue(minMagnitude);
  p_invertGradientsControl->setValue(invertGradients);
  p_gammaControl->setValue(gamma);
  
  setLayout(mainLayout);
}

DetectSpheresParametersDialog::~DetectSpheresParametersDialog()
{}

ChannelSpecs* DetectSpheresParametersDialog::dataChannel() const
{
  return p_dataChannelSelector->selectedChannel();
}

void DetectSpheresParametersDialog::setDataChannel(ChannelSpecs *channel)
{
  p_dataChannelSelector->setValue(channel);
}

std::string DetectSpheresParametersDialog::annotationChannelName() const
{
  return p_annotationChannelControlElement->value();
}

void DetectSpheresParametersDialog::setAnnotationChannelName(
    std::string const &name)
{
  p_annotationChannelControlElement->setValue(name);
}

blitz::TinyVector<double,2> DetectSpheresParametersDialog::radiusRangeUm() const
{
  return blitz::TinyVector<double,2>(
      p_radiusRangeControl->lowerBound(), p_radiusRangeControl->upperBound());
}

void DetectSpheresParametersDialog::setRadiusRangeUm(
    blitz::TinyVector<double,2> const &range)
{
  p_radiusRangeControl->setLowerBound(range(0));
  p_radiusRangeControl->setUpperBound(range(1));  
}

double DetectSpheresParametersDialog::radiusStepUm() const
{
  return p_radiusStepControl->value();
}

void DetectSpheresParametersDialog::setRadiusStepUm(double radiusStepUm)
{
  p_radiusStepControl->setValue(radiusStepUm);
}

double DetectSpheresParametersDialog::preSmoothingSigmaUm() const
{
  return p_preSmoothingSigmaUmControl->value();
}

void DetectSpheresParametersDialog::setPreSmoothingSigmaUm(double sigmaUm)
{
  p_preSmoothingSigmaUmControl->setValue(sigmaUm);
}

double DetectSpheresParametersDialog::postSmoothingSigmaUm() const
{
  return p_postSmoothingSigmaUmControl->value();
}

void DetectSpheresParametersDialog::setPostSmoothingSigmaUm(double sigmaUm)
{
  p_postSmoothingSigmaUmControl->setValue(sigmaUm);
}

double DetectSpheresParametersDialog::minMagnitude() const
{
  return p_minMagnitudeControl->value();
}

void DetectSpheresParametersDialog::setMinMagnitude(double minMagnitude)
{
  p_minMagnitudeControl->setValue(minMagnitude);
}

bool DetectSpheresParametersDialog::invertGradients() const
{
  return p_invertGradientsControl->value();
}

void DetectSpheresParametersDialog::setInvertGradients(bool invert)
{
  p_invertGradientsControl->setValue(invert);
}

double DetectSpheresParametersDialog::gamma() const
{
  return p_gammaControl->value();
}

void DetectSpheresParametersDialog::setGamma(double gamma)
{
  p_gammaControl->setValue(gamma);
}

std::string DetectSpheresParametersDialog::check()
{
  if (dataChannel() == NULL) return "The data channel has not been set.";

  // Check syntax of annotationGroup string
  try 
  {
    std::string annotationGroup =
        BlitzH5File::simplifyGroupDescriptor(annotationChannelName());
    setAnnotationChannelName(annotationGroup);
  }
  catch (BlitzH5Error&)
  {
    return "The annotation channel name you entered is no valid hdf5 group "
        "descriptor.";
  }
    
  return "";
}

void DetectSpheresParametersDialog::checkAndAccept()
{
  std::string res = check();
  
  if (res != "")
  {
    p_mainWidget->showError(res.c_str());
    return;
  }
  
  QSettings settings;

  if (p_mainWidget->fileName() == "")
  {
    settings.setValue(
        "SphereDetector/DataChannel",
        p_dataChannelSelector->toStdString().c_str());
    settings.setValue(
        "SphereDetector/AnnotationChannel", annotationChannelName().c_str());
    settings.setValue("SphereDetector/rMin", radiusRangeUm()(0));
    settings.setValue("SphereDetector/rMax", radiusRangeUm()(1));
    settings.setValue("SphereDetector/rStep", radiusStepUm());
    settings.setValue(
        "SphereDetector/preSmoothingSigmaUm", preSmoothingSigmaUm());
    settings.setValue(
        "SphereDetector/postSmoothingSigmaUm", postSmoothingSigmaUm());
    settings.setValue("SphereDetector/minMagnitude", minMagnitude());
    settings.setValue("SphereDetector/invertGradient", invertGradients());
    settings.setValue("SphereDetector/gamma", gamma());
  }
  else
  {
    settings.setValue(
        ("SphereDetector/DataChannel/" + p_mainWidget->fileName()).c_str(),
        p_dataChannelSelector->toStdString().c_str());
    settings.setValue(
        ("SphereDetector/AnnotationChannel/" +
         p_mainWidget->fileName()).c_str(), annotationChannelName().c_str());
    settings.setValue(
        ("SphereDetector/rMin/" + p_mainWidget->fileName()).c_str(),
        radiusRangeUm()(0));
    settings.setValue(
        ("SphereDetector/rMax/" + p_mainWidget->fileName()).c_str(),
        radiusRangeUm()(1));
    settings.setValue(
        ("SphereDetector/rStep/" + p_mainWidget->fileName()).c_str(),
        radiusStepUm());
    settings.setValue(
        ("SphereDetector/preSmoothingSigmaUm/" +
         p_mainWidget->fileName()).c_str(), preSmoothingSigmaUm());
    settings.setValue(
        ("SphereDetector/postSmoothingSigmaUm/" +
         p_mainWidget->fileName()).c_str(), postSmoothingSigmaUm());
    settings.setValue(
        ("SphereDetector/minMagnitude/" + p_mainWidget->fileName()).c_str(),
        minMagnitude());
    settings.setValue(
        ("SphereDetector/invertGradient/" + p_mainWidget->fileName()).c_str(),
        invertGradients());
    settings.setValue(
        ("SphereDetector/gamma/" + p_mainWidget->fileName()).c_str(), gamma());
  }

  accept();
}
