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

#include "EpidermisLabellingThread.hh"

#include <libIRoCS/EpidermisLabellingWorker.hh>

#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/ChannelSpecsRenderer.hh>
#include <liblabelling_qt4/DataChannelSpecs.hh>
#include <liblabelling_qt4/VisualizationChannelSpecs.hh>
#include <liblabelling_qt4/NucleusMarker.hh>

EpidermisLabellingThread::EpidermisLabellingThread(
    EpidermisLabellingParametersDialog const &parameters,
    iRoCS::ProgressReporter *progress,
    LabellingMainWidget* mainWidget,
    QWidget* parent) 
        : QThread(parent), _parameters(parameters), p_progress(progress),
          p_mainWidget(mainWidget)
{}

EpidermisLabellingThread::~EpidermisLabellingThread()
{}

void EpidermisLabellingThread::run()
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
  for (std::vector<Marker*>::const_iterator it =
           _parameters.annotationChannel()->begin();
       it != _parameters.annotationChannel()->end(); ++it)
  {
    atb::Nucleus nc;
    nuclei.push_back(nc);
    (*it)->copyToATBNucleus(nuclei.back());
  }
  
  iRoCS::labelEpidermis(
      data, nuclei, _parameters.modelFileName(), _parameters.cacheFileName(),
      _parameters.forceFeatureComputation(), p_progress);
  
  int i = 0;
  for (std::vector<Marker*>::const_iterator it =
           _parameters.annotationChannel()->begin();
       it != _parameters.annotationChannel()->end(); ++it, ++i)
      (*it)->copyFromATBNucleus(nuclei[i]);
}
