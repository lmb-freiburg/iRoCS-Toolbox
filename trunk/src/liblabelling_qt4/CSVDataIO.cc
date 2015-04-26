/**************************************************************************
 *
 * This file belongs to the iRoCS Toolbox.
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

#include "CSVDataIO.hh"

#include <QtCore/QSettings>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include "LabellingMainWidget.hh"
#include "PointMarker.hh"
#include "SphereMarker.hh"
#include "NucleusMarker.hh"

#include "MultiChannelModel.hh"
#include "AnnotationChannelSpecs.hh"

#include <fstream>

void CSVDataIO::importAnnotationChannel(LabellingMainWidget* mainWidget)
{
  if (mainWidget->multiChannelModel()->nChannels() == 0)
  {
    QMessageBox::critical(
        qApp->activeWindow(), tr("Annotation Channel creation failed"),
        tr("Could not create new annotation channel. You need to load a "
           "data or visualization channel before loading annotations"));
    return;
  }

  QSettings settings;
  QString startPath = settings.value(
      "FileDialog/startpath", getenv("HOME")).toString();
  startPath.truncate(startPath.lastIndexOf("/"));
  QString fileName = QFileDialog::getOpenFileName(
      mainWidget, tr("Choose CSV Annotation File"), startPath,
      tr("Comma Separated Values (*.csv)"));
  if (fileName == QString::Null()) return;

  AnnotationChannelSpecs *channel = AnnotationChannelSpecs::loadCSV(
      fileName.toStdString());
  if (channel == NULL) return;
  mainWidget->multiChannelModel()->addChannel(channel);
}

void CSVDataIO::exportAnnotationChannel(LabellingMainWidget* mainWidget)
{
  if (mainWidget->multiChannelModel()->selectedChannel()->channelType() !=
      ChannelSpecs::Annotation)
  {
    QMessageBox::information(
        mainWidget, tr("CSV annotation export error"),
        tr("The selected channel '") +
        mainWidget->multiChannelModel()->selectedChannel()->name().c_str() +
        tr("' is no annotation channel. Please select an annotation channel."));
    return;    
  }
  AnnotationChannelSpecs *channel = static_cast<AnnotationChannelSpecs*>(
      mainWidget->multiChannelModel()->selectedChannel());
  if (channel->markers().size() == 0)
  {
    QMessageBox::information(
        mainWidget, tr("CSV annotation export error"),
        tr("The selected channel '%1' does not contain markers. "
           "Nothing to save.").arg(channel->name().c_str()));
    return;    
  }
  
  QSettings settings;
  QString startPath = settings.value(
      "FileDialog/startpath", getenv("HOME")).toString();  
  startPath.truncate(startPath.lastIndexOf("/"));
  QString fileName = QFileDialog::getSaveFileName(
      mainWidget, tr("Enter CSV Annotation File name"), startPath,
      tr("CSV (*.csv)"));
  if (fileName == QString::Null()) return;

  channel->saveCSV(fileName.toStdString());
}
