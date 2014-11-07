#include "BlitzHdf5Light.hh"

/*-----------------------------------------------------------------------
 *  BlitzH5Error class implementation
 *-----------------------------------------------------------------------*/

BlitzH5Error::BlitzH5Error() 
        : std::exception()
{}

BlitzH5Error::BlitzH5Error( const std::string& message)
        : std::exception(), _message(message)
{}

BlitzH5Error::~BlitzH5Error() throw()
{}

std::string const &BlitzH5Error::str() const
{
  return _message;
}

char const *BlitzH5Error::what() const throw()
{
  return _message.c_str();
} 


/*-----------------------------------------------------------------------
 *  BlitzH5File class implementation
 *-----------------------------------------------------------------------*/

BlitzH5File::BlitzH5File()
        : _fileId(-1), _mode(ReadOnly)
{
  // Disable error stacks
  H5Eset_auto2(H5E_DEFAULT, NULL, NULL);
}

BlitzH5File::BlitzH5File(std::string const &fileName, FileMode mode)
        : _fileId(-1), _mode(mode)
{
  // Disable error stacks
  H5Eset_auto2(H5E_DEFAULT, NULL, NULL);

  htri_t isHDF5;
  std::ifstream ifile(fileName.c_str());
  if (ifile) isHDF5 = H5Fis_hdf5(fileName.c_str());
  else isHDF5 = -1;
  
  switch (mode)
  {
  case ReadOnly:
  {
    if (isHDF5 == -1)
        throw BlitzH5Error()
            << "Could not open file '" << fileName
            << "'. File not found.";
    if (isHDF5 == 0)
        throw BlitzH5Error()
            << "Could not open file '" << fileName << "'. No HDF5 file.";
    _fileId = H5Fopen(
        fileName.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if (_fileId < 0)
    {
      _fileId = H5Fopen(
          fileName.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
      if (_fileId < 0)
          throw BlitzH5Error()
              << "Could not open file '" << fileName
              << "'. Insufficient file permissions.";
      std::cerr
          << "Warning: Could not open file '" << fileName
          << "' in ReadOnly mode as requested. It is already opened in "
          << "Write mode from another location. Setting access rights to "
          << "Write.";
    }
    break;
  }
  case Write:
  {
    if (isHDF5 == -1)
        throw BlitzH5Error()
            << "Could not open file '" << fileName
            << "'. File not found.";
    if (isHDF5 == 0)
        throw BlitzH5Error()
            << "Could not open file '" << fileName << "'. No HDF5 file.";
    _fileId = H5Fopen(
        fileName.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
    if (_fileId < 0)
    {
      hid_t fileId = H5Fopen(
        fileName.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
      if (fileId < 0)
          throw BlitzH5Error()
              << "Could not open file '" << fileName
              << "'. Insufficient file permissions.";
      H5Fclose(fileId);
      _fileId = -1;
      throw BlitzH5Error()
          << "Could not open file '" << fileName
          << "' in Write mode. It is already opened in ReadOnly mode from "
          << "another location.";
    }
    break;
  }
  case Replace:
  {
    _fileId = H5Fcreate(
        fileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (_fileId < 0)
    {
      if (isHDF5 == 1)
      {
        hid_t fileId = H5Fopen(fileName.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
        if (fileId < 0)
            fileId = H5Fopen(fileName.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
        if (fileId >= 0)
        {
          H5Fclose(fileId);
          throw BlitzH5Error()
              << "Could not replace file '" << fileName
              << "'. It is already opened in ReadOnly mode from another "
              << "location.";        
        }
      }
      throw BlitzH5Error()
          << "Could not replace file '" << fileName
          << "'. Insufficient file permissions.";        
    }
    break;
  }
  case New:
  {
    if (isHDF5 != -1)
        throw BlitzH5Error()
            << "Could not create file '" << fileName
            << "'. A file with that name already exists.";
    _fileId = H5Fcreate(
        fileName.c_str(), H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
    if (_fileId < 0)
        throw BlitzH5Error()
            << "Could not create file '" << fileName
            << "'. Check whether the destination folder exists and "
            << "you have write permission.";    
    break;
  }
  case WriteOrNew:
  {
    if (isHDF5 == 0)
        throw BlitzH5Error()
            << "Could not create or open file '" << fileName
            << "'. A non-HDF5 file with that name already exists.";        
    if (isHDF5 == -1)
    {
      _fileId = H5Fcreate(
          fileName.c_str(), H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
      if (_fileId < 0)
          throw BlitzH5Error()
              << "Could not create file '" << fileName
              << "'. Check whether the destination folder exists and "
              << "you have write permission.";   
      return;
    }
    _fileId = H5Fopen(
        fileName.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
    if (_fileId < 0)
    {
      hid_t fileId = H5Fopen(
        fileName.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
      if (fileId < 0)
          throw BlitzH5Error()
              << "Could not open file '" << fileName
              << "'. Insufficient file permissions.";
      H5Fclose(fileId);
      _fileId = -1;
      throw BlitzH5Error()
          << "Could not open file '" << fileName
          << "' in Write mode. It is already opened in ReadOnly mode from "
          << "another location.";
    }
    break;
  }
  }

  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not open or create '" << fileName
          << "'. Unknown reason.";
}

BlitzH5File::~BlitzH5File()
{
  if (_fileId < 0) return;
  size_t nOpenObjectsLocal =
      H5Fget_obj_count(_fileId, H5F_OBJ_ALL | H5F_OBJ_LOCAL);

  if (nOpenObjectsLocal > 1)
  {
    // Close locally opened attributes
    size_t nOpenAttributesLocal =
        H5Fget_obj_count(_fileId, H5F_OBJ_ATTR | H5F_OBJ_LOCAL);
    if (nOpenAttributesLocal != 0)
    {
      std::cerr << "  " << nOpenAttributesLocal
                << " attributes were not properly closed." << std::endl;
      std::vector<hid_t> ids(nOpenAttributesLocal);
      H5Fget_obj_ids(
          _fileId, H5F_OBJ_ATTR | H5F_OBJ_LOCAL, nOpenAttributesLocal,
          ids.data());
      for (size_t i = 0; i < nOpenAttributesLocal; ++i) H5Aclose(ids[i]);
    }
    
    // Close all objects that can be closed by an H5Oclose call
    nOpenObjectsLocal =
        H5Fget_obj_count(_fileId, H5F_OBJ_GROUP | H5F_OBJ_DATATYPE |
                         H5F_OBJ_DATASET | H5F_OBJ_LOCAL); 
    std::vector<hid_t> ids(nOpenObjectsLocal);
    H5Fget_obj_ids(
        _fileId, H5F_OBJ_GROUP | H5F_OBJ_DATATYPE | H5F_OBJ_DATASET |
        H5F_OBJ_LOCAL, nOpenObjectsLocal, ids.data());
    for (size_t i = 0; i < nOpenObjectsLocal; ++i) H5Oclose(ids[i]); 
  }

  nOpenObjectsLocal = H5Fget_obj_count(_fileId, H5F_OBJ_ALL | H5F_OBJ_LOCAL);
  assert(nOpenObjectsLocal == 1);

  size_t nOpenObjects = H5Fget_obj_count(_fileId, H5F_OBJ_ALL);
  if (nOpenObjects > 1)
      std::cerr << "Warning: The BlitzH5File destructor cannot close the file. "
                << "It is accessed using multiple identifiers. As soon as the "
                << "last object identifier is released, the file will close "
                << "automatically." << std::endl;
  
  H5Fclose(_fileId);
}

std::string BlitzH5File::simplifyGroupDescriptor(std::string const& group)
{
  if (group.size() == 0) return "";

  std::string simplified = (group[0] == '/') ? "/" : "";

  // Group descriptors may not contain relative paths with ..
  if (group.find("../") == 0 ||
      group.find("/../") != std::string::npos ||
      (group.size() >= 3 && group.rfind("/..") == group.size() - 3) ||
      group == "..")
      throw BlitzH5Error()
          << "Invalid group descriptor '" << group
          << "'. Group descriptors may not contain '..'!";

  // Remove trailing '.'
  size_t beginPos = 0;
  size_t endPos = group.size();
  if (endPos >= 2 && group.rfind("/.") == endPos - 2) endPos--;

  size_t delimPos = 0;
  while (beginPos < endPos)
  {
    while(beginPos < endPos &&
          (group.find("/", beginPos) == beginPos ||
           group.find("./", beginPos) == beginPos)) beginPos++;
    if (beginPos == endPos) break;

    delimPos = group.find('/', beginPos);
    if (delimPos == std::string::npos)
    {
      simplified += group.substr(beginPos) + "/";
      break;
    }
    simplified += group.substr(beginPos, delimPos - beginPos) + "/";
    beginPos = delimPos + 1;
  }
  if (simplified.rfind("/") == simplified.size() - 1)
      simplified = simplified.substr(0, simplified.size() - 1);
  return simplified;
}

bool BlitzH5File::existsDataset(std::string const &name) const
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not check existence of dataset '" << name
          << "'. The BlitzH5File is not open.";
  hid_t datasetId = H5Dopen2(_fileId, name.c_str(), H5P_DEFAULT);
  if (datasetId < 0) return false;
  H5Dclose(datasetId);
  return true;
}

std::vector<hsize_t> BlitzH5File::getDatasetShape(std::string const &name) const
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not get shape of dataset '" << name
          << "'. The BlitzH5File is not open.";
  hid_t datasetId = H5Dopen2(_fileId, name.c_str(), H5P_DEFAULT);
  if (datasetId < 0)
      throw BlitzH5Error()
          << "Could not get shape of dataset '" << name
          << "'. Dataset not found.";
  hid_t dataspaceId = H5Dget_space(datasetId);
  if (dataspaceId < 0)
  {
    H5Dclose(datasetId);
    throw BlitzH5Error()
        << "Could not get shape of dataset '" << name
        << "'. Could not open dataspace.";
  }
  int nDims = H5Sget_simple_extent_ndims(dataspaceId);
  std::vector<hsize_t> extents(nDims);
  H5Sget_simple_extent_dims(dataspaceId, extents.data(), NULL);
  H5Sclose(dataspaceId);
  H5Dclose(datasetId);
  return extents;
}

std::vector<std::string>
BlitzH5File::allDatasets(std::string const &startGroup) const
{
  std::vector<std::string> datasets;
  std::vector<std::string> objects(getObjects(startGroup));
  for (size_t i = 0; i < objects.size(); ++i)
  {
    if (existsGroup(startGroup + "/" + objects[i]))
    {
      std::vector<std::string> subDatasets(
          allDatasets(startGroup + "/" + objects[i]));
      datasets.insert(
          datasets.end(), subDatasets.begin(), subDatasets.end());
    }
    else datasets.push_back(
        simplifyGroupDescriptor(startGroup) + "/" + objects[i]);
  }
  return datasets;
}

hid_t BlitzH5File::getDatasetType(std::string const &name) const
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not get type of dataset '" << name
          << "'. The BlitzH5File is not open.";
  hid_t datasetId = H5Dopen2(_fileId, name.c_str(), H5P_DEFAULT);
  if (datasetId < 0)
      throw BlitzH5Error()
          << "Could not determine type of '" << name
          << "'. Dataset not found.";
  hid_t datatype = H5Dget_type(datasetId);
  hid_t nativeType = H5Tget_native_type(datatype, H5T_DIR_ASCEND);
  H5Tclose(datatype);
  H5Dclose(datasetId);
  return nativeType;
}

void BlitzH5File::readDataset(
    std::string &data, std::string const &name,
    iRoCS::ProgressReporter */*pr*/) const
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not read dataset '" << name
          << "'. The BlitzH5File is not open.";
  hid_t datasetId = H5Dopen2(_fileId, name.c_str(), H5P_DEFAULT);
  if (datasetId < 0)
      throw BlitzH5Error()
          << "Could not read dataset '" << name
          << "'. Dataset not found.";
  hid_t datatypeId = H5Dget_type(datasetId);
  if (datatypeId < 0)
  {
    H5Dclose(datasetId);
    throw BlitzH5Error()
        << "Could not read dataset '" << name
        << "'. Could not get dataset type.";
  }

  // For the sake of compatibility also allow char Arrays as strings *sigh*
  if (H5Tget_class(datatypeId) != H5T_STRING && (
          H5Tget_class(datatypeId) != H5T_INTEGER ||
          H5Tget_size(datatypeId) != 1 ||
          H5Tget_sign(datatypeId) != H5T_SGN_2))
  {
    H5Tclose(datatypeId);
    H5Dclose(datasetId);
    throw BlitzH5Error()
        << "Could not read dataset '" << name
        << "'. Non-string datasets cannot be loaded into std::string "
        << "variables.";
  }
  if (H5Tget_class(datatypeId) == H5T_STRING && H5Tis_variable_str(datatypeId))
  {
    H5Tclose(datatypeId);
    H5Dclose(datasetId);
    throw BlitzH5Error()
        << "Could not read dataset '" << name
        << "'. Variable length strings are currently not supported.";
  }
  size_t stringLength = H5Tget_size(datatypeId);
  hid_t dataspaceId = H5Dget_space(datasetId);
  if (dataspaceId < 0)
  {
    H5Tclose(datatypeId);
    H5Dclose(datasetId);
    throw BlitzH5Error()
        << "Could not read dataset '" << name
        << "'. Could not get dataspace.";
  }
  hssize_t nElements = H5Sget_simple_extent_npoints(dataspaceId);
  H5Sclose(dataspaceId);
  stringLength *= nElements;
  char *buf = new char[stringLength + 1];
  herr_t err = H5Dread(
      datasetId, datatypeId, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
  buf[stringLength] = 0;
  data = buf;
  delete[] buf;
  H5Tclose(datatypeId);
  H5Dclose(datasetId);
  if (err < 0)
  {
    throw BlitzH5Error()
        << "Could not read dataset '" << name << ":"
        << "'. Could not read string data.";
  }
}

void BlitzH5File::writeDataset(
    std::string const &data, std::string const &name,
    iRoCS::ProgressReporter */*pr*/)
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not write dataset '" << name
          << "'. The BlitzH5File is not open.";
  if (_mode == ReadOnly)
      throw BlitzH5Error()
          << "Could not write dataset '" << name
          << "'. File is opened ReadOnly.";

  if (existsDataset(name)) deleteDataset(name);

  hid_t dataspaceId = H5Screate(H5S_SCALAR);
  if (dataspaceId < 0)
      throw BlitzH5Error()
          << "Could not write dataset '" << name
          << "'. Could not create dataspace.";
          
  hid_t datatypeId = H5Tcopy(H5T_C_S1);
  if (datatypeId < 0)
  {
    H5Sclose(dataspaceId);
    throw BlitzH5Error()
        << "Could not write dataset '" << name
        << "'. Could not copy datatype.";
  }
  herr_t err = H5Tset_size(datatypeId, data.size());
  if (err < 0)
  {
    H5Tclose(datatypeId);
    H5Sclose(dataspaceId);
    throw BlitzH5Error()
        << "Could not write dataset '" << name
        << "'. Could not set string length.";
  }
  
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);

  hid_t datasetId = H5Dcreate2(
      _fileId, name.c_str(), datatypeId, dataspaceId,
      linkCreationPropertiesId, H5P_DEFAULT, H5P_DEFAULT);
  H5Pclose(linkCreationPropertiesId);
  if (datasetId < 0)
  {
    H5Tclose(datatypeId);
    H5Sclose(dataspaceId);
    throw BlitzH5Error()
        << "Could not write dataset '" << name
        << "'. Could not create dataset.";
  }
          
  char *buf = new char[data.size() + 1];
  for (size_t i = 0; i < data.size(); ++i) buf[i] = data[i];
  buf[data.size()] = 0;
  err = H5Dwrite(datasetId, datatypeId, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
  H5Dclose(datasetId);
  H5Tclose(datatypeId);
  H5Sclose(dataspaceId);
  delete[] buf;
  if (err < 0)
      throw BlitzH5Error()
          << "Could not write dataset '" << name << "'. H5Dwrite failed.";
}

void BlitzH5File::deleteDataset(std::string const &name)
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not delete dataset '" << name
          << "'. The BlitzH5File is not open.";
  if (_mode == ReadOnly)
      throw BlitzH5Error()
          << "Could not delete dataset '" << name
          << "'. File is opened ReadOnly.";
  if (!existsDataset(name))
      throw BlitzH5Error()
          << "Could not delete dataset '" << name
          << "'. Dataset not found.";
  
  herr_t err = H5Ldelete(_fileId, name.c_str(), H5P_DEFAULT);
  if (err < 0)
      throw BlitzH5Error()
          << "Could not delete dataset '" << name
          << "'. Could not delete link.";
}

bool BlitzH5File::existsAttribute(
    std::string const &attName, std::string const &objectName) const
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not check whether attribute '" << objectName
          << ":" << attName << "' exists. The BlitzH5File is not open.";
  return (H5Aexists_by_name(
              _fileId, objectName.c_str(),
              attName.c_str(), H5P_DEFAULT) > 0);
}

std::vector<hsize_t> BlitzH5File::getAttributeShape(
    std::string const &attName, std::string const &objectName) const
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not get shape of attribute '" << objectName
          << ":" << attName << "'. The BlitzH5File is not open.";
  hid_t attributeId =
      H5Aopen_by_name(
          _fileId, objectName.c_str(), attName.c_str(),
          H5P_DEFAULT, H5P_DEFAULT);
  if (attributeId < 0)
      throw BlitzH5Error()
          << "Could not get shape of attribute '"
          << objectName << ":" << attName << "'. Attribute not found.";
  hid_t dataspaceId = H5Aget_space(attributeId);
  if (dataspaceId < 0)
  {
    H5Aclose(attributeId);
    throw BlitzH5Error()
        << "Could not get shape of attribute '"
        << objectName << ":" << attName << "'. Could not open "
        << "attribute's dataspace.";
  }
  int nDims = H5Sget_simple_extent_ndims(dataspaceId);
  std::vector<hsize_t> extents(nDims);
  herr_t err = H5Sget_simple_extent_dims(
      dataspaceId, extents.data(), NULL);
  if (err < 0)
  {
    H5Sclose(dataspaceId);
    H5Aclose(attributeId);
    throw BlitzH5Error()
        << "Could not get shape of attribute '"
        << objectName << ":" << attName << "'. Could not determine "
        << "dataspace extents.";
  }
  H5Sclose(dataspaceId);
  H5Aclose(attributeId);
  return extents;
}

hid_t BlitzH5File::getAttributeType(
    const std::string& attName, const std::string& objectName) const
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not get type of attribute '" << objectName
          << ":" << attName << "'. The BlitzH5File is not open.";
  hid_t attributeId =
      H5Aopen_by_name(
          _fileId, objectName.c_str(), attName.c_str(),
          H5P_DEFAULT, H5P_DEFAULT);
  if (attributeId < 0)
      throw BlitzH5Error()
          << "Could not get type of attribute '"
          << objectName << ":" << attName << "'. Attribute not found.";
  hid_t datatypeId = H5Aget_type(attributeId);
  if (datatypeId < 0)
  {
    H5Aclose(attributeId);
    throw BlitzH5Error()
        << "Could not get type of attribute '"
        << objectName << ":" << attName << "'. Internal error.";
  }
  hid_t nativeType = H5Tget_native_type(datatypeId, H5T_DIR_ASCEND);
  H5Tclose(datatypeId);
  H5Aclose(attributeId);
  return nativeType;
}

void BlitzH5File::readAttribute(
    std::string &data, std::string const &attName,
    std::string const &objectName) const
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not read attribute '" << objectName
          << ":" << attName << "'. The BlitzH5File is not open.";
  hid_t attributeId =
      H5Aopen_by_name(
          _fileId, objectName.c_str(), attName.c_str(),
          H5P_DEFAULT, H5P_DEFAULT);
  if (attributeId < 0)
      throw BlitzH5Error()
          << "Could not read attribute '" << objectName << ":"
          << attName << "'. Attribute not found.";
  hid_t datatypeId = H5Aget_type(attributeId);
  if (datatypeId < 0)
  {
    H5Aclose(attributeId);
    throw BlitzH5Error()
        << "Could not read attribute '" << objectName << ":"
        << attName << "'. Could not get attribute type.";
  }
  if (H5Tget_class(datatypeId) != H5T_STRING)
  {
    H5Tclose(datatypeId);
    H5Aclose(attributeId);
    throw BlitzH5Error()
        << "Could not read attribute '" << objectName << ":"
        << attName << "'. Non-string attributes cannot be loaded into "
        << "std::string variables.";
  }
  if (H5Tis_variable_str(datatypeId))
  {
    H5Tclose(datatypeId);
    H5Aclose(attributeId);
    throw BlitzH5Error()
        << "Could not read attribute '" << objectName << ":"
        << attName << "'. Variable length strings are currently not "
        << "supported.";
  }
  size_t stringLength = H5Tget_size(datatypeId);
  hid_t dataspaceId = H5Aget_space(attributeId);
  if (dataspaceId < 0)
  {
    H5Tclose(datatypeId);
    H5Aclose(attributeId);
    throw BlitzH5Error()
        << "Could not read attribute '" << objectName << ":"
        << attName << "'. Could not get the attribute's dataspace.";
  }
  hssize_t nElements = H5Sget_simple_extent_npoints(dataspaceId);
  H5Sclose(dataspaceId);
  stringLength *= nElements;
  char *buf = new char[stringLength + 1];
  herr_t err = H5Aread(attributeId, datatypeId, buf);
  buf[stringLength] = 0;
  data = buf;
  delete[] buf;
  H5Tclose(datatypeId);
  H5Aclose(attributeId);
  if (err < 0)
  {
    throw BlitzH5Error()
        << "Could not read attribute '" << objectName << ":"
        << attName << "'. Could not read string data.";
  }
}

void BlitzH5File::writeAttribute(
    std::string const &in, std::string const &attName,
    std::string const &objectName)
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not write attribute '" << objectName
          << ":" << attName << "'. The BlitzH5File is not open.";
  if (_mode == ReadOnly)
      throw BlitzH5Error()
          << "Could not write attribute '" << objectName
          << ":" << attName << "'. File is opened ReadOnly.";

  if (existsAttribute(attName, objectName))
      deleteAttribute(attName, objectName);

  hid_t dataspaceId = H5Screate(H5S_SCALAR);
  if (dataspaceId < 0)
      throw BlitzH5Error()
          << "Could not write attribute '" << objectName
          << ":" << attName << "'. Could not create dataspace.";
          
  hid_t datatypeId = H5Tcopy(H5T_C_S1);
  if (datatypeId < 0)
  {
    H5Sclose(dataspaceId);
    throw BlitzH5Error()
        << "Could not write attribute '" << objectName
        << ":" << attName << "'. Could not copy datatype.";
  }
  herr_t err = H5Tset_size(datatypeId, in.size());
  if (err < 0)
  {
    H5Tclose(datatypeId);
    H5Sclose(dataspaceId);
    throw BlitzH5Error()
        << "Could not write attribute '" << objectName
        << ":" << attName << "'. Could not set string length.";
  }
          
  if (simplifyGroupDescriptor(objectName) != "")
  {
    htri_t exists = H5Lexists(_fileId, objectName.c_str(), H5P_DEFAULT);
    if (exists <= 0)
    {
      hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
      H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
      hid_t groupId = H5Gcreate2(
          _fileId, objectName.c_str(), linkCreationPropertiesId,
          H5P_DEFAULT, H5P_DEFAULT);
      H5Pclose(linkCreationPropertiesId);
      if (groupId < 0)
      {
        H5Tclose(datatypeId);
        H5Sclose(dataspaceId);
        throw BlitzH5Error()
            << "Could not write attribute '" << objectName
            << ":" << attName << "'. Could not create group.";
      }
      H5Gclose(groupId);
    }
  }

  hid_t attributeId = H5Acreate_by_name(
      _fileId, objectName.c_str(), attName.c_str(),
      datatypeId, dataspaceId, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  if (attributeId < 0)
  {
    H5Tclose(datatypeId);
    H5Sclose(dataspaceId);
    throw BlitzH5Error()
        << "Could not write attribute '" << objectName
        << ":" << attName << "'. Could not create attribute.";
  }
          
  char *buf = new char[in.size() + 1];
  for (size_t i = 0; i < in.size(); ++i) buf[i] = in[i];
  buf[in.size()] = 0;
  err = H5Awrite(attributeId, datatypeId, buf);
  delete[] buf;
  H5Aclose(attributeId);
  H5Tclose(datatypeId);
  H5Sclose(dataspaceId);
  if (err < 0)
      throw BlitzH5Error()
          << "Could not write attribute '" << objectName
          << ":" << attName << "'. Could not write attribute data.";
}

void BlitzH5File::deleteAttribute(
    std::string const &attName, std::string const &objectName)
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not delete attribute '" << objectName
          << ":" << attName << "'. The BlitzH5File is not open.";
  if (_mode == ReadOnly)
      throw BlitzH5Error()
          << "Could not delete attribute '" << objectName
          << ":" << attName << "'. File is opened ReadOnly.";

  herr_t err =
      H5Adelete_by_name(
          _fileId, objectName.c_str(), attName.c_str(), H5P_DEFAULT);
  if (err < 0)
      throw BlitzH5Error()
          << "Could not delete attribute '" << objectName
          << ":" << attName << "'.";
}

std::vector<std::string>
BlitzH5File::getObjects(std::string const &groupName) const
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not get objects in group '" << groupName
          << "'. The BlitzH5File is not open.";
  H5G_info_t groupInfo;
  std::string h5GroupName =
      (simplifyGroupDescriptor(groupName) == "") ? "." : groupName;
  herr_t err = H5Gget_info_by_name(
      _fileId, h5GroupName.c_str(), &groupInfo, H5P_DEFAULT);
  if (err < 0)
  {
    throw BlitzH5Error()
        << "Could not retrieve contents of group '"
        << h5GroupName << "'. Group not found.";
  }
  std::vector<std::string> objects(groupInfo.nlinks);
  for (hsize_t i = 0; i < groupInfo.nlinks; ++i)
  {
    int objectNameLength =
        H5Lget_name_by_idx(
            _fileId, h5GroupName.c_str(), H5_INDEX_NAME, H5_ITER_INC,
            i, NULL, 0, H5P_DEFAULT);
    char *buf = new char[objectNameLength + 1];
    H5Lget_name_by_idx(
        _fileId, h5GroupName.c_str(), H5_INDEX_NAME, H5_ITER_INC,
        i, buf, objectNameLength + 1, H5P_DEFAULT);
    objects[i] = buf;
    delete[] buf;
  }
  return objects;
}

std::vector<std::string>
BlitzH5File::getGroups(std::string const &groupName) const
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not get groups below '" << groupName
          << "'. The BlitzH5File is not open.";
  std::vector<std::string> objectNames(getObjects(groupName));
  std::vector<std::string> groups;
  for (size_t i = 0; i < objectNames.size(); ++i)
      if (existsGroup(groupName + "/" + objectNames[i]))
          groups.push_back(objectNames[i]);
  return groups;
}

std::vector<std::string>
BlitzH5File::getAttributes(std::string const &objectName) const
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not get attributes of '" << objectName
          << "'. The BlitzH5File is not open.";
  H5O_info_t objectInfo;
  herr_t err = H5Oget_info_by_name(
      _fileId, objectName.c_str(), &objectInfo, H5P_DEFAULT);
  if (err < 0)
      throw BlitzH5Error()
          << "Could not retrieve attributes of '" << objectName
          << "'. Object no found.";
  std::vector<std::string> attributes(objectInfo.num_attrs);
  for (hsize_t i = 0; i < objectInfo.num_attrs; ++i)
  {
    int attributeNameLength =
        H5Aget_name_by_idx(
            _fileId, objectName.c_str(), H5_INDEX_NAME, H5_ITER_INC,
            i, NULL, 0, H5P_DEFAULT);
    char *buf = new char[attributeNameLength + 1];
    H5Aget_name_by_idx(
        _fileId, objectName.c_str(), H5_INDEX_NAME, H5_ITER_INC,
        i, buf, attributeNameLength + 1, H5P_DEFAULT);
    attributes[i] = buf;
    delete[] buf;
  }
  return attributes;
}

bool BlitzH5File::existsGroup(std::string const &groupName) const
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not check existence of group '" << groupName
          << "'. The BlitzH5File is not open.";
  hid_t groupId = H5Gopen2(_fileId, groupName.c_str(), H5P_DEFAULT);
  if (groupId < 0) return false;
  H5Gclose(groupId);
  return true;
}

void BlitzH5File::createGroup(std::string const &groupName)
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not create group '" << groupName
          << "'. The BlitzH5File is not open.";
  if (_mode == ReadOnly)
      throw BlitzH5Error()
          << "Could not create group '" << groupName
          << "'. File is opened ReadOnly.";
  if (existsGroup(groupName)) return;
  
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t groupId = H5Gcreate2(
      _fileId, groupName.c_str(), linkCreationPropertiesId, H5P_DEFAULT,
      H5P_DEFAULT);
  H5Pclose(linkCreationPropertiesId);
  if (groupId < 0)
      throw BlitzH5Error() << "Could not create group '" << groupName << "'.";
  H5Gclose(groupId);
}

void BlitzH5File::deleteGroup(std::string const &groupName)
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not delete group '" << groupName
          << "'. The BlitzH5File is not open.";
  if (_mode == ReadOnly)
      throw BlitzH5Error()
          << "Could not delete group '" << groupName
          << "'. File is opened ReadOnly.";
  if (!existsGroup(groupName))
      throw BlitzH5Error()
          << "Could not delete group '" << groupName
          << "'. Group not found.";
          
  std::vector<std::string> objectsToDelete(getObjects(groupName));
  while (objectsToDelete.size() > 0)
  {
    if (existsDataset(groupName + "/" + objectsToDelete.back()))
        deleteDataset(groupName + "/" + objectsToDelete.back());
    else deleteGroup(groupName + "/" + objectsToDelete.back());
    objectsToDelete.pop_back();
  }
  herr_t err = H5Ldelete(_fileId, groupName.c_str(), H5P_DEFAULT);
  if (err < 0)
      throw BlitzH5Error()
          << "Could not delete group '" << groupName
          << "'. H5Ldelete failed.";
}

void BlitzH5File::copyObject(
    std::string const &objectName, BlitzH5File &outFile, int compression)
{
  if (_fileId < 0)
      throw BlitzH5Error()
          << "Could not copy object '" << objectName
          << "'. The BlitzH5File is not open.";

  if (existsGroup(objectName))
  {
    // Create group if it does not already exist
    if (!outFile.existsGroup(objectName)) outFile.createGroup(objectName);

    // Recursively call copy object for sub-groups and datasets
    std::vector<std::string> subObjects(getObjects(objectName));
    for(size_t i = 0; i < subObjects.size(); ++i)
        copyObject(objectName + "/" + subObjects[i], outFile, compression);
  }
  else if (existsDataset(objectName))
  {
    // Copy dataset
    if(outFile.existsDataset(objectName))
        throw BlitzH5Error()
            << "Dataset '" << objectName << "' already exists. Copy aborted.";
    hid_t datasetId = H5Dopen2(_fileId, objectName.c_str(), H5P_DEFAULT);
    if (datasetId < 0)
        throw BlitzH5Error()
            << "Could not copy '" << objectName << "'. Could not open dataset.";
    hid_t datatypeId = H5Dget_type(datasetId);
    if (datatypeId < 0)
    {
      H5Dclose(datasetId);
      throw BlitzH5Error()
          << "Could not copy '" << objectName << "'. Could not get datatype.";
    }
    hid_t dataspaceId = H5Dget_space(datasetId);
    if (dataspaceId < 0)
    {
      H5Tclose(datatypeId);
      H5Dclose(datasetId);
      throw BlitzH5Error()
          << "Could not copy '" << objectName << "'. Could not get dataspace.";
    }
    int nDims = H5Sget_simple_extent_ndims(dataspaceId);
    std::vector<hsize_t> dims(nDims);
    std::vector<hsize_t> maxDims(nDims);
    H5Sget_simple_extent_dims(dataspaceId, &dims[0], &maxDims[0]);
    hsize_t bufSize = H5Tget_size(datatypeId);
    for(int i = 0; i < nDims; i++) bufSize *= dims[i];
    char* buf = new char[bufSize];
    herr_t err = H5Dread(
        datasetId, datatypeId, H5S_ALL, dataspaceId, H5P_DEFAULT, buf);
    if(err < 0)
    {
      delete[] buf;
      H5Sclose(dataspaceId);
      H5Tclose(datatypeId);
      H5Dclose(datasetId);
      throw BlitzH5Error()
          << "Could not copy '" << objectName << "'. Could not read dataset.";
    }
    
    hid_t targetDataspaceId = H5Scopy(dataspaceId);
    if (targetDataspaceId < 0)
    {
      delete[] buf;
      H5Sclose(dataspaceId);
      H5Tclose(datatypeId);
      H5Dclose(datasetId);
      throw BlitzH5Error()
          << "Could not copy '" << objectName << "'. Could not copy dataspace.";
    }
    
    hid_t targetDatasetCreationPropertiesId = -1;
    if (compression == -1)
    {
      targetDatasetCreationPropertiesId = H5Dget_create_plist(datasetId);
      if (targetDatasetCreationPropertiesId < 0)
      {
        H5Sclose(targetDataspaceId);
        delete[] buf;
        H5Sclose(dataspaceId);
        H5Tclose(datatypeId);
        H5Dclose(datasetId);
        throw BlitzH5Error()
            << "Could not copy '" << objectName
            << "'. Could not copy dataset creation properties.";
      }
    }
    else
    {
      targetDatasetCreationPropertiesId = H5Pcreate(H5P_DATASET_CREATE);
      if (targetDatasetCreationPropertiesId < 0)
      {
        H5Sclose(targetDataspaceId);
        delete[] buf;
        H5Sclose(dataspaceId);
        H5Tclose(datatypeId);
        H5Dclose(datasetId);
        throw BlitzH5Error()
            << "Could not copy '" << objectName
            << "'. Could not setup dataset creation properties.";
      }
      std::vector<hsize_t> chunkShape(nDims);
      for (int d = 0; d < nDims; ++d)
          if (d < nDims - 2) chunkShape[d] = 1;
          else chunkShape[d] = dims[d];
      herr_t err = H5Pset_chunk(
          targetDatasetCreationPropertiesId, nDims, &chunkShape[0]);
      if (err < 0)
      {
        H5Pclose(targetDatasetCreationPropertiesId);
        H5Sclose(targetDataspaceId);
        delete[] buf;
        H5Sclose(dataspaceId);
        H5Tclose(datatypeId);
        H5Dclose(datasetId);
        throw BlitzH5Error()
            << "Could not copy '" << objectName
            << "'. Could not setup chunking.";
      }
      err = H5Pset_deflate(targetDatasetCreationPropertiesId, compression);
      if (err < 0)
      {
        H5Pclose(targetDatasetCreationPropertiesId);
        H5Sclose(targetDataspaceId);
        delete[] buf;
        H5Sclose(dataspaceId);
        H5Tclose(datatypeId);
        H5Dclose(datasetId);
        throw BlitzH5Error()
            << "Could not copy '" << objectName
            << "'. Could not setup compression.";
      }
    }

    hid_t targetLinkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
    err = H5Pset_create_intermediate_group(targetLinkCreationPropertiesId, 1);
    if (err < 0)
    {
      H5Pclose(targetDatasetCreationPropertiesId);
      H5Sclose(targetDataspaceId);
      delete[] buf;
      H5Sclose(dataspaceId);
      H5Tclose(datatypeId);
      H5Dclose(datasetId);
      throw BlitzH5Error()
          << "Could not copy '" << objectName
          << "'. Could not set intermediate group creation flag.";
    }

    hid_t targetDatasetId = H5Dcreate2(
        outFile.id(), objectName.c_str(), datatypeId, targetDataspaceId,
        targetLinkCreationPropertiesId, targetDatasetCreationPropertiesId,
        H5P_DEFAULT);
    if (targetDatasetId < 0)
    {
      H5Pclose(targetLinkCreationPropertiesId);
      H5Pclose(targetDatasetCreationPropertiesId);
      H5Sclose(targetDataspaceId);
      delete[] buf;
      H5Sclose(dataspaceId);
      H5Tclose(datatypeId);
      H5Dclose(datasetId);
      throw BlitzH5Error()
          << "Could not copy '" << objectName << "'. Could not create dataset.";
    }
    err = H5Dwrite(
        targetDatasetId, datatypeId, H5S_ALL, targetDataspaceId, H5P_DEFAULT,
        buf);
    H5Dclose(targetDatasetId);
    H5Pclose(targetLinkCreationPropertiesId);
    H5Pclose(targetDatasetCreationPropertiesId);
    H5Sclose(targetDataspaceId);
    delete[] buf;
    H5Sclose(dataspaceId);
    H5Tclose(datatypeId);
    H5Dclose(datasetId);
    if (err < 0)
        throw BlitzH5Error()
            << "Could not copy '" << objectName
            << "'. Could not write dataset.";
  }
  else
  {
    throw BlitzH5Error()
        << "Could not copy '" << objectName << "'. No such group/dataset.";
  }
  
  // copy attributes of this object
  std::vector<std::string> attributes(getAttributes(objectName));
  for (size_t i = 0; i < attributes.size(); ++i)
  {
    if (outFile.existsAttribute(attributes[i], objectName))
        throw BlitzH5Error()
            << "Attribute '" << objectName << ":" << attributes[i]
            << "' already exists. Copy aborted.";

    // Open source attribute
    hid_t attributeId =
        H5Aopen_by_name(
            _fileId, objectName.c_str(), attributes[i].c_str(),
            H5P_DEFAULT, H5P_DEFAULT);
    if (attributeId < 0)
        throw BlitzH5Error()
            << "Could not read attribute '" << objectName << ":"
            << attributes[i] << "'.";
    
    // Open target object
    hid_t objectId = H5Oopen(outFile.id(), objectName.c_str(), H5P_DEFAULT);
    if (objectId < 0)
    {
      H5Aclose(attributeId);
      throw BlitzH5Error() 
          << "Could not open object '" << objectName << "' in output file.";
    }

    // Copy
    try
    {
      _copyAttribute(attributeId, objectId);
    }
    catch (BlitzH5Error &e)
    {
      H5Oclose(objectId);
      H5Aclose(attributeId);
      throw BlitzH5Error()
          << "Could not read attribute '" << objectName << ":"
          << attributes[i] << "': " << e.what();
    }
    H5Oclose(objectId);
    H5Aclose(attributeId);
  }
}

void BlitzH5File::_copyAttribute(hid_t attributeId, hid_t targetId)
{
  std::string name;
  ssize_t nameLength = H5Aget_name(attributeId, 0, NULL);
  char *nameBuf = new char[nameLength + 1];
  H5Aget_name(attributeId, nameLength + 1, nameBuf);
  name = nameBuf;
  delete[] nameBuf;

  hid_t datatypeId = H5Aget_type(attributeId);
  if (datatypeId < 0) throw BlitzH5Error() << "Could not get datatype.";
  hid_t dataspaceId = H5Aget_space(attributeId);
  if (dataspaceId < 0)
  {
    H5Tclose(datatypeId);
    throw BlitzH5Error() 
        << "Could not get dataspace for attribute '" << name << "'.";
  }
  int attNDims = H5Sget_simple_extent_ndims(dataspaceId);
  std::vector<hsize_t> attDims(attNDims);
  std::vector<hsize_t> attMaxDims(attNDims);
  H5Sget_simple_extent_dims(dataspaceId, &attDims[0], &attMaxDims[0]);
  hsize_t attBufSize = H5Tget_size(datatypeId);
  for (int i = 0; i < attNDims; ++i) attBufSize *= attDims[i];
  char *buf = new char[attBufSize];
  herr_t err = H5Aread(attributeId, datatypeId, buf);
  if (err < 0)
  {
    delete[] buf;
    H5Sclose(dataspaceId);
    H5Tclose(datatypeId);
    throw BlitzH5Error()
        << "Could not copy attribute data for attribute '" << name
        << "' to processing buffer.";
  }
  
  hid_t newDataspaceId = H5Scopy(dataspaceId);
  if (newDataspaceId < 0)
  {
    delete[] buf;
    H5Sclose(dataspaceId);
    H5Tclose(datatypeId);
    throw BlitzH5Error() 
        << "Could not create target dataspace for attribute '" << name << "'.";
  }
  hid_t newAttributeId = H5Acreate2(
      targetId, name.c_str(), datatypeId, newDataspaceId, H5P_DEFAULT,
      H5P_DEFAULT);
  if (newAttributeId < 0)
  {
    delete[] buf;
    H5Sclose(newDataspaceId);
    H5Sclose(dataspaceId);
    H5Tclose(datatypeId);
    throw BlitzH5Error() << "Could not create attribute '" << name << "'.";
  }
  err = H5Awrite(newAttributeId, datatypeId, buf);
  delete[] buf;
  H5Aclose(newAttributeId);
  H5Sclose(newDataspaceId);
  H5Sclose(dataspaceId);
  H5Tclose(datatypeId);
  if (err < 0)
      throw BlitzH5Error() << "Could not write attribute '" << name << "'.";
}

