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

#include "DetectNucleiThread.hh"

#include <libIRoCS/DetectNucleiWorker.hh>

#include <liblabelling_qt4/NucleusMarker.hh>
#include <liblabelling_qt4/DataChannelSpecs.hh>
#include <liblabelling_qt4/VisualizationChannelSpecs.hh>
#include <liblabelling_qt4/AnnotationChannelSpecs.hh>

DetectNucleiThread::DetectNucleiThread(
    DetectNucleiParametersDialog const &parameters,
    AnnotationChannelSpecs &markerChannel,
    iRoCS::ProgressReporter *progress, 
    LabellingMainWidget *mainWidget,
    QWidget *parent) 
        : QThread(parent), _parameters(parameters),
          _markerChannel(markerChannel), p_progress(progress),
          p_mainWidget(mainWidget)
{}

DetectNucleiThread::~DetectNucleiThread()
{}

void DetectNucleiThread::run()
{
  atb::Array<double,3> data;
  if (_parameters.dataChannel()->channelType() == ChannelSpecs::Data)
  {
    DataChannelSpecs *channel =
        static_cast<DataChannelSpecs*>(_parameters.dataChannel());
    data.resize(channel->data()->shape());
    data.setElementSizeUm(channel->data()->elementSizeUm());
    data = blitz::cast<double>(*channel->data());
  }
  else if (_parameters.dataChannel()->channelType() ==
           ChannelSpecs::Visualization)
  {
    VisualizationChannelSpecs *channel =
        static_cast<VisualizationChannelSpecs*>(_parameters.dataChannel());
    data.resize(channel->data()->shape());
    data.setElementSizeUm(channel->data()->elementSizeUm());
    data = blitz::cast<double>(*channel->data());
  }
  
  std::vector<atb::Nucleus> nuclei;

  detectNuclei(
      data, nuclei, _parameters.modelFileName(),
      _parameters.memoryLimit(), _parameters.cacheFileName(), p_progress);
  
  if (p_progress != NULL && !p_progress->updateProgressMessage(
          tr("Populating annotation channel").toStdString())) return;
  for (size_t i = 0; i < nuclei.size(); ++i)
  {
    NucleusMarker* m =
        new NucleusMarker(nuclei[i].positionUm(), nuclei[i].radiusUm());
    m->setValue(nuclei[i].value());
    _markerChannel.addMarker(m);
  }
}
