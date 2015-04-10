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

#include "HDF5DataIO.hh"

#include <QtGui/QTreeWidget>
#include <QtGui/QProgressDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>
#include <QtGui/QStatusBar>
#include <QtGui/QInputDialog>

#include "HDF5SelectionDialog.hh"
#include "PointMarker.hh"
#include "SphereMarker.hh"
#include "NucleusMarker.hh"
#include "CylinderMarker.hh"
#include "SHSurfaceMarker.hh"
#include "SurfaceMarker.hh"
#include "CellMarker.hh"

#include "LabellingMainWidget.hh"
#include "MultiChannelModel.hh"

#include "DataChannelSpecs.hh"
#include "RGBChannelSpecs.hh"
#include "VisualizationChannelSpecs.hh"
#include "AnnotationChannelSpecs.hh"
#include "SplineCurveChannelSpecs.hh"
#include "IRoCSChannelSpecs.hh"
#include "IRoCSSCTChannelSpecs.hh"

#include <libBlitzHdf5/BlitzHdf5Light.hh>

/*-----------------------------------------------------------------------
 * Generic ChannelMetaData 
 *-----------------------------------------------------------------------*/

ChannelMetaData::ChannelMetaData()
        : channelName(""), channelType(""), alpha(1.0f), visible(true)
{}

ChannelMetaData::ChannelMetaData(ChannelSpecs *specs)
        : channelName(specs->name()), channelType(""),
          alpha(specs->alpha()), visible(specs->visible())
{}

ChannelMetaData::~ChannelMetaData()
{}

void ChannelMetaData::load(const std::string& h5FileName, int index)
{
  BlitzH5File inFile(h5FileName);
  
  std::stringstream groupStream;
  groupStream << "/.labelling_qt4/channel_" << index;
  
  inFile.readAttribute(channelName, "name", groupStream.str());
  inFile.readAttribute(channelType, "type", groupStream.str());
  inFile.readAttribute(alpha, "alpha", groupStream.str());
  int tmp;
  inFile.readAttribute(tmp, "visible", groupStream.str());
  visible = (tmp == 1);
}

int ChannelMetaData::save(const std::string& h5FileName)
{
  assert(channelName != "" && channelType != "");
  
  int index = 0;
  try
  {
    BlitzH5File outFile(h5FileName, BlitzH5File::WriteOrNew);
    if (outFile.existsGroup("/.labelling_qt4"))
    {
      int nChannels;
      outFile.readAttribute(nChannels, "nChannels", "/.labelling_qt4");
      if (nChannels != 0) 
      {
        std::string channelName;
        while (index < nChannels && channelName != this->channelName)
        {
          std::stringstream groupStream;
          groupStream << "/.labelling_qt4/channel_" << index;
          outFile.readAttribute(channelName, "name", groupStream.str());
          ++index;
        }
        if (index == nChannels && channelName != this->channelName) 
        {
          ++nChannels;
          outFile.writeAttribute(nChannels, "nChannels", "/.labelling_qt4");
        }
        else --index;
      }
    }
    else 
    {
      int nChannels = 1;
      outFile.writeAttribute(nChannels, "nChannels", "/.labelling_qt4");
    }
    std::stringstream groupStream;
    groupStream << "/.labelling_qt4/channel_" << index;
    outFile.writeAttribute(channelName, "name", groupStream.str());
    outFile.writeAttribute(channelType, "type", groupStream.str());
    outFile.writeAttribute(alpha, "alpha", groupStream.str());
    int tmp = (visible) ? 1 : 0;
    outFile.writeAttribute(tmp, "visible", groupStream.str());
  }
  catch (BlitzH5Error& e)
  {
    std::cerr << "Error while writing ChannelMetaData to '" << h5FileName
              << "': " << e.what() << std::endl;
    return -1;
  }
  return index;
}

std::ostream& operator<<(std::ostream& os, const ChannelMetaData& channel)
{
  os << "ChannelMetaData { Name = " << channel.channelName << ", Type = "
     << channel.channelType << ", Alpha = " << channel.alpha
     << ", Visible = " << (channel.visible ? "Yes" : "No") << " }";
  return os; 
}

/*-----------------------------------------------------------------------
 *  DataChannelMetaData
 *-----------------------------------------------------------------------*/

DataChannelMetaData::DataChannelMetaData()
        : ChannelMetaData(), color(blitz::TinyVector<float,3>(1.0, 1.0, 1.0)),
          displayMin(0.0), displayMax(1.0), gamma(1.0f),
          showExposureProblems(false)
{
  channelType = "data";
}

DataChannelMetaData::DataChannelMetaData(DataChannelSpecs *specs)
        : ChannelMetaData(specs), color(specs->color()),
          displayMin(specs->displayMin()), displayMax(specs->displayMax()),
          gamma(specs->gamma()),
          showExposureProblems(specs->showExposureProblems())
{
  channelType = "data";
}

DataChannelMetaData::~DataChannelMetaData()
{}

void DataChannelMetaData::load(const std::string& h5FileName, int index)
{
  ChannelMetaData::load(h5FileName, index);

  BlitzH5File inFile(h5FileName);
  
  std::stringstream groupStream;
  groupStream << "/.labelling_qt4/channel_" << index;
  
  inFile.readAttribute(color, "color", groupStream.str());
  inFile.readAttribute(gamma, "gamma", groupStream.str());
  try
  {
    inFile.readAttribute(displayMin, "displayMin", groupStream.str());
    inFile.readAttribute(displayMax, "displayMax", groupStream.str());
  }
  catch (BlitzH5Error &)
  {
    displayMin = 0.0;
    displayMax = 1.0;
  }
  int tmp;
  inFile.readAttribute(tmp, "showExposureProblems", groupStream.str());
  showExposureProblems = (tmp == 1);
}

int DataChannelMetaData::save(const std::string& h5FileName) 
{
  int index = ChannelMetaData::save(h5FileName);
  if (index == -1) return index;
  try
  {
    BlitzH5File outFile(h5FileName, BlitzH5File::Write);
    
    std::stringstream groupStream;
    groupStream << "/.labelling_qt4/channel_" << index;
    
    outFile.writeAttribute(color, "color", groupStream.str());
    outFile.writeAttribute(gamma, "gamma", groupStream.str());
    outFile.writeAttribute(displayMin, "displayMin", groupStream.str());
    outFile.writeAttribute(displayMax, "displayMax", groupStream.str());
    int tmp = (showExposureProblems) ? 1 : 0;
    outFile.writeAttribute(tmp, "showExposureProblems", groupStream.str());
  }
  catch (BlitzH5Error&)
  {}
  return index;
}

std::ostream& operator<<(std::ostream& os, const DataChannelMetaData& channel)
{
  os << "DataChannelMetaData { Name = " << channel.channelName << ", Type = "
     << channel.channelType << ", Alpha = " << channel.alpha
     << ", Visible = " << (channel.visible ? "Yes" : "No") << ", Gamma = "
     << channel.gamma << ", displayRange = (" << channel.displayMin << " - "
     << channel.displayMax << "), showExposureProblems = "
     << (channel.showExposureProblems ? "Yes" : "No")
     << " }";
  return os;
}

/*-----------------------------------------------------------------------
 *  RGBChannelMetaData
 *-----------------------------------------------------------------------*/

RGBChannelMetaData::RGBChannelMetaData()
        : ChannelMetaData(), gamma(1.0f), normalized(false),
          showExposureProblems(false)
{
  channelType = "rgb";
}

RGBChannelMetaData::RGBChannelMetaData(RGBChannelSpecs *specs)
        : ChannelMetaData(specs), gamma(specs->gamma()),
          normalized(specs->normalize()),
          showExposureProblems(specs->showExposureProblems())
{
  channelType = "rgb";
}

RGBChannelMetaData::~RGBChannelMetaData()
{}

void RGBChannelMetaData::load(const std::string& h5FileName, int index)
{
  ChannelMetaData::load(h5FileName, index);

  BlitzH5File inFile(h5FileName);
  
  std::stringstream groupStream;
  groupStream << "/.labelling_qt4/channel_" << index;
  
  inFile.readAttribute(gamma, "gamma", groupStream.str());
  int tmp;
  inFile.readAttribute(tmp, "normalized", groupStream.str());
  normalized = (tmp == 1);
  inFile.readAttribute(tmp, "showExposureProblems", groupStream.str());
  showExposureProblems = (tmp == 1);
}

int RGBChannelMetaData::save(const std::string& h5FileName) 
{
  int index = ChannelMetaData::save(h5FileName);
  if (index == -1) return index;
  try
  {
    BlitzH5File outFile(h5FileName, BlitzH5File::Write);
    
    std::stringstream groupStream;
    groupStream << "/.labelling_qt4/channel_" << index;
    
    outFile.writeAttribute(gamma, "gamma", groupStream.str());
    int tmp = (normalized) ? 1 : 0;
    outFile.writeAttribute(tmp, "normalized", groupStream.str());
    tmp = (showExposureProblems) ? 1 : 0;
    outFile.writeAttribute(tmp, "showExposureProblems", groupStream.str());
  }
  catch (BlitzH5Error&)
  {}
  return index;
}

std::ostream& operator<<(std::ostream& os, const RGBChannelMetaData& channel)
{
  os << "RGBChannelMetaData { Name = " << channel.channelName << ", Type = "
     << channel.channelType << ", Alpha = " << channel.alpha
     << ", Visible = " << (channel.visible ? "Yes" : "No") << ", Gamma = "
     << channel.gamma << ", Normalized = "
     << (channel.normalized ? "Yes" : "No") << ", showExposureProblems = "
     << (channel.showExposureProblems ? "Yes" : "No")
     << " }";
  return os;
}

/*-----------------------------------------------------------------------
 *  AnnotationChannelMetaData
 *-----------------------------------------------------------------------*/

AnnotationChannelMetaData::AnnotationChannelMetaData()
        : ChannelMetaData(), markerType(Marker::Nucleus),
          activeLabels(std::vector<int>()),
          colorMap(std::map< int, blitz::TinyVector<float,3> >())
{
  channelType = "annotation";
}

AnnotationChannelMetaData::AnnotationChannelMetaData(
    AnnotationChannelSpecs *specs)
        : ChannelMetaData(specs), markerType(specs->markerType()),
          colorMap(specs->colorMap())
{
  channelType = "annotation";
  activeLabels.resize(specs->colorMap().size());
  size_t i = 0;
  for (std::map<int,blitz::TinyVector<float,3> >::const_iterator it =
           specs->colorMap().begin(); it != specs->colorMap().end(); ++it, ++i)
      activeLabels[i] = it->first;
}

AnnotationChannelMetaData::~AnnotationChannelMetaData()
{}

void AnnotationChannelMetaData::load(const std::string& h5FileName, int index)
{
  ChannelMetaData::load(h5FileName, index);
  
  BlitzH5File inFile(h5FileName);
  
  std::stringstream groupStream;
  groupStream << "/.labelling_qt4/channel_" << index;
  
  int type;
  inFile.readAttribute(type, "markerType", groupStream.str());

  switch (type)
  {
  case int(Marker::Point):
    markerType = Marker::Point;
    break;
  case int(Marker::Sphere):
    markerType = Marker::Sphere;
    break;
  case int(Marker::SHSurface):
    markerType = Marker::SHSurface;
    break;
  case int(Marker::Nucleus):
    markerType = Marker::Nucleus;
    break;
  case int(Marker::Cylinder):
    markerType = Marker::Cylinder;
    break;
  case int(Marker::Surface):
    markerType = Marker::Surface;
    break;
  case int(Marker::Cell):
    markerType = Marker::Cell;
    break;
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Unknown marker type"
              << std::endl;
    exit(-1);
  }

  if (inFile.existsAttribute("activeLabels", groupStream.str()))
  {
    inFile.readAttribute(activeLabels, "activeLabels", groupStream.str());
    try 
    {
      blitz::Array<blitz::TinyVector<double,3>,1> colors;
      inFile.readDataset(colors, groupStream.str() + "/colors");
      for (size_t i = 0; i < activeLabels.size(); ++i)
          colorMap[activeLabels[i]] = (i < static_cast<size_t>(colors.size())) ?
              colors(static_cast<ptrdiff_t>(i)) :
              blitz::TinyVector<double,3>(0, 0, 0);
    }
    catch (BlitzH5Error &) // OK, this file has an old color map
    {
      std::cout << "Loading old style marker color map" << std::endl;
      for (std::vector<int>::const_iterator it = activeLabels.begin();
           it != activeLabels.end(); ++it) 
      {
        std::stringstream attNameStream;
        attNameStream << "colorForLabel_" << *it;
        if (inFile.existsAttribute(attNameStream.str(), groupStream.str())) 
            inFile.readAttribute(
                colorMap[*it], attNameStream.str(), groupStream.str());
        else colorMap[*it] = blitz::TinyVector<double,3>(0, 0, 0);
      }
    }
  }
}

int AnnotationChannelMetaData::save(const std::string& h5FileName) 
{
  int index = ChannelMetaData::save(h5FileName);
  if (index == -1) return index;
  try
  {
    BlitzH5File outFile(h5FileName, BlitzH5File::Write);
    
    std::stringstream groupStream;
    groupStream << "/.labelling_qt4/channel_" << index;
    
    outFile.writeAttribute(int(markerType), "markerType", groupStream.str());
    if (activeLabels.size() != 0)
    {
      outFile.writeAttribute(activeLabels, "activeLabels", groupStream.str());
      blitz::Array<blitz::TinyVector<double,3>,1> colors(
          static_cast<ptrdiff_t>(activeLabels.size()));
      for (size_t i = 0; i < activeLabels.size(); ++i) 
          colors(static_cast<ptrdiff_t>(i)) = colorMap[activeLabels[i]];  
      outFile.writeDataset(colors, groupStream.str() + "/colors");
    }
  }
  catch (BlitzH5Error&)
  {}
  return index;
}

std::ostream& operator<<(
    std::ostream& os, const AnnotationChannelMetaData& channel)
{
  os << "AnnotationChannelMetaData { Name = " << channel.channelName
     << ", Type = " << channel.channelType << ", Alpha = " << channel.alpha
     << ", Visible = " << (channel.visible ? "Yes" : "No") << ", MarkerType = "
     << int(channel.markerType) << ", activeLabels = (";
  for (size_t i = 0; i < channel.activeLabels.size(); ++i)
      os << " " << channel.activeLabels[i];
  os << " ), colorMap = (";
  for (std::map< int, blitz::TinyVector<float,3> >::const_iterator it =
           channel.colorMap.begin(); it != channel.colorMap.end();
       ++it)
      os << " " << it->first << " -> " << it->second;
  os << " ) }";
  return os;
}

/*-----------------------------------------------------------------------
 *  VisualizationChannelMetaData
 *-----------------------------------------------------------------------*/ 

VisualizationChannelMetaData::VisualizationChannelMetaData()
        : ChannelMetaData()
{
  channelType = "visualization";
}

VisualizationChannelMetaData::VisualizationChannelMetaData(
    VisualizationChannelSpecs *specs)
        : ChannelMetaData(specs)
{
  colorMap = specs->colorMap();
  channelType = "visualization";
}

VisualizationChannelMetaData::~VisualizationChannelMetaData()
{}

void VisualizationChannelMetaData::load(
    const std::string& h5FileName, int index)
{
  ChannelMetaData::load(h5FileName, index);
  
  std::stringstream groupStream;
  groupStream << "/.labelling_qt4/channel_" << index;

  try
  {
    colorMap.load(h5FileName, groupStream.str() + "/colormap");
  }
  catch (BlitzH5Error & e)
  {
    std::cerr << "Could not load color map for Visualization channel "
              << index << ": " << e.what() << std::endl;
  }
}

int VisualizationChannelMetaData::save(const std::string& h5FileName) 
{
  int index = ChannelMetaData::save(h5FileName);
  if (index == -1) return index;

  std::stringstream groupStream;
  groupStream << "/.labelling_qt4/channel_" << index;

  try
  {
    colorMap.save(h5FileName, groupStream.str() + "/colormap");
  }
  catch (BlitzH5Error &e)
  {
    std::cerr << "Could not save color map for Visualization channel"
              << index << ": " << e.what() << std::endl;
  }
  return index;
}

std::ostream& operator<<(
    std::ostream& os, const VisualizationChannelMetaData& channel)
{
  os << "VisualizationChannelMetaData { Name = " << channel.channelName
     << ", Type = " << channel.channelType << ", Alpha = " << channel.alpha
     << ", Visible = " << (channel.visible ? "Yes" : "No") << " }";
  return os;
}

/*-----------------------------------------------------------------------
 *  SplineCurveChannelMetaData
 *-----------------------------------------------------------------------*/ 

SplineCurveChannelMetaData::SplineCurveChannelMetaData()
        : ChannelMetaData(), color(1.0f, 1.0f, 1.0f)
{
  channelType = "B-Spline";
}

SplineCurveChannelMetaData::SplineCurveChannelMetaData(
    SplineCurveChannelSpecs *specs)
        : ChannelMetaData(specs)
{
  color = blitz::TinyVector<double,3>(specs->color());
  channelType = "B-Spline";
}

SplineCurveChannelMetaData::~SplineCurveChannelMetaData()
{}

void SplineCurveChannelMetaData::load(
    const std::string& h5FileName, int index)
{
  ChannelMetaData::load(h5FileName, index);
  
  std::stringstream groupStream;
  groupStream << "/.labelling_qt4/channel_" << index;

  try
  {
    BlitzH5File inFile(h5FileName);
    inFile.readAttribute(color, "color", groupStream.str());
  }
  catch (BlitzH5Error & e)
  {
    std::cerr << "Could not load color for SplineCurve channel "
              << index << ": " << e.what() << std::endl;
  }
}

int SplineCurveChannelMetaData::save(const std::string& h5FileName) 
{
  int index = ChannelMetaData::save(h5FileName);
  if (index == -1) return index;

  std::stringstream groupStream;
  groupStream << "/.labelling_qt4/channel_" << index;

  try
  {
    BlitzH5File outFile(h5FileName, BlitzH5File::Write);
    outFile.writeAttribute(color, "color", groupStream.str());
  }
  catch (BlitzH5Error &e)
  {
    std::cerr << "Could not save color for SplineCurve channel"
              << index << ": " << e.what() << std::endl;
  }
  return index;
}

std::ostream& operator<<(
    std::ostream& os, const SplineCurveChannelMetaData& channel)
{
  os << "SplineCurveChannelMetaData { Name = " << channel.channelName
     << ", Type = " << channel.channelType << ", Alpha = " << channel.alpha
     << ", Visible = " << (channel.visible ? "Yes" : "No") << ", Color = "
     << channel.color << " }";
  return os;
}

/*-----------------------------------------------------------------------
 *  IRoCSChannelMetaData
 *-----------------------------------------------------------------------*/ 

IRoCSChannelMetaData::IRoCSChannelMetaData()
        : ChannelMetaData(), color(1.0f, 1.0f, 1.0f)
{
  channelType = "iRoCS";
}

IRoCSChannelMetaData::IRoCSChannelMetaData(
    IRoCSChannelSpecs *specs)
        : ChannelMetaData(specs)
{
  color = blitz::TinyVector<double,3>(specs->color());
  channelType = "iRoCS";
}

IRoCSChannelMetaData::~IRoCSChannelMetaData()
{}

void IRoCSChannelMetaData::load(
    const std::string& h5FileName, int index)
{
  ChannelMetaData::load(h5FileName, index);
  
  std::stringstream groupStream;
  groupStream << "/.labelling_qt4/channel_" << index;

  try
  {
    BlitzH5File inFile(h5FileName);
    inFile.readAttribute(color, "color", groupStream.str());
  }
  catch (BlitzH5Error & e)
  {
    std::cerr << "Could not load color for IRoCS channel "
              << index << ": " << e.what() << std::endl;
  }
}

int IRoCSChannelMetaData::save(const std::string& h5FileName) 
{
  int index = ChannelMetaData::save(h5FileName);
  if (index == -1) return index;

  std::stringstream groupStream;
  groupStream << "/.labelling_qt4/channel_" << index;

  try
  {
    BlitzH5File outFile(h5FileName, BlitzH5File::Write);
    outFile.writeAttribute(color, "color", groupStream.str());
  }
  catch (BlitzH5Error &e)
  {
    std::cerr << "Could not save color for IRoCS channel"
              << index << ": " << e.what() << std::endl;
  }
  return index;
}

std::ostream& operator<<(
    std::ostream& os, const IRoCSChannelMetaData& channel)
{
  os << "IRoCSChannelMetaData { Name = " << channel.channelName
     << ", Type = " << channel.channelType << ", Alpha = " << channel.alpha
     << ", Visible = " << (channel.visible ? "Yes" : "No") << ", Color = "
     << channel.color << " }";
  return os;
}

/*-----------------------------------------------------------------------
 *  IRoCSSCTChannelMetaData
 *-----------------------------------------------------------------------*/ 

IRoCSSCTChannelMetaData::IRoCSSCTChannelMetaData()
        : ChannelMetaData(), color(1.0f, 1.0f, 1.0f)
{
  channelType = "iRoCS Shell Coordinate Transform";
}

IRoCSSCTChannelMetaData::IRoCSSCTChannelMetaData(
    IRoCSSCTChannelSpecs *specs)
        : ChannelMetaData(specs)
{
  color = blitz::TinyVector<double,3>(specs->color());
  channelType = "iRoCS Shell Coordinate Transform";
}

IRoCSSCTChannelMetaData::~IRoCSSCTChannelMetaData()
{}

void IRoCSSCTChannelMetaData::load(
    const std::string& h5FileName, int index)
{
  ChannelMetaData::load(h5FileName, index);
  
  std::stringstream groupStream;
  groupStream << "/.labelling_qt4/channel_" << index;

  try
  {
    BlitzH5File inFile(h5FileName);
    inFile.readAttribute(color, "color", groupStream.str());
  }
  catch (BlitzH5Error & e)
  {
    std::cerr << "Could not load color for IRoCSSCT channel "
              << index << ": " << e.what() << std::endl;
  }
}

int IRoCSSCTChannelMetaData::save(const std::string& h5FileName) 
{
  int index = ChannelMetaData::save(h5FileName);
  if (index == -1) return index;

  std::stringstream groupStream;
  groupStream << "/.labelling_qt4/channel_" << index;

  try
  {
    BlitzH5File outFile(h5FileName, BlitzH5File::Write);
    outFile.writeAttribute(color, "color", groupStream.str());
  }
  catch (BlitzH5Error &e)
  {
    std::cerr << "Could not save color for IRoCSSCT channel"
              << index << ": " << e.what() << std::endl;
  }
  return index;
}

std::ostream& operator<<(
    std::ostream& os, const IRoCSSCTChannelMetaData& channel)
{
  os << "IRoCSSCTChannelMetaData { Name = " << channel.channelName
     << ", Type = " << channel.channelType << ", Alpha = " << channel.alpha
     << ", Visible = " << (channel.visible ? "Yes" : "No") << ", Color = "
     << channel.color << " }";
  return os;
}

/*-----------------------------------------------------------------------
 *  Main class
 *-----------------------------------------------------------------------*/

HDF5DataIO::HDF5DataIO(
    LabellingMainWidget *mainWidget, MultiChannelModel *model,
    std::string const &fileName, DataAccessMode mode) 
        : QObject(), p_model(model), _fileName(fileName), _accessMode(mode),
          p_mainWidget(mainWidget), p_progress(NULL)
{}

HDF5DataIO::~HDF5DataIO() 
{}

HDF5DataIO::RetVal HDF5DataIO::run() 
{
  p_progress = p_mainWidget->progressReporter();
  p_progress->reset();
  p_progress->setVisible(true);

  p_mainWidget->setUserInteractionEnabled(false);

  RetVal res = Ok;
  switch (_accessMode) 
  {
  case Open : 
  {
    try
    {
      int nChannels;
      {
        BlitzH5File inFile(_fileName);
        inFile.readAttribute(nChannels, "nChannels", "/.labelling_qt4");
      }
      if (!p_progress->updateProgress(1)) res = Abort;
      for (int i = 0; i < nChannels; ++i)
      {
        p_progress->setTaskProgressMin((100 * i) / nChannels);
        p_progress->setTaskProgressMax((100 * (i + 1)) / nChannels - 1);
        if (p_progress->isAborted()) break;
        ChannelMetaData metaData;
        metaData.load(_fileName, i);
        if (metaData.channelType == "data")
        {
          DataChannelMetaData md;
          md.load(_fileName, i);
          res = readChannel(md);
        }
        else if (metaData.channelType == "rgb")
        {
          RGBChannelMetaData md;
          md.load(_fileName, i);
          res = readChannel(md);
        }
        else if (metaData.channelType == "visualization")
        {
          VisualizationChannelMetaData md;
          md.load(_fileName, i);
          res = readChannel(md);
        }
        else if (metaData.channelType == "annotation")
        {
          AnnotationChannelMetaData md;
          md.load(_fileName, i);
          res = readChannel(md);
        }
        else if (metaData.channelType == "B-Spline")
        {
          SplineCurveChannelMetaData md;
          md.load(_fileName, i);
          res = readChannel(md);
        }
        else if (metaData.channelType == "iRoCS")
        {
          IRoCSChannelMetaData md;
          md.load(_fileName, i);
          res = readChannel(md);
        }
        else if (metaData.channelType == "iRoCS Shell Coordinate Transform")
        {
          IRoCSChannelMetaData md;
          md.load(_fileName, i);
          res = readChannel(md);
        }
        else
        {
          QMessageBox::warning(
              qApp->activeWindow(), tr("Invalid channel"),
              tr("The type of channel ") + metaData.channelName.c_str() +
              tr(" could not be determined. Omitting."));
          p_progress->setVisible(false);
          p_progress->reset();
          p_mainWidget->setUserInteractionEnabled(true);
          return ReadError;
        }
        p_progress->updateProgress(p_progress->taskProgressMax());
      }
    }
    catch (BlitzH5Error&)
    {
      QMessageBox::critical(
          qApp->activeWindow(), tr("Invalid labelling project"),
          tr("The file you selected is no valid labelling project! Use "
             "'Channel -> Import Channels...' to open plain hdf5 datasets and "
             "annotations."));
      p_progress->setVisible(false);
      p_progress->reset();
      p_mainWidget->setUserInteractionEnabled(true);
      return ReadError;
    }
    res = p_progress->isAborted() ? Abort : Ok;
    p_progress->setVisible(false);
    p_progress->reset();
    p_mainWidget->setUserInteractionEnabled(true);
    return res;
  }
  case Save :
  {
    try
    {
      BlitzH5File outFile(_fileName, BlitzH5File::WriteOrNew);
      if (outFile.existsGroup("/.labelling_qt4"))
          outFile.deleteGroup("/.labelling_qt4");
    }
    catch (BlitzH5Error &e)
    {
      QMessageBox::critical(
          qApp->activeWindow(), tr("Save error"),
          tr("Error while saving iRoCS project: %1").arg(e.what()));
      p_progress->setVisible(false);
      p_progress->reset();
      p_mainWidget->setUserInteractionEnabled(true);
      return WriteError;
    }
    int i = 0;
    for (std::vector<ChannelSpecs*>::const_iterator it =
             p_model->begin(); it != p_model->end() && res == Ok; ++it, ++i)
    {
      p_progress->setTaskProgressMin((100 * i) / p_model->nChannels());
      p_progress->setTaskProgressMax(
          (100 * (i + 1)) / p_model->nChannels() - 1);
      if (p_progress->isAborted()) break;
      switch ((*it)->channelType())
      {
      case ChannelSpecs::Data :
      {
        res = writeChannel(reinterpret_cast<DataChannelSpecs*>(*it));
        break;
      }
      case ChannelSpecs::RGB :
      {
        res = writeChannel(reinterpret_cast<RGBChannelSpecs*>(*it));
        break;
      }
      case ChannelSpecs::Visualization :
      {
        res = writeChannel(reinterpret_cast<VisualizationChannelSpecs*>(*it));
        break;
      }
      case ChannelSpecs::Annotation :
      {
        res = writeChannel(reinterpret_cast<AnnotationChannelSpecs*>(*it));
        break;
      }
      case ChannelSpecs::SplineCurve :
      {
        res = writeChannel(reinterpret_cast<SplineCurveChannelSpecs*>(*it));
        break;
      }
      case ChannelSpecs::IRoCS :
      {
        res = writeChannel(reinterpret_cast<IRoCSChannelSpecs*>(*it));
        break;
      }
      case ChannelSpecs::IRoCSSCT :
      {
        res = writeChannel(reinterpret_cast<IRoCSSCTChannelSpecs*>(*it));
        break;
      }
      default:
        std::cerr << "Error in " << __FILE__ << ":" << __LINE__
                  << ": Channel type not handled in HDF5DataIO" << std::endl;
        throw std::runtime_error("Missing Implementation error");
      }
      p_progress->updateProgress(p_progress->taskProgressMax());
    }
    break;
  }
  case ImportChannels :
  {
    std::vector<std::string> selectedItems = 
        HDF5SelectionDialog::selectObjects(
            _fileName.c_str(), p_model, qApp->activeWindow());
    if (selectedItems.size() == 0)
    {
      p_progress->setVisible(false);
      p_progress->reset();
      p_mainWidget->setUserInteractionEnabled(true);
      return Abort;
    }
    
    std::vector<DataChannelMetaData> dataChannels;
    std::vector<RGBChannelMetaData> rgbChannels;
    std::vector<AnnotationChannelMetaData> annotationChannels;
    std::vector<VisualizationChannelMetaData> visualizationChannels;
    std::vector<SplineCurveChannelMetaData> splineCurveChannels;
    std::vector<IRoCSChannelMetaData> iRoCSChannels;
    std::vector<IRoCSSCTChannelMetaData> iRoCSSCTChannels;
    
    std::vector<std::string> remainingItems;
    
    // Check, whether there is corresponding MetaData available
    try
    {
      int nChannels = 0;
      {
        BlitzH5File inFile(_fileName);
        inFile.readAttribute(nChannels, "nChannels", "/.labelling_qt4");
      }
      for (size_t i = 0; i < selectedItems.size(); ++i)
      {
        int j = 0;
        ChannelMetaData metaData;
        for (; j < nChannels; ++j)
        {
          metaData.load(_fileName, j);
          if (metaData.channelName == selectedItems[i]) break;
        }
        if (j == nChannels) // No MetaData found
        {
          remainingItems.push_back(selectedItems[i]);
          continue;
        }
        if (metaData.channelType == "data")
        {
          DataChannelMetaData md;
          md.load(_fileName, j);
          dataChannels.push_back(md);
        }
        else if (metaData.channelType == "rgb")
        {
          RGBChannelMetaData md;
          md.load(_fileName, j);
          rgbChannels.push_back(md);
        }
        else if (metaData.channelType == "visualization")
        {
          VisualizationChannelMetaData md;
          md.load(_fileName, j);
          visualizationChannels.push_back(md);
        }
        else if (metaData.channelType == "annotation")
        {
          AnnotationChannelMetaData md;
          md.load(_fileName, j);
          annotationChannels.push_back(md);
        }
        else if (metaData.channelType == "B-Spline")
        {
          SplineCurveChannelMetaData md;
          md.load(_fileName, j);
          splineCurveChannels.push_back(md);
        }
        else if (metaData.channelType == "iRoCS")
        {
          IRoCSChannelMetaData md;
          md.load(_fileName, j);
          iRoCSChannels.push_back(md);
        }
        else if (metaData.channelType == "iRoCS Shell Coordinate Transform")
        {
          IRoCSSCTChannelMetaData md;
          md.load(_fileName, j);
          iRoCSSCTChannels.push_back(md);
        }
        else remainingItems.push_back(selectedItems[i]);
      }
    }
    catch (BlitzH5Error &)
    {
      // No labelling project .labelling_qt4 structure found
      remainingItems = selectedItems;
    }
      
    for (std::vector<std::string>::const_iterator it = remainingItems.begin();
         it != remainingItems.end(); ++it)
    {
      bool isDataset = false;
      try
      {
        BlitzH5File inFile(_fileName);
        isDataset = inFile.existsDataset(*it);
      }
      catch (BlitzH5Error &)
      {}
      
      if (isDataset)
      {
        try
        {
          BlitzH5File inFile(_fileName);
          std::vector<hsize_t> dsExtents(inFile.getDatasetShape(*it));
          switch (dsExtents.size())
          {
          case 2:
          case 3:
          {
            if (dsExtents.size() == 3 && dsExtents[2] == 3)
            {
              // Check whether it is actually 2D RGB
              try
              {
                std::string dimInterpretation;
                inFile.readAttribute(
                    dimInterpretation, "dim_interpretation", *it);
                if (dimInterpretation[dimInterpretation.size() - 1] == 'c')
                {
                  RGBChannelMetaData md;
                  md.channelName = *it;
                  rgbChannels.push_back(md);                  
                  break;
                }
              }
              catch (BlitzH5Error &)
              {}
            }

            // Ordinary gray value data
            hid_t datasetTypeId = inFile.getDatasetType(*it);
            if (H5Tget_class(datasetTypeId) == H5T_INTEGER)
            {
              VisualizationChannelMetaData md;
              md.channelName = *it;
              visualizationChannels.push_back(md);
            }
            else
            {
              DataChannelMetaData md;
              md.channelName = *it;
              dataChannels.push_back(md);
            }
            H5Tclose(datasetTypeId);
            break;
          }
          case 4:
          {
            if (dsExtents[3] == 3)
            {
              RGBChannelMetaData md;
              md.channelName = *it;
              rgbChannels.push_back(md);
            }
            else throw BlitzH5Error("Incompatible dataset");
            break;
          }
          default:
            throw BlitzH5Error()
                << dsExtents.size() << "-D datasets are not supported.";
          }
        }
        catch (BlitzH5Error &e)
        {
          std::cerr << "Warning: Could not load dataset '" << _fileName << ":"
                    << *it << "': " << e.what() << std::endl;
        }
      }
      else
      {
        try
        {
          atb::BSpline< blitz::TinyVector<double,3> > spline;
          spline.load(_fileName, *it, true);
          SplineCurveChannelMetaData md;
          md.channelName = *it;
          splineCurveChannels.push_back(md);
        }
        catch (BlitzH5Error &)
        {}
        try
        {
          atb::IRoCS rct;
          rct.load(_fileName, *it, true);
          IRoCSChannelMetaData md;
          md.channelName = *it;
          iRoCSChannels.push_back(md);
        }
        catch (BlitzH5Error &)
        {}
        try
        {
          ShellCoordinateTransform sct;
          sct.load(_fileName, *it);
          IRoCSSCTChannelMetaData md;
          md.channelName = *it;
          iRoCSSCTChannels.push_back(md);
        }
        catch (BlitzH5Error &)
        {}
        try
        {
          BlitzH5File inFile(_fileName);
          if (inFile.existsDataset(*it + "/position_um"))
          {
            AnnotationChannelMetaData md;
            md.channelName = *it;
            if (inFile.existsDataset(*it + "/radius_um"))
            {
              if (inFile.existsDataset(*it + "/mitotic"))
                  md.markerType = Marker::Nucleus;
              else if (inFile.existsDataset(*it + "/coefficients"))
                  md.markerType = Marker::SHSurface;
              else if (inFile.existsDataset(*it + "/length_um"))
                  md.markerType = Marker::Cylinder;
              else md.markerType = Marker::Sphere;
            }
            else
            {
              if (inFile.existsDataset(*it + "/mitotic"))
                  md.markerType = Marker::Cell;
              if (inFile.existsDataset(*it + "/nElements"))
                  md.markerType = Marker::Surface;
              else md.markerType = Marker::Point;
            }
            annotationChannels.push_back(md);
          }
        }
        catch (BlitzH5Error &)
        {}
      }
    }
      
    int nChannels = dataChannels.size() + rgbChannels.size() +
        visualizationChannels.size() + annotationChannels.size() +
        splineCurveChannels.size() + iRoCSChannels.size() +
        iRoCSSCTChannels.size();
    int i = 0;
    for (std::vector<DataChannelMetaData>::const_iterator it =
             dataChannels.begin(); it != dataChannels.end() && res != Abort;
         ++it, ++i)
    {
      p_progress->setTaskProgressMin((100 * i) / nChannels);
      p_progress->setTaskProgressMax((100 * (i + 1)) / nChannels - 1);
      if (p_progress->isAborted()) break;
      res = readChannel(*it);
      if (res == ReadError)
      {
        QMessageBox::warning(
            qApp->activeWindow(), tr("Channel invalid"),
            tr("'") + it->channelName.c_str() +
            tr("' is no valid data channel."));
      }
      p_progress->updateProgress(p_progress->taskProgressMax());
    }
    
    for (std::vector<RGBChannelMetaData>::const_iterator it =
             rgbChannels.begin(); it != rgbChannels.end() && res != Abort;
         ++it, ++i)
    {
      p_progress->setTaskProgressMin((100 * i) / nChannels);
      p_progress->setTaskProgressMax((100 * (i + 1)) / nChannels - 1);
      if (p_progress->isAborted()) break;
      res = readChannel(*it);
      if (res == ReadError)
      {
        QMessageBox::warning(
            qApp->activeWindow(), tr("Channel invalid"),
            tr("'") + it->channelName.c_str() +
            tr("' is no valid RGB channel."));
      }
      p_progress->updateProgress(p_progress->taskProgressMax());
    }
    
    for (std::vector<VisualizationChannelMetaData>::const_iterator it =
             visualizationChannels.begin();
         it != visualizationChannels.end() && res != Abort; ++it, ++i)
    {
      p_progress->setTaskProgressMin((100 * i) / nChannels);
      p_progress->setTaskProgressMax((100 * (i + 1)) / nChannels - 1);
      if (p_progress->isAborted()) break;
      res = readChannel(*it);
      if (res == ReadError)
      {
        QMessageBox::warning(
            qApp->activeWindow(), tr("Channel invalid"),
            tr("'") + it->channelName.c_str() +
            tr("' is no valid visualization channel."));
      }
      p_progress->updateProgress(p_progress->taskProgressMax());
    }
    
    if (!p_progress->isAborted() && p_model->nChannels() == 0)
    {
      QMessageBox::critical(
          qApp->activeWindow(),
          tr("No data or visualization channel available"),
          tr("You need to load one data or visualization channel before "
             "loading annotations. Within the selected objects no valid "
             "dataset could be identified, please select at least one 3-D "
             "gray value dataset!"));
      res = Abort;
    }
    
    for (std::vector<AnnotationChannelMetaData>::const_iterator it =
             annotationChannels.begin();
         it != annotationChannels.end() && res != Abort; ++it, ++i)
    {
      p_progress->setTaskProgressMin((100 * i) / nChannels);
      p_progress->setTaskProgressMax((100 * (i + 1)) / nChannels - 1);
      if (p_progress->isAborted()) break;
      res = readChannel(*it);
      if (res == ReadError)
      {
        QMessageBox::warning(
            qApp->activeWindow(), tr("Channel invalid"),
            tr("'") + it->channelName.c_str() +
            tr("' is no valid annotation channel."));
      }
      p_progress->updateProgress(p_progress->taskProgressMax());
    }
    
    for (std::vector<SplineCurveChannelMetaData>::const_iterator it =
             splineCurveChannels.begin();
         it != splineCurveChannels.end() && res != Abort; ++it, ++i)
    {
      p_progress->setTaskProgressMin((100 * i) / nChannels);
      p_progress->setTaskProgressMax((100 * (i + 1)) / nChannels - 1);
      if (p_progress->isAborted()) break;
      res = readChannel(*it);
      if (res == ReadError)
      {
        QMessageBox::warning(
            qApp->activeWindow(), tr("Channel invalid"),
            tr("'") + it->channelName.c_str() +
            tr("' is no valid Spline Curve channel."));
      }
      p_progress->updateProgress(p_progress->taskProgressMax());
    }
    
    for (std::vector<IRoCSChannelMetaData>::const_iterator it =
             iRoCSChannels.begin();
         it != iRoCSChannels.end() && res != Abort; ++it, ++i)
    {
      p_progress->setTaskProgressMin((100 * i) / nChannels);
      p_progress->setTaskProgressMax((100 * (i + 1)) / nChannels - 1);
      if (p_progress->isAborted()) break;
      res = readChannel(*it);
      if (res == ReadError)
      {
        QMessageBox::warning(
            qApp->activeWindow(), tr("Channel invalid"),
            tr("'") + it->channelName.c_str() +
            tr("' is no valid iRoCS axis channel."));
      }
      p_progress->updateProgress(p_progress->taskProgressMax());
    }

    for (std::vector<IRoCSSCTChannelMetaData>::const_iterator it =
             iRoCSSCTChannels.begin();
         it != iRoCSSCTChannels.end() && res != Abort; ++it, ++i)
    {
      p_progress->setTaskProgressMin((100 * i) / nChannels);
      p_progress->setTaskProgressMax((100 * (i + 1)) / nChannels - 1);
      if (p_progress->isAborted()) break;
      res = readChannel(*it);
      if (res == ReadError)
      {
        QMessageBox::warning(
            qApp->activeWindow(), tr("Channel invalid"),
            tr("'") + it->channelName.c_str() +
            tr("' is no valid iRoCS Shell Coordinate Transform channel."));
      }
      p_progress->updateProgress(p_progress->taskProgressMax());
    }
    break;
  }
  case ExportChannel :
    p_progress->setTaskProgressMin(0);
    p_progress->setTaskProgressMax(100);
    switch (p_model->selectedChannel()->channelType())
    {
    case ChannelSpecs::Data :
      res = writeChannel(
          static_cast<DataChannelSpecs*>(p_model->selectedChannel()));
      break;
    case ChannelSpecs::RGB :
      res = writeChannel(
          static_cast<RGBChannelSpecs*>(p_model->selectedChannel()));
      break;
    case ChannelSpecs::Visualization :
      res = writeChannel(
          static_cast<VisualizationChannelSpecs*>(p_model->selectedChannel()));
      break;
    case ChannelSpecs::Annotation :
      res = writeChannel(
          static_cast<AnnotationChannelSpecs*>(p_model->selectedChannel()));
      break;
    case ChannelSpecs::SplineCurve :
      res = writeChannel(
          static_cast<SplineCurveChannelSpecs*>(p_model->selectedChannel()));
      break;
    case ChannelSpecs::IRoCS :
      res = writeChannel(
          static_cast<IRoCSChannelSpecs*>(p_model->selectedChannel()));
      break;
    case ChannelSpecs::IRoCSSCT :
      res = writeChannel(
          static_cast<IRoCSSCTChannelSpecs*>(p_model->selectedChannel()));
      break;
    default:
      std::cerr << "Error in " << __FILE__ << ":" << __LINE__
                << ": Channel type not handled in HDF5DataIO" << std::endl;
      throw std::runtime_error("Missing Implementation error");
    }
    p_progress->updateProgress(p_progress->taskProgressMax());
    break;
  }
  
  if (p_progress->isAborted()) res = Abort;
  p_progress->setVisible(false);
  p_progress->reset();
  p_mainWidget->setUserInteractionEnabled(true);
  return res;
}

HDF5DataIO::RetVal HDF5DataIO::readChannel(const DataChannelMetaData& metaData)
{
#ifdef HDF5DATAIODEBUG
  std::cerr << "HDF5DataIO::readChannel(" << metaData << ")" << std::endl;
#endif
  p_progress->updateProgressMessage(
      ("Loading data channel '" + _fileName + ":" +
       metaData.channelName + "'").c_str());
  atb::Array<float,3>* data = new atb::Array<float,3>();
  try
  {
    BlitzH5File inFile(_fileName);
    std::vector<hsize_t> dsShape(inFile.getDatasetShape(metaData.channelName));
    if (dsShape.size() == 2)
    {
      atb::Array<float,2> tmp;
      tmp.load(inFile, metaData.channelName, p_progress);
      data->resize(1, tmp.extent(0), tmp.extent(1));
      std::memcpy(
          data->dataFirst(), tmp.dataFirst(), tmp.size() * sizeof(float));
      data->setElementSizeUm(
          blitz::TinyVector<double,3>(
              1.0, tmp.elementSizeUm()(0), tmp.elementSizeUm()(1)));
      blitz::TinyMatrix<double,4,4> trafo(
          atb::traits< blitz::TinyMatrix<double,4,4> >::one);
      for (int r = 1; r < 4; ++r)
          for (int c = 1; c < 4; ++c)
              trafo(r, c) = tmp.transformation()(r - 1, c - 1);
      data->setTransformation(trafo);
    }
    else data->load(inFile, metaData.channelName, p_progress);

    if (p_progress->isAborted())
    {
      delete data;
      return Abort;
    }
    
    DataChannelSpecs* specs = p_model->addDataChannel(data);
    if (specs == NULL) throw BlitzH5Error();
    specs->setUpdatesEnabled(false);
    specs->setName(metaData.channelName);
    specs->setAlpha(metaData.alpha);
    specs->setColor(metaData.color);
    specs->setGamma(metaData.gamma);
    specs->setDisplayRange(metaData.displayMin, metaData.displayMax);
    specs->setShowExposureProblems(metaData.showExposureProblems);
    specs->setVisible(metaData.visible);
    specs->setUpdatesEnabled(true);
    specs->update();
  }
  catch (BlitzH5Error&)
  {
    delete data;
    return ReadError;
  }
  return Ok;
}

HDF5DataIO::RetVal HDF5DataIO::readChannel(const RGBChannelMetaData& metaData)
{
  p_progress->updateProgressMessage(
      ("Loading RGB channel '" + _fileName + ":" +
       metaData.channelName + "'").c_str());
  atb::Array<blitz::TinyVector<float,3>,3>* data =
      new atb::Array<blitz::TinyVector<float,3>,3>();
  try
  {
    BlitzH5File inFile(_fileName);
    std::vector<hsize_t> dsShape(inFile.getDatasetShape(metaData.channelName));
    if (dsShape.size() == 3)
    {
      atb::Array<blitz::TinyVector<float,3>,2> tmp;
      tmp.load(inFile, metaData.channelName, p_progress);
      data->resize(1, tmp.extent(0), tmp.extent(1));
      std::memcpy(
          data->dataFirst(), tmp.dataFirst(), 3 * tmp.size() * sizeof(float));
      data->setElementSizeUm(
          blitz::TinyVector<double,3>(
              1.0, tmp.elementSizeUm()(0), tmp.elementSizeUm()(1)));
      blitz::TinyMatrix<double,4,4> trafo(
          atb::traits< blitz::TinyMatrix<double,4,4> >::one);
      for (int r = 1; r < 4; ++r)
          for (int c = 1; c < 4; ++c)
              trafo(r, c) = tmp.transformation()(r - 1, c - 1);
      data->setTransformation(trafo);
    }
    else data->load(inFile, metaData.channelName, p_progress);

    if (p_progress->isAborted())
    {
      delete data;
      return Abort;
    }
    
    RGBChannelSpecs* specs = p_model->addRGBChannel(data);
    if (specs == NULL) throw BlitzH5Error();
    specs->setUpdatesEnabled(false);
    specs->setName(metaData.channelName);
    specs->setAlpha(metaData.alpha);
    specs->setGamma(metaData.gamma);
    specs->setNormalize(metaData.normalized);
    specs->setShowExposureProblems(metaData.showExposureProblems);
    specs->setVisible(metaData.visible);
    specs->setUpdatesEnabled(true);
    specs->update();
  }
  catch (BlitzH5Error& e)
  {
    std::cerr << "Error: Could not read RGB channel: " << e.what()
              << std::endl;
    delete data;
    return ReadError;
  }
  return Ok;
}

HDF5DataIO::RetVal HDF5DataIO::readChannel(
    const VisualizationChannelMetaData& metaData)
{
#ifdef HDF5DATAIODEBUG
  std::cerr << "HDF5DataIO::readChannel(" << metaData << ")" << std::endl;
#endif
  p_progress->updateProgressMessage(
      ("Loading indexed channel '" + _fileName + ":" +
       metaData.channelName + "'").c_str());
  atb::Array<int,3>* data = new atb::Array<int,3>();
  bool sign = true;
  int bitDepth = 32;
  try
  {
    BlitzH5File inFile(_fileName);
    std::vector<hsize_t> dsShape(inFile.getDatasetShape(metaData.channelName));
    if (dsShape.size() == 2)
    {
      atb::Array<int,2> tmp;
      tmp.load(inFile, metaData.channelName, p_progress);
      data->resize(1, tmp.extent(0), tmp.extent(1));
      std::memcpy(data->dataFirst(), tmp.dataFirst(), tmp.size() * sizeof(int));
      data->setElementSizeUm(
          blitz::TinyVector<double,3>(
              1.0, tmp.elementSizeUm()(0), tmp.elementSizeUm()(1)));
      blitz::TinyMatrix<double,4,4> trafo(
          atb::traits< blitz::TinyMatrix<double,4,4> >::one);
      for (int r = 1; r < 4; ++r)
          for (int c = 1; c < 4; ++c)
              trafo(r, c) = tmp.transformation()(r - 1, c - 1);
      data->setTransformation(trafo);
    }
    else data->load(inFile, metaData.channelName, p_progress);

    if (p_progress->isAborted())
    {
      delete data;
      return Abort;
    }
    
    /*---------------------------------------------------------------------
     *  The following block searches for possible translation maps from
     *  segment indices to classes. This is used to apply a classification
     *  using Kun's external RF classification script to the segmentation.
     *---------------------------------------------------------------------*/
    try
    {
      std::vector<std::string> dsNames(inFile.allDatasets());
      QStringList segmentToClassMappings;
      int maxLabel = -1;
      for (std::vector<std::string>::const_iterator it = dsNames.begin();
           it != dsNames.end(); ++it)
      {
        if (p_progress->isAborted()) break;
        std::vector<hsize_t> extents(inFile.getDatasetShape(*it));
        if (extents.size() != 1) continue;
        if (maxLabel == -1) maxLabel = blitz::max(*data);
        if (static_cast<int>(extents[0]) != maxLabel) continue;
        segmentToClassMappings << it->c_str();
      }
      if (segmentToClassMappings.size() != 0)
      {
        segmentToClassMappings.push_front(tr("--- None ---"));
        bool ok;
        QString item = QInputDialog::getItem(
            p_mainWidget, tr("Segment to class mapping"), tr("Mapping:"),
            segmentToClassMappings, 0, false, &ok);
        if (ok && item != tr("--- None ---"))
        {
          blitz::Array<int,1> mapping;
          inFile.readDataset(mapping, item.toStdString());
#ifdef _OPENMP
#pragma omp parallel for
#endif
          for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(data->size()); ++i)
          {
            if (data->data()[i] != 0)
                data->data()[i] = mapping(data->data()[i] - 1);
          }
        }
      }
    }
    catch (BlitzH5Error &)
    {}
    
    hid_t datasetTypeId = inFile.getDatasetType(metaData.channelName);
    sign = (H5Tget_sign(datasetTypeId) == H5T_SGN_2);
    bitDepth = static_cast<int>(H5Tget_size(datasetTypeId)) * 8;
    H5Tclose(datasetTypeId);

    if (p_progress->isAborted())
    {
      delete data;
      return Abort;
    }
    
    VisualizationChannelSpecs* specs = p_model->addVisualizationChannel(data);
    if (specs == NULL) throw BlitzH5Error();
    specs->setUpdatesEnabled(false);
    specs->setName(metaData.channelName);
    specs->setAlpha(metaData.alpha);
    specs->setSign(sign);
    specs->setBitDepth(bitDepth);
    IColorMapEditor *tmp = specs->colorMap().colorMapEditor();
    specs->colorMap() = metaData.colorMap;
    specs->colorMap().setColorMapEditor(tmp);
    specs->setVisible(metaData.visible);
    specs->setUpdatesEnabled(true);
    specs->update();
  }
  catch (BlitzH5Error &e)
  {
    delete data;
    std::cerr << "Error: " << e.what() << std::endl;
    return ReadError;
  }
  return Ok;
}

HDF5DataIO::RetVal HDF5DataIO::readChannel(
    const AnnotationChannelMetaData& metaData)
{
#ifdef HDF5DATAIODEBUG
  std::cerr << "HDF5DataIO::readChannel(" << metaData << ")" << std::endl;
#endif
  p_progress->updateProgressMessage(
      ("Loading annotation channel '" + _fileName + ":" +
       metaData.channelName + "' of type " +
       Marker::markerTypeToString(metaData.markerType).toStdString()).c_str());
  AnnotationChannelSpecs* specs = 0;
  try 
  {
    BlitzH5File inFile(_fileName);
    specs = p_model->addAnnotationChannel(metaData.markerType);
    specs->setUpdatesEnabled(false);
    specs->setName(metaData.channelName);
    specs->load(inFile, p_progress);
    if (p_progress->isAborted())
    {
      p_model->removeChannel(specs);
      return Abort;
    }
    specs->setAlpha(metaData.alpha);
    for (std::vector<int>::const_iterator it = metaData.activeLabels.begin();
         it != metaData.activeLabels.end(); ++it)
        specs->setColorForLabel(*it, metaData.colorMap.find(*it)->second);
    specs->setVisible(metaData.visible);
    specs->setUpdatesEnabled(true);
    specs->update();
  }
  catch (BlitzH5Error& e)
  {
    std::cerr << "Could not read annotation channel '" << _fileName << ":"
              << metaData.channelName << "': " << e.what() << std::endl;
    p_model->removeChannel(specs);
    return ReadError;
  }
  if (p_progress->isAborted())
  {
    p_model->removeChannel(specs);
    return Abort;
  }
  return Ok;
}

HDF5DataIO::RetVal HDF5DataIO::readChannel(
    const SplineCurveChannelMetaData& metaData)
{
#ifdef HDF5DATAIODEBUG
  std::cerr << "HDF5DataIO::readChannel(" << metaData << ")" << std::endl;
#endif
  p_progress->updateProgressMessage(
      ("Loading spline curve channel '" + _fileName + ":" +
       metaData.channelName + "'").c_str());
  SplineCurveChannelSpecs* specs = NULL;
  int pMin = p_progress->taskProgressMin();
  int pMax = p_progress->taskProgressMax();
  try 
  {
    atb::BSpline< blitz::TinyVector<double,3> > *spline =
        new atb::BSpline< blitz::TinyVector<double,3> >();
    spline->load(_fileName, metaData.channelName, true);
    specs = p_model->addSplineCurveChannel(spline);
    if (specs == NULL)
        throw BlitzH5Error("Channel could not be added to model");

    specs->setUpdatesEnabled(false);
    p_progress->updateProgress(static_cast<int>(pMin + 0.7 * (pMax - pMin)));
    specs->setName(metaData.channelName);
    specs->setAlpha(metaData.alpha);
    p_progress->updateProgress(static_cast<int>(pMin + 0.9 * (pMax - pMin)));
    specs->setVisible(metaData.visible);
    specs->setColor(blitz::TinyVector<float,3>(metaData.color));
    specs->setUpdatesEnabled(true);
    specs->update();
  }
  catch (BlitzH5Error& e)
  {
    std::cerr << "Could not read B-Spline channel '" << _fileName << ":"
              << metaData.channelName << "': " << e.what() << std::endl;
    if (specs != NULL) p_model->removeChannel(specs);
    return (p_progress->updateProgress(pMax)) ? ReadError : Abort;
  }
  return (p_progress->updateProgress(pMax)) ? Ok : Abort;
}

HDF5DataIO::RetVal HDF5DataIO::readChannel(
    const IRoCSChannelMetaData& metaData)
{
#ifdef HDF5DATAIODEBUG
  std::cerr << "HDF5DataIO::readChannel(" << metaData << ")" << std::endl;
#endif
  p_progress->updateProgressMessage(
      ("Loading iRoCS channel '" + _fileName + ":" +
       metaData.channelName + "'").c_str());
  IRoCSChannelSpecs* specs = NULL;
  int pMin = p_progress->taskProgressMin();
  int pMax = p_progress->taskProgressMax();
  try 
  {
    atb::IRoCS *rct = new atb::IRoCS(p_progress);
    rct->load(_fileName, metaData.channelName, true);
    specs = p_model->addIRoCSChannel(rct);
    if (specs == NULL)
        throw BlitzH5Error("Channel could not be added to model");

    specs->setUpdatesEnabled(false);
    p_progress->updateProgress(static_cast<int>(pMin + 0.7 * (pMax - pMin)));
    specs->setName(metaData.channelName);
    specs->setAlpha(metaData.alpha);
    p_progress->updateProgress(static_cast<int>(pMin + 0.9 * (pMax - pMin)));
    specs->setVisible(metaData.visible);
    specs->setColor(blitz::TinyVector<float,3>(metaData.color));
    specs->setUpdatesEnabled(true);
    specs->update();
  }
  catch (BlitzH5Error& e)
  {
    std::cerr << "Could not read iRoCS axis channel '" << _fileName << ":"
              << metaData.channelName << "': " << e.what() << std::endl;
    if (specs != NULL) p_model->removeChannel(specs);
    return (p_progress->updateProgress(pMax)) ? ReadError : Abort;
  }
  return (p_progress->updateProgress(pMax)) ? Ok : Abort;
}

HDF5DataIO::RetVal HDF5DataIO::readChannel(
    const IRoCSSCTChannelMetaData& metaData)
{
#ifdef HDF5DATAIODEBUG
  std::cerr << "HDF5DataIO::readChannel(" << metaData << ")" << std::endl;
#endif
  p_progress->updateProgressMessage(
      ("Loading iRoCS Shell Coordinate Transform channel '" + _fileName + ":" +
       metaData.channelName + "'").c_str());
  IRoCSSCTChannelSpecs* specs = NULL;
  int pMin = p_progress->taskProgressMin();
  int pMax = p_progress->taskProgressMax();
  try
  {
    ShellCoordinateTransform *sct = new ShellCoordinateTransform();
    sct->load(_fileName, metaData.channelName);
    specs = p_model->addIRoCSSCTChannel(sct);
    if (specs == NULL)
        throw BlitzH5Error("Channel could not be added to model");

    specs->setUpdatesEnabled(false);
    p_progress->updateProgress(static_cast<int>(pMin + 0.7 * (pMax - pMin)));
    specs->setName(metaData.channelName);
    specs->setAlpha(metaData.alpha);
    p_progress->updateProgress(static_cast<int>(pMin + 0.9 * (pMax - pMin)));
    specs->setVisible(metaData.visible);
    specs->setColor(blitz::TinyVector<float,3>(metaData.color));
    specs->setUpdatesEnabled(true);
    specs->update();
  }
  catch (BlitzH5Error& e)
  {
    std::cerr << "Could not read iRoCS Shell Coordinate Transform channel '"
              << _fileName << ":" << metaData.channelName << "': " << e.what()
              << std::endl;
    if (specs != NULL) p_model->removeChannel(specs);
    return (p_progress->updateProgress(pMax)) ? ReadError : Abort;
  }
  return (p_progress->updateProgress(pMax)) ? Ok : Abort;
}

HDF5DataIO::RetVal HDF5DataIO::writeChannel(DataChannelSpecs* specs) 
{
  p_progress->updateProgressMessage(
      ("Saving data channel '" + _fileName + ":" +
       specs->name() + "'").c_str());

  DataChannelMetaData metaData(specs);

  try 
  {
    bool exists = false;
    {
      BlitzH5File outFile(_fileName, BlitzH5File::WriteOrNew);
      exists = outFile.existsDataset(specs->name());
    }
    if (!exists)
    {
      char const *val = ::getenv("IROCS_DEFAULT_COMPRESSION_FACTOR");
      int compression = (val == NULL) ? 1 : atoi(val);
      specs->data()->save(_fileName, specs->name(), compression, p_progress);
    }
    else
    {
      BlitzH5File outFile(_fileName, BlitzH5File::WriteOrNew);
      if (outFile.existsAttribute("transformation", specs->name()))
      {
        blitz::TinyMatrix<double,4,4> trafo;
        outFile.readAttribute(trafo, "transformation", specs->name());
        if (blitz::any(trafo != specs->transformation()))
        outFile.writeAttribute(
            specs->transformation(), "transformation", specs->name());
      }
      else
          outFile.writeAttribute(
              specs->transformation(), "transformation", specs->name()); 
    }
  }
  catch (BlitzH5Error& e) 
  {
    QMessageBox::critical(
        qApp->activeWindow(), tr("Save error"),
        tr("Error while saving Data channel '") + specs->name().c_str() +
        tr("':\n") + e.str().c_str());
    return WriteError;
  }
  if (p_progress->isAborted()) return Abort;
  metaData.save(_fileName);
  return Ok;
}

HDF5DataIO::RetVal HDF5DataIO::writeChannel(RGBChannelSpecs* specs) 
{
  p_progress->updateProgressMessage(
      ("Saving RGB channel '" + _fileName + ":" + specs->name() + "'").c_str());

  RGBChannelMetaData metaData(specs);

  try 
  {
    bool exists = false;
    {
      BlitzH5File outFile(_fileName, BlitzH5File::WriteOrNew);
      exists = outFile.existsDataset(specs->name());
    }
    if (!exists)
    {
      char const *val = ::getenv("IROCS_DEFAULT_COMPRESSION_FACTOR");
      int compression = (val == NULL) ? 1 : atoi(val);
      specs->data()->save(_fileName, specs->name(), compression, p_progress);
    }
    else
    {
      BlitzH5File outFile(_fileName, BlitzH5File::WriteOrNew);
      if (outFile.existsAttribute("transformation", specs->name()))
      {
        blitz::TinyMatrix<double,4,4> trafo;
        outFile.readAttribute(trafo, "transformation", specs->name());
        if (blitz::any(trafo != specs->transformation()))
        outFile.writeAttribute(
            specs->transformation(), "transformation", specs->name());
      }
      else
          outFile.writeAttribute(
              specs->transformation(), "transformation", specs->name()); 
    }
  }
  catch (BlitzH5Error& e) 
  {
    QMessageBox::critical(
        qApp->activeWindow(), tr("Save error"),
        tr("Error while saving RGB channel '") + specs->name().c_str() +
        tr("':\n") + e.str().c_str());
    return WriteError;
  }
  if (p_progress->isAborted()) return Abort;
  metaData.save(_fileName);
  return Ok;
}

template<typename DataT>
void HDF5DataIO::writeVisualizationChannelAs(
    VisualizationChannelSpecs *channel, std::string const &fileName)
{
  atb::Array<DataT,3> conv(
      channel->data()->shape(), channel->data()->elementSizeUm(),
      channel->data()->transformation());
  conv = blitz::cast<DataT>(*channel->data());
  char const *val = ::getenv("IROCS_DEFAULT_COMPRESSION_FACTOR");
  int compression = (val == NULL) ? 1 : atoi(val);
  conv.save(fileName, channel->name(), compression, p_progress);
}

HDF5DataIO::RetVal HDF5DataIO::writeChannel(VisualizationChannelSpecs* specs) 
{
  p_progress->updateProgressMessage(
      ("Saving indexed channel '" + _fileName + ":" +
       specs->name() + "'").c_str());

  VisualizationChannelMetaData metaData(specs);

  try 
  {
    bool exists = false;
    {
      BlitzH5File outFile(_fileName, BlitzH5File::WriteOrNew);
      exists = outFile.existsDataset(specs->name());
    }
    if (!exists)
    {
      if (specs->sign())
      {
        if (sizeof(int) * 8 == specs->bitDepth())
            specs->data()->save(_fileName, specs->name(), 1, p_progress);
        else
        {
          switch (specs->bitDepth())
          {
          case 8 :
            writeVisualizationChannelAs<char>(specs, _fileName);
            break;
          case 16 :
            writeVisualizationChannelAs<short>(specs, _fileName);
            break;
          case 32 :
            writeVisualizationChannelAs<int>(specs, _fileName);
            break;
          case 64 :
            writeVisualizationChannelAs<long long>(specs, _fileName);
            break;
          default:
            throw BlitzH5Error() << "Data type not supported for writing";
          }
        }
      }
      else
      {
        switch (specs->bitDepth())
        {
        case 8 :
          writeVisualizationChannelAs<unsigned char>(specs, _fileName);
          break;
        case 16 :
          writeVisualizationChannelAs<unsigned short>(specs, _fileName);
          break;
        case 32 :
          writeVisualizationChannelAs<unsigned int>(specs, _fileName);
          break;
        case 64 :
          writeVisualizationChannelAs<unsigned long long>(specs, _fileName);
          break;
        default:
          throw BlitzH5Error() << "Data type not supported for writing";
        }
      }
    }
    else
    {
      BlitzH5File outFile(_fileName, BlitzH5File::WriteOrNew);
      if (outFile.existsAttribute("transformation", specs->name()))
      {
        blitz::TinyMatrix<double,4,4> trafo;
        outFile.readAttribute(trafo, "transformation", specs->name());
        if (blitz::any(trafo != specs->transformation()))
            outFile.writeAttribute(
                specs->transformation(), "transformation", specs->name());
      }
      else
          outFile.writeAttribute(
              specs->transformation(), "transformation", specs->name()); 
    }
  }
  catch (BlitzH5Error& e) 
  {
    QMessageBox::critical(
        qApp->activeWindow(), tr("Save error"),
        tr("Error while saving Data channel '") + specs->name().c_str() +
        tr("':\n") + e.str().c_str());
    return WriteError;
  }
  if (p_progress->isAborted()) return Abort;
  metaData.save(_fileName);
  return Ok;
}

HDF5DataIO::RetVal HDF5DataIO::writeChannel(AnnotationChannelSpecs* specs) 
{
  p_progress->updateProgressMessage(
      ("Saving annotation channel '" + _fileName + ":" +
       specs->name() + "'").c_str());

  AnnotationChannelMetaData metaData(specs);

  int pMax = p_progress->taskProgressMax();
  try 
  {
    BlitzH5File outFile(_fileName, BlitzH5File::WriteOrNew);
    metaData.markerType = specs->markerType();
    specs->save(outFile);
  }
  catch (BlitzH5Error& e) 
  {
    QMessageBox::warning(
        qApp->activeWindow(), tr("Save error"),
        tr("Error while saving Annotation channel '") +
        metaData.channelName.c_str() + tr("':\n") + e.str().c_str());    
    return (p_progress->updateProgress(pMax)) ? WriteError : Abort;
  }
  metaData.save(_fileName);
  return (p_progress->updateProgress(pMax)) ? Ok : Abort;
}

HDF5DataIO::RetVal HDF5DataIO::writeChannel(SplineCurveChannelSpecs* specs) 
{
  p_progress->updateProgressMessage(
      ("Saving spline curve channel '" + _fileName + ":" +
       specs->name() + "'").c_str());
  SplineCurveChannelMetaData metaData(specs);

  int pMax = p_progress->taskProgressMax();
  try 
  {
    specs->save(_fileName, specs->name(), true);
  }
  catch (BlitzH5Error& e) 
  {
    QMessageBox::critical(
        qApp->activeWindow(), tr("Save error"),
        tr("Error while saving B-Spline channel '") + specs->name().c_str() +
        tr("':\n") + e.str().c_str());
    return (p_progress->updateProgress(pMax)) ? WriteError : Abort;
  }
  metaData.save(_fileName);
  return (p_progress->updateProgress(pMax)) ? Ok : Abort;
}

HDF5DataIO::RetVal HDF5DataIO::writeChannel(IRoCSChannelSpecs* specs) 
{
  p_progress->updateProgressMessage(
      ("Saving iRoCS channel '" + _fileName + ":" +
       specs->name() + "'").c_str());
  IRoCSChannelMetaData metaData(specs);

  int pMax = p_progress->taskProgressMax();
  try 
  {
    specs->rct()->save(_fileName, specs->name(), true);
  }
  catch (BlitzH5Error& e) 
  {
    QMessageBox::critical(
        qApp->activeWindow(), tr("Save error"),
        tr("Error while saving iRoCS axis channel '") + specs->name().c_str() +
        tr("':\n") + e.str().c_str());
    return (p_progress->updateProgress(pMax)) ? WriteError : Abort;
  }
  metaData.save(_fileName);
  return (p_progress->updateProgress(pMax)) ? Ok : Abort;
}

HDF5DataIO::RetVal HDF5DataIO::writeChannel(IRoCSSCTChannelSpecs* specs) 
{
  p_progress->updateProgressMessage(
      ("Saving iRoCS Shell Coordinate Transform channel '" + _fileName + ":" +
       specs->name() + "'").c_str());
  IRoCSSCTChannelMetaData metaData(specs);

  int pMax = p_progress->taskProgressMax();
  try 
  {
    specs->sct()->save(_fileName, specs->name());
  }
  catch (BlitzH5Error& e) 
  {
    QMessageBox::critical(
        qApp->activeWindow(), tr("Save error"),
        tr("Error while saving iRoCS Shell Coordinate Transform channel "
           "'%1':\n%2").arg(specs->name().c_str()).arg(e.what()));
    return (p_progress->updateProgress(pMax)) ? WriteError : Abort;
  }
  metaData.save(_fileName);
  return (p_progress->updateProgress(pMax)) ? Ok : Abort;
}
