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

#include "PluginAssignLayersToCellSegmentation.hh"
#include "AssignLayersToCellSegmentationThread.hh"

#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/VisualizationChannelSpecs.hh>
#include <liblabelling_qt4/IRoCSSCTChannelSpecs.hh>
#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/FileNameSelectionControlElement.hh>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#endif

PluginAssignLayersToCellSegmentation::
PluginAssignLayersToCellSegmentation() 
        : QObject(), p_mainWidget(NULL), p_dialog(NULL), _interactive(true),
          p_progress(NULL)
{}

PluginAssignLayersToCellSegmentation::
~PluginAssignLayersToCellSegmentation()
{
  delete p_dialog;
}

QString PluginAssignLayersToCellSegmentation::name() const
{
  return tr("C03 - Assign layers to segmented cells");
}

void PluginAssignLayersToCellSegmentation::setLabellingMainWidget(
    LabellingMainWidget* parent)
{
  p_mainWidget = parent;
  delete p_dialog;
  p_dialog = new AssignLayersToCellSegmentationParametersDialog(
      p_mainWidget, p_mainWidget);
}

void PluginAssignLayersToCellSegmentation::setParameters(
    const std::map<std::string,std::string>& parameters)
{
  std::map<std::string,std::string>::const_iterator it;
  if ((it = parameters.find("segmentationChannel")) != parameters.end())
      p_dialog->setSegmentationChannel(
          static_cast<VisualizationChannelSpecs*>(
              p_mainWidget->multiChannelModel()->channel(it->second)));
  if ((it = parameters.find("iRoCSChannel")) != parameters.end())
      p_dialog->setIRoCSChannel(
          static_cast<IRoCSSCTChannelSpecs*>(
              p_mainWidget->multiChannelModel()->channel(it->second)));
  if ((it = parameters.find("outFileName")) != parameters.end())
      p_dialog->setOutFileName(it->second);
  if ((it = parameters.find("labelName")) != parameters.end())
      p_dialog->setLabelName(it->second);
  if ((it = parameters.find("modelFileName")) != parameters.end())
      p_dialog->setModelFileName(it->second);
  if ((it = parameters.find("featureFileName")) != parameters.end())
      p_dialog->setFeatureFileName(it->second);
  if ((it = parameters.find("featureGroup")) != parameters.end())
      p_dialog->setFeatureGroup(it->second);
  if ((it = parameters.find("volumeThresholdUm3")) != parameters.end())
      p_dialog->setVolumeThresholdUm3(atof(it->second.c_str()));
  if ((it = parameters.find("backgroundLabel")) != parameters.end())
      p_dialog->setBackgroundLabel(atoi(it->second.c_str()));
  _interactive = false;
}

void PluginAssignLayersToCellSegmentation::run()
{
  // Check whether at least one visualization and one annotation channel are
  // loaded, otherwise give up
  if (p_mainWidget->multiChannelModel()->nChannels(ChannelSpecs::IRoCSSCT) < 1 ||
      p_mainWidget->multiChannelModel()->nChannels(ChannelSpecs::Visualization)
      < 1)
  {
    QMessageBox::warning(
        p_mainWidget, tr("Plugin prerequisites not met"),
        tr("This plugin needs one visualization channel containing "
           "a cellular segmentation and one iRoCS shell coordinate transform "
           "channel containing the root coordinate system.\nThe segmentation "
           "channel is the output of the 'Segment Cells' plugin.\nThe iRoCS "
           "channel is the output of the 'Attach iRoCS to cell segmentation' "
           "plugin."));
    return;
  }
  if (_interactive && p_dialog->exec() == QDialog::Rejected) return;

  p_progress = p_mainWidget->progressReporter();
  p_progress->reset();
  p_progress->setVisible(true);

  AssignLayersToCellSegmentationThread *workerThread =
      new AssignLayersToCellSegmentationThread(
          *p_dialog, p_progress, p_mainWidget, p_mainWidget);
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
    // Set the segment labels to their class label. This is not optimal, since
    // the segment indices get lost when the model is saved to a new file.
    try
    {
      BlitzH5File inFile(p_dialog->outFileName());
      blitz::Array<int,1> predictedLabels;
      inFile.readDataset(predictedLabels, p_dialog->labelName());

      p_progress->updateProgressMessage(
          "Assigning predicted labels to segmentation masks");
      atb::Array<int,3> &segmentation = *p_dialog->segmentationChannel()->data();
      int p = 0;
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(segmentation.size());
           ++i)
      {
        if (p_progress->isAborted()) continue;
        int progress;
#ifdef _OPENMP
#pragma omp critical
#endif
        {
          progress = p;
          ++p;
        }
        if (progress %
            std::max(1, static_cast<int>(segmentation.size()) / 100) == 0)
            p_progress->updateProgress(
                100 * static_cast<double>(progress) /
                static_cast<double>(segmentation.size() - 1));
        if (segmentation.data()[i] > 0)
            segmentation.data()[i] = predictedLabels(segmentation.data()[i] - 1);
      }
      p_dialog->segmentationChannel()->revalidate();
      p_dialog->segmentationChannel()->update();
    }
    catch (BlitzH5Error &e)
    {
      p_mainWidget->showError(
          tr("Could not load the predicted labels: %1").arg(e.what()));
    }
  }

  delete workerThread;
  p_progress->reset();
  p_progress->setVisible(false);
  p_mainWidget->setUserInteractionEnabled(true);
}

void PluginAssignLayersToCellSegmentation::abort()
{
  if (p_progress != NULL) p_progress->abort();
}

Q_EXPORT_PLUGIN2(PluginAssignLayersToCellSegmentation, PluginAssignLayersToCellSegmentation)
