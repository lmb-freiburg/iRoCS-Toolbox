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

#ifndef SURFACEMARKER_HH
#define SURFACEMARKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
