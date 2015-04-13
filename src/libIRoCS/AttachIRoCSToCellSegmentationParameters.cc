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

#include "AttachIRoCSToCellSegmentationParameters.hh"

AttachIRoCSToCellSegmentationParameters::
AttachIRoCSToCellSegmentationParameters()
        : p_segmentationChannel(NULL), p_qcChannel(NULL),
          _iRoCSChannelName("/annotation/axis"), _kappa(1.0), _lambda(0.0),
          _mu(0.0), _tau(0.1), _nIter(INT_MAX)
{}

AttachIRoCSToCellSegmentationParameters::
~AttachIRoCSToCellSegmentationParameters()
{}

VisualizationChannelSpecs*
AttachIRoCSToCellSegmentationParameters::segmentationChannel() const
{
  return p_segmentationChannel;
}

void AttachIRoCSToCellSegmentationParameters::setSegmentationChannel(
    VisualizationChannelSpecs *channel)
{
  p_segmentationChannel = channel;
}

AnnotationChannelSpecs* AttachIRoCSToCellSegmentationParameters::qcChannel()
    const
{
  return p_qcChannel;
}

void AttachIRoCSToCellSegmentationParameters::setQcChannel(
    AnnotationChannelSpecs *channel)
{
  p_qcChannel = channel;
}

std::string AttachIRoCSToCellSegmentationParameters::iRoCSChannelName() const
{
  return _iRoCSChannelName;
}

void AttachIRoCSToCellSegmentationParameters::setIRoCSChannelName(
    std::string const &name)
{
  _iRoCSChannelName = name;
}

double AttachIRoCSToCellSegmentationParameters::dataWeight() const
{
  return _kappa;
}

void AttachIRoCSToCellSegmentationParameters::setDataWeight(double kappa)
{
  _kappa = kappa;
}

double AttachIRoCSToCellSegmentationParameters::axisStiffnessWeight() const
{
  return _lambda;
}

void AttachIRoCSToCellSegmentationParameters::setAxisStiffnessWeight(
    double lambda)
{
  _lambda = lambda;
}

double AttachIRoCSToCellSegmentationParameters::thicknessConstancyWeight() const
{
  return _mu;
}

void AttachIRoCSToCellSegmentationParameters::setThicknessConstancyWeight(
    double mu)
{
  _mu = mu;
}

int AttachIRoCSToCellSegmentationParameters::nIterations() const
{
  return _nIter;
}

void AttachIRoCSToCellSegmentationParameters::setNIterations(int nIter)
{
  _nIter = nIter;
}

double AttachIRoCSToCellSegmentationParameters::optimizationTimeStep() const
{
  return _tau;
}

void AttachIRoCSToCellSegmentationParameters::setOptimizationTimeStep(
    double tau)
{
  _tau = tau;
}

std::string AttachIRoCSToCellSegmentationParameters::check()
{
  try
  {
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

