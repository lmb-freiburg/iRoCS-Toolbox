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

#ifndef ANNOTATIONCHANNELSPECS_HH
#define ANNOTATIONCHANNELSPECS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelSpecs.hh"

#include "HDF5DataIO.hh"
#include "Marker.hh"

class QComboBox;
class QStackedWidget;
class QPushButton;

class AnnotationChannelSpecs : public ChannelSpecs
{

  Q_OBJECT

public:
  
  enum Operation {
      AddMarker = 0x0000, LabelMarker = 0x0001, EditMarker = 0x0002,
      DeleteMarker = 0x0003, UnknownOperation = 0xffff };

  typedef std::vector<Marker*>::const_iterator const_iterator;
  typedef std::vector<Marker*>::iterator iterator;

  AnnotationChannelSpecs(
      Marker::MarkerType markerType, MultiChannelModel *model = NULL);
  ~AnnotationChannelSpecs();
  
  ChannelSpecs::ChannelType channelType() const;

  float valueAt(blitz::TinyVector<double,3> const &positionUm) const;
  std::string stringValueAt(
      blitz::TinyVector<double,3> const &positionUm) const;

  int selectedViewType() const;
  Operation selectedOperation() const;

  MarkerPresetWidget *markerPresetWidget();
  MarkerControlWidget *markerControlWidget();

  Marker *closestMarker(
      blitz::TinyVector<double,3> const &positionUm, bool intersecting = false);
  Marker *addMarker(blitz::TinyVector<double,3> const &positionUm);

  void addMarker(Marker *marker);
  void labelMarker(Marker *marker);
  void removeMarker(Marker *marker);

  void selectMarker(Marker *marker);
  Marker const *selectedMarker() const;
  Marker *selectedMarker();

  Marker::MarkerType markerType() const;

  blitz::TinyVector<float,3> colorForLabel(int label) const;
  void setColorForLabel(int label, blitz::TinyVector<float,3> const &color);
  std::map< int,blitz::TinyVector<float,3> > &colorMap();

  float alphaForLabel(int label) const;
  void setAlphaForLabel(int label, float alpha);
  std::map< int,float > &alphaMap();

  void resizeFeatures(size_t newSize);

  std::vector<Marker*> const &markers() const;
  const_iterator begin() const;
  iterator begin();
  const_iterator end() const;
  iterator end();
  Marker const *operator[](size_t index) const;
  Marker *operator[](size_t index);

  ChannelSpecsRenderer *addRenderer(ViewWidget *view);

  blitz::TinyVector<double,3> const &elementSizeUm() const;

  HDF5DataIO::RetVal load(
      std::string const &fileName, iRoCS::ProgressReporter *pr = NULL);
  void load(BlitzH5File const &inFile, iRoCS::ProgressReporter *pr = NULL);
  HDF5DataIO::RetVal save(
      std::string const &fileName, iRoCS::ProgressReporter *pr = NULL) const;
  void save(BlitzH5File &outFile, iRoCS::ProgressReporter *pr = NULL) const;

  void saveCSV(std::string const &fileName) const;
  static AnnotationChannelSpecs *loadCSV(std::string const &fileName);

signals:
        
  void featureSizeChanged(int size);

public slots:
        
  void setCoordinateUpdateButtonEnabled(bool enable);

private slots:
  
  void updateBoundingBox();
  void updateSelectedMarker();
  void updateCoordinates();

private:
  
  Marker::MarkerType _markerType;
  Marker *_selectedMarker;
  std::vector<Marker*> _markers;

  MarkerPresetWidget *p_presetWidget;
  MarkerControlWidget *p_controlWidget;

  QComboBox *p_viewTypeComboBox;
  QComboBox *p_operationComboBox;
  QPushButton *p_coordinateUpdateButton;

  std::map< int,blitz::TinyVector<float,3> > _colorMap;
  std::map< int,float > _alphaMap;

};
    
#endif
