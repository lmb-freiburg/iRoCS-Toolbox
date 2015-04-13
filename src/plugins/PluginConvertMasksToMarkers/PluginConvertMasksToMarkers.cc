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

#include "PluginConvertMasksToMarkers.hh"

#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/AnnotationChannelSpecs.hh>
#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/StringControlElement.hh>
#include <liblabelling_qt4/StringSelectionControlElement.hh>
#include <liblabelling_qt4/DoubleControlElement.hh>

#include <QtGui/QMessageBox>
#include <QtGui/QStatusBar>
#include <QtGui/QProgressBar>
#include <QtGui/QApplication>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#endif

PluginConvertMasksToMarkers::PluginConvertMasksToMarkers() 
        : QObject(), p_mainWidget(NULL), p_dialog(NULL), p_workerThread(NULL),
          _interactive(true)
{}

PluginConvertMasksToMarkers::~PluginConvertMasksToMarkers()
{
  delete p_dialog;
}

QString PluginConvertMasksToMarkers::name() const
{
  return QString(tr("Convert Masks To Markers"));
}

void PluginConvertMasksToMarkers::setLabellingMainWidget(
    LabellingMainWidget* parent)
{
  p_mainWidget = parent;
  delete p_dialog;
  p_dialog = new ConvertMasksToMarkersParameters(p_mainWidget, p_mainWidget);
}

void PluginConvertMasksToMarkers::setParameters(
    const std::map<std::string,std::string>& parameters)
{
  assert(p_dialog != NULL);
  p_dialog->p_maskChannelSelector->setValue(
      parameters.find("maskChannel")->second);
  p_dialog->p_annotationChannelControlElement->setValue(
      parameters.find("annotationChannel")->second);
  p_dialog->p_markerTypeControlElement->setValue(
      parameters.find("markerType")->second);
  p_dialog->p_smoothingSigmaPxControl->setValue(
      atof(parameters.find("sigmaPx")->second.c_str()));
}

void PluginConvertMasksToMarkers::run()
{
  // Check whether at least one visualization channel is loaded,
  // otherwise give up
  if (p_mainWidget->multiChannelModel()->nChannels(
          ChannelSpecs::Visualization) == 0)
  {
    QMessageBox::warning(
        p_mainWidget, tr("Plugin prerequisites not met"),
        tr("This plugin needs one visualization channel containing the "
           "masks but none is available."));
    return;
  }
  if (_interactive && p_dialog->exec() == QDialog::Rejected) return;

  AnnotationChannelSpecs* markerChannel =
      p_mainWidget->multiChannelModel()->addAnnotationChannel(
          p_dialog->markerType());
  markerChannel->setName(p_dialog->annotationChannelName());
  markerChannel->setUpdatesEnabled(false);

  p_mainWidget->setUserInteractionEnabled(false);

  iRoCS::ProgressReporterQt4* pr = p_mainWidget->progressReporter();
  pr->reset();
  pr->setVisible(true);
  
  p_workerThread = new ConvertMasksToMarkersWorker(
      p_dialog, p_mainWidget, markerChannel, pr);
  p_workerThread->start();
  while (p_workerThread->isRunning()) 
  {
#if defined(_WIN32) || defined(_WIN64)
    Sleep(10);
#else
    usleep(10000);
#endif
    qApp->processEvents();
  }
  if (pr->isAborted())
      p_mainWidget->multiChannelModel()->removeChannel(markerChannel);    
  else
  {
    markerChannel->setUpdatesEnabled(true);
    markerChannel->update();
  }
  delete p_workerThread;
  p_workerThread = NULL;

  pr->setVisible(false);
  pr->reset();

  p_mainWidget->setUserInteractionEnabled(true);
}

void PluginConvertMasksToMarkers::abort()
{
  if (p_workerThread != NULL) p_workerThread->abort();
}

Q_EXPORT_PLUGIN2(PluginConvertMasksToMarkers, PluginConvertMasksToMarkers)
