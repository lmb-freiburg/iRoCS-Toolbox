/**************************************************************************
**       Title: Spheric marker
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2008/01/18 19:53:53  tschmidt
**  - Corrected some bugs
**  - caching of marker visuals speeds up redraw cycle
**
**  Revision 1.1  2008/01/17 17:05:50  tschmidt
**  - Initial revision
**
**
**************************************************************************/

#ifndef SPHEREMARKER_HH
#define SPHEREMARKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "PointMarker.hh"

class OrthoViewWidget;
class AnnotationChannelSpecs;
class DoubleControlElement;

class SphereMarkerPresetWidget : public PointMarkerPresetWidget 
{

  Q_OBJECT

public:

  SphereMarkerPresetWidget(
      AnnotationChannelSpecs* channel, QWidget* parent = 0);
  virtual ~SphereMarkerPresetWidget();
  
  virtual void getValues(Marker* marker) const;
  virtual void setValues(const Marker* marker);
  
protected:
  
  DoubleControlElement* p_radiusControl;

};


class SphereMarkerControlWidget : public PointMarkerControlWidget
{

  Q_OBJECT

public:

  SphereMarkerControlWidget(
      AnnotationChannelSpecs* channel, QWidget* parent = 0);
  virtual ~SphereMarkerControlWidget();
  
  virtual void getValues(Marker* marker) const;

public slots:

  virtual void setValues(const Marker* marker);
  
protected:
  
  DoubleControlElement* p_radiusControl;

};


class SphereMarker : public PointMarker
{
  
  Q_OBJECT

public:
  
  SphereMarker(const blitz::TinyVector<double,3>& position = 0.0,
               const double radius = 5.0,
               AnnotationChannelSpecs* channel = NULL);
  SphereMarker(const SphereMarker& marker);
  virtual ~SphereMarker();

  SphereMarker &operator=(const SphereMarker& marker);

  double radiusUm() const;
  void setRadiusUm(const double radius);
  
  virtual Marker::MarkerType markerType() const;
  virtual bool inherits(Marker::MarkerType type) const;
  
  virtual MarkerRenderer *addRenderer(ViewWidget* view);

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

  virtual bool occupiesPositionUm(
      blitz::TinyVector<double,3> const &positionUm) const;

  virtual void copyToATBNucleus(atb::Nucleus &nc) const;
  virtual void copyFromATBNucleus(atb::Nucleus const &nc);

protected:
  
  virtual void _updateBoundingBox() const;

  double _radiusUm;

};

#endif
