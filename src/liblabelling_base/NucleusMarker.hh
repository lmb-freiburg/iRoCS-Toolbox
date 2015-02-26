/**************************************************************************
**       Title: Nucleus marker
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
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

  static void save(const AnnotationChannelSpecs* channel,
                   BlitzH5File& outFile, const std::string& group);
  static void load(AnnotationChannelSpecs* channel,
                   const BlitzH5File& inFile, const std::string& group);

  virtual void writeCSVHeader(std::ofstream &out) const;
  virtual void saveCSV(std::ofstream &out) const;
  virtual void loadFromMap(std::map<std::string,std::string> const &values);

  virtual Marker::MarkerType markerType() const;
  virtual bool inherits(Marker::MarkerType type) const;

  virtual MarkerRenderer *addRenderer(ViewWidget* view);

private:

  double _value, _confidence, _sphase, _borderDistance, _volume;
  bool _mitotic, _predictedMitotic;
  int _subtype, _predictedSubtype, _continuity, _predictedContinuity;

};

#endif
