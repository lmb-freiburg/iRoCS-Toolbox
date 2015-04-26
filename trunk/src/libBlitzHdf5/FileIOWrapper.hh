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

#ifndef FILEIOWRAPPER_HH
#define FILEIOWRAPPER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <iostream>
#include <fstream>
#include "BlitzHdf5Light.hh"

#include <sstream>

#include <blitz/array.h>

#include <libBlitzAnalyze/BlitzAnalyzeFile.hh>

// HDF5 Reader and Writer
namespace HDF5IOWrapper
{

/*======================================================================*/
/*! 
 *   Read a scalar n-dimensional dataset with element size.
 *
 *   \param data        The blitz::Array to read the dataset to
 *   \param elSize      The blitz::TinyVector to read the element size to
 *   \param fileName    The name of the hdf5 file
 *   \param dsName      The full path of the hdf5 dataset
 *   \param throwErrors If given, BlitzH5Error will be thrown on failure
 *     otherwise an error message is written to std::cout and the program
 *     terminated.
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *
 *   \exception BlitzH5Error If throwErrors is true
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int Dim>
  void readDataset(
      blitz::Array<DataT,Dim> &data, blitz::TinyVector<ElSizeT,Dim> &elSize,
      std::string const &fileName, std::string const &dsName,
      bool throwErrors = false, iRoCS::ProgressReporter *pr = NULL);
  
/*======================================================================*/
/*! 
 *   Read a scalar n-dimensional dataset with element size.
 *
 *   \param data        The blitz::Array to read the dataset to
 *   \param elSize      The blitz::TinyVector to read the element size to
 *   \param inFile      The BlitzH5File containing the dataset to read
 *   \param dsName      The full path of the hdf5 dataset
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *
 *   \exception BlitzH5Error If the dataset cannot be read for any reason
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int Dim>
  void readDataset(
      blitz::Array<DataT,Dim> &data, blitz::TinyVector<ElSizeT,Dim> &elSize,
      BlitzH5File const &inFile, std::string const &dsName,
      iRoCS::ProgressReporter *pr = NULL);
  
/*======================================================================*/
/*! 
 *   Read a scalar n-dimensional timelapse dataset with element size.
 *
 *   \param data        The blitz::Array to read the dataset to
 *   \param elSize      The blitz::TinyVector to read the element size to
 *   \param fileName    The name of the hdf5 file
 *   \param dsName      The full path of the hdf5 dataset
 *   \param throwErrors If given, BlitzH5Error will be thrown on failure
 *     otherwise an error message is written to std::cout and the program
 *     terminated.
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *
 *   \exception BlitzH5Error If throwErrors is true
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int Dim>
  void readDataset(
      blitz::Array<DataT,Dim> &data, blitz::TinyVector<ElSizeT,Dim-1> &elSize,
      std::string const &fileName, std::string const &dsName,
      bool throwErrors = false, iRoCS::ProgressReporter *pr = NULL);
  
/*======================================================================*/
/*! 
 *   Read a scalar n-dimensional timelapse dataset with element size.
 *
 *   \param data        The blitz::Array to read the dataset to
 *   \param elSize      The blitz::TinyVector to read the element size to
 *   \param inFile      The BlitzH5File containing the dataset to read
 *   \param dsName      The full path of the hdf5 dataset
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *
 *   \exception BlitzH5Error If the dataset cannot be read for any reason
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int Dim>
  void readDataset(
      blitz::Array<DataT,Dim> &data, blitz::TinyVector<ElSizeT,Dim-1> &elSize,
      BlitzH5File const &inFile, std::string const &dsName,
      iRoCS::ProgressReporter *pr = NULL);
  
/*======================================================================*/
/*! 
 *   Write a vectorial n-dimensional dataset with element size and additional
 *   meta-information for dataset interpretation.
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector"
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param fileName    The name of the hdf5 file
 *   \param dsName      The full path of the hdf5 dataset
 *   \param displayMin  The value to use as minimum value in visualizations
 *   \param displayMax  The value to use as maximum value in visualizations
 *   \param throwErrors If given, BlitzH5Error will be thrown on failure
 *     otherwise an error message is written to std::cout and the program
 *     terminated.
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "[z][y]xc" will be generated.
 *
 *   \exception BlitzH5Error If throwErrors is true
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      blitz::Array<blitz::TinyVector<DataT,InnerDim>,OuterDim> const &data,
      blitz::TinyVector<ElSizeT,OuterDim> const &elSize,
      std::string const &fileName, std::string const &dsName,
      DataT displayMin, DataT displayMax, bool throwErrors = false,
      int compression = 0, iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");
  
/*======================================================================*/
/*! 
 *   Write a vectorial n-dimensional dataset with element size and additional
 *   meta-information for dataset interpretation.
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector"
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param outFile     The hdf5 file to write the dataset to
 *   \param dsName      The full path of the hdf5 dataset
 *   \param displayMin  The value to use as minimum value in visualizations
 *   \param displayMax  The value to use as maximum value in visualizations
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "[z][y]xc" will be generated.
 *
 *   \exception BlitzH5Error If the dataset cannot be writen for any reason
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      blitz::Array<blitz::TinyVector<DataT,InnerDim>,OuterDim> const &data,
      blitz::TinyVector<ElSizeT,OuterDim> const &elSize,
      BlitzH5File &outFile, std::string const &dsName,
      DataT displayMin, DataT displayMax,
      int compression = 0, iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");
  
/*======================================================================*/
/*! 
 *   Write a vectorial n-dimensional 8Bit unsigned integer dataset with
 *   element size and additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector", display_min := 0, display_max := 255
 *
 *   \remark This function is a convenience function calling the general version
 *     for n-D vectorial datasets of arbitrary datatype.
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param fileName    The name of the hdf5 file
 *   \param dsName      The full path of the hdf5 dataset
 *   \param throwErrors If given, BlitzH5Error will be thrown on failure
 *     otherwise an error message is written to std::cout and the program
 *     terminated.
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "[z][y]xc" will be generated.
 *
 *   \exception BlitzH5Error If throwErrors is true
 */
/*======================================================================*/
  template<typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      blitz::Array<blitz::TinyVector<unsigned char,InnerDim>,OuterDim> const &
      data, blitz::TinyVector<ElSizeT,OuterDim>const &elSize,
      std::string const &fileName, std::string const &dsName,
      bool throwErrors = false, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");
  
/*======================================================================*/
/*! 
 *   Write a vectorial n-dimensional 8Bit unsigned integer dataset with
 *   element size and additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector", display_min := 0, display_max := 255
 *
 *   \remark This function is a convenience function calling the general version
 *     for n-D vectorial datasets of arbitrary datatype.
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param outFile     The hdf5 file to write the dataset to
 *   \param dsName      The full path of the hdf5 dataset
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "[z][y]xc" will be generated.
 *
 *   \exception BlitzH5Error If the dataset cannot be written for any reason
 */
/*======================================================================*/
  template<typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      blitz::Array<blitz::TinyVector<unsigned char,InnerDim>,OuterDim> const &
      data, blitz::TinyVector<ElSizeT,OuterDim>const &elSize,
      BlitzH5File &outFile, std::string const &dsName, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");

/*======================================================================*/
/*! 
 *   Write a vectorial n-dimensional dataset with element size and additional
 *   meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector", display_min := min(0, min(data)),
 *     display_max := max(data)
 *
 *   \remark This function is a convenience function calling the general version
 *     for n-D vectorial datasets of arbitrary datatype.
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param fileName    The name of the hdf5 file
 *   \param dsName      The full path of the hdf5 dataset
 *   \param throwErrors If given, BlitzH5Error will be thrown on failure
 *     otherwise an error message is written to std::cout and the program
 *     terminated.
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "[z][y]xc" will be generated.
 *
 *   \exception BlitzH5Error If throwErrors is true
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      blitz::Array<blitz::TinyVector<DataT,InnerDim>,OuterDim> const &data,
      blitz::TinyVector<ElSizeT,OuterDim> const &elSize,
      std::string const &fileName, std::string const &dsName,
      bool throwErrors = false, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");

/*======================================================================*/
/*! 
 *   Write a vectorial n-dimensional dataset with element size and additional
 *   meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector", display_min := min(0, min(data)),
 *     display_max := max(data)
 *
 *   \remark This function is a convenience function calling the general version
 *     for n-D vectorial datasets of arbitrary datatype.
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param outFile     The hdf5 file to write the dataset to
 *   \param dsName      The full path of the hdf5 dataset
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "[z][y]xc" will be generated.
 *
 *   \exception BlitzH5Error If the dataset cannot be written for any reason
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      blitz::Array<blitz::TinyVector<DataT,InnerDim>,OuterDim> const &data,
      blitz::TinyVector<ElSizeT,OuterDim> const &elSize,
      BlitzH5File &outFile, std::string const &dsName, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");

/*======================================================================*/
/*! 
 *   Write a vectorial n-dimensional timelapse dataset with element size and
 *   additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector"
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param fileName    The name of the hdf5 file
 *   \param dsName      The full path of the hdf5 dataset
 *   \param displayMin  The value to use as minimum value in visualizations
 *   \param displayMax  The value to use as maximum value in visualizations
 *   \param throwErrors If given, BlitzH5Error will be thrown on failure
 *     otherwise an error message is written to std::cout and the program
 *     terminated.
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "t[z][y]xc" will be generated.
 *
 *   \exception BlitzH5Error If throwErrors is true
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      blitz::Array<blitz::TinyVector<DataT,InnerDim>,OuterDim> const &data,
      blitz::TinyVector<ElSizeT,OuterDim-1> const &elSize,
      std::string const &fileName, std::string const &dsName,
      DataT displayMin, DataT displayMax, bool throwErrors = false,
      int compression = 0, iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");
      
/*======================================================================*/
/*! 
 *   Write a vectorial n-dimensional timelapse dataset with element size and
 *   additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector"
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param outFile     The hdf5 file to write the dataset to
 *   \param dsName      The full path of the hdf5 dataset
 *   \param displayMin  The value to use as minimum value in visualizations
 *   \param displayMax  The value to use as maximum value in visualizations
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "t[z][y]xc" will be generated.
 *
 *   \exception BlitzH5Error If the dataset cannot be written for any reason
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      blitz::Array<blitz::TinyVector<DataT,InnerDim>,OuterDim> const &data,
      blitz::TinyVector<ElSizeT,OuterDim-1> const &elSize,
      BlitzH5File &outFile, std::string const &dsName, DataT displayMin,
      DataT displayMax, int compression = 0, iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");
      
/*======================================================================*/
/*! 
 *   Write a vectorial n-dimensional timelapse 8Bit unsigned integer dataset
 *   with element size and additional meta-information for dataset
 *   interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector", display_min := 0, display_max := 255
 *
 *   \remark This function is a convenience function calling the general version
 *     for n-D vectorial timelapse datasets of arbitrary datatype.
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param fileName    The name of the hdf5 file
 *   \param dsName      The full path of the hdf5 dataset
 *   \param throwErrors If given, BlitzH5Error will be thrown on failure
 *     otherwise an error message is written to std::cout and the program
 *     terminated.
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "t[z][y]xc" will be generated.
 *
 *   \exception BlitzH5Error If throwErrors is true
 */
/*======================================================================*/
  template<typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      blitz::Array<blitz::TinyVector<unsigned char,InnerDim>,OuterDim> const &
      data, blitz::TinyVector<ElSizeT,OuterDim-1> const &elSize,
      std::string const &fileName, std::string const &dsName,
      bool throwErrors = false, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");

/*======================================================================*/
/*! 
 *   Write a vectorial n-dimensional timelapse 8Bit unsigned integer dataset
 *   with element size and additional meta-information for dataset
 *   interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector", display_min := 0, display_max := 255
 *
 *   \remark This function is a convenience function calling the general version
 *     for n-D vectorial timelapse datasets of arbitrary datatype.
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param outFile     The hdf5 file to write the dataset to
 *   \param dsName      The full path of the hdf5 dataset
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "t[z][y]xc" will be generated.
 *
 *   \exception BlitzH5Error If the dataset cannot be written for any reason
 */
/*======================================================================*/
  template<typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      blitz::Array<blitz::TinyVector<unsigned char,InnerDim>,OuterDim> const &
      data, blitz::TinyVector<ElSizeT,OuterDim-1> const &elSize,
      BlitzH5File &outFile, std::string const &dsName,
      int compression = 0, iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");

/*======================================================================*/
/*! 
 *   Write a vectorial n-dimensional timelapse dataset with element size and
 *   additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector", display_min := min(0, min(data)),
 *     display_max := max(data)
 *
 *   \remark This function is a convenience function calling the general version
 *     for n-D vectorial datasets of arbitrary datatype.
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param fileName    The name of the hdf5 file
 *   \param dsName      The full path of the hdf5 dataset
 *   \param throwErrors If given, BlitzH5Error will be thrown on failure
 *     otherwise an error message is written to std::cout and the program
 *     terminated.
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "t[z][y]xc" will be generated.
 *
 *   \exception BlitzH5Error If throwErrors is true
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      blitz::Array<blitz::TinyVector<DataT,InnerDim>,OuterDim> const &data,
      blitz::TinyVector<ElSizeT,OuterDim-1> const &elSize,
      std::string const &fileName, std::string const &dsName,
      bool throwErrors = false, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");

/*======================================================================*/
/*! 
 *   Write a vectorial n-dimensional timelapse dataset with element size and
 *   additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector", display_min := min(0, min(data)),
 *     display_max := max(data)
 *
 *   \remark This function is a convenience function calling the general version
 *     for n-D vectorial datasets of arbitrary datatype.
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param outFile     The hdf5 file to write the dataset to
 *   \param dsName      The full path of the hdf5 dataset
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "t[z][y]xc" will be generated.
 *
 *   \exception BlitzH5Error If the dataset cannot be written for any reason
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      blitz::Array<blitz::TinyVector<DataT,InnerDim>,OuterDim> const &data,
      blitz::TinyVector<ElSizeT,OuterDim-1> const &elSize,
      BlitzH5File &outFile, std::string const &dsName, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");

/*======================================================================*/
/*! 
 *   Write a scalar n-dimensional dataset with element size and
 *   additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "intensity"
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param fileName    The name of the hdf5 file
 *   \param dsName      The full path of the hdf5 dataset
 *   \param displayMin  The value to use as minimum value in visualizations
 *   \param displayMax  The value to use as maximum value in visualizations
 *   \param throwErrors If given, BlitzH5Error will be thrown on failure
 *     otherwise an error message is written to std::cout and the program
 *     terminated.
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "[z][y]x" will be generated.
 *
 *   \exception BlitzH5Error If throwErrors is true
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int Dim>
  void writeDataset(
      blitz::Array<DataT,Dim> const &data,
      blitz::TinyVector<ElSizeT,Dim> const &elSize, std::string const &fileName,
      std::string const &dsName, DataT displayMin, DataT displayMax,
      bool throwErrors = false, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");

/*======================================================================*/
/*! 
 *   Write a scalar n-dimensional dataset with element size and
 *   additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "intensity"
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param outFile     The hdf5 file to write the dataset to
 *   \param dsName      The full path of the hdf5 dataset
 *   \param displayMin  The value to use as minimum value in visualizations
 *   \param displayMax  The value to use as maximum value in visualizations
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "[z][y]x" will be generated.
 *
 *   \exception BlitzH5Error If the dataset cannot be written for any reason
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int Dim>
  void writeDataset(
      blitz::Array<DataT,Dim> const &data,
      blitz::TinyVector<ElSizeT,Dim> const &elSize, BlitzH5File &outFile,
      std::string const &dsName, DataT displayMin, DataT displayMax,
      int compression = 0, iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");
  
/*======================================================================*/
/*! 
 *   Write a scalar n-dimensional 8Bit unsigned integer dataset with
 *   element size and additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "intensity", display_min := 0, display_max := 255
 *
 *   \remark This function is a convenience function calling the general version
 *     for n-D datasets of arbitrary datatype.
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param fileName    The name of the hdf5 file
 *   \param dsName      The full path of the hdf5 dataset
 *   \param throwErrors If given, BlitzH5Error will be thrown on failure
 *     otherwise an error message is written to std::cout and the program
 *     terminated.
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "[z][y]x" will be generated.
 *
 *   \exception BlitzH5Error If throwErrors is true
 */
/*======================================================================*/
  template<typename ElSizeT, int Dim>
  void writeDataset(
      blitz::Array<unsigned char,Dim> const &data,
      blitz::TinyVector<ElSizeT,Dim> const &elSize, std::string const &fileName,
      std::string const &dsName, bool throwErrors = false, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");

/*======================================================================*/
/*! 
 *   Write a scalar n-dimensional 8Bit unsigned integer dataset with
 *   element size and additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "intensity", display_min := 0, display_max := 255
 *
 *   \remark This function is a convenience function calling the general version
 *     for n-D datasets of arbitrary datatype.
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param outFile     The hdf5 file to write the dataset to
 *   \param dsName      The full path of the hdf5 dataset
 *   \param throwErrors If given, BlitzH5Error will be thrown on failure
 *     otherwise an error message is written to std::cout and the program
 *     terminated.
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "[z][y]x" will be generated.
 *
 *   \exception BlitzH5Error If the dataset cannot be written for any reason
 */
/*======================================================================*/
  template<typename ElSizeT, int Dim>
  void writeDataset(
      blitz::Array<unsigned char,Dim> const &data,
      blitz::TinyVector<ElSizeT,Dim> const &elSize, BlitzH5File &outFile,
      std::string const &dsName, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");

/*======================================================================*/
/*! 
 *   Write a scalar n-dimensional dataset with element size and
 *   additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector", display_min := min(0, min(data)),
 *     display_max := max(data)
 *
 *   \remark This function is a convenience function calling the general version
 *     for n-D datasets of arbitrary datatype.
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param fileName    The name of the hdf5 file
 *   \param dsName      The full path of the hdf5 dataset
 *   \param throwErrors If given, BlitzH5Error will be thrown on failure
 *     otherwise an error message is written to std::cout and the program
 *     terminated.
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "[z][y]x" will be generated.
 *
 *   \exception BlitzH5Error If throwErrors is true
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int Dim>
  void writeDataset(
      blitz::Array<DataT,Dim> const & data,
      blitz::TinyVector<ElSizeT,Dim> const &elSize, std::string const &fileName,
      std::string const &dsName, bool throwErrors = false, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");

/*======================================================================*/
/*! 
 *   Write a scalar n-dimensional dataset with element size and
 *   additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector", display_min := min(0, min(data)),
 *     display_max := max(data)
 *
 *   \remark This function is a convenience function calling the general version
 *     for n-D datasets of arbitrary datatype.
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param outFile     The hdf5 file to write the dataset to
 *   \param dsName      The full path of the hdf5 dataset
 *   \param throwErrors If given, BlitzH5Error will be thrown on failure
 *     otherwise an error message is written to std::cout and the program
 *     terminated.
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "[z][y]x" will be generated.
 *
 *   \exception BlitzH5Error If the dataset cannot be written for any reason
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int Dim>
  void writeDataset(
      blitz::Array<DataT,Dim> const & data,
      blitz::TinyVector<ElSizeT,Dim> const &elSize, BlitzH5File &outFile,
      std::string const &dsName, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");

/*======================================================================*/
/*! 
 *   Write a complex n-dimensional dataset with element size and
 *   additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector",
 *     display_min := max(0, min(Re(data))) + i * max(0, min(Im(data))),
 *     display_max := max(Re(data)) + i * max(Im(data))
 *
 *   \remark This function is a convenience function calling the general version
 *     for n-D datasets of arbitrary datatype.
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param fileName    The name of the hdf5 file
 *   \param dsName      The full path of the hdf5 dataset
 *   \param throwErrors If given, BlitzH5Error will be thrown on failure
 *     otherwise an error message is written to std::cout and the program
 *     terminated.
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "[z][y]x" will be generated.
 *
 *   \exception BlitzH5Error If throwErrors is true
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int Dim>
  void writeDataset(
      blitz::Array<std::complex<DataT>,Dim> const &data,
      blitz::TinyVector<ElSizeT,Dim> const &elSize, std::string const &fileName,
      std::string const &dsName, bool throwErrors = false, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");

/*======================================================================*/
/*! 
 *   Write a complex n-dimensional dataset with element size and
 *   additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector",
 *     display_min := max(0, min(Re(data))) + i * max(0, min(Im(data))),
 *     display_max := max(Re(data)) + i * max(Im(data))
 *
 *   \remark This function is a convenience function calling the general version
 *     for n-D datasets of arbitrary datatype.
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param outFile     The hdf5 file to write the dataset to
 *   \param dsName      The full path of the hdf5 dataset
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "[z][y]x" will be generated.
 *
 *   \exception BlitzH5Error If the dataset cannot be written for any reason
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int Dim>
  void writeDataset(
      blitz::Array<std::complex<DataT>,Dim> const &data,
      blitz::TinyVector<ElSizeT,Dim> const &elSize, BlitzH5File &outFile,
      std::string const &dsName, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");

/*======================================================================*/
/*! 
 *   Write a scalar n-dimensional timelapse dataset with element size and
 *   additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "intensity"
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param fileName    The name of the hdf5 file
 *   \param dsName      The full path of the hdf5 dataset
 *   \param displayMin  The value to use as minimum value in visualizations
 *   \param displayMax  The value to use as maximum value in visualizations
 *   \param throwErrors If given, BlitzH5Error will be thrown on failure
 *     otherwise an error message is written to std::cout and the program
 *     terminated.
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "t[z][y]x" will be generated.
 *
 *   \exception BlitzH5Error If throwErrors is true
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int Dim>
  void writeDataset(
      blitz::Array<DataT,Dim> const &data,
      blitz::TinyVector<ElSizeT,Dim-1> const &elSize,
      std::string const &fileName, std::string const &dsName, DataT displayMin,
      DataT displayMax, bool throwErrors = false, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");
  
/*======================================================================*/
/*! 
 *   Write a scalar n-dimensional timelapse dataset with element size and
 *   additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "intensity"
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param outFile     The hdf5 file to write the dataset to
 *   \param dsName      The full path of the hdf5 dataset
 *   \param displayMin  The value to use as minimum value in visualizations
 *   \param displayMax  The value to use as maximum value in visualizations
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "t[z][y]x" will be generated.
 *
 *   \exception BlitzH5Error If the dataset cannot be written for any reason
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int Dim>
  void writeDataset(
      blitz::Array<DataT,Dim> const &data,
      blitz::TinyVector<ElSizeT,Dim-1> const &elSize, BlitzH5File &outFile,
      std::string const &dsName, DataT displayMin, DataT displayMax,
      int compression = 0, iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");
  
/*======================================================================*/
/*! 
 *   Write a scalar n-dimensional timelapse 8Bit unsigned integer dataset
 *   with element size and additional meta-information for dataset
 *   interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "intensity", displayMin = 0, displayMax = 255
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param fileName    The name of the hdf5 file
 *   \param dsName      The full path of the hdf5 dataset
 *   \param throwErrors If given, BlitzH5Error will be thrown on failure
 *     otherwise an error message is written to std::cout and the program
 *     terminated.
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "t[z][y]x" will be generated.
 *
 *   \exception BlitzH5Error If throwErrors is true
 */
/*======================================================================*/
  template<typename ElSizeT, int Dim>
  void writeDataset(
      blitz::Array<unsigned char,Dim> const &data,
      blitz::TinyVector<ElSizeT,Dim-1> const &elSize,
      std::string const &fileName, std::string const &dsName,
      bool throwErrors = false, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");

/*======================================================================*/
/*! 
 *   Write a scalar n-dimensional timelapse 8Bit unsigned integer dataset
 *   with element size and additional meta-information for dataset
 *   interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "intensity", displayMin = 0, displayMax = 255
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param outFile     The hdf5 file to write the dataset to
 *   \param dsName      The full path of the hdf5 dataset
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "t[z][y]x" will be generated.
 *
 *   \exception BlitzH5Error If the dataset cannot be written for any reason
 */
/*======================================================================*/
  template<typename ElSizeT, int Dim>
  void writeDataset(
      blitz::Array<unsigned char,Dim> const &data,
      blitz::TinyVector<ElSizeT,Dim-1> const &elSize, BlitzH5File &outFile,
      std::string const &dsName, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &dim_interpretation = "");

/*======================================================================*/
/*! 
 *   Write a complex n-dimensional timelapse dataset with element size and
 *   additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector",
 *     display_min := max(0, min(Re(data))) + i * max(0, min(Im(data))),
 *     display_max := max(Re(data)) + i * max(Im(data))
 *
 *   \remark This function is a convenience function calling the general version
 *     for n-D datasets of arbitrary datatype.
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param fileName    The name of the hdf5 file
 *   \param dsName      The full path of the hdf5 dataset
 *   \param throwErrors If given, BlitzH5Error will be thrown on failure
 *     otherwise an error message is written to std::cout and the program
 *     terminated.
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "t[z][y]x" will be generated.
 *
 *   \exception BlitzH5Error If throwErrors is true
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int Dim>
  void writeDataset(
     blitz::Array<std::complex<DataT>,Dim> const &data,
     blitz::TinyVector<ElSizeT,Dim-1> const &elSize,
     std::string const &fileName, std::string const &dsName,
     bool throwErrors = false, int compression = 0,
     iRoCS::ProgressReporter *pr = NULL,
     std::string const &dim_interpretation = "");

/*======================================================================*/
/*! 
 *   Write a complex n-dimensional timelapse dataset with element size and
 *   additional meta-information for dataset interpretation. 
 *
 *   Meta-Information which are stored as local attributes are:
 *     \li element_size_um      - The element size in micrometers
 *     \li display_min          - Hint on the minimum value for visualization
 *     \li display_max          - Hint on the maximum value for visualization
 *     \li dim_interpretation   - Hint on how to interpret the different
 *       dimensions of the dataset (string made up of 'x', 'y', 'z', 'c', 't'
 *       characters)
 *     \li value_interpretation - Hint on how to interpret the channels
 *
 *   Written fixed metadata using this function:
 *     value_interpretation := "vector",
 *     display_min := max(0, min(Re(data))) + i * max(0, min(Im(data))),
 *     display_max := max(Re(data)) + i * max(Im(data))
 *
 *   \remark This function is a convenience function calling the general version
 *     for n-D datasets of arbitrary datatype.
 *
 *   \param data        The blitz::Array to write
 *   \param elSize      The blitz::TinyVector containing the element size in
 *     micrometers to write
 *   \param outFile     The hdf5 file to write the dataset to
 *   \param dsName      The full path of the hdf5 dataset
 *   \param compression The compression level to use (0-9).
 *     0 means no compression
 *   \param pr          If given, progress will be reported via the given
 *     iRoCS::ProgressReporter
 *   \param dim_interpretation A hint of how to interpret the dimensions
 *     of the dataset. E.g. "zyxc" means that the first three dimensions
 *     are ordinary coordinates and the last is a channel index. If not
 *     provided by the user a default of "t[z][y]x" will be generated.
 *
 *   \exception BlitzH5Error If the dataset cannot be written for any reason
 */
/*======================================================================*/
  template<typename DataT, typename ElSizeT, int Dim>
  void writeDataset(
     blitz::Array<std::complex<DataT>,Dim> const &data,
     blitz::TinyVector<ElSizeT,Dim-1> const &elSize, BlitzH5File &outFile,
     std::string const &dsName, int compression = 0,
     iRoCS::ProgressReporter *pr = NULL,
     std::string const &dim_interpretation = "");

}

// Analyze Reader and Writer
namespace AnalyzeIOWrapper
{

  template<typename DataT, typename ElSizeT, int Dim>
  void readDataset(blitz::Array<DataT,Dim>& /*data*/,
                   blitz::TinyVector<ElSizeT,Dim>& /*elSize*/,
                   const std::string& /*fileName*/,
                   const std::string& /*dsName*/);
  
  template<typename ElSizeT>
  void writeDataset(const blitz::Array<unsigned char,3>& data,
                    const blitz::TinyVector<ElSizeT,3>& elSize,
                    const std::string& fileName,
                    const std::string& /*dsName*/);

  template<typename ElSizeT>
  void writeDataset(const blitz::Array<short,3>& data,
                    const blitz::TinyVector<ElSizeT,3>& elSize,
                    const std::string& fileName,
                    const std::string& /*dsName*/);

  template<typename ElSizeT>
  void writeDataset(
      const blitz::Array<blitz::TinyVector<unsigned char,3>,3>& data,
      const blitz::TinyVector<ElSizeT,3>& elSize,
      const std::string& fileName,
      const std::string& /*dsName*/);

  template<typename DataT, typename ElSizeT>
  void writeDataset(const blitz::Array<DataT,3>& data,
                    const blitz::TinyVector<ElSizeT,3>& elSize,
                    const std::string& fileName,
                    const std::string& /*dsName*/);

  template<typename DataT, typename ElSizeT>
  void writeDataset(const blitz::Array<blitz::TinyVector<DataT,3>,3>& data,
                    const blitz::TinyVector<ElSizeT,3>& elSize,
                    const std::string& fileName,
                    const std::string& /*dsName*/);

  template<typename DataT, typename ElSizeT, int Dim>
  void writeDataset(const blitz::Array<DataT,Dim>& /*data*/,
                    const blitz::TinyVector<ElSizeT,Dim>& /*elSize*/,
                    const std::string& /*fileName*/,
                    const std::string& /*dsName*/);
  
}

#include "FileIOWrapper.icc"  

#endif
