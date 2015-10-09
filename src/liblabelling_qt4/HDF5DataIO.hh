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

#ifndef HDF5DATAIO_HH
#define HDF5DATAIO_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtCore/QObject>

#include "ColorMap.hh"
#include "Marker.hh"

#include <libProgressReporter_qt4/ProgressReporterQt4.hh>

class LabellingMainWidget;
class MultiChannelModel;
class ChannelSpecs;
class DataChannelSpecs;
class RGBChannelSpecs;
class VisualizationChannelSpecs;
class AnnotationChannelSpecs;
class SplineCurveChannelSpecs;
class IRoCSChannelSpecs;
class IRoCSSCTChannelSpecs;
class QTreeWidget;

struct ChannelMetaData
{
  ChannelMetaData();
  ChannelMetaData(ChannelSpecs *specs);
  virtual ~ChannelMetaData();

/*======================================================================*/
/*! 
 *   Load the general metadata of channel with index &lt;index&gt; out of the
 *   .labelling_qt4 group of the hdf5 file specified by filename
 *
 *   \param h5FileName  The name of the hdf5 file to read the metadata from
 *   \param index       The channel index
 *
 *   \exception BlitzH5Error when something goes wrong
 */
/*======================================================================*/
  virtual void load(const std::string& h5FileName, int index);

/*======================================================================*/
/*! 
 *   Save the general metadata stored in this struct into the
 *   .labelling_qt4 group of the hdf5 file specified by filename.
 *   If a .labelling_qt4 group exists an entry with corresponding
 *   channelName is searched and updated if found. If not a new
 *   set of attributes is created, and the nChannels attribute is
 *   incremented.
 *
 *   \param h5FileName  The name of the hdf5 file to write the metadata to
 *
 *   \exception BlitzH5Error when something goes wrong
 *
 *   \return The index of the channel within the structure
 */
/*======================================================================*/
  virtual int save(const std::string& h5FileName);

  std::string channelName;
  std::string channelType;
  float alpha;
  bool visible;
};

std::ostream& operator<<(std::ostream& os, const ChannelMetaData& channel);

struct DataChannelMetaData : public ChannelMetaData
{
  DataChannelMetaData();
  DataChannelMetaData(DataChannelSpecs *specs);
  ~DataChannelMetaData();

  void load(const std::string& h5FileName, int index);
  int save(const std::string& h5FileName);

  blitz::TinyVector<double,3> color;
  float displayMin, displayMax;
  float gamma;
  bool showExposureProblems;
};

std::ostream& operator<<(std::ostream& os, const DataChannelMetaData& channel);

struct RGBChannelMetaData : public ChannelMetaData
{
  RGBChannelMetaData();
  RGBChannelMetaData(RGBChannelSpecs *specs);
  ~RGBChannelMetaData();

  void load(const std::string& h5FileName, int index);
  int save(const std::string& h5FileName);

  float gamma;
  bool normalized;
  bool showExposureProblems;
};

std::ostream& operator<<(std::ostream& os, const RGBChannelMetaData& channel);

struct AnnotationChannelMetaData : public ChannelMetaData
{
  AnnotationChannelMetaData();
  AnnotationChannelMetaData(AnnotationChannelSpecs *specs);
  ~AnnotationChannelMetaData();

  void load(const std::string& h5FileName, int index);
  int save(const std::string& h5FileName);

  Marker::MarkerType markerType;
  std::vector<int> activeLabels;
  std::map< int, blitz::TinyVector<float,3> > colorMap;
};

std::ostream& operator<<(
    std::ostream& os, const AnnotationChannelMetaData& channel);

struct VisualizationChannelMetaData : public ChannelMetaData
{
  VisualizationChannelMetaData();
  VisualizationChannelMetaData(VisualizationChannelSpecs *specs);
  ~VisualizationChannelMetaData();

  void load(const std::string& h5FileName, int index);
  int save(const std::string& h5FileName);

  ColorMap colorMap;
};

std::ostream& operator<<(
    std::ostream& os, const VisualizationChannelMetaData& channel);

struct SplineCurveChannelMetaData : public ChannelMetaData
{
  SplineCurveChannelMetaData();
  SplineCurveChannelMetaData(SplineCurveChannelSpecs *specs);
  ~SplineCurveChannelMetaData();

  void load(const std::string& h5FileName, int index);
  int save(const std::string& h5FileName);

  blitz::TinyVector<double,3> color;
};

std::ostream& operator<<(
    std::ostream& os, const SplineCurveChannelMetaData& channel);

struct IRoCSChannelMetaData : public ChannelMetaData
{
  IRoCSChannelMetaData();
  IRoCSChannelMetaData(IRoCSChannelSpecs *specs);
  ~IRoCSChannelMetaData();

  void load(const std::string& h5FileName, int index);
  int save(const std::string& h5FileName);

  blitz::TinyVector<double,3> color;
};

std::ostream& operator<<(
    std::ostream& os, const IRoCSChannelMetaData& channel);

struct IRoCSSCTChannelMetaData : public ChannelMetaData
{
  IRoCSSCTChannelMetaData();
  IRoCSSCTChannelMetaData(IRoCSSCTChannelSpecs *specs);
  ~IRoCSSCTChannelMetaData();

  void load(const std::string& h5FileName, int index);
  int save(const std::string& h5FileName);

  blitz::TinyVector<double,3> color;
};

std::ostream& operator<<(
    std::ostream& os, const IRoCSSCTChannelMetaData& channel);

class HDF5DataIO : public QObject 
{

  Q_OBJECT

public:
  
  enum DataAccessMode { Open, Save, ImportChannels, ExportChannel };

  enum RetVal { Ok, Skip, Abort, ReadError, WriteError };

  HDF5DataIO(LabellingMainWidget *mainWidget, MultiChannelModel *model,
             std::string const &fileName, DataAccessMode mode);
  ~HDF5DataIO();
  
  RetVal run();

private:
  
  // Read channel by meta data
  RetVal readChannel(const DataChannelMetaData& metaData);
  RetVal readChannel(const RGBChannelMetaData& metaData);
  RetVal readChannel(const AnnotationChannelMetaData& metaData);
  RetVal readChannel(const VisualizationChannelMetaData& metaData);
  RetVal readChannel(const SplineCurveChannelMetaData& metaData);
  RetVal readChannel(const IRoCSChannelMetaData& metaData);
  RetVal readChannel(const IRoCSSCTChannelMetaData& metaData);

  template<typename DataT>
  void writeVisualizationChannelAs(
      VisualizationChannelSpecs *channel, std::string const &fileName);
  
  RetVal writeChannel(DataChannelSpecs* specs);
  RetVal writeChannel(RGBChannelSpecs* specs);
  RetVal writeChannel(AnnotationChannelSpecs* specs);
  RetVal writeChannel(VisualizationChannelSpecs* specs);
  RetVal writeChannel(SplineCurveChannelSpecs* specs);
  RetVal writeChannel(IRoCSChannelSpecs* specs);
  RetVal writeChannel(IRoCSSCTChannelSpecs* specs);

  MultiChannelModel* p_model;
  std::string _fileName;
  DataAccessMode _accessMode;

  LabellingMainWidget *p_mainWidget;
  iRoCS::ProgressReporterQt4 *p_progress;
};

#endif
