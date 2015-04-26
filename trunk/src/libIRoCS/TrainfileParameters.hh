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

#ifndef TRAINFILEPARAMETERS_HH
#define TRAINFILEPARAMETERS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>

class TrainfileParameters
{

public:

  TrainfileParameters(bool needsIRoCSChannel = false);
  virtual ~TrainfileParameters();
  
  void setNeedsIRoCSChannel(bool needsIRoCSChannel);
  bool needsIRoCSChannel() const;

  virtual void setTrainFileName(std::string const &name);
  virtual std::string trainFileName() const;

  virtual void setDataChannelName(std::string const &name);
  virtual std::string dataChannelName() const;

  virtual void setAnnotationChannelName(std::string const &name);
  virtual std::string annotationChannelName() const;

  virtual void setIRoCSChannelName(std::string const &name);
  virtual std::string iRoCSChannelName() const;

  virtual void setCacheFileName(std::string const &name);
  virtual std::string cacheFileName() const;

  virtual std::string check(int &nPositiveSamples, int &nSamples);

private:
  
  bool _needsIRoCSChannel;
  std::string _trainFileName, _dataChannelName;
  std::string _annotationChannelName, _iRoCSChannelName, _cacheFileName;

};

#endif
