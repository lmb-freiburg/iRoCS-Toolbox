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

#include "PluginAttachIRoCS.hh"
#include "AttachIRoCSThread.hh"

#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/IRoCSChannelSpecs.hh>
#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/FileNameSelectionControlElement.hh>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#endif

PluginAttachIRoCS::PluginAttachIRoCS() 
        : QObject(), p_mainWidget(NULL), p_dialog(NULL), _interactive(true),
          p_progress(NULL)
{}

PluginAttachIRoCS::~PluginAttachIRoCS()
{
  delete p_dialog;
}

QString PluginAttachIRoCS::name() const
{
  return QString(tr("03 - Attach iRoCS"));
}

void PluginAttachIRoCS::setLabellingMainWidget(LabellingMainWidget* parent)
{
  p_mainWidget = parent;
  delete p_dialog;
  p_dialog = new AttachIRoCSParametersDialog(p_mainWidget, p_mainWidget);
}

void PluginAttachIRoCS::setParameters(
    const std::map<std::string,std::string>& parameters)
{
  std::map<std::string,std::string>::const_iterator it;
  if ((it = parameters.find("markerChannel")) != parameters.end())
      p_dialog->setMarkerChannel(
          static_cast<AnnotationChannelSpecs*>(
              p_mainWidget->multiChannelModel()->channel(it->second)));
  if ((it = parameters.find("qcChannel")) != parameters.end())
      p_dialog->setQcChannel(
          static_cast<AnnotationChannelSpecs*>(
              p_mainWidget->multiChannelModel()->channel(it->second)));
  if ((it = parameters.find("iRoCSChannelName")) != parameters.end())
      p_dialog->setIRoCSChannelName(it->second);
  if ((it = parameters.find("nIterations")) != parameters.end())
      p_dialog->setNIterations(atoi(it->second.c_str()));
  if ((it = parameters.find("dataWeight")) != parameters.end())
      p_dialog->setDataWeight(atof(it->second.c_str()));
  if ((it = parameters.find("axisStiffnessWeight")) != parameters.end())
      p_dialog->setAxisStiffnessWeight(atof(it->second.c_str()));
  if ((it = parameters.find("thicknessConstancyWeight")) != parameters.end())
      p_dialog->setThicknessConstancyWeight(atof(it->second.c_str()));
  if ((it = parameters.find("optimizationTimeStep")) != parameters.end())
      p_dialog->setOptimizationTimeStep(atof(it->second.c_str()));
  _interactive = false;
}

void PluginAttachIRoCS::run()
{
  // Check whether at least two annotation channels are
  // loaded, otherwise give up
  if (p_mainWidget->multiChannelModel()->nChannels(ChannelSpecs::Annotation)
      < 2)
  {
    QMessageBox::warning(
        p_mainWidget, tr("Plugin prerequisites not met"),
        tr("This plugin needs one annotation channel containing "
           "pre-classified nucleus candidates and one annotation channel "
           "containing one marker at the QC position.\nThe pre-classifed "
           "marker channel typically is the output of the 'Detect Nuclei' "
           "and 'Label Epidermis' plugins. You can also attach the "
           "coordinate system based on a sufficient amount of manually "
           "annotated epidermis nuclei, for this create a new annotation "
           "channel and add markers with label 2 (Epidermis).\nThe QC "
           "annotation channel has to be prepared manually by creating a new "
           "annotation channel and clicking the QC position once. Only the "
           "first marker of the channel will be regarded, so at best specify "
           "only one to not confuse yourself."));
    return;
  }
  if (_interactive && p_dialog->exec() == QDialog::Rejected) return;

  p_progress = p_mainWidget->progressReporter();
  p_progress->reset();
  p_progress->setVisible(true);

  atb::IRoCS *rct = new atb::IRoCS(p_progress);

  AttachIRoCSThread *workerThread = new AttachIRoCSThread(
      *p_dialog, rct, p_progress, p_mainWidget, p_mainWidget);
  p_mainWidget->setUserInteractionEnabled(false);

  workerThread->start();
  while (workerThread->isRunning()) 
  {
#if defined(_WIN32) || defined(_WIN64)
    Sleep(10);
#else
    usleep(10000);
#endif
    qApp->processEvents();
  }

  if (!p_progress->isAborted())
  {
    IRoCSChannelSpecs* channel =
        p_mainWidget->multiChannelModel()->addIRoCSChannel(rct);
    if (channel != NULL)
    {
      channel->setName(p_dialog->iRoCSChannelName());
      channel->setColor(blitz::TinyVector<float,3>(1.0, 1.0, 0.0));
    }
    else
    {
      QMessageBox::warning(
          p_mainWidget, tr("Channel creation failed"),
          tr("Root axis channel could not be created. The root coordinate "
             "system is invalid."));
    }
  }
  delete workerThread;
  p_progress->reset();
  p_progress->setVisible(false);
  p_mainWidget->setUserInteractionEnabled(true);
}

void PluginAttachIRoCS::abort()
{
  if (p_progress != NULL) p_progress->abort();
}

Q_EXPORT_PLUGIN2(PluginAttachIRoCS, PluginAttachIRoCS)
