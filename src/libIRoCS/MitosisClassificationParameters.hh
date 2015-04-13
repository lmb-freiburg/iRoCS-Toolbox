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

#ifndef MITOSISCLASSIFICATIONPARAMETERS_HH
#define MITOSISCLASSIFICATIONPARAMETERS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>

#include <liblabelling_qt4/DataChannelSpecs.hh>
#include <liblabelling_qt4/AnnotationChannelSpecs.hh>

class MitosisClassificationParameters
{

public:
  
  MitosisClassificationParameters();
  virtual ~MitosisClassificationParameters();

  virtual DataChannelSpecs *dataChannel() const;
  virtual void setDataChannel(DataChannelSpecs *channel);

  virtual AnnotationChannelSpecs *annotationChannel() const;
  virtual void setAnnotationChannel(AnnotationChannelSpecs *channel);

  virtual std::string cacheFileName() const;
  virtual void setCacheFileName(std::string const &cacheFileName);

  virtual std::string modelFileName() const;
  virtual void setModelFileName(std::string const &modelFileName);

  std::string check();

private:
  
  DataChannelSpecs *p_dataChannel;
  AnnotationChannelSpecs *p_annotationChannel;
  std::string _cacheFileName;
  std::string _modelFileName;

};

#endif
