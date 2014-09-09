/**************************************************************************
**       Title: Spherical harmonic surface marker
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef SHSURFACEMARKER_HH
#define SHSURFACEMARKER_HH

#include "SphereMarker.hh"

#include <SH_tools.hh>
#include <atb/SurfaceGeometry.hh>

class SHSurfaceMarkerPresetWidget : public SphereMarkerPresetWidget
{
  
  Q_OBJECT

public:

  SHSurfaceMarkerPresetWidget(
      AnnotationChannelSpecs* channel, QWidget* parent = 0);
  virtual ~SHSurfaceMarkerPresetWidget();
  
};


class SHSurfaceMarkerControlWidget : public SphereMarkerControlWidget
{
  
  Q_OBJECT

public:

  SHSurfaceMarkerControlWidget(
      AnnotationChannelSpecs* channel, QWidget* parent = 0);
  virtual ~SHSurfaceMarkerControlWidget();
  
public slots:
  
  virtual void setValues(const Marker* marker);
  
protected slots:
  
  virtual void toggleReadOnly();

protected:

  StringControlElement* p_coefficientsControl;
 
};


class SHSurfaceMarker : public SphereMarker
{
  
  Q_OBJECT

public:
  
  SHSurfaceMarker(
      blitz::TinyVector<double,3> const& position = 0.0,
      double radius = 5.0,
      segmentation::FrequencyArray const &coefficients =
      segmentation::FrequencyArray(),
      AnnotationChannelSpecs *channel = NULL);
  SHSurfaceMarker(const SHSurfaceMarker& marker);
  virtual ~SHSurfaceMarker();

  SHSurfaceMarker &operator=(const SHSurfaceMarker& marker);

  virtual Marker::MarkerType markerType() const;
  virtual bool inherits(Marker::MarkerType type) const;

  virtual MarkerRenderer *addRenderer(ViewWidget* view);

  int bw() const;
  segmentation::FrequencyArray const &coefficients() const;
  void setCoefficients(segmentation::FrequencyArray const &coefficients);

  std::vector<atb::SurfaceGeometry::VertexT> const &vertices() const;
  std::vector<atb::SurfaceGeometry::NormalT> const &normals() const;
  std::vector<atb::SurfaceGeometry::IndexT> const &indices() const;

  static void save(const AnnotationChannelSpecs* channel,
                   BlitzH5File& outFile, const std::string& group);
  static void load(AnnotationChannelSpecs* channel,
                   const BlitzH5File& inFile, const std::string& group);

  virtual void writeCSVHeader(std::ofstream &out) const;
  virtual void saveCSV(std::ofstream &out) const;
  virtual void loadFromMap(std::map<std::string,std::string> const &values);

  virtual bool occupiesPositionUm(
      blitz::TinyVector<double,3> const &positionUm) const;

private:

  void _updateTriangles() const;
  void _updateBoundingBox() const;

  segmentation::FrequencyArray _coefficients;
  mutable atb::SurfaceGeometry _surface;

};

#endif
