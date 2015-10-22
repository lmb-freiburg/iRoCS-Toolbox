/**************************************************************************
 *
 * This file is part of the XuV Tools suite. see http://www.xuvtools.org
 * for more information
 *
 * Copyright (C) 2015 Mario Emmenlauer
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

#ifndef BASEFILE_HH
#define BASEFILE_HH BASEFILE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>
#include <vector>

class BaseFile
{
public:
  /**
   * BaseName returns the file name component of the path. Note
   * that it will return the original name, if no '/' character is
   * found in the path name.
   *
   * @return file name
   */
  static std::string BaseName(const std::string& aPathName);

  /**
   * DirName returns the directory name component ofthe path. Note
   * that it will return an empty string if no '/' character is
   * found in the path name.
   *
   * @return directory name
   */
  static std::string DirName(const std::string& aPathName);

  /**
   * BaseNamePath returns the fully qualified path to the
   * file. It expands all symbolic links and resolves references
   * to /./, /../ and extra '/' characters, to produce a
   * canonicalized absolute pathname.
   *
   * @return string the path to the file
   */
  static std::string BaseNamePath(const std::string& aPathName);

  /**
   * Exists tests for existance of a file or directory using
   * the stat() function call (which succeeds even if no read
   * permissions are available.
   *
   * @param aPathName the path to the file/directory to check
   * @return bool returns true if the file or directory exists,
   *         false otherwise.
   */
  static bool Exists(const std::string& aPathName);

  /**
   * IsFile tests whether a file system entry is a normal file
   * or not.
   *
   * @param aPathName the path to the file/directory to check
   * @return bool returns true if the entry is a normal file,
   *         false otherwise.
   */
  static bool IsFile(const std::string& aPathName);

  /**
   * IsLink tests whether a file system entry is a symlink
   * or not.
   *
   * @param aPathName the path to the file/directory to check
   * @return bool returns true if the entry is a symbolic link,
   *         false otherwise.
   */
  static bool IsLink(const std::string& aPathName);

  /**
   * IsDirectory tests whether a file system entry is a directory
   * or not.
   *
   * @param aPathName the path to the file/directory to check
   * @return bool returns true if the entry is a directory, false
   *         otherwise.
   */
  static bool IsDirectory(const std::string& aPathName);

  /**
   * DirectoryCreate cretaes a directory at the specified path. If
   * the directory already exists, nothing is done.
   *
   * @param aPathName the path to the directory to create.
   * @return bool returns true if the directory was created or
   *         exists already, false otherwise.
   */
  static bool DirectoryCreate(const std::string& aPathName);

  /**
   * Lists the files in a directory
   *
   * @param aInputDir the path to the directory
   * @param aFiles a vector of fully qualified entries
   * @param aPattern
   * @return bool returns true if the file or directory was
   *         successfully listed, false otherwise.
   */
  static bool ListDir(
      const std::string& aInputDir, std::vector<std::string>& aFiles,
      const std::string& aPattern);

  /**
   * Remove deletes a file or directory
   *
   * @param aPathName the path to the file/directory to remove
   * @return bool returns true if the file or directory was
   *         successfully removed, false otherwise.
   */
  static bool Remove(const std::string& aPathName);

  /**
   * Move renames a file or directory
   *
   * @param aOldName the path to the file/directory to rename
   * @param aNewName the path to the new file/directory name
   * @return bool returns true if the file or directory was
   *         successfully renamed, false otherwise.
   */
  static bool Move(const std::string& aOldName, const std::string& aNewName);

  /**
   * Copy copies a file or directory
   *
   * @param aOldName the path to the file/directory to rename
   * @param aNewName the path to the new file/directory name
   * @return bool returns true if the file or directory was
   *         successfully copied, false otherwise.
   */
  static bool Copy(const std::string& aOldName, const std::string& aNewName);

  /**
   * Generates a new unique file name using pattern
   *
   * @param aBaseName Prefix
   * @param aPattern Pattern
   * @return A new filename
   */
  static std::string
  FindUniqueUnexistingName(std::string const &aBaseName, char aPattern);

  /**
   * BeautifyFilePath sanitizes a path to a file or directory. It
   * replaces duplicate slashes with signle slashes, uses forward
   * slashes where possible, and other neat things.
   *
   * @param aPath the path to the file/directory to beautify
   * @return the beautified path
   */
  static std::string BeautifyFilePath(const std::string& aPath);

private:
#ifdef _WIN32
  static char* realpath(const char *source, char *target);
#endif
};

#endif
