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

#ifndef ATTACHIROCSTOCELLSEGMENTATIONPARAMETERS_HH
#define ATTACHIROCSTOCELLSEGMENTATIONPARAMETERS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>

#include <liblabelling_qt4/AnnotationChannelSpecs.hh>

class AttachIRoCSToCellSegmentationParameters
{

public:
  
  AttachIRoCSToCellSegmentationParameters();
  virtual ~AttachIRoCSToCellSegmentationParameters();

  virtual VisualizationChannelSpecs *segmentationChannel() const;
  virtual void setSegmentationChannel(VisualizationChannelSpecs *channel);

  virtual AnnotationChannelSpecs *qcChannel() const;
  virtual void setQcChannel(AnnotationChannelSpecs *channel);

  virtual std::string iRoCSChannelName() const;
  virtual void setIRoCSChannelName(std::string const &name);

  virtual double dataWeight() const;
  virtual void setDataWeight(double kappa);

  virtual double axisStiffnessWeight() const;
  virtual void setAxisStiffnessWeight(double lambda);

  virtual double thicknessConstancyWeight() const;
  virtual void setThicknessConstancyWeight(double mu);

  virtual int nIterations() const;
  virtual void setNIterations(int nIter);

  virtual double optimizationTimeStep() const;
  virtual void setOptimizationTimeStep(double tau);

  std::string check();

private:
  
  VisualizationChannelSpecs *p_segmentationChannel;
  AnnotationChannelSpecs *p_qcChannel;
  std::string _iRoCSChannelName;
  double _kappa, _lambda, _mu, _tau;
  int _nIter;

};

#endif
