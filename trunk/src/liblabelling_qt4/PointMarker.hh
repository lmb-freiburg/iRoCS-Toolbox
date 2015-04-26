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

#ifndef POINTMARKER_HH
#define POINTMARKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
