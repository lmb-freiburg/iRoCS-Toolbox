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

#include "PluginEpidermisLabelling.hh"
#include "EpidermisLabellingThread.hh"

#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/FileNameSelectionControlElement.hh>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#endif

PluginEpidermisLabelling::PluginEpidermisLabelling() 
        : QObject(), p_mainWidget(NULL), p_dialog(NULL), _interactive(true),
          p_progress(NULL)
{}

PluginEpidermisLabelling::~PluginEpidermisLabelling()
{
  delete p_dialog;
}

QString PluginEpidermisLabelling::name() const
{
  return QString(tr("02 - Label Epidermis"));
}

void PluginEpidermisLabelling::setLabellingMainWidget(
    LabellingMainWidget* parent)
{
  p_mainWidget = parent;
  delete p_dialog;
  p_dialog = new EpidermisLabellingParametersDialog(p_mainWidget, p_mainWidget);
}

void PluginEpidermisLabelling::setParameters(
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

  if (parameters.find("cacheFile") != parameters.end())
      p_dialog->setCacheFileName(parameters.find("cacheFile")->second);

  if (parameters.find("modelFile") != parameters.end())
      p_dialog->setModelFileName(parameters.find("modelFile")->second);

  _interactive = false;
}

void PluginEpidermisLabelling::run()
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
  if (_interactive && p_dialog->exec() == QDialog::Rejected) return;

  p_progress = p_mainWidget->progressReporter();
  p_progress->reset();
  p_progress->setVisible(true);

  EpidermisLabellingThread *workerThread = new EpidermisLabellingThread(
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

void PluginEpidermisLabelling::abort()
{
  if (p_progress != NULL) p_progress->abort();
}

Q_EXPORT_PLUGIN2(PluginEpidermisLabelling, PluginEpidermisLabelling)
