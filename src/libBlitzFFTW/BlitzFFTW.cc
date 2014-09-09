/**************************************************************************
**       Title: fftw Wrapper for fast fftw Calculation using blitz++ Arrays
**    $RCSfile$
**   $Revision: 4548 $$Name$
**       $Date: 2011-01-02 18:43:49 +0100 (Sun, 02 Jan 2011) $
**   Copyright: GPL $Author: emmenlau $
** Description:
**
**   Wrapper for libfftw and libblitz++, providing arbitrary dimensional
**   fft's
**
**************************************************************************/
#include <BlitzFFTW.hh>

#include <libBaseFunctions/BaseEnvironment.hh>
#include <libBaseFunctions/BaseFile.hh>

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

/*-------------------------------------------------------------------------
 *  Float specializations
 *-------------------------------------------------------------------------*/

template<>
void BlitzFFTW<float>::loadWisdom()
{
#ifdef _OPENMP
#pragma omp critical (wisdomfileaccess)
{
#endif
  std::string dirName = BaseEnvironment::UserHomeDirectory() + "/.fftw";
  std::string fftwfFileName = dirName + "/" + BaseEnvironment::HostName() + ".wisdom.fftwf";
  FILE* fp;

  int status = 0;
#if defined(BLITZFFTW_DEBUG)
  std::cout << "BlitzFFTW<float>::loadWisdom(): trying to open wisdom file "
          << fftwfFileName << std::endl;
#endif
  fp = fopen( fftwfFileName.c_str(), "r");
  if( fp != NULL) {  // returns non-zero on success, zero on error
#if defined(BLITZFFTW_DEBUG)
    std::cout << "successfully opened " << fftwfFileName << std::endl;
#endif
    status = fftwf_import_wisdom_from_file(fp);
    fclose(fp);
#if defined(BLITZFFTW_DEBUG)
    if( status != 0) {
      std::cout << "successfully read wisdom" << std::endl;
    } else {
      std::cout << "error reading wisdom (differ fftw-versions?)" << std::endl;
    }
#endif
  } else {
#if defined(BLITZFFTW_DEBUG)
    std::cout << "could not open wisdom file " << fftwfFileName << std::endl;
#endif
    status = 0;
  }
  if( status == 0) {  // returns non-zero on success, zero on error
#if defined(BLITZFFTW_DEBUG)
    std::cout << "trying to read system wisdom" << std::endl;
#endif
    status = fftwf_import_system_wisdom();
    if( status == 0) {  // returns non-zero on success, zero on error
#if defined(BLITZFFTW_DEBUG)
      std::cout << "could not read system wisdom" << std::endl;
#endif
    }
  }
#ifdef _OPENMP
}
#endif
}

template<>
void BlitzFFTW<float>::saveWisdom() const
{
#ifdef _OPENMP
#pragma omp critical (wisdomfileaccess)
{
#endif
  std::string dirName = BaseEnvironment::UserHomeDirectory() +"/.fftw";
  std::string fftwfFileName = dirName + "/" + BaseEnvironment::HostName() + ".wisdom.fftwf";
  FILE* fp;

#if defined(BLITZFFTW_DEBUG)
  std::cout << "BlitzFFTW<float>::saveWisdom(): trying to open wisdom file "
          << fftwfFileName << std::endl;
#endif
  fp = fopen( fftwfFileName.c_str(), "w");
  if( fp == NULL) {
    BaseFile::DirectoryCreate(dirName);
    fp = fopen( fftwfFileName.c_str(), "w");
  }
  if( fp != NULL) {
#if defined(BLITZFFTW_DEBUG)
    std::cout << "successfully opened " << fftwfFileName << std::endl;
#endif
    fftwf_export_wisdom_to_file(fp);
    fclose(fp);
  }
#ifdef _OPENMP
}
#endif
}

/*-------------------------------------------------------------------------
 *  Double specializations
 *-------------------------------------------------------------------------*/

template<>
void BlitzFFTW<double>::loadWisdom()
{
#ifdef _OPENMP
#pragma omp critical (wisdomfileaccess)
{
#endif
  std::string dirName = BaseEnvironment::UserHomeDirectory() + "/.fftw";
  std::string fftwFileName = dirName + "/" + BaseEnvironment::HostName() + ".wisdom.fftw";
  FILE* fp;

  int status = 0;
#if defined(BLITZFFTW_DEBUG)
  std::cout << "BlitzFFTW<double>::loadWisdom(): trying to open wisdom file "
          << fftwFileName << std::endl;
#endif
  fp = fopen( fftwFileName.c_str(), "r");
  if( fp != NULL) {  // returns non-zero on success, zero on error
#if defined(BLITZFFTW_DEBUG)
    std::cout << "successfully opened " << fftwFileName << std::endl;
#endif
    status = fftw_import_wisdom_from_file(fp);
    fclose(fp);
#if defined(BLITZFFTW_DEBUG)
    if( status != 0) {
      std::cout << "successfully read wisdom" << std::endl;
    } else {
      std::cout << "error reading wisdom (differ fftw-versions?)" << std::endl;
    }
#endif
  } else {
#if defined(BLITZFFTW_DEBUG)
    std::cout << "could not open wisdom file " << fftwFileName << std::endl;
#endif
    status = 0;
  }
  if( status == 0) {  // returns non-zero on success, zero on error
#if defined(BLITZFFTW_DEBUG)
    std::cout << "trying to read system wisdom" << std::endl;
#endif
    status = fftw_import_system_wisdom();
    if( status == 0) {  // returns non-zero on success, zero on error
#if defined(BLITZFFTW_DEBUG)
      std::cout << "could not read system wisdom" << std::endl;
#endif
    }
  }
#ifdef _OPENMP
}
#endif
}

template<>
void BlitzFFTW<double>::saveWisdom() const
{
#ifdef _OPENMP
#pragma omp critical (wisdomfileaccess)
{
#endif
  std::string dirName = BaseEnvironment::UserHomeDirectory() + "/.fftw";
  std::string fftwFileName = dirName + "/" + BaseEnvironment::HostName() + ".wisdom.fftw";
  FILE* fp;

#if defined(BLITZFFTW_DEBUG)
  std::cout << "BlitzFFTW<double>::saveWisdom(): trying to open wisdom file "
          << fftwFileName << std::endl;
#endif
  fp = fopen( fftwFileName.c_str(), "w");
  if( fp == NULL) {
    BaseFile::DirectoryCreate(dirName);
    fp = fopen( fftwFileName.c_str(), "w");
  }
  if( fp != NULL) {
#if defined(BLITZFFTW_DEBUG)
    std::cout << "successfully opened " << fftwFileName << std::endl;
#endif
    fftw_export_wisdom_to_file(fp);
    fclose(fp);
  }
#ifdef _OPENMP
}
#endif
}


template<>
BlitzFFTW<float>::BlitzFFTW()
{
  static BlitzFFTWDestructor w;
  w.init();
  this->loadWisdom();
  this->prepareFFTSizes();

  this->blitz_fftw_execute = &fftwf_execute;
  this->blitz_fftw_plan_dft = &fftwf_plan_dft;
  this->blitz_fftw_execute_dft = &fftwf_execute_dft;
  this->blitz_fftw_plan_dft_r2c = &fftwf_plan_dft_r2c;
  this->blitz_fftw_plan_dft_c2r = &fftwf_plan_dft_c2r;
  this->blitz_fftw_execute_dft_r2c = &fftwf_execute_dft_r2c;
  this->blitz_fftw_execute_dft_c2r = &fftwf_execute_dft_c2r;
  this->blitz_fftw_destroy_plan = &fftwf_destroy_plan;
  this->blitz_fftw_cleanup = &fftwf_cleanup;
  this->blitz_fftw_malloc = &fftwf_malloc;
}


template<>
BlitzFFTW<double>::BlitzFFTW()
{
  static BlitzFFTWDestructor w;
  w.init();
  this->loadWisdom();
  this->prepareFFTSizes();

  this->blitz_fftw_execute = &fftw_execute;
  this->blitz_fftw_plan_dft = &fftw_plan_dft;
  this->blitz_fftw_execute_dft = &fftw_execute_dft;
  this->blitz_fftw_plan_dft_r2c = &fftw_plan_dft_r2c;
  this->blitz_fftw_plan_dft_c2r = &fftw_plan_dft_c2r;
  this->blitz_fftw_execute_dft_r2c = &fftw_execute_dft_r2c;
  this->blitz_fftw_execute_dft_c2r = &fftw_execute_dft_c2r;
  this->blitz_fftw_destroy_plan = &fftw_destroy_plan;
  this->blitz_fftw_cleanup = &fftw_cleanup;
  this->blitz_fftw_malloc = &fftw_malloc;
}


template<>
BlitzFFTW<float>*
BlitzFFTW<float>::instance()
{
#ifdef _OPENMP
#pragma omp critical (getinstance)
  {
#endif
    if (p_instance == 0) p_instance = new BlitzFFTW<float>();
#ifdef _OPENMP
  }
#endif
  return p_instance;
}


template<>
BlitzFFTW<double>*
BlitzFFTW<double>::instance()
{
#ifdef _OPENMP
#pragma omp critical (getinstance)
  {
#endif
  if (p_instance == 0) p_instance = new BlitzFFTW<double>();
#ifdef _OPENMP
  }
#endif
  return p_instance;
}
