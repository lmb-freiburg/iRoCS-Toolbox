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

#include "TrainingParameters.hh"
#include "TrainfileParameters.hh"

TrainingParameters::TrainingParameters()
        : _trainFiles(), _featureGroup("/features"),
          _generateRandomSamples(false), _nInRootSamples(0),
          _nOutRootSamples(0), _modelFileName("svmModel.h5"),
          _sdNormalization(iRoCS::Features::FeatureZeroMeanStddev),
          _houghNormalization(iRoCS::Features::FeatureZeroMeanStddev),
          _cost(100.0), _gamma(0.01)
{}

TrainingParameters::~TrainingParameters()
{
  for (size_t i = 0; i < _trainFiles.size(); ++i) delete _trainFiles[i];
}

void TrainingParameters::addTrainFile(TrainfileParameters* parameters)
{
  _trainFiles.push_back(parameters);
}

std::vector<TrainfileParameters*> TrainingParameters::trainFiles() const
{
  return _trainFiles;
}

void TrainingParameters::setFeatureGroup(std::string const &group)
{
  _featureGroup = group;
}

std::string TrainingParameters::featureGroup() const
{
  return _featureGroup;
}

void TrainingParameters::setGenerateRandomSamples(bool generate)
{
  _generateRandomSamples = generate;
}

bool TrainingParameters::generateRandomSamples() const
{
  return _generateRandomSamples;
}

void TrainingParameters::setNInRootSamples(int samples)
{
  _nInRootSamples = samples;
}

int TrainingParameters::nInRootSamples() const
{
  return _nInRootSamples;
}

void TrainingParameters::setNOutRootSamples(int samples)
{
  _nOutRootSamples = samples;
}

int TrainingParameters::nOutRootSamples() const
{
  return _nOutRootSamples;
}

void TrainingParameters::setModelFileName(std::string const &name)
{
  _modelFileName = name;
}

std::string TrainingParameters::modelFileName() const
{
  return _modelFileName;
}

void TrainingParameters::setSdFeatureNormalization(
    iRoCS::Features::NormalizationType normType)
{
  _sdNormalization = normType;
}

iRoCS::Features::NormalizationType
TrainingParameters::sdFeatureNormalization() const
{
  return _sdNormalization;
}

void TrainingParameters::setHoughFeatureNormalization(
    iRoCS::Features::NormalizationType normType)
{
  _houghNormalization = normType;
}

iRoCS::Features::NormalizationType
TrainingParameters::houghFeatureNormalization() const
{
  return _houghNormalization;
}

void TrainingParameters::setCost(double cost)
{
  _cost = cost;
}

double TrainingParameters::cost() const
{
  return _cost;
}

void TrainingParameters::setGamma(double gamma)
{
  _gamma = gamma;
}

double TrainingParameters::gamma() const
{
  return _gamma;
}

std::string TrainingParameters::check()
{
  // Check input files
  std::vector<TrainfileParameters*> files(trainFiles());
  if (files.size() == 0) return "No input file selected for training";

  int nPositiveSamples = 0, nSamples = 0;
  for (size_t i = 0; i < files.size(); ++i)
  {
    std::string res = files[i]->check(nPositiveSamples, nSamples);
    if (res != "") return res;
  }
  if (nSamples == 0) return "No training samples found in the given files";
  if (nPositiveSamples == 0)
      return "No positive training samples found in the given files";
  if (nPositiveSamples == nSamples && nInRootSamples() + nOutRootSamples() == 0)
      return "No negative samples available for discriminative training. "
          "You need to generate random negative samples.";

  return "";
}

