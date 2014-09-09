#include "AttachIRoCSParametersDialog.hh"

#include <QtGui/QDialogButtonBox>
#include <QtGui/QMessageBox>
#include <QtCore/QSettings>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>

#include <liblabelling_qt4/MultiChannelModel.hh>

AttachIRoCSParametersDialog::AttachIRoCSParametersDialog(
    LabellingMainWidget* mainWidget, QWidget* parent, Qt::WindowFlags f)
        : QDialog(parent, f), p_mainWidget(mainWidget)
{
  QVBoxLayout* mainLayout = new QVBoxLayout;
  setLayout(mainLayout);

  QFrame* parameterPanel = new QFrame;
  parameterPanel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  QVBoxLayout* parameterLayout = new QVBoxLayout;
  parameterPanel->setLayout(parameterLayout);
  
  p_markerChannelSelector = new ChannelSelectionControlElement(
      tr("Marker channel"), mainWidget->multiChannelModel(),
      ChannelSpecs::Annotation);
  p_markerChannelSelector->setToolTip(
      tr("Select the channel containing the nucleus markers, of which "
         "Epidermis nuclei are marked with label 2"));
  parameterLayout->addWidget(p_markerChannelSelector);
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
  
  p_kappaControl = new DoubleControlElement(
      tr("Data term weight (kappa)"), 1.0);
  p_kappaControl->setToolTip(
      tr("For extreme values of lambda and mu below you may need to adapt "
         "kappa as well to stay numerically stable. Usually you should keep "
         "it at its default value of 1.0"));
  p_kappaControl->setRange(0.0, 100.0);
  p_kappaControl->setSingleStep(0.001);
  iRoCSParametersLayout->addWidget(p_kappaControl);
  
  p_lambdaControl = new DoubleControlElement(
      tr("Axis stiffness weight (lambda)"), 0.0);
  p_lambdaControl->setToolTip(
      tr("If the axis oscillates around your expectated axis increase this "
         "value. Usually very small values (0 - 0.1) are appropriate."));
  p_lambdaControl->setRange(0.0, 100.0);
  p_lambdaControl->setSingleStep(0.001);
  iRoCSParametersLayout->addWidget(p_lambdaControl);
  
  p_muControl = new DoubleControlElement(
      tr("Thickness constancy weight (mu)"), 0.0);
  p_muControl->setToolTip(
      tr("If the thickness oscillates a lot increase this value. Usually very "
         "small values (0 - 0.1) are appropriate."));
  p_muControl->setRange(0.0, 100.0);
  p_muControl->setSingleStep(0.001);
  iRoCSParametersLayout->addWidget(p_muControl);
  
  p_nIterControl = new IntControlElement(
      tr("Iterations till refinement"), 1000000);
  p_nIterControl->setToolTip(
      tr("The maximum number of iterations till a forced reparametrization is "
         "applied. Normally the fitting is run till convergence and then the "
         "reparametrization is automatically applied so large values here are "
         "usually a good choice."));
  p_nIterControl->setRange(0, 1000000);
  iRoCSParametersLayout->addWidget(p_nIterControl);
  
  p_tauControl = new DoubleControlElement(
      tr("Optimization time step (tau)"), 0.1);
  p_tauControl->setToolTip(
      tr("During Gradient descent optimization tau is the length of the step "
         "along the gradient to update the energy functional. The time-step is "
         "adapted automatically to avoid oscillations, but a sensible initial "
         "timestep must be given here to avoid immediate divergence. If the "
         "optimization oscillates reduce the time step."));
  p_tauControl->setRange(0.0000001, 100.0);
  p_tauControl->setSingleStep(0.0000001);
  iRoCSParametersLayout->addWidget(p_tauControl);  

  p_searchRadiusUmControl = new DoubleControlElement(
      tr("Search Radius [micron]"), 0.0);
  p_searchRadiusUmControl->setSwitchable(true);
  p_searchRadiusUmControl->setActive(false);
  p_searchRadiusUmControl->setToolTip(
      tr("Set the search radius for structure elongation. If disabled or set "
         "to zero, it is automatically chosen."));
  p_searchRadiusUmControl->setRange(0.0, 10000.0);
  p_searchRadiusUmControl->setSingleStep(0.1);
  iRoCSParametersLayout->addWidget(p_searchRadiusUmControl);  

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

  QString annotationName;
  QString qcName;
  QString iRoCSName;
  int nIter;
  bool searchRadiusEnabled;
  double kappa, lambda, mu, tau, searchRadiusUm;

  if (p_mainWidget->fileName() == "")
  {
    annotationName = settings.value(
        "iRoCSPipeline/AnnotationChannel",
        p_markerChannelSelector->toStdString().c_str()).toString();
    qcName = settings.value(
        "iRoCSPipeline/QcChannel",
        p_qcChannelSelector->toStdString().c_str()).toString();
    iRoCSName = settings.value(
        "iRoCSPipeline/AxisChannel", "/annotation/axis").toString();
    iRoCSName = "/annotation/axis";
    nIter = settings.value("iRoCSPipeline/nIter", 1000000).toInt();
    kappa = settings.value("iRoCSPipeline/kappa", 1.0).toDouble();
    lambda = settings.value("iRoCSPipeline/lambda", 0.0).toDouble();
    mu = settings.value("iRoCSPipeline/mu", 0.0).toDouble();
    tau = settings.value("iRoCSPipeline/tau", 0.01).toDouble();
    searchRadiusEnabled = settings.value(
        "iRoCSPipeline/searchRadiusEnabled", false).toBool();
    searchRadiusUm = settings.value(
        "iRoCSPipeline/searchRadiusUm", 0.0).toDouble();
  }
  else
  {
    annotationName = settings.value(
        ("iRoCSPipeline/" + p_mainWidget->fileName() +
         "/AnnotationChannel").c_str(),
        p_markerChannelSelector->toStdString().c_str()).toString();
    qcName = settings.value(
        ("iRoCSPipeline/" + p_mainWidget->fileName() + "/QcChannel").c_str(),
        p_qcChannelSelector->toStdString().c_str()).toString();
    iRoCSName = settings.value(
        ("iRoCSPipeline/" + p_mainWidget->fileName() + "/AxisChannel").c_str(),
        "/annotation/axis").toString();
    nIter = settings.value(
        ("iRoCSPipeline/" + p_mainWidget->fileName() + "/nIter").c_str(),
        1000000).toInt();
    kappa = settings.value(
        ("iRoCSPipeline/" + p_mainWidget->fileName() + "/kappa").c_str(),
        1.0).toDouble();
    lambda = settings.value(
        ("iRoCSPipeline/" + p_mainWidget->fileName() + "/lambda").c_str(),
        0.0).toDouble();
    mu = settings.value(
        ("iRoCSPipeline/" + p_mainWidget->fileName() + "/mu").c_str(),
        0.0).toDouble();
    tau = settings.value(
        ("iRoCSPipeline/" + p_mainWidget->fileName() + "/tau").c_str(),
        0.01).toDouble();
    searchRadiusEnabled = settings.value(
        ("iRoCSPipeline/" + p_mainWidget->fileName() +
         "/searchRadiusEnabled").c_str(), false).toBool();
    searchRadiusUm = settings.value(
        ("iRoCSPipeline/" + p_mainWidget->fileName() +
         "/searchRadiusUm").c_str(), 0.0).toDouble();
  }
  if (p_markerChannelSelector->contains(annotationName.toStdString()))
      p_markerChannelSelector->setValue(annotationName.toStdString());
  if (p_qcChannelSelector->contains(qcName.toStdString()))
      p_qcChannelSelector->setValue(qcName.toStdString());
  p_iRoCSChannelNameControlElement->setValue(iRoCSName.toStdString());
  p_kappaControl->setValue(kappa);
  p_lambdaControl->setValue(lambda);
  p_muControl->setValue(mu);
  p_nIterControl->setValue(nIter);
  p_tauControl->setValue(tau);
  p_searchRadiusUmControl->setActive(searchRadiusEnabled);
  p_searchRadiusUmControl->setValue(searchRadiusUm);
}

AttachIRoCSParametersDialog::~AttachIRoCSParametersDialog()
{}

AnnotationChannelSpecs* AttachIRoCSParametersDialog::markerChannel() const
{
  return static_cast<AnnotationChannelSpecs*>(
      p_mainWidget->multiChannelModel()->channel(
          p_markerChannelSelector->toStdString()));
}

void AttachIRoCSParametersDialog::setMarkerChannel(
    AnnotationChannelSpecs *channel)
{
  if (p_markerChannelSelector->contains(channel->name()))
  {
    p_markerChannelSelector->setValue(channel->name());
  }
}

AnnotationChannelSpecs* AttachIRoCSParametersDialog::qcChannel() const
{
  return static_cast<AnnotationChannelSpecs*>(
      p_mainWidget->multiChannelModel()->channel(
          p_qcChannelSelector->toStdString()));
}

void AttachIRoCSParametersDialog::setQcChannel(AnnotationChannelSpecs *channel)
{
  if (p_qcChannelSelector->contains(channel->name()))
  {
    p_qcChannelSelector->setValue(channel->name());
  }
}

std::string AttachIRoCSParametersDialog::iRoCSChannelName() const
{
  return p_iRoCSChannelNameControlElement->value();
}

void AttachIRoCSParametersDialog::setIRoCSChannelName(std::string const &name)
{
  p_iRoCSChannelNameControlElement->setValue(name);
}

double AttachIRoCSParametersDialog::dataWeight() const
{
  return p_kappaControl->value();
}

void AttachIRoCSParametersDialog::setDataWeight(double kappa)
{
  p_kappaControl->setValue(kappa);
}

double AttachIRoCSParametersDialog::axisStiffnessWeight() const
{
  return p_lambdaControl->value();
}

void AttachIRoCSParametersDialog::setAxisStiffnessWeight(double lambda)
{
  p_lambdaControl->setValue(lambda);
}

double AttachIRoCSParametersDialog::thicknessConstancyWeight() const
{
  return p_muControl->value();
}

void AttachIRoCSParametersDialog::setThicknessConstancyWeight(double mu)
{
  p_muControl->setValue(mu);
}

double AttachIRoCSParametersDialog::searchRadiusUm() const
{
  if (!p_searchRadiusUmControl->isActive()) return 0.0;
  return p_searchRadiusUmControl->value();
}

void AttachIRoCSParametersDialog::setSearchRadiusUm(double searchRadiusUm)
{
  p_searchRadiusUmControl->setValue(searchRadiusUm);
  p_searchRadiusUmControl->setActive(true);
}

int AttachIRoCSParametersDialog::nIterations() const
{
  return p_nIterControl->value();
}

void AttachIRoCSParametersDialog::setNIterations(int nIter)
{
  p_nIterControl->setValue(nIter);
}

double AttachIRoCSParametersDialog::optimizationTimeStep() const
{
  return p_tauControl->value();
}

void AttachIRoCSParametersDialog::setOptimizationTimeStep(double tau)
{
  p_tauControl->setValue(tau);
}

std::string AttachIRoCSParametersDialog::check()
{
  try
  {
    // Check whether the selected marker channel contains enough markers
    int nEpidermisMarkers = 0;
    for (std::vector<Marker*>::const_iterator it = markerChannel()->begin();
         it != markerChannel()->end(); ++it)
    {
      if ((*it)->label() == 2) nEpidermisMarkers++;
    }
    if (nEpidermisMarkers < 9)
        return "Too few Epidermis markers in the provided annotation channel";

    // Check whether the selected QC channel contains markers
    if (qcChannel()->markers().size() == 0) return "No QC marker found";

    if (iRoCSChannelName() == "") return "No axis channel name specified";

    if (dataWeight() < 0.0) return "dataWeight must be >=0";
    if (axisStiffnessWeight() < 0.0) return "axisStiffnessWeight must be >=0";
    if (thicknessConstancyWeight() < 0.0)
        return "thicknessConstancyWeight must be >=0";
    if (nIterations() < 0) return "nIterations must be >=0";
    if (optimizationTimeStep() <= 0) return "Optimization time step must be >0";
  }
  catch (std::exception& e)
  {
    return std::string("Error while checking the parameters: ") + e.what();
  }
  return "";
}

void AttachIRoCSParametersDialog::checkAndAccept()
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
        "iRoCSPipeline/AnnotationChannel",
        p_markerChannelSelector->toStdString().c_str());
    settings.setValue(
        "iRoCSPipeline/QcChannel",
        p_qcChannelSelector->toStdString().c_str());
    settings.setValue(
        "iRoCSPipeline/AxisChannel",
        p_iRoCSChannelNameControlElement->toStdString().c_str());
    settings.setValue("iRoCSPipeline/nIter", p_nIterControl->value());
    settings.setValue("iRoCSPipeline/kappa", p_kappaControl->value());
    settings.setValue("iRoCSPipeline/lambda", p_lambdaControl->value());
    settings.setValue("iRoCSPipeline/mu", p_muControl->value());
    settings.setValue("iRoCSPipeline/tau", p_tauControl->value());
    settings.setValue(
        "iRoCSPipeline/searchRadiusEnabled",
        p_searchRadiusUmControl->isActive());
    settings.setValue(
        "iRoCSPipeline/searchRadiusUm", p_searchRadiusUmControl->value());
  }
  else
  {
    settings.setValue(
        ("iRoCSPipeline/" + p_mainWidget->fileName() +
         "/AnnotationChannel").c_str(),
        p_markerChannelSelector->toStdString().c_str());
    settings.setValue(
        ("iRoCSPipeline/" + p_mainWidget->fileName() + "/QcChannel").c_str(),
        p_qcChannelSelector->toStdString().c_str());
    settings.setValue(
        ("iRoCSPipeline/" + p_mainWidget->fileName() + "/AxisChannel").c_str(),
        p_iRoCSChannelNameControlElement->toStdString().c_str());
    settings.setValue(("iRoCSPipeline/" + p_mainWidget->fileName() +
                       "/nIter").c_str(), p_nIterControl->value());
    settings.setValue(("iRoCSPipeline/" + p_mainWidget->fileName() +
                       "/kappa").c_str(), p_kappaControl->value());
    settings.setValue(("iRoCSPipeline/" + p_mainWidget->fileName() +
                       "/lambda").c_str(), p_lambdaControl->value());
    settings.setValue(("iRoCSPipeline/" + p_mainWidget->fileName() +
                       "/mu").c_str(), p_muControl->value());
    settings.setValue(("iRoCSPipeline/" + p_mainWidget->fileName() +
                       "/tau").c_str(), p_tauControl->value());
    settings.setValue(
        ("iRoCSPipeline/" + p_mainWidget->fileName() +
         "searchRadiusEnabled").c_str(),
        p_searchRadiusUmControl->isActive());
    settings.setValue(
        ("iRoCSPipeline/" + p_mainWidget->fileName() +
         "searchRadiusUm").c_str(), p_searchRadiusUmControl->value());
  }
  
  accept();
}
