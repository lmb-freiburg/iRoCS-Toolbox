/**************************************************************************
 **       Title: Lightweight blitz to HDF5 file reader/writer
 **    $RCSfile$
 **   $Revision: $ $Name$
 **       $Date: $
 **   Copyright: GPL $Author: $
 ** Description:
 **
 ** Lightweight alternative to libBlitzHDF5 providing its basic functionality.
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file BlitzHdf5Light.hh
 *  \brief Lightweight alternative to libBlitzHDF5 providing its basic
 *    functionality.
 */
/*======================================================================*/

#ifndef BLITZHDF5LIGHT_HH
#define BLITZHDF5LIGHT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// general includes
#include <cstring>
#include <sstream>
#include <fstream>

// For modification time stamps of datasets
#include <ctime>

// Blitz++
#include <blitz/array.h>

// HDF5 includes
#include <hdf5.h>

#include <libProgressReporter/ProgressReporter.hh>

#include "BlitzH5Traits.hh"

/*======================================================================*/
/*!
 *  \class BlitzH5Error BlitzHdf5Light.hh <BlitzHdf5Light.hh>
 *  \brief The main error class for the BlitzHDF5 interface
 */
/*======================================================================*/
class BlitzH5Error : public std::exception
{
    
public:
    
  /*======================================================================*/
  /*! 
   *   Default constructor. Creates an object without error message.
   *
   *   \sa BlitzH5Error::operator<<
   */
  /*======================================================================*/
  BlitzH5Error();
    
  /*======================================================================*/
  /*! 
   *   Creates an error object with the given message.
   *
   *   \overload
   *
   *   \param message error message
   */
  /*======================================================================*/
  BlitzH5Error( const std::string& message);

  /*======================================================================*/
  /*! 
   *  Default destructor
   */
  /*======================================================================*/
  ~BlitzH5Error() throw();
  
  /*======================================================================*/
  /*! 
   *   Appends the given data to the message string.
   *
   *   \param data
   */
  /*======================================================================*/
  template<typename DataT>
  BlitzH5Error &operator<<(DataT const &data);

  /*======================================================================*/
  /*! 
   *   Returns the error message.
   *
   *   \return error message
   */
  /*======================================================================*/
  std::string const &str() const;

  /*======================================================================*/
  /*! 
   *   Returns the error message as c-string.
   *
   *   \return error message
   */
  /*======================================================================*/
  char const *what() const throw();
  
private:

  std::string _message;
  
};

class BlitzH5File
{

public:
    
  enum FileMode { ReadOnly, Write, Replace, New, WriteOrNew };

  /*======================================================================*/
  /*!
   *   Creates a stub BlitzH5File object.
   */
  /*======================================================================*/
  BlitzH5File();

  /*======================================================================*/
  /*!
   *   Opens/Creates an HDF5File, with the given file name. When creating a
   *   new file, the mode will be automatically set to ReadWrite disregarding
   *   the given mode. When accessing an existing file it will be opened
   *   ReadOnly by default.
   *
   *   \overload
   *
   *   \param fileName  file to be opened
   *   \param mode      file mode to be used (default: \c ReadOnly),
   *
   *   \see BlitzH5File::FileMode
   *
   *   \exception BlitzH5Error
   */
  /*======================================================================*/
  BlitzH5File(std::string const &fileName, FileMode mode = ReadOnly);

  /*-----------------------------------------------------------------------
   *  Destructor
   *-----------------------------------------------------------------------*/
  ~BlitzH5File();

  /*======================================================================*/
  /*! 
   *   Get HDF5 id for this file
   *
   *   \exception BlitzH5Error
   *
   *   \return The raw HDF5 file ID for direct access to the file using the
   *     HDF5 C interface.
   */
  /*======================================================================*/ 
  hid_t id()
        {
          if (_fileId < 0)
              throw BlitzH5Error() << "Invalid file id. The file is not open.";
          return _fileId;
        }

  /*======================================================================*/
  /*! 
   *   Get the file name.
   *
   *   \return The name of this hdf5 file
   */
  /*======================================================================*/
  std::string const &name() const
        {
          return _fileName;
        }

  /*======================================================================*/
  /*! 
   *   Get a normalized version of the given h5 object path string.
   *   Extra '/' and './' will be removed. The resulting string is guaranteed
   *   to be the simplest possible equivalent version of the group string.
   *   The returned string always ends with a non-slash character. The input
   *   string may not contain any occurrence of '../'
   *
   *   e.g. the input bla/./blub//bleablub produces bla/blub/bleablub
   *
   *   \param group The group string to simplify
   *
   *   \return The simplified group descriptor
   */
  /*======================================================================*/
  static std::string simplifyGroupDescriptor(std::string const& group);

  /*======================================================================*/
  /*!
   *   Checks if there is a dataset with the specified name.
   *
   *   \param name dataset path descriptor
   *
   *   \return \c true if the dataset exists, \c false otherwise
   */
  /*======================================================================*/
  bool existsDataset(std::string const &name) const;

  /*======================================================================*/
  /*!
   *   Returns the dimension extents of a dataset. For scalar datasets a zero
   *   length vector is returned!
   *
   *   \param name dataset path descriptor
   *
   *   \return The dataset shape as vector of extents.
   */
  /*=======================================================================*/
  std::vector<hsize_t> getDatasetShape(std::string const &name) const;

  /*======================================================================*/
  /*!
   *   Returns the absolute paths of all datasets below the given group
   *
   *   \param startGroup The names of all datasets in and below this group
   *     are returned
   *
   *   \return A vector containing the absolute dataset paths
   */
  /*=======================================================================*/
  std::vector<std::string>
  allDatasets(std::string const &startGroup = "/") const;
  
  /*======================================================================*/
  /*!
   *   Get the HDF5 type of the dataset. The returned hid_t must be released
   *   after usage with H5Tclose().
   *
   *   \param name dataset path descriptor
   *
   *   \return An hid_t corresponding to the HDF5 data type of the dataset
   */
  /*=======================================================================*/
  hid_t getDatasetType(std::string const &name) const;

  /*======================================================================*/
  /*!
   *   Reads a simple, fixed size numeric data set. This can be a simple
   *   scalar, a TinyVector or a TinyMatrix. Complex should also work but is
   *   not tested.
   *
   *   \param data  output variable
   *   \param name  dataset path descriptor
   *
   *   \exception BlitzH5Error If the dataset can not be read this error is
   *     thrown
   *
   *   \sa existsDataset() to query the existence of a dataset before reading
   */
  /*======================================================================*/
  template<typename FixedNumericT>
  void readDataset(FixedNumericT &data, std::string const &name) const;

  /*======================================================================*/
  /*!
   *   Reads a simple, 1D vectorial numerical data set.
   *   The type is automatically converted to the vector type and the output
   *   vector will be resized to fit the dataset extents.
   *
   *   \param data  output vector
   *   \param name  dataset path descriptor
   *
   *   \exception BlitzH5Error If the dataset can not be read this error is
   *     thrown
   *
   *   \sa existsDataset() to query the existence of a dataset before reading
   */
  /*======================================================================*/
  template<typename DataT>
  void readDataset(
      std::vector<DataT> &data, std::string const &name) const;

  /*======================================================================*/
  /*!
   *   Reads a simple, multi dimensional data set.
   *   The type is automatically converted to the Array type and the output
   *   Array will be resized to fit the dataset extents.
   *
   *   \param data  output array
   *   \param name  dataset path descriptor
   *   \param pr    If given progress will be reported to this ProgressReporter
   *
   *   \exception BlitzH5Error If the dataset can not be read this error is
   *     thrown
   *
   *   \sa existsDataset() to query the existence of a dataset before reading
   */
  /*======================================================================*/
  template<typename DataT, int Rank>
  void readDataset(
      blitz::Array<DataT,Rank> &data, std::string const &name,
      iRoCS::ProgressReporter *pr = NULL) const;

  /*======================================================================*/
  /*!
   *   Reads a simple, string data set.
   *
   *   \param data  output string
   *   \param name  dataset path descriptor
   *   \param pr    If given progress will be reported to this PorgressReporter
   *
   *   \exception BlitzH5Error If the dataset can not be read this error is
   *     thrown
   *
   *   \sa existsDataset() to query the existence of a dataset before reading
   */
  /*======================================================================*/
  void readDataset(
      std::string &data, std::string const &name,
      iRoCS::ProgressReporter *pr = NULL) const;

  /*======================================================================*/
  /*!
   *   Writes a simple data set without chunking or compression. This should
   *   only be used for scalars or very small datasets.
   *
   *   \param data  variable to write
   *   \param name  dataset path descriptor
   *
   *   \exception BlitzH5Error If the dataset can not be read this error is
   *     thrown
   *
   *   \sa existsDataset() to query the existence of a dataset before reading
   */
  /*======================================================================*/
  template<typename DataT>
  void writeDataset(DataT const &data, std::string const &name);

  /*======================================================================*/
  /*!
   *   Writes a simple, multi dimensional data set, creating the data
   *   set if needed. The datatype in the file is chosen according to
   *   the datatype of the blitz array. If a dataset with the given name
   *   already exists with different dimensionality, extents or data type
   *   it will be replaced.
   *
   *   \param data        array to be written into file
   *   \param name        dataset path descriptor
   *   \param compression level of compression = 0..9,
   *                      default = 1 (low compression)
   *   \param pr    If given progress will be reported to this PorgressReporter
   *
   *   \exception BlitzH5Error If the dataset can not be written this error
   *     is thrown
   *
   *   \sa existsDataset() to query the existence of a dataset before writing
   */
  /*======================================================================*/
  template<typename DataT, int Rank>
  void writeDataset(
      blitz::Array<DataT,Rank> const &data, std::string const &name,
      int compression = 1, iRoCS::ProgressReporter *pr = NULL);

  /*======================================================================*/
  /*!
   *   Writes a simple, string data set, creating the data
   *   set if needed. If a dataset with the given name
   *   already exists with different dimensionality, extents or data type
   *   it will be replaced.
   *
   *   \param data        string to be written into file
   *   \param name        dataset path descriptor
   *   \param pr    If given progress will be reported to this PorgressReporter
   *
   *   \exception BlitzH5Error If the dataset can not be written this error
   *     is thrown
   *
   *   \sa existsDataset() to query the existence of a dataset before writing
   */
  /*======================================================================*/
  void writeDataset(
      std::string const &data, std::string const &name,
      iRoCS::ProgressReporter *pr = NULL);

  /*======================================================================*/
  /*!
   *   Avoid using this function! If you really need to use it, your
   *   file may become severely fragmented. Use some external tool to
   *   copy the contents if the file gets too bloated.
   *
   *   Remove association between dataset descriptor and dataset data.
   *   This will not actually release the file space used by the dataset
   *   but will mark the space as reusable until the file is closed.
   *   After closing the file free space information is lost!
   *
   *   \param name  dataset path descriptor
   *
   *   \exception BlitzH5Error If the dataset can not be deleted this error
   *     is thrown
   */
  /*======================================================================*/
  void deleteDataset(std::string const &name);

  /*======================================================================*/
  /*!
   *   Checks if there is an attribute with the specified name in the
   *   specified group.
   *
   *   \param attName     name of the attribute
   *   \param objectName  path to parent dataset or group
   *
   *   \return \c true if existing, \c false otherwise
   */
  /*======================================================================*/
  bool existsAttribute(
      std::string const &attName, std::string const &objectName = "/") const;

  /*======================================================================*/
  /*!
   *   Retrieve the shape of an attribute. For scalars or C-Strings it will
   *   return 1, otherwise the extents of the (multidimensional) attribute
   *
   *   \param attName     name of the attribute
   *   \param objectName  path to parent dataset or group
   *
   *   \return The attribute shape as vector of extents
   */
  /*======================================================================*/
  std::vector<hsize_t> getAttributeShape(
      std::string const &attName, std::string const &objectName = "/") const;

  /*======================================================================*/
  /*!
   *   Get the HDF5 type of the attribute.
   *
   *   \param attName     name of the attribute
   *   \param objectName  path to parent dataset or group
   *
   *   \return An hid_t corresponding to the HDF5 data type of the dataset
   */
  /*=======================================================================*/
  hid_t getAttributeType(
      const std::string& attName, const std::string& objectName = "/") const;

  /*======================================================================*/
  /*!
   *   Reads an attribute.
   *
   *   \param data   variable to store the attribute data to
   *   \param attName     name of the attribute
   *   \param objectName  path to parent dataset or group
   */
  /*======================================================================*/
  template<typename DataT>
  void readAttribute(
      DataT &data, std::string const &attName,
      std::string const &objectName = "/") const;

  /*======================================================================*/
  /*!
   *   Reads an attribute.
   *
   *   \param data   variable to store the attribute data to
   *   \param attName     name of the attribute
   *   \param objectName  path to parent dataset or group
   */
  /*======================================================================*/
  template<typename DataT, int Dim>
  void readAttribute(
      blitz::TinyVector<DataT,Dim> &data, std::string const &attName,
      std::string const &objectName = "/") const;

  /*======================================================================*/
  /*!
   *   Reads an attribute.
   *
   *   \param data   variable to store the attribute data to
   *   \param attName     name of the attribute
   *   \param objectName  path to parent dataset or group
   */
  /*======================================================================*/
  template<typename DataT, int NRows, int NCols>
  void readAttribute(
      blitz::TinyMatrix<DataT,NRows,NCols> &data, std::string const &attName,
      std::string const &objectName = "/") const;

  /*======================================================================*/
  /*!
   *   Reads an attribute.
   *
   *   \param data   variable to store the attribute data to
   *   \param attName     name of the attribute
   *   \param objectName  path to parent dataset or group
   */
  /*======================================================================*/
  template<typename DataT>
  void readAttribute(
      std::vector<DataT> &data, std::string const &attName,
      std::string const &objectName = "/") const;

  /*======================================================================*/
  /*!
   *   Reads a string attribute.
   *
   *   \param data         variable to store the attribute data to
   *   \param attName     name of the attribute
   *   \param objectName  path to parent dataset or group
   */
  /*======================================================================*/
  void readAttribute(
      std::string &data, std::string const &attName,
      std::string const &objectName = "/") const;

  /*======================================================================*/
  /*!
   *   Writes an attribute.
   *
   *   \param in          variable to be written to the file
   *   \param attName     name of the attribute
   *   \param objectName  path to parent dataset or group
   */
  /*======================================================================*/
  template<typename DataT>
  void writeAttribute(
      DataT const &in, std::string const &attName,
      std::string const &objectName = "/");

  /*======================================================================*/
  /*!
   *   Writes a string attribute.
   *
   *   \param in          variable to be written to the file
   *   \param attName     name of the attribute
   *   \param objectName  path to parent dataset or group
   */
  /*======================================================================*/
  void writeAttribute(
      std::string const &in, std::string const &attName,
      std::string const &objectName = "/");

  /*======================================================================*/
  /*!
   *   Deletes an attribute.
   *
   *   \param attName     name of the attribute
   *   \param objectName  path to parent dataset or group
   */
  /*======================================================================*/
  void deleteAttribute(
      std::string const &attName, std::string const &objectName = "/");

  /*======================================================================*/
  /*!
   *   Gets the list of names of all objects in a group. Objects are
   *   datasets and groups.
   *
   *   \param groupName  group name to be searched in; default is root '/'
   *
   *   \return The names of the objects in a std::vector
   */
  /*======================================================================*/
  std::vector<std::string> getObjects(std::string const &groupName = "/") const;

  /*======================================================================*/
  /*!
   *   Gets the list of all subgroups in the given group.
   *
   *   \param groupName  group name to be searched in; default is root '/'
   *
   *   \return The names of the subgroups in a std::vector
   */
  /*======================================================================*/
  std::vector<std::string> getGroups(std::string const &groupName = "/") const;

  /*======================================================================*/
  /*!
   *   Get all attributes of a group or dataset.
   *
   *   \param objectName path to group or dataset, if not specified, the
   *     global root attibutes are returned
   *
   *   \return The names of the attributes in a std::vector
   */
  /*=======================================================================*/
  std::vector<std::string> getAttributes(std::string const &objectName = "/")
      const;

  /*======================================================================*/
  /*!
   *   Checks if there is a group with the specified name.
   *
   *   \param groupName  absolute group path
   *
   *   \return \c true if group exists, \c false otherwise
   */
  /*======================================================================*/
  bool existsGroup(std::string const &groupName) const;

  /*======================================================================*/
  /*!
   *   Create a group with specified name.
   *
   *   \param groupName  absolute group path
   *
   *   \exception BlitzH5Error
   */
  /*======================================================================*/
  void createGroup(std::string const &groupName);

  /*======================================================================*/
  /*!
   *   Delete the group specified by groupName. All Objects within and
   *   below the group are recursively deleted.
   *
   *   \param groupName absolute group path
   */
  /*======================================================================*/
  void deleteGroup(std::string const &groupName);

  /*======================================================================*/
  /*!
   *   Recursively copy the given object to the given BlitzH5File. Object can
   *   be a dataset or a group.
   *
   *   \param objectName Name of the dataset or group
   *   \param outFile A reference to the output BlitzH5File
   *   \param compression Compression for the destination. Values in [0..9]
   *                      change the compression. Value -1 (default)
   *                      preserves the compression setting found in the
   *                      original dataset.
   */
  /*=======================================================================*/
  void copyObject(
      std::string const &objectName, BlitzH5File &outFile,
      int compression = -1);
  
private:

  void _copyAttribute(hid_t attributeId, hid_t targetId);

  template<typename SourceT, typename DestT>
  void _loadAttribute(hid_t attributeId, DestT *target) const;

  template<typename SourceT, typename DestT>
  void _loadChunkedDataset(
      hid_t datasetId, std::vector<hsize_t> const &datasetDims,
      std::vector<hsize_t> const &chunkDims, DestT *target,
      iRoCS::ProgressReporter *pr = NULL) const;

  template<typename SourceT, typename DestT>
  void _loadContiguousDataset(hid_t datasetId, DestT *target) const;

  std::string _fileName;
  hid_t _fileId;
  FileMode _mode;

};

#include "BlitzHdf5Light.icc"

#endif
