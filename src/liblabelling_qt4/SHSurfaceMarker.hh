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

#ifndef SHSURFACEMARKER_HH
#define SHSURFACEMARKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "SphereMarker.hh"

#include <libsegmentation/SH_tools.hh>
#include <libArrayToolbox/SurfaceGeometry.hh>

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

  atb::SurfaceGeometry const &geometry() const;
  std::vector<atb::SurfaceGeometry::VertexT> const &vertices() const;
  std::vector<atb::SurfaceGeometry::NormalT> const &normals() const;
  std::vector<atb::SurfaceGeometry::IndexT> const &indices() const;

  static void save(
      AnnotationChannelSpecs const *channel,
      BlitzH5File &outFile, std::string const &group,
      iRoCS::ProgressReporter *pr = NULL);
  static void load(
      AnnotationChannelSpecs *channel, BlitzH5File const &inFile,
      std::string const &group, iRoCS::ProgressReporter *pr = NULL);

  virtual void writeCSVHeader(std::ofstream &out) const;
  virtual void saveCSV(std::ofstream &out) const;
  virtual void loadFromMap(std::map<std::string,std::string> const &values);

  virtual bool occupiesPositionUm(
      blitz::TinyVector<double,3> const &positionUm) const;

  virtual void copyToATBNucleus(atb::Nucleus &nc) const;
  virtual void copyFromATBNucleus(atb::Nucleus const &nc);

private:

  void _updateTriangles() const;
  void _updateBoundingBox() const;

  segmentation::FrequencyArray _coefficients;
  mutable atb::SurfaceGeometry _surface;

};

#endif
