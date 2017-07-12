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

#include "BaseFile.hh"

 // standard libraries
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <cstdio>

#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#else
#include <dirent.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/resource.h>
#include <sys/sysctl.h>
#include <sys/mman.h>
#endif

std::string BaseFile::BaseName(std::string const &aPathName) {
#ifdef _WIN32
  char delim = '\\';
#else
  char delim = '/';
#endif
  size_t vPos = aPathName.rfind(delim);
  if (vPos != std::string::npos) return aPathName.substr(vPos + 1);
  return aPathName;
}

std::string BaseFile::DirName(std::string const &aPathName) {
#ifdef _WIN32
  char delim = '\\';
#else
  char delim = '/';
#endif
  size_t vPos = aPathName.rfind(delim);
  if (vPos != std::string::npos) return aPathName.substr(0, vPos);
  return "";
}

std::string BaseFile::BaseNamePath(std::string const &aPathName) {
#ifdef _WIN32
  char delim = '\\';
#else
  char delim = '/';
#endif
  char *realpath_res = realpath(aPathName.c_str(), NULL);
  if (realpath_res == NULL) return std::string();

  std::string vFullyQualifiedPath(realpath_res);
  free(realpath_res);

  if (vFullyQualifiedPath.length() > 1) {
    size_t vPos = vFullyQualifiedPath.rfind(delim);
    if (vPos != std::string::npos) {
      // truncate the string to only the directory
      // (removing the filename, if it exists)
      vFullyQualifiedPath.erase(vPos);
    }
  }

  return vFullyQualifiedPath;
}

bool BaseFile::Exists(std::string const &aPathName) {
  // Attempt to get the file attributes
  struct stat st;
  return (stat(aPathName.c_str(), &st) == 0);
}

bool BaseFile::IsFile(std::string const &aPathName) {
#ifdef _WIN32
  return Exists(aPathName) && !IsLink(aPathName) && !IsDirectory(aPathName);
#else
  struct stat st;
  if (lstat(aPathName.c_str(), &st) != 0) return false;
  return (S_ISREG(st.st_mode) != 0);
#endif
}

bool BaseFile::IsLink(std::string const &aPathName) {
#ifdef _WIN32
  return Exists(aPathName) && (GetFileAttributes(aPathName.c_str()) & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
#else
  struct stat st;
  if (lstat(aPathName.c_str(), &st) != 0) return false;
  return (S_ISLNK(st.st_mode) != 0);
#endif
}

bool BaseFile::IsDirectory(std::string const &aPathName) {
#ifdef _WIN32
  return Exists(aPathName) && (GetFileAttributes(aPathName.c_str()) & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
  struct stat st;
  if (lstat(aPathName.c_str(), &st) != 0) return false;
  return (S_ISDIR(st.st_mode) != 0);
#endif
}

bool BaseFile::DirectoryCreate(std::string const &aPathName) {
  const std::string vBeautifyPathName = BaseFile::BeautifyFilePath(aPathName);

  if (BaseFile::IsDirectory(vBeautifyPathName)) return true;
  if (BaseFile::Exists(vBeautifyPathName)) return false;

  // ignore the drive letter or root directory
#if defined(_WIN32)
  size_t vPos = 3;
  char delim = '\\';
#else
  size_t vPos = 1;
  char delim = '/';
#endif

  vPos = vBeautifyPathName.find_first_of(delim, vPos);
  while (vPos != std::string::npos) {
    const std::string vPreDirName = vBeautifyPathName.substr(0, vPos);
    if (!BaseFile::Exists(vPreDirName)) {
#ifdef _WIN32
      if (!CreateDirectory(vPreDirName.c_str(), NULL)) return false;
#else
      if (mkdir(vPreDirName.c_str(), 0755) != 0) return false;
#endif
    }
    vPos = vBeautifyPathName.find_first_of("/\\", vPos + 1);
  }

  if (!BaseFile::Exists(vBeautifyPathName)) {
#if defined(_WIN32)
    if (!CreateDirectory(vBeautifyPathName.c_str(), NULL)) return false;
#else
    if (mkdir(vBeautifyPathName.c_str(), 0755) != 0) return false;
#endif
  }

  return true;
}

bool BaseFile::Remove(std::string const &aPathName) {
  if (remove(aPathName.c_str()) == -1) return false;
  return true;
}

bool BaseFile::Move(std::string const &aOldName, std::string const &aNewName) {
  int vTrie = 0;
  int vStatus = 1;

  while (vStatus != 0 && vTrie < 4) {
    vStatus = remove(aNewName.c_str());
    ++vTrie;
  }

  vTrie = 0;
  vStatus = 1;
  while (vStatus != 0 && vTrie < 4) {
#ifdef _WIN32
    // The Windows file system is not very fast
    Sleep(100);
#endif
    vStatus = rename(aOldName.c_str(), aNewName.c_str());
    ++vTrie;
  }
  return (vStatus == 0);
}

bool BaseFile::Copy(std::string const &aOldName, std::string const &aNewName) {
#ifdef _WIN32
  CopyFile(aOldName.c_str(), aNewName.c_str(), FALSE);
#else
  struct stat stat_buf;
  off_t offset = 0;

  /* Open the input file. */
  int read_fd = open(aOldName.c_str(), O_RDONLY);
  if (read_fd < 0) {
    std::cerr << "Error opening input file." << std::endl;
    return false;
  }

  /* Stat the input file to obtain its size. */
  int stat_ret = fstat(read_fd, &stat_buf);
  if (stat_ret < 0) {
    std::cerr << "Error checking input file size." << std::endl;
    close(read_fd);
    return false;
  }

  /* Open the output file for writing, with the same permissions as the
     source file. */
  int write_fd = open(aNewName.c_str(), O_WRONLY | O_CREAT | O_TRUNC, stat_buf.st_mode);
  if (write_fd < 0) {
    std::cerr << "Error opening output file." << std::endl;
    close(read_fd);
    return false;
  }

  /* Blast the bytes from one file to the other. */
  ssize_t sendf_ret = sendfile(write_fd, read_fd, &offset, stat_buf.st_size);
  if (sendf_ret < 0) {
    std::cerr << "Error copying file contents, removing target." << std::endl;
    close(read_fd);
    close(write_fd);
    unlink(aNewName.c_str());
    return false;
  }

  /* Close up. */
  close(read_fd);
  close(write_fd);
#endif

  return true;
}

bool BaseFile::ListDir(
  std::string const &aInputDir, std::vector<std::string> &aFiles,
  std::string const &/*aPattern*/) {
#ifdef _WIN32
  // Remove trailing backslashes and slashes
  std::string dirName(aInputDir);
  while (dirName.rfind("\\") == dirName.size() - 1)
    dirName = dirName.substr(0, dirName.size() - 2);
  while (dirName.rfind("/") == dirName.size() - 1)
    dirName = dirName.substr(0, dirName.size() - 2);
  WIN32_FIND_DATA findFileData;
  HANDLE fileIterator = FindFirstFile(dirName.c_str(), &findFileData);
  if (fileIterator == INVALID_HANDLE_VALUE) return false;
  BOOL moreFiles = false;
  while (moreFiles) {
    std::string fileName(findFileData.cFileName);
    if (IsFile(fileName)) aFiles.push_back(fileName);
    moreFiles = FindNextFile(fileIterator, &findFileData);
  }
#else
  DIR *dp = opendir(aInputDir.c_str());
  if (dp == NULL) return false;

  struct dirent* dirp = readdir(dp);
  while (dirp != NULL) {
    if (dirp->d_type == DT_REG) {
      std::string vFullFilename =
        BaseFile::BeautifyFilePath(aInputDir + "/" + dirp->d_name);
      aFiles.push_back(vFullFilename);
    }
    dirp = readdir(dp);
  }
  closedir(dp);
#endif
  std::sort(aFiles.begin(), aFiles.end());

  return true;
}

std::string BaseFile::FindUniqueUnexistingName(
  std::string const &aBaseName, char aPattern) {
  srand((unsigned int)time(NULL));
  std::string vUniqueName;

  do {
    // String to store file full file name
    vUniqueName = aBaseName;

    size_t vSepPos = vUniqueName.find_last_of("/\\");
    if (vSepPos == std::string::npos) vSepPos = 0;
    for (size_t vCurrPos = vSepPos; vCurrPos < vUniqueName.size(); ++vCurrPos) {
      if (vUniqueName[vCurrPos] == aPattern)
        vUniqueName[vCurrPos] = static_cast<char>('A' + rand() % 25);
    }
  } while (BaseFile::Exists(vUniqueName));

  return vUniqueName;
}

std::string BaseFile::BeautifyFilePath(std::string const &aPath) {
  std::string vNewPath = aPath;
#ifdef _WIN32
  // On Windows, we can replace backslash with slash, both
  // are valid path separators. But only starting from the
  // second character, since it could be a network drive
  // path, which goes '\\server\path\'.
  // Do not replace backslash on Linux, because backslash
  // is used as an escape character on Linux.
  if (vNewPath.size() > 2) {
    std::replace(vNewPath.begin() + 2, vNewPath.end(), '\\', '/');

    // On windows, make the first character uppercase, if
    // it is a drive letter
    if (vNewPath[1] == ':') {
      vNewPath[0] = toupper(vNewPath[0]);
    }
  }
#else
  char *realpath_res = NULL;
  // Try to find the fully qualified path
  if ((realpath_res = realpath(vNewPath.c_str(), NULL)) != NULL) {
    vNewPath = std::string(realpath_res);
    free(realpath_res);
  }
#endif
  size_t vPos;

  // Replace '/./' with '/'
  vPos = vNewPath.find("/./");
  while (vPos != std::string::npos) {
    vNewPath.erase(vPos, 2);
    vPos = vNewPath.find("/./", vPos);
  }

  // Replace all double '//' with a single '/'
  vPos = vNewPath.find("//");
  while (vPos != std::string::npos) {
    vNewPath.erase(vPos, 1);
    vPos = vNewPath.find("//", vPos);
  }
  return vNewPath;
}

#ifdef _WIN32
char *BaseFile::realpath(const char *path, char *resolved_path) {
  char *return_path = 0;

  if (path) //Else EINVAL
  {
    if (resolved_path) {
      return_path = resolved_path;
    }
    else {
      //Non standard extension that glibc uses
      return_path = (char*)malloc(MAX_PATH);
    }

    if (return_path) //Else EINVAL
    {
      //This is a Win32 API function similar to what realpath() is supposed to do
      DWORD size = GetFullPathNameA(path, MAX_PATH, return_path, 0);

      //GetFullPathNameA() returns a size larger than buffer if buffer is too small
      if (size > MAX_PATH) {
        if (return_path != resolved_path) //Malloc'd buffer - Unstandard extension retry
        {
          DWORD new_size;

          free(return_path);
          return_path = (char*)malloc(size);

          if (return_path) {
            new_size = GetFullPathNameA(path, size, return_path, 0); //Try again

            if (new_size > size) //If it's still too large, we have a problem, don't try again
            {
              free(return_path);
              return_path = 0;
              errno = ENAMETOOLONG;
            }
            else {
              size = new_size;
            }
          }
          else {
            //I wasn't sure what to return here, but the standard does say to return EINVAL
            //if resolved_path is null, and in this case we couldn't malloc large enough buffer
            errno = EINVAL;
          }
        }
        else //resolved_path buffer isn't big enough
        {
          return_path = 0;
          errno = ENAMETOOLONG;
        }
      }

      //GetFullPathNameA() returns 0 if some path resolve problem occured
      if (!size) {
        if (return_path != resolved_path) //Malloc'd buffer
        {
          free(return_path);
        }

        return_path = 0;

        //Convert MS errors into standard errors
        switch (GetLastError()) {
          case ERROR_FILE_NOT_FOUND:
            errno = ENOENT;
            break;

          case ERROR_PATH_NOT_FOUND: case ERROR_INVALID_DRIVE:
            errno = ENOTDIR;
            break;

          case ERROR_ACCESS_DENIED:
            errno = EACCES;
            break;

          default: //Unknown Error
            errno = EIO;
            break;
        }
      }

      //If we get to here with a valid return_path, we're still doing good
      if (return_path) {
        struct stat stat_buffer;

        //Make sure path exists, stat() returns 0 on success
        if (stat(return_path, &stat_buffer)) {
          if (return_path != resolved_path) {
            free(return_path);
          }

          return_path = 0;
          //stat() will set the correct errno for us
        }
        //else we succeeded!
      }
    }
    else {
      errno = EINVAL;
    }
  }
  else {
    errno = EINVAL;
  }

  return return_path;
}
#endif
