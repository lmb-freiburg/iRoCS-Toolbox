/**************************************************************************
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

#include "FloatChannel.hh"
#include "ChannelObserver.hh"

#include <map>

namespace iRoCS {

  FloatChannel::FloatChannel()
    : Channel(), _data(), _colormap() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatChannel@" << this << "::FloatChannel _data=\""
      << &_data << "\" _colormap=\"" << &_colormap << "\" />"
      << std::endl;
#endif
  }

  FloatChannel::~FloatChannel() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatChannel@" << this << "::~FloatChannel _data=\""
      << &_data << "\" _colormap=\"" << &_colormap << "\" />"
      << std::endl;
#endif
  }

  Channel::Type FloatChannel::type() const {
    return Float;
  }

  FloatColormap const &FloatChannel::colormap() const {
    return _colormap;
  }

  FloatColormap &FloatChannel::colormap() {
    return _colormap;
  }

  void FloatChannel::setElementSizeUm(
    blitz::TinyVector<double, 3> const &elementSizeUm) {
    if (blitz::all(elementSizeUm == _data.elementSizeUm()) &&
        blitz::all(elementSizeUm == this->elementSizeUm())) return;
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatChannel@" << this
      << "::setElementSizeUm elementSizeUm=\"" << elementSizeUm(0)
      << "," << elementSizeUm(1) << "," << elementSizeUm(2)
      << "\" _data=\"" << &_data << "\" _data.elementSizeUm=\""
      << _data.elementSizeUm()(0) << "," << _data.elementSizeUm()(1)
      << "," << _data.elementSizeUm()(2) << "\" _colormap=\""
      << &_colormap << "\">" << std::endl;
#endif
    if (blitz::any(elementSizeUm != _data.elementSizeUm()))
        _data.setElementSizeUm(elementSizeUm);
    Channel::setElementSizeUm(elementSizeUm);
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</FloatChannel@" << this << "::setElementSizeUm>"
      << std::endl;
#endif
  }

  void FloatChannel::setTransformation(
    blitz::TinyMatrix<double, 4, 4> const &transformation) {
    if (blitz::all(transformation == _data.transformation()) &&
        blitz::all(transformation == this->transformation())) return;
#ifdef DEBUG_VERBOSE_XML
    blitz::TinyMatrix<double,4,4> const &tP = transformation;
    blitz::TinyMatrix<double,4,4> const &tD = _data.transformation();
    std::cerr << "<FloatChannel@" << this
      << "::setTransformation transformation=("
      << tP(0,0) << "," << tP(0,1) << "," << tP(0,2) << "," << tP(0,3) << ";"
      << tP(1,0) << "," << tP(1,1) << "," << tP(1,2) << "," << tP(1,3) << ";"
      << tP(2,0) << "," << tP(2,1) << "," << tP(2,2) << "," << tP(2,3) << ";"
      << tP(3,0) << "," << tP(3,1) << "," << tP(3,2) << "," << tP(3,3) << ")"
      << " _data=\"" << &_data << "\" _data.transformation=("
      << tD(0,0) << "," << tD(0,1) << "," << tD(0,2) << "," << tD(0,3) << ";"
      << tD(1,0) << "," << tD(1,1) << "," << tD(1,2) << "," << tD(1,3) << ";"
      << tD(2,0) << "," << tD(2,1) << "," << tD(2,2) << "," << tD(2,3) << ";"
      << tD(3,0) << "," << tD(3,1) << "," << tD(3,2) << "," << tD(3,3) << ")"
      << ") _colormap=\"" << &_colormap << "\">" << std::endl;
#endif
    if (blitz::any(transformation != _data.transformation()))
        _data.setTransformation(transformation);
    Channel::setTransformation(transformation);
  }

  blitz::TinyVector<double, 3> FloatChannel::boundingBoxLowerBoundUm()
    const {
    return _data.lowerBoundUm();
  }

  blitz::TinyVector<double, 3> FloatChannel::boundingBoxUpperBoundUm()
    const {
    return _data.upperBoundUm();
  }

  float FloatChannel::valueAt(
    blitz::TinyVector<double, 3> const &positionUm) const {
    return _data.valueAt(positionUm);
  }

  std::string FloatChannel::stringValueAt(
    blitz::TinyVector<double, 3> const &positionUm) const {
    std::stringstream s;
    s << _data.valueAt(positionUm);
    return s.str();
  }

  atb::Array<float, 3> const &FloatChannel::data() const {
    return _data;
  }

  float *FloatChannel::dataFirst() {
    return _data.dataFirst();
  }

  void FloatChannel::load(
    BlitzH5File const &inFile, std::string const &dsName,
    ProgressReporter *pr) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatChannel@" << this << "::load inFile=\""
      << inFile.name() << "\" dsName=\"" << dsName << "\" pr=\""
      << pr << "\" _data=\"" << &_data << "\" _colormap=\""
      << &_colormap << "\">" << std::endl;
#endif
    try {
      int nChannels = 0;
      inFile.readAttribute(nChannels, "nChannels", "/.labelling_qt4");
      for (int c = 0; c < nChannels; ++c) {
        std::stringstream groupString;
        groupString << "/.labelling_qt4/channel_" << c;
        std::string projectChannelName;
        inFile.readAttribute(projectChannelName, "name", groupString.str());
        if (projectChannelName == dsName) {
          load(inFile, c, pr);
#ifdef DEBUG_VERBOSE_XML
          std::cerr << "</FloatChannel@" << this << "::load>" << std::endl;
#endif
          return;
        }
      }
    }
    catch (BlitzH5Error &) {
    }

    setName(dsName);
    setVisible(true);
    setAlpha(1.0);
    _colormap.reset();
    loadDataset(inFile, dsName, pr);
    setElementSizeUm(_data.elementSizeUm());
    setTransformation(_data.transformation());
    setModified(false);

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</FloatChannel@" << this << "::load>" << std::endl;
#endif
  }

  void FloatChannel::save(
    BlitzH5File &outFile, size_t modelIndex, ProgressReporter *pr) {
    Channel::save(outFile, modelIndex, pr);
    std::stringstream channelMetadataGroupStream;
    channelMetadataGroupStream << "/.labelling_qt4/channel_" << modelIndex;
    _colormap.save(outFile, channelMetadataGroupStream.str() + "/colormap");

    if (!outFile.existsDataset(name()) || modified()) {
      if (outFile.existsDataset(name())) {
        try {
          _data.save(outFile, name(), 3, pr);
        }
        catch (BlitzH5Error &) {
          outFile.deleteDataset(name());
          _data.save(outFile, name(), 3, pr);
        }
      }
      else _data.save(outFile, name(), 3, pr);
    }

    setModified(false);

    if (pr != NULL) pr->updateProgress(pr->taskProgressMax());
  }

  void FloatChannel::load(
    BlitzH5File const &inFile, size_t modelIndex, ProgressReporter *pr) {
    Channel::load(inFile, modelIndex, pr);
    std::stringstream channelMetadataGroupStream;
    channelMetadataGroupStream << "/.labelling_qt4/channel_" << modelIndex;
    _colormap.load(inFile, channelMetadataGroupStream.str() + "/colormap");
    loadDataset(inFile, name(), pr);
    setModified(false);
    if (pr != NULL) pr->updateProgress(pr->taskProgressMax());
  }

  void FloatChannel::loadDataset(
    BlitzH5File const &inFile, std::string const &dsName,
    ProgressReporter *pr) {
    std::vector<hsize_t> dsShape(inFile.getDatasetShape(dsName));
    if (dsShape.size() == 2) {
      atb::Array<float, 2> tmp;
      tmp.load(inFile, dsName, pr);
      _data.resize(1, tmp.extent(0), tmp.extent(1));
      std::memcpy(
        _data.dataFirst(), tmp.dataFirst(), tmp.size() * sizeof(float));
      _data.setElementSizeUm(
        blitz::TinyVector<double, 3>(
          1.0, tmp.elementSizeUm()(0), tmp.elementSizeUm()(1)));
      blitz::TinyMatrix<double, 4, 4> trafo(
        atb::traits< blitz::TinyMatrix<double, 4, 4> >::one);
      for (int r = 1; r < 4; ++r)
        for (int c = 1; c < 4; ++c)
          trafo(r, c) = tmp.transformation()(r - 1, c - 1);
      _data.setTransformation(trafo);
    }
    else _data.load(inFile, dsName, pr);
    if (pr != NULL && pr->isAborted()) {
      _data.resize(atb::BlitzIndexT(0));
      _data.setElementSizeUm(1.0);
      _data.setTransformation(
        atb::traits< blitz::TinyMatrix<double, 4, 4> >::one);
      return;
    }
  }

}
