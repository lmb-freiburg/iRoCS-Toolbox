/**************************************************************************
**       Title: Single Point marker
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

#ifndef POINTMARKER_HH
#define POINTMARKER_HH

#include "Marker.hh"
#include "MarkerPresetWidget.hh"
#include "MarkerControlWidget.hh"

class PointMarkerPresetWidget : public MarkerPresetWidget
{

  Q_OBJECT

public:

  PointMarkerPresetWidget(
      AnnotationChannelSpecs* channel, QWidget* parent = 0);
  ~PointMarkerPresetWidget();

};


class PointMarkerControlWidget : public MarkerControlWidget
{
  
  Q_OBJECT

public:

  PointMarkerControlWidget(
      AnnotationChannelSpecs* channel, QWidget* parent = 0);
  ~PointMarkerControlWidget();
  
};


class PointMarker : public Marker
{
  
  Q_OBJECT

public:
  
  PointMarker(const blitz::TinyVector<double,3>& position = 0.0,
              AnnotationChannelSpecs* channel = NULL);
  PointMarker(const PointMarker& marker);
  virtual ~PointMarker();

  PointMarker &operator=(const PointMarker& marker);

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

  virtual void saveCSV(std::ofstream &out) const;
  virtual void loadFromMap(std::map<std::string,std::string> const &values);

  virtual bool occupiesPositionUm(
      blitz::TinyVector<double,3> const &positionUm) const;

protected:
  
  virtual void _updateBoundingBox() const;

};

#endif
