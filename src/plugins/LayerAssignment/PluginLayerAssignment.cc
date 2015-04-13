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

#include "PluginLayerAssignment.hh"
#include "LayerAssignmentThread.hh"

#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

#include <liblabelling_qt4/MultiChannelModel.hh>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#endif

PluginLayerAssignment::PluginLayerAssignment() 
        : QObject(), p_mainWidget(NULL), p_dialog(NULL), _interactive(true),
          p_progress(NULL)
{}

PluginLayerAssignment::~PluginLayerAssignment()
{
  delete p_dialog;
}

QString PluginLayerAssignment::name() const
{
  return QString(tr("04 - Assign Layers"));
}

void PluginLayerAssignment::setLabellingMainWidget(
    LabellingMainWidget* parent)
{
  p_mainWidget = parent;
  delete p_dialog;
  p_dialog = new LayerAssignmentParametersDialog(p_mainWidget, p_mainWidget);
}

void PluginLayerAssignment::setParameters(
    const std::map<std::string,std::string>& parameters)
{
  if (parameters.find("dataChannel") != parameters.end())
      p_dialog->setDataChannel(
          p_mainWidget->multiChannelModel()->channel(
              parameters.find("dataChannel")->second));

  if (parameters.find("annotationChannel") != parameters.end())
      p_dialog->setAnnotationChannel(
          static_cast<AnnotationChannelSpecs*>(
              p_mainWidget->multiChannelModel()->channel(
                  parameters.find("annotationChannel")->second)));

  if (parameters.find("iRoCSChannel") != parameters.end())
      p_dialog->setIRoCSChannel(
          static_cast<IRoCSChannelSpecs*>(
              p_mainWidget->multiChannelModel()->channel(
                  parameters.find("iRoCSChannel")->second)));

  if (parameters.find("cacheFile") != parameters.end())
      p_dialog->setCacheFileName(parameters.find("cacheFile")->second);

  if (parameters.find("modelFile") != parameters.end())
      p_dialog->setModelFileName(parameters.find("modelFile")->second);

  if (parameters.find("cacheCoordinates") != parameters.end())
      p_dialog->setCacheCoordinates(
          parameters.find("cacheCoordinates")->second == "1");

  if (parameters.find("updateMitoses") != parameters.end())
      p_dialog->setUpdateMitoses(
          parameters.find("updateMitoses")->second == "1");

  _interactive = false;
}

void PluginLayerAssignment::run()
{
  // Check whether at least one annotation channel is loaded, otherwise give up
  if (p_mainWidget->multiChannelModel()->nChannels(ChannelSpecs::Annotation)
      == 0)
  {
    QMessageBox::warning(
        p_mainWidget, tr("Plugin prerequisites not met"),
        tr("This plugin needs one annotation channel to classify nucleus "
           "candidates in. Please load an annotation channel."));
    return;
  }
  // Check for an iRoCS Channel
  if (p_mainWidget->multiChannelModel()->nChannels(ChannelSpecs::IRoCS) == 0)
  {
    QMessageBox::warning(
        p_mainWidget, tr("Plugin prerequisites not met"),
        tr("This plugin needs an axis channel. Please load an iRoCS axis "
           "model or run the 'Attach iRoCS' plugin."));
    return;
  }
  if (_interactive && p_dialog->exec() == QDialog::Rejected) return;

  p_progress = p_mainWidget->progressReporter();
  p_progress->reset();
  p_progress->setVisible(true);

  LayerAssignmentThread *workerThread = new LayerAssignmentThread(
      *p_dialog, p_progress, p_mainWidget, p_mainWidget);
  p_mainWidget->setUserInteractionEnabled(false);
  p_dialog->annotationChannel()->setUpdatesEnabled(false);

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
  delete workerThread;
  p_progress->reset();
  p_progress->setVisible(false);
  p_mainWidget->setUserInteractionEnabled(true);
  p_dialog->annotationChannel()->setUpdatesEnabled(true);
  p_dialog->annotationChannel()->update();
}

void PluginLayerAssignment::abort()
{
  if (p_progress != NULL) p_progress->abort();
}

Q_EXPORT_PLUGIN2(PluginLayerAssignment, PluginLayerAssignment)
