/***************************************************************************
 *   This source code is distributed under the license noted in the        *
 *   LICENSE file you should have received along with this source code.    *
 *   If you do not have received the LICENSE file, you can obtain a copy   *
 *   from http://www.xuvtools.org/license.                                 *
 *   The source code is copyright by its author(s). See the AUTHORS        *
 *   file for the list of authors.                                         *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 *                                                                         *
 *         Title:                                                          *
 *   Description:                                                          *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
#include <BaseFile.hh>

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

#ifdef _MSC_VER
#include "MSVC_compat/dirent.h"
#else
#include <dirent.h>
#endif

#ifdef WIN32
#include <direct.h>
#include <windows.h>
#else
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/resource.h>
#include <sys/sysctl.h>
#include <sys/mman.h>
#endif


std::string
BaseFile::
BaseName(const std::string& aPathName)
{
  const size_t vPos = aPathName.rfind('/');
  if (vPos != std::string::npos) {
    return aPathName.substr(vPos+1);
  }

  return aPathName;
}


std::string
BaseFile::
DirName(const std::string& aPathName)
{
  const size_t vPos = aPathName.rfind('/');
  if (vPos != std::string::npos) {
    return aPathName.substr(0, vPos);
  }

  return "";
}


std::string
BaseFile::
BaseNamePath(const std::string& aPathName)
{
  std::string vFullyQualifiedPath;

  // Try to find the fully qualified path
  char* realpath_res = new char[65535];
  if (realpath_res == NULL) {
    return "";
  }

  if (realpath(aPathName.c_str(), realpath_res) != NULL) {
    vFullyQualifiedPath = std::string(realpath_res);
  }
  free(realpath_res);
  realpath_res = NULL;

  if (vFullyQualifiedPath.length() > 1) {
    const size_t vPos = vFullyQualifiedPath.rfind('/');
    if (vPos != std::string::npos) {
      // truncate the string to only the directory
      // (removing the filename, if it exists)
      vFullyQualifiedPath.erase(vPos);
    }
  }

  return vFullyQualifiedPath;
}


bool
BaseFile::
Exists(const std::string& aPathName)
{
  // Attempt to get the file attributes
  struct stat vFileInfo;
  return (stat(aPathName.c_str(), &vFileInfo) == 0);
}


bool
BaseFile::
IsFile(const std::string& aPathName)
{
  if (IsLink(aPathName)) return false;
  struct stat st;
  if (stat(aPathName.c_str(),&st) == 0)
      return ((st.st_mode & S_IFREG) == S_IFREG);
  return false;
}


bool
BaseFile::
IsLink(const std::string& aPathName)
{
  char buf;
  return (readlink(aPathName.c_str(), &buf, 1) != -1);
  // struct stat st;
  // if (stat(aPathName.c_str(),&st) == 0)
  //     return ((st.st_mode & S_IFLNK) == S_IFLNK);
  // return false;
}


bool
BaseFile::
IsDirectory(const std::string& aPathName)
{
  struct stat st;
  if (stat(aPathName.c_str(),&st) == 0)
      return ((st.st_mode & S_IFDIR) == S_IFDIR);
  return false;
}


bool
BaseFile::
DirectoryCreate(const std::string& aPathName)
{
  const std::string vBeautifyPathName = BaseFile::BeautifyFilePath(aPathName);

  if (BaseFile::IsDirectory(vBeautifyPathName)) {
    return true;
  }
  if (BaseFile::Exists(vBeautifyPathName)) {
    return false;
  }


  // ignore the drive letter or root directory
#if defined(_WIN32)
  size_t vPos = 3;
#else
  size_t vPos = 1;
#endif

  vPos = vBeautifyPathName.find_first_of("/\\", vPos);
  while (vPos != std::string::npos) {
    const std::string vPreDirName = vBeautifyPathName.substr(0, vPos);
    if (!BaseFile::Exists(vPreDirName)) {
      //Reporter::messageCallback(4, "Creating Directory '"+vPreDirName+"'\n");
#if defined(_WIN32)
      if (_mkdir(vPreDirName.c_str()) != 0)
          return false;
#else
      if (mkdir(vPreDirName.c_str(), 0755) != 0)
          return false;
#endif
    }
    vPos = vBeautifyPathName.find_first_of("/\\", vPos+1);
  }

  if (!BaseFile::Exists(vBeautifyPathName)) {
    //Reporter::messageCallback(4, "Creating Directory '"+vBeautifyPathName+"'\n");
#if defined(_WIN32)
    if (_mkdir(vBeautifyPathName.c_str()) != 0)
        return false;
#else
    if (mkdir(vBeautifyPathName.c_str(), 0755) != 0)
        return false;
#endif
  }

  return true;
}


bool
BaseFile::
Remove(const std::string& aPathName)
{
  // NOTE: unlink is only for files, remove for files
  // and directorys.

  if (remove(aPathName.c_str()) == -1)
      return false;
  return true;
}


bool
BaseFile::
Move(const std::string& aOldName, const std::string& aNewName)
{
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


bool
BaseFile::
Copy(const std::string& aOldName,
     const std::string& aNewName)
{
#ifndef _WIN32
/*
  int input, output;
  size_t filesize;
  void *source, *target;

  if((input = open(aOldName.c_str(), O_RDONLY)) == -1) {
  fprintf(stderr, "%s: Error: opening file: %s\n", "mmap", aOldName.c_str());
  return false;
  }

  if((output = open(aNewName.c_str(), O_RDWR|O_CREAT|O_TRUNC, 0666)) == -1) {
  fprintf(stderr, "%s: Error: opening file: %s\n", "mmap", aNewName.c_str());
  return false;
  }

  filesize = lseek(input, 0, SEEK_END);
  lseek(output, filesize - 1, SEEK_SET);
  write(output, '\0', 1);

  if((source = mmap(0, filesize, PROT_READ, MAP_SHARED, input, 0)) == (void *) -1) {
  fprintf(stderr, "Error mapping input file: %s\n", aOldName.c_str());
  return false;
  }

  if((target = mmap(0, filesize, PROT_WRITE, MAP_SHARED, output, 0)) == (void *) -1) {
  fprintf(stderr, "Error mapping ouput file: %s\n", aNewName.c_str());
  return false;
  }

  memcpy(target, source, filesize);

  munmap(source, filesize);
  munmap(target, filesize);

  close(input);
  close(output);
*/

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
    const std::string& aInputDir, std::vector<std::string>& aFiles,
    const std::string& /*aPattern*/)
{
  //Reporter::messageCallback(4, std::string("Opening input directory '") + aInputDir + "'.\n");
  DIR *dp = opendir(aInputDir.c_str());
  if (dp == NULL) {
    //Reporter::messageCallback(2, std::string("Error opening input directory '") + aInputDir + "'.\n");
    return false;
  }

  struct dirent* dirp = readdir(dp);
  while (dirp != NULL) {
    if (dirp->d_type == DT_REG) {
      const std::string vFullFilename = BaseFile::BeautifyFilePath(aInputDir + "/" + dirp->d_name);
      //Reporter::messageCallback(20, "BaseFile::ListDir(): Found file '" + vFullFilename + "'.\n");
      aFiles.push_back(vFullFilename);
    }

    dirp = readdir(dp);
  }
  closedir(dp);

  std::sort(aFiles.begin(), aFiles.end());

  return true;
}


std::string
BaseFile::
FindUniqueUnexistingName(const std::string aBaseName, const char aPattern)
{
  srand((unsigned int)time(NULL));
  std::string vUniqueName;

  do {
    // String to store file full file name
    vUniqueName = aBaseName;

    size_t vSepPos = vUniqueName.find_last_of("/\\");
    if (vSepPos == std::string::npos) {
      vSepPos = 0;
    }
    for (size_t vCurrPos = vSepPos; vCurrPos < vUniqueName.size(); ++vCurrPos) {
      if (vUniqueName[vCurrPos] == aPattern) {
        vUniqueName[vCurrPos] = static_cast<char>('A' + rand() % 25);
      }
    }
  } while (BaseFile::Exists(vUniqueName));

  return vUniqueName;
}


std::string
BaseFile::
BeautifyFilePath(const std::string& aPath)
{
  std::string vNewPath = aPath;
#ifdef _WIN32
  // On Windows, we can replace backslash with slash, both
  // are valid path separators. But only starting from the
  // second character, since it could be a network drive
  // path, which goes '\\server\path\'.
  // Do not replace backslash on Linux, because backslash
  // is used as an escape character on Linux.
  if (vNewPath.size() > 2) {
    std::replace(vNewPath.begin()+2, vNewPath.end(), '\\',  '/');

    // On windows, make the first character uppercase, if
    // it is a drive letter
    if (vNewPath[1] == ':') {
      vNewPath[0] = toupper(vNewPath[0]);
    }
  }
#else
  // Try to find the fully qualified path
  char* realpath_res = new char[65535];
  if (realpath_res == NULL) {
    return "";
  }

  if (realpath(vNewPath.c_str(), realpath_res) != NULL) {
    vNewPath = std::string(realpath_res);
  }
  free(realpath_res);
  realpath_res = NULL;
#endif
  size_t vPos;

  // Replace '/./' with '/'
  vPos = vNewPath.find("/./");
  while (vPos != std::string::npos) {
    vNewPath.erase(vPos,2);
    vPos = vNewPath.find("/./", vPos);
  }

  // Replace all double '//' with a single '/'
  vPos = vNewPath.find("//");
  while (vPos != std::string::npos) {
    vNewPath.erase(vPos,1);
    vPos = vNewPath.find("//", vPos);
  }
  return vNewPath;
}


#ifdef _WIN32
int
BaseFile::
realpath(const char *source, char *target)
{
  int r = 0;
  while (*source) {
    if (*source == '\\') *target = '/';
    else *target = *source;
    source++;
    target++;
    if (++r == 65535-1) break;
  }
  *target = 0;
  return r;
}
#endif

