/**************************************************************************
**       Title: Surface marker for arbitrary surfaces
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef SURFACEMARKER_HH
#define SURFACEMARKER_HH

#include "PointMarker.hh"

#include <libArrayToolbox/SurfaceGeometry.hh>

class SurfaceMarkerPresetWidget : public PointMarkerPresetWidget
{
  
  Q_OBJECT

public:

  SurfaceMarkerPresetWidget(
      AnnotationChannelSpecs* channel, QWidget* parent = 0);
  virtual ~SurfaceMarkerPresetWidget();
  
};


class SurfaceMarkerControlWidget : public PointMarkerControlWidget
{
  
  Q_OBJECT

public:

  SurfaceMarkerControlWidget(
      AnnotationChannelSpecs* channel, QWidget* parent = 0);
  virtual ~SurfaceMarkerControlWidget();
  
};


class SurfaceMarker : public PointMarker
{
  
  Q_OBJECT

public:
  
  SurfaceMarker(
      blitz::TinyVector<double,3> const& position = 0.0,
      atb::SurfaceGeometry const * const surface = NULL,
      AnnotationChannelSpecs *channel = NULL);
  SurfaceMarker(const SurfaceMarker& marker);
  virtual ~SurfaceMarker();

  SurfaceMarker &operator=(const SurfaceMarker& marker);

  virtual Marker::MarkerType markerType() const;
  virtual bool inherits(Marker::MarkerType type) const;

  virtual MarkerRenderer *addRenderer(ViewWidget* view);

  atb::SurfaceGeometry const &surface() const;
  atb::SurfaceGeometry &surface();  
  std::vector<atb::SurfaceGeometry::VertexT> const &vertices() const;
  std::vector<atb::SurfaceGeometry::VertexT> &vertices();
  std::vector<atb::SurfaceGeometry::NormalT> const &normals() const;
  std::vector<atb::SurfaceGeometry::NormalT> &normals();
  std::vector<atb::SurfaceGeometry::IndexT> const &indices() const;
  std::vector<atb::SurfaceGeometry::IndexT> &indices();
  
  void setVertices(std::vector<atb::SurfaceGeometry::VertexT> const &vertices);
  void setNormals(std::vector<atb::SurfaceGeometry::NormalT> const & normals);
  void setIndices(std::vector<atb::SurfaceGeometry::IndexT> const &indices);

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

private:

  virtual void _updateBoundingBox() const;

  atb::SurfaceGeometry _surface;

};

#endif
