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

#ifndef TRAININGPARAMETERS_HH
#define TRAININGPARAMETERS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>
#include <vector>

#include "iRoCSFeatures.hh"

class TrainfileParameters;

class TrainingParameters
{

public:
  
  TrainingParameters();
  virtual ~TrainingParameters();

  void addTrainFile(TrainfileParameters* parameters);
  virtual std::vector<TrainfileParameters*> trainFiles() const;

  virtual void setFeatureGroup(std::string const &group);
  virtual std::string featureGroup() const;

  virtual void setGenerateRandomSamples(bool generate);
  virtual bool generateRandomSamples() const;

  virtual void setNInRootSamples(int samples);
  virtual int nInRootSamples() const;

  virtual void setNOutRootSamples(int samples);
  virtual int nOutRootSamples() const;

  virtual void setModelFileName(std::string const &name);
  virtual std::string modelFileName() const;

  virtual void setSdFeatureNormalization(
      iRoCS::Features::NormalizationType normType);
  virtual iRoCS::Features::NormalizationType sdFeatureNormalization() const;

  virtual void setHoughFeatureNormalization(
      iRoCS::Features::NormalizationType normType);
  virtual iRoCS::Features::NormalizationType houghFeatureNormalization() const;

  virtual void setCost(double cost);
  virtual double cost() const;

  virtual void setGamma(double gamma);
  virtual double gamma() const;

  std::string check();

private:

  std::vector<TrainfileParameters*> _trainFiles;
  std::string _featureGroup;
  bool _generateRandomSamples;
  int _nInRootSamples, _nOutRootSamples;
  std::string _modelFileName;
  iRoCS::Features::NormalizationType _sdNormalization, _houghNormalization;
  double _cost, _gamma;
  
};

#endif
