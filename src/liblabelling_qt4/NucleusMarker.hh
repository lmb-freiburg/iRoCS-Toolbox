/**************************************************************************
 *
 * This file belongs to the iRoCS Toolbox.
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

#ifndef NUCLEUSMARKER_HH
#define NUCLEUSMARKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "SHSurfaceMarker.hh"

class BoolControlElement;

class NucleusMarkerPresetWidget : public SHSurfaceMarkerPresetWidget
{
  
  Q_OBJECT

public:

  NucleusMarkerPresetWidget(
      AnnotationChannelSpecs* channel, QWidget* parent = 0);
  virtual ~NucleusMarkerPresetWidget();
  
  virtual void getValues(Marker* marker) const;
  virtual void setValues(const Marker* marker);
  
protected slots:
  
  void updateLabelSuffix();
  void updateSubtypeSuffix();

protected:
  
  IntControlElement* p_subtypeControl;
  BoolControlElement* p_mitoticControl;
  IntControlElement* p_continuityControl;
  
};


class NucleusMarkerControlWidget : public SHSurfaceMarkerControlWidget
{
  
  Q_OBJECT

public:

  NucleusMarkerControlWidget(
      AnnotationChannelSpecs* channel, QWidget* parent = 0);
  virtual ~NucleusMarkerControlWidget();
  
  virtual void getValues(Marker* marker) const;
  
public slots:
  
  virtual void setValues(const Marker* marker);
  
protected slots:
  
  virtual void toggleReadOnly();
  void updateLabelSuffix();
  void updatePredictedLabelSuffix();
  void updateSubtypeSuffix();
  void updatePredictedSubtypeSuffix();

protected:

  IntControlElement* p_subtypeControl;
  IntControlElement* p_predictedSubtypeControl;
  DoubleControlElement* p_valueControl;
  DoubleControlElement* p_confidenceControl;
  DoubleControlElement* p_sPhaseControl;
  BoolControlElement* p_mitoticControl;
  BoolControlElement* p_predictedMitoticControl;
  IntControlElement* p_continuityControl;
  IntControlElement* p_predictedContinuityControl;
  DoubleControlElement* p_borderDistanceUmControl;
  DoubleControlElement* p_volumeUm3Control;
 
};


class NucleusMarker : public SHSurfaceMarker
{
  
  Q_OBJECT

public:
  
  NucleusMarker(const blitz::TinyVector<double,3>& position = 0.0,
                double radius = 5.0,
                segmentation::FrequencyArray const &coefficients =
                segmentation::FrequencyArray(),
                AnnotationChannelSpecs* channel = NULL);
  NucleusMarker(const NucleusMarker& marker);
  virtual ~NucleusMarker();

  NucleusMarker &operator=(const NucleusMarker& marker);

  int subtype() const;
  void setSubtype(const int subtype);
  
  int predictedSubtype() const;
  void setPredictedSubtype(const int subtype);
  
  double value() const;
  void setValue(const double value);
  
  double confidence() const;
  void setConfidence(const double confidence);
  
  double sPhase() const;
  void setSPhase(const double sPhase);
  
  bool mitotic() const;
  void setMitotic(const bool mitotic);
  
  bool predictedMitotic() const;
  void setPredictedMitotic(const bool mitotic);
  
  int continuity() const;
  void setContinuity(const int continuity);
  
  int predictedContinuity() const;
  void setPredictedContinuity(const int continuity);
  
  double borderDistanceUm() const;
  void setBorderDistanceUm(const double borderDistanceUm);
  
  double volumeUm3() const;
  void setVolumeUm3(const double volumeUm3);

  bool operator<(const NucleusMarker& rhs) const;

  static void save(
      AnnotationChannelSpecs const *channel,
      BlitzH5File &outFile, std::string const &group,
      iRoCS::ProgressReporter *pr = NULL);
  static void load(
      AnnotationChannelSpecs *channel,
      BlitzH5File const &inFile, std::string const &group,
      iRoCS::ProgressReporter *pr = NULL);

  virtual void writeCSVHeader(std::ofstream &out) const;
  virtual void saveCSV(std::ofstream &out) const;
  virtual void loadFromMap(std::map<std::string,std::string> const &values);

  virtual Marker::MarkerType markerType() const;
  virtual bool inherits(Marker::MarkerType type) const;

  virtual MarkerRenderer *addRenderer(ViewWidget* view);

  virtual void copyToATBNucleus(atb::Nucleus &nc) const;
  virtual void copyFromATBNucleus(atb::Nucleus const &nc);

private:

  double _value, _confidence, _sphase, _borderDistance, _volume;
  bool _mitotic, _predictedMitotic;
  int _subtype, _predictedSubtype, _continuity, _predictedContinuity;

};

#endif
