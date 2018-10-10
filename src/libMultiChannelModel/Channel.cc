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

#include "Channel.hh"
#include "FloatChannel.hh"
 // #include "IndexedChannel.hh"
 // #include "RGBChannel.hh"
 // #include "AnnotationChannel.hh"
#include "ChannelObserver.hh"
#include "MultichannelModel.hh"

#include <stdexcept>

namespace iRoCS {

#ifdef DEBUG_VERBOSE_XML
  std::string Channel::printState() const {
    std::stringstream stateStream;
    stateStream << "p_model=\"" << p_model << "\" _name=\"" << _name
      << "\" _filename=\"" << _filename << "\" _alpha=\""
      << _alpha << "\" _visible=\"" << _visible << "\" _modified=\""
      << _modified << "\" _observers=\"";
    if (_observers.size() != 0) {
      std::set<ChannelObserver*>::const_iterator it = _observers.begin();
      stateStream << *it;
      ++it;
      for (; it != _observers.end(); ++it) stateStream << "," << *it;
    }
    stateStream << "\"";
    return stateStream.str();
  }
#endif

  Channel::Types const Channel::Any =
    Channel::Float | Channel::Indexed | Channel::RGB | Channel::Annotation |
    Channel::SplineCurve | Channel::IRoCS | Channel::IRoCSSCT;

  std::string Channel::typeToString(Type type) {
    switch (type) {
      case Float:
        return "data";
      case Indexed:
        return "visualization";
      case RGB:
        return "rgb";
      case Annotation:
        return "annotation";
      case SplineCurve:
        return "B-Spline";
      case IRoCS:
        return "iRoCS";
      case IRoCSSCT:
        return "iRoCS Shell Coordinate Transform";
      default:
      {
        std::stringstream msg;
        msg << __FILE__ << ":" << __LINE__ << ": Missing implementation for "
          << "Channel::Type " << int(type);
        throw std::runtime_error(msg.str().c_str());
      }
    }
    return "";
  }

  Channel::Type Channel::typeFromString(std::string const &typeString) {
    if (typeString == "data") return Float;
    if (typeString == "visualization") return Indexed;
    if (typeString == "rgb") return RGB;
    if (typeString == "annotation") return Annotation;
    if (typeString == "B-Spline") return SplineCurve;
    if (typeString == "iRoCS") return IRoCS;
    if (typeString == "iRoCS Shell Coordinate Transform") return IRoCSSCT;
    std::stringstream msg;
    msg << __FILE__ << ":" << __LINE__ << ": Missing implementation for "
      << "type string '" << typeString << "'";
    throw std::runtime_error(msg.str().c_str());
  }

  Channel::Channel()
    : Observable(), _observers(), p_model(NULL), _name(""),
    _filename(""), _alpha(1.0), _visible(true), _modified(false),
    _elementSizeUm(1.0),
    _transformation(atb::traits< blitz::TinyMatrix<double, 4, 4> >::one),
    _boundingBoxLowerBoundUm(0.0), _boundingBoxUpperBoundUm(0.0) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<Channel@" << this << "::Channel " << printState() << " />"
      << std::endl;
#endif
  }

  Channel::~Channel() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<Channel@" << this << "::~Channel " << printState() << ">"
      << std::endl;
#endif

    if (p_model != NULL) {
#ifdef DEBUG_VERBOSE_XML
      std::cerr << __FILE__ << ":" << __LINE__ << ": p_model.removeChannel("
        << this << ")" << std::endl;
#endif
      p_model->removeChannel(this);
    }

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</Channel@" << this << "::~Channel>" << std::endl;
#endif
  }

  std::set<ChannelObserver*> const &Channel::observers() const {
    return _observers;
  }

  void Channel::setModel(MultichannelModel *model) {
    if (p_model == model) return;

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<Channel@" << this << "::setModel model=\""
      << model << "\" " << printState() << ">" << std::endl;
#endif

    if (p_model != NULL) p_model->removeChannel(this);
    p_model = model;
    if (p_model != NULL) p_model->addChannel(this);

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</Channel@" << this << "::setModel>" << std::endl;
#endif
  }

  MultichannelModel const *Channel::model() const {
    return p_model;
  }

  void Channel::setName(std::string const &name) {
    std::string simpleName = simplified(name);
    if (_name == simpleName) return;
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<Channel@" << this << "::setName name=\"" << name
              << "\" p_model=\"" << p_model << "\" " << printState() << ">"
              << std::endl;
#endif
    _name = simpleName;
    if (p_model != NULL) _name = p_model->ensureValidName(this);
    for (std::set<ChannelObserver*>::const_iterator it = _observers.begin();
      it != _observers.end(); ++it) (*it)->updateName();
    if (p_model != NULL) p_model->setModified(true);
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</Channel@" << this << "::setName>" << std::endl;
#endif
  }

  std::string const &Channel::name() const {
    return _name;
  }

  std::vector<std::string> Channel::nameComponents() const {
    return nameComponents(_name);
  }

  void Channel::setFilename(std::string const &filename) {
    if (_filename == filename) return;
    _filename = filename;
    for (std::set<ChannelObserver*>::const_iterator it = _observers.begin();
      it != _observers.end(); ++it) (*it)->updateFilename();
  }

  std::string const &Channel::filename() const {
    return _filename;
  }

  void Channel::setAlpha(double alpha) {
    if (_alpha == alpha) return;
    _alpha = alpha;
    for (std::set<ChannelObserver*>::const_iterator it = _observers.begin();
      it != _observers.end(); ++it) (*it)->updateAlpha();
    if (p_model != NULL) p_model->setModified(true);
  }

  double Channel::alpha() const {
    return _alpha;
  }

  void Channel::setVisible(bool visible) {
    if (_visible == visible) return;
    _visible = visible;
    for (std::set<ChannelObserver*>::const_iterator it = _observers.begin();
      it != _observers.end(); ++it) (*it)->updateVisibilityFlag();
    if (p_model != NULL) p_model->setModified(true);
  }

  bool Channel::visible() const {
    return _visible;
  }

  void Channel::setModified(bool modified) {
    if (_modified == modified) return;
    _modified = modified;
    for (std::set<ChannelObserver*>::const_iterator it = _observers.begin();
      it != _observers.end(); ++it) (*it)->updateModifiedFlag();
  }

  bool Channel::modified() const {
    return _modified;
  }

  void Channel::setElementSizeUm(
    blitz::TinyVector<double, 3> const &elementSizeUm) {
    if (blitz::all(_elementSizeUm == elementSizeUm)) return;
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<Channel@" << this << "::setElementSizeUm elementSizeUm=\""
      << elementSizeUm(0) << "," << elementSizeUm(1) << ","
      << elementSizeUm(2) << "\" " << printState()
      << " _elementSizeUm=\"" << _elementSizeUm(0) << ","
      << _elementSizeUm(1) << "," << _elementSizeUm(2) << "\">"
      << std::endl;
#endif
    _elementSizeUm = elementSizeUm;
    for (std::set<ChannelObserver*>::const_iterator it = _observers.begin();
      it != _observers.end(); ++it) (*it)->updateElementSizeUm();
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</Channel@" << this << "::setElementSizeUm>" << std::endl;
#endif
  }

  blitz::TinyVector<double, 3> const &Channel::elementSizeUm() const {
    return _elementSizeUm;
  }

  void Channel::setTransformation(
    blitz::TinyMatrix<double, 4, 4> const &transformation) {
    if (blitz::all(_transformation == transformation)) return;
    _transformation = transformation;
    for (std::set<ChannelObserver*>::const_iterator it = _observers.begin();
      it != _observers.end(); ++it) (*it)->updateTransformation();
  }

  blitz::TinyMatrix<double, 4, 4> const &Channel::transformation() const {
    return _transformation;
  }

  blitz::TinyVector<double, 3> Channel::boundingBoxLowerBoundUm() const {
    return _boundingBoxLowerBoundUm;
  }

  blitz::TinyVector<double, 3> Channel::boundingBoxUpperBoundUm() const {
    return _boundingBoxUpperBoundUm;
  }

  void Channel::save(
    BlitzH5File &outFile, size_t modelIndex, ProgressReporter *) {
    std::stringstream channelMetadataGroupStream;
    channelMetadataGroupStream << "/.labelling_qt4/channel_" << modelIndex;
    if (outFile.existsGroup(channelMetadataGroupStream.str()))
      outFile.deleteGroup(channelMetadataGroupStream.str());
    outFile.writeAttribute(name(), "name", channelMetadataGroupStream.str());
    outFile.writeAttribute(
      typeToString(type()), "type", channelMetadataGroupStream.str());
    outFile.writeAttribute(alpha(), "alpha", channelMetadataGroupStream.str());
    int tmp = (visible()) ? 1 : 0;
    outFile.writeAttribute(tmp, "visible", channelMetadataGroupStream.str());

    setFilename(outFile.name());
  }

  Channel *Channel::createFromFile(
    BlitzH5File const &inFile, size_t modelIndex, ProgressReporter *pr) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<Channel::createFromFile inFile=\""
      << inFile.name() << "\" modelIndex=\"" << modelIndex
      << "\" pr=\"" << pr << "\">" << std::endl;
#endif

    std::stringstream channelMetadataGroupStream;
    channelMetadataGroupStream << "/.labelling_qt4/channel_" << modelIndex;
    std::string typeString;
    inFile.readAttribute(typeString, "type", channelMetadataGroupStream.str());
    Type channelType;
    try {
      channelType = typeFromString(typeString);
    }
    catch (std::exception &) {
      if (pr != NULL)
        pr->abortWithError("Cannot load channel: Unknown channel type");
#ifdef DEBUG_VERBOSE_XML
      std::cerr << "return NULL" << std::endl;
      std::cerr << "</Channel::createFromFile>" << std::endl;
#endif
      return NULL;
    }
    Channel *res = NULL;
    switch (channelType) {
      case Float:
        res = new FloatChannel();
        break;
        // case Indexed:
        //   res = new IndexedChannel();
        //   break;
        // case RGB:
        //   res = new RGBChannel();
        //   break;
        // case Annotation:
        //   res = new AnnotationChannel();
        //   break;
        // case SplineCurve:
        //   res = new SplineCurveChannel();
        //   break;
        // case IRoCS:
        //   res = new IRoCSChannel();
        //   break;
        // case IRoCSSCT:
        //   res = new IRoCSSCTChannel();
        //   break;
      default:
      {
        std::stringstream msg;
        msg << __FILE__ << ":" << __LINE__ << ": Missing implementation for "
          << "type '" << typeString << "'";
#ifdef DEBUG_VERBOSE_XML
        std::cerr << "throw std::runtime_error(\"" << msg.str() << "\")"
          << std::endl;
        std::cerr << "</Channel::createFromFile>" << std::endl;
#endif
        throw std::runtime_error(msg.str().c_str());
      }
    }
    res->load(inFile, modelIndex, pr);
    res->setFilename(inFile.name());
    if (pr != NULL && pr->isAborted()) {
      delete res;
      res = NULL;
    }

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "return " << res << std::endl;
    std::cerr << "</Channel::createFromFile>" << std::endl;
#endif
    return res;
  }

  Channel *Channel::createFromFile(
    BlitzH5File const &inFile, std::string const &name,
    ProgressReporter *pr) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<Channel::createFromFile inFile=\""
      << inFile.name() << "\" name=\"" << name << "\" pr=\"" << pr
      << "\">" << std::endl;
#endif

    Channel *channel = NULL;

    // First check whether meta-data for that channel exist
    try {
      int nChannels = 0;
      inFile.readAttribute(nChannels, "nChannels", "/.labelling_qt4");
      for (int c = 0; c < nChannels; ++c) {
        std::stringstream groupString;
        groupString << "/.labelling_qt4/channel_" << c;
        std::string projectChannelName;
        inFile.readAttribute(projectChannelName, "name", groupString.str());
        if (projectChannelName == name) {
          channel = createFromFile(inFile, c, pr);
#ifdef DEBUG_VERBOSE_XML
          std::cerr << "return " << channel << std::endl;
          std::cerr << "</Channel::createFromFile>" << std::endl;
#endif
          return channel;
        }
      }
    }
    catch (BlitzH5Error &) {
    }

    if (inFile.existsDataset(name)) {
      std::vector<hsize_t> dsExtents(inFile.getDatasetShape(name));
      switch (dsExtents.size()) {
        case 2:
        case 3:
        {
          if (dsExtents.size() == 3 && dsExtents[2] == 3) {
            // Check whether it is actually 2D RGB
            try {
              std::string dimInterpretation;
              inFile.readAttribute(
                dimInterpretation, "dim_interpretation", name);
              if (dimInterpretation[dimInterpretation.size() - 1] == 'c') {
                std::stringstream msg;
                msg << __FILE__ << ":" << __LINE__ << ": RGBChannel not "
                  << "implemented";
                throw std::runtime_error(msg.str().c_str());
                // channel = new RGBChannel();
                // static_cast<RGBChannel*>(channel)->load(inFile, name, pr);
                break;
              }
            }
            catch (BlitzH5Error &) {
            }
          }

          // Ordinary gray value data
          hid_t datasetTypeId = inFile.getDatasetType(name);
          switch (H5Tget_class(datasetTypeId)) {
            case H5T_INTEGER:
            {
              H5Tclose(datasetTypeId);
              std::stringstream msg;
              msg << __FILE__ << ":" << __LINE__ << ": IndexedChannel not "
                << "implemented";
              throw std::runtime_error(msg.str().c_str());
              // channel = new IndexedChannel();
              // static_cast<IndexedChannel*>(channel)->load(inFile, name, pr);
              break;
            }
            case H5T_FLOAT:
            {
              H5Tclose(datasetTypeId);
              channel = new FloatChannel();
              static_cast<FloatChannel*>(channel)->load(inFile, name, pr);
              break;
            }
            default:
              H5Tclose(datasetTypeId);
#ifdef DEBUG_VERBOSE_XML
              std::cerr << "throw BlitzH5Error(\"Datatype of dataset '" << name
                << "' not unsupported.\")" << std::endl;
              std::cerr << "</Channel::createFromFile>" << std::endl;
#endif
              throw BlitzH5Error(
                "Datatype of dataset '" + name + "' not unsupported.");
          }
          break;
        }
        case 4:
        {
          if (dsExtents[3] == 3) {
            std::stringstream msg;
            msg << __FILE__ << ":" << __LINE__ << ": RGBChannel not "
              << "implemented";
            throw std::runtime_error(msg.str().c_str());
            // channel = new RGBChannel();
            // static_cast<RGBChannel*>(channel)->load(inFile, name, pr);
            // break;
          }
          // fall through
        }
        default:
#ifdef DEBUG_VERBOSE_XML
          std::cerr << "throw BlitzH5Error(\"" << dsExtents.size()
            << "-D datasets are not supported.\")" << std::endl;
          std::cerr << "</Channel::createFromFile>" << std::endl;
#endif
          throw BlitzH5Error()
            << dsExtents.size() << "-D datasets are not supported.";
      }
      channel->setFilename(inFile.name());
#ifdef DEBUG_VERBOSE_XML
      std::cerr << "return " << channel << std::endl;
      std::cerr << "</Channel::createFromFile>" << std::endl;
#endif
      return channel;
    }

    if (inFile.existsGroup(name)) {
      if (inFile.existsDataset(name + "/position_um")) {
        std::stringstream msg;
        msg << __FILE__ << ":" << __LINE__ << ": AnnotationChannel not "
          << "implemented";
        throw std::runtime_error(msg.str().c_str());
        // channel = new AnnotationChannel();
        // static_cast<AnnotationChannel*>(channel)->load(inFile, name, pr);
      }
      else {
#ifdef DEBUG_VERBOSE_XML
        std::cerr << "throw BlitzH5Error(\"ToDo: Vectorial channel type: "
          << "Implement me\")" << std::endl;
        std::cerr << "</Channel::createFromFile>" << std::endl;
#endif
        throw BlitzH5Error("ToDo: Vectorial channel type: Implement me");
      }
      channel->setFilename(inFile.name());
#ifdef DEBUG_VERBOSE_XML
      std::cerr << "return " << channel << std::endl;
      std::cerr << "</Channel::createFromFile>" << std::endl;
#endif
      return channel;
    }

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "throw BlitzH5Error(\"No such group or dataset\")"
      << std::endl;
    std::cerr << "</Channel::createFromFile>" << std::endl;
#endif
    throw BlitzH5Error("No such group or dataset");
  }

  void Channel::load(
    BlitzH5File const &inFile, size_t modelIndex, ProgressReporter *pr) {
    setFilename(inFile.name());
    std::stringstream channelMetadataGroupStream;
    channelMetadataGroupStream << "/.labelling_qt4/channel_" << modelIndex;
    std::string name;
    inFile.readAttribute(name, "name", channelMetadataGroupStream.str());
    setName(name);
    if (pr != NULL && !pr->updateProgressMessage(
      "Loading '" + inFile.name() + ":" + name + "'")) return;
    double alpha;
    inFile.readAttribute(alpha, "alpha", channelMetadataGroupStream.str());
    setAlpha(alpha);
    int tmp;
    inFile.readAttribute(tmp, "visible", channelMetadataGroupStream.str());
    setVisible(tmp != 0);
  }

  void Channel::setBoundingBoxLowerBoundUm(
    blitz::TinyVector<double, 3> const &boundingBoxLowerBoundUm) {
    _boundingBoxLowerBoundUm = boundingBoxLowerBoundUm;
  }

  void Channel::setBoundingBoxUpperBoundUm(
    blitz::TinyVector<double, 3> const &boundingBoxUpperBoundUm) {
    _boundingBoxUpperBoundUm = boundingBoxUpperBoundUm;
  }

  std::string Channel::simplified(std::string const &name) {
    std::vector<std::string> components(nameComponents(name));
    if (components.size() == 0) return "";
    std::string out = "";
    for (size_t i = 0; i < components.size(); ++i)
      out += "/" + components[i];
    return out;
  }

  std::vector<std::string> Channel::nameComponents(std::string const &name) {
    std::vector<std::string> components;
    std::string resid = name;
    while (resid.size() != 0) {
      // Strip leading '/' characters
      while (resid.find("/") == 0) resid = resid.substr(1, std::string::npos);
      if (resid.size() == 0) break;
      if (resid.find("/") == std::string::npos) {
        components.push_back(resid);
        resid = "";
      }
      else {
        components.push_back(resid.substr(0, resid.find("/")));
        resid = resid.substr(resid.find("/"), std::string::npos);
      }
    }
    return components;
  }

}
