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

#include "AttachIRoCSToCellSegmentationParametersDialog.hh"

#include <QtGui/QDialogButtonBox>
#include <QtGui/QMessageBox>
#include <QtCore/QSettings>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>

#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/VisualizationChannelSpecs.hh>

AttachIRoCSToCellSegmentationParametersDialog::
AttachIRoCSToCellSegmentationParametersDialog(
    LabellingMainWidget* mainWidget, QWidget* parent, Qt::WindowFlags f)
        : QDialog(parent, f), AttachIRoCSToCellSegmentationParameters(),
          p_mainWidget(mainWidget)
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
      tr("Select the channel containing the cell segmentation masks."));
  parameterLayout->addWidget(p_segmentationChannelSelector);
  p_qcChannelSelector = new ChannelSelectionControlElement(
      tr("QC channel"), p_mainWidget->multiChannelModel(),
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
  p_muControl->setSingleStep(0.0000001);
  iRoCSParametersLayout->addWidget(p_tauControl);  

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
  int nIter;
  double kappa, lambda, mu, tau;

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
    iRoCSName = "/annotation/axis";
    nIter = settings.value("iRoCSPipeline/nIter", 1000000).toInt();
    kappa = settings.value("iRoCSPipeline/kappa", 1.0).toDouble();
    lambda = settings.value("iRoCSPipeline/lambda", 0.0).toDouble();
    mu = settings.value("iRoCSPipeline/mu", 0.0).toDouble();
    tau = settings.value("iRoCSPipeline/tau", 0.01).toDouble();
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
  }
  if (p_segmentationChannelSelector->contains(segmentationName.toStdString()))
      p_segmentationChannelSelector->setValue(segmentationName.toStdString());
  if (p_qcChannelSelector->contains(qcName.toStdString()))
      p_qcChannelSelector->setValue(qcName.toStdString());
  p_iRoCSChannelNameControlElement->setValue(iRoCSName.toStdString());
  p_kappaControl->setValue(kappa);
  p_lambdaControl->setValue(lambda);
  p_muControl->setValue(mu);
  p_nIterControl->setValue(nIter);
  p_tauControl->setValue(tau);
}

AttachIRoCSToCellSegmentationParametersDialog::
~AttachIRoCSToCellSegmentationParametersDialog()
{}

VisualizationChannelSpecs*
AttachIRoCSToCellSegmentationParametersDialog::segmentationChannel() const
{
  return static_cast<VisualizationChannelSpecs*>(
      p_mainWidget->multiChannelModel()->channel(
          p_segmentationChannelSelector->toStdString()));
}

void AttachIRoCSToCellSegmentationParametersDialog::setSegmentationChannel(
    VisualizationChannelSpecs *channel)
{
  if (p_segmentationChannelSelector->contains(channel->name()))
  {
    p_segmentationChannelSelector->setValue(channel->name());
  }
}

AnnotationChannelSpecs*
AttachIRoCSToCellSegmentationParametersDialog::qcChannel() const
{
  return static_cast<AnnotationChannelSpecs*>(
      p_mainWidget->multiChannelModel()->channel(
          p_qcChannelSelector->toStdString()));
}

void AttachIRoCSToCellSegmentationParametersDialog::setQcChannel(
    AnnotationChannelSpecs *channel)
{
  if (p_qcChannelSelector->contains(channel->name()))
  {
    p_qcChannelSelector->setValue(channel->name());
  }
}

std::string AttachIRoCSToCellSegmentationParametersDialog::iRoCSChannelName()
    const
{
  return p_iRoCSChannelNameControlElement->value();
}

void AttachIRoCSToCellSegmentationParametersDialog::setIRoCSChannelName(
    std::string const &name)
{
  p_iRoCSChannelNameControlElement->setValue(name);
}

double AttachIRoCSToCellSegmentationParametersDialog::dataWeight() const
{
  return p_kappaControl->value();
}

void AttachIRoCSToCellSegmentationParametersDialog::setDataWeight(double kappa)
{
  p_kappaControl->setValue(kappa);
}

double AttachIRoCSToCellSegmentationParametersDialog::axisStiffnessWeight()
    const
{
  return p_lambdaControl->value();
}

void AttachIRoCSToCellSegmentationParametersDialog::setAxisStiffnessWeight(
    double lambda)
{
  p_lambdaControl->setValue(lambda);
}

double
AttachIRoCSToCellSegmentationParametersDialog::thicknessConstancyWeight() const
{
  return p_muControl->value();
}

void AttachIRoCSToCellSegmentationParametersDialog::setThicknessConstancyWeight(
    double mu)
{
  p_muControl->setValue(mu);
}

int AttachIRoCSToCellSegmentationParametersDialog::nIterations() const
{
  return p_nIterControl->value();
}

void AttachIRoCSToCellSegmentationParametersDialog::setNIterations(int nIter)
{
  p_nIterControl->setValue(nIter);
}

double AttachIRoCSToCellSegmentationParametersDialog::optimizationTimeStep()
    const
{
  return p_tauControl->value();
}

void AttachIRoCSToCellSegmentationParametersDialog::setOptimizationTimeStep(
    double tau)
{
  p_tauControl->setValue(tau);
}

void AttachIRoCSToCellSegmentationParametersDialog::checkAndAccept()
{
  std::string res = check();
  
  if (res != "")
  {
    QMessageBox::warning(p_mainWidget, tr("Plugin prerequisites not met"),
                         res.c_str());
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
    settings.setValue("iRoCSPipeline/nIter", p_nIterControl->value());
    settings.setValue("iRoCSPipeline/kappa", p_kappaControl->value());
    settings.setValue("iRoCSPipeline/lambda", p_lambdaControl->value());
    settings.setValue("iRoCSPipeline/mu", p_muControl->value());
    settings.setValue("iRoCSPipeline/tau", p_tauControl->value());
  }
  else
  {
    settings.setValue(("iRoCSPipeline/" + p_mainWidget->fileName() +
                       "/SegmentationChannel").c_str(),
                      p_segmentationChannelSelector->toStdString().c_str());
    settings.setValue(("iRoCSPipeline/" + p_mainWidget->fileName() +
                       "/QcChannel").c_str(),
                      p_qcChannelSelector->toStdString().c_str());
    settings.setValue(("iRoCSPipeline/" + p_mainWidget->fileName() +
                       "/AxisChannel").c_str(),
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
  }
  
  accept();
}
