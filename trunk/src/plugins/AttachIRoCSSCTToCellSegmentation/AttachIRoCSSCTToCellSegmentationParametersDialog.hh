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

#ifndef ATTACHIROCSSCTTOCELLSEGMENTATIONPARAMETERSDIALOG_HH
#define ATTACHIROCSSCTTOCELLSEGMENTATIONPARAMETERSDIALOG_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QDialog>

#include <liblabelling_qt4/AnnotationChannelSpecs.hh>
#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/StringControlElement.hh>
#include <liblabelling_qt4/IntControlElement.hh>
#include <liblabelling_qt4/DoubleControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

class AttachIRoCSSCTToCellSegmentationParametersDialog : public QDialog
{

  Q_OBJECT

  public:
  
  AttachIRoCSSCTToCellSegmentationParametersDialog(
      LabellingMainWidget* mainWidget,
      QWidget* parent = NULL, Qt::WindowFlags f = NULL);
  ~AttachIRoCSSCTToCellSegmentationParametersDialog();

  VisualizationChannelSpecs *segmentationChannel() const;
  void setSegmentationChannel(VisualizationChannelSpecs *channel);

  AnnotationChannelSpecs *qcChannel() const;
  void setQcChannel(AnnotationChannelSpecs *channel);

  std::string iRoCSChannelName() const;
  void setIRoCSChannelName(std::string const &name);

  int backgroundLabel() const;
  void setBackgroundLabel(int backgroundLabel);

  double segmentLengthUm() const;
  void setSegmentLengthUm(double segmentLengthUm);

  double marginUm() const;
  void setMarginUm(double marginUm);

  int downsampleRatio() const;
  void setDownsampleRatio(int downsampleRatio);

  std::string check();

public slots:
  
  void checkAndAccept();

private:
  
  LabellingMainWidget *p_mainWidget;

  ChannelSelectionControlElement *p_segmentationChannelSelector;
  ChannelSelectionControlElement *p_qcChannelSelector;
  StringControlElement *p_iRoCSChannelNameControlElement;

  IntControlElement *p_backgroundLabelControl;
  DoubleControlElement *p_segmentLengthUmControl;
  DoubleControlElement *p_marginUmControl;
  IntControlElement *p_downsampleRatioControl;

};

#endif
