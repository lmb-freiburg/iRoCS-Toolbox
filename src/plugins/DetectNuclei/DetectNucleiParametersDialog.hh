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

#ifndef DETECTNUCLEIPARAMETERSDIALOG_HH
#define DETECTNUCLEIPARAMETERSDIALOG_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QDialog>

#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/FileNameSelectionControlElement.hh>
#include <liblabelling_qt4/StringControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <string>

class DetectNucleiParametersDialog : public QDialog
{

  Q_OBJECT

  public:
  
  DetectNucleiParametersDialog(
      LabellingMainWidget* mainWidget,
      QWidget* parent = NULL, Qt::WindowFlags f = NULL);
  ~DetectNucleiParametersDialog();

  ChannelSpecs* dataChannel() const;
  void setDataChannel(ChannelSpecs *channel);

  std::string annotationChannelName() const;
  void setAnnotationChannelName(std::string const &name);

  std::string cacheFileName() const;
  void setCacheFileName(std::string const &cacheFileName);

  std::string modelFileName() const;
  void setModelFileName(std::string const &modelFileName);

  ptrdiff_t memoryLimit() const;
  void setMemoryLimit(ptrdiff_t memoryLimit);

  std::string check();

public slots:
  
  void checkAndAccept();

private:
  
  LabellingMainWidget* p_mainWidget;

  ChannelSelectionControlElement* p_dataChannelSelector;
  StringControlElement* p_annotationChannelControlElement;
  FileNameSelectionControlElement* p_cacheFileControlElement;
  FileNameSelectionControlElement* p_modelFileNameControlElement;

  ptrdiff_t _memoryLimit;

};

#endif
