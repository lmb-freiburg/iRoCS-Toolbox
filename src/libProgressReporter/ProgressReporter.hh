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

#ifndef IROCSPROGRESSREPORTER_HH
#define IROCSPROGRESSREPORTER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>

#ifdef _WIN32
#ifdef ProgressReporter_EXPORTS
#define PROGRESSREPORTERDLL_API __declspec(dllexport)
#else
#define PROGRESSREPORTERDLL_API __declspec(dllimport)
#endif
#else
#define PROGRESSREPORTERDLL_API
#endif

namespace iRoCS
{
  
  class PROGRESSREPORTERDLL_API ProgressReporter
  {

  public:

    ProgressReporter()
            : _taskProgressMin(0), _taskProgressMax(0)
          {}

    virtual ~ProgressReporter()
          {}

    virtual void setProgressMin(int progressMin) = 0;

    virtual int progressMin() const = 0;

    virtual void setProgressMax(int progressMax) = 0;

    virtual int progressMax() const = 0;

    void setTaskProgressRange(int progressMin, int progressMax);

    void setTaskProgressMin(int progressMin);

    int taskProgressMin() const;

    void setTaskProgressMax(int progressMax);

    int taskProgressMax() const;

    virtual void setHeaderMessage(std::string const &message) = 0;

    virtual std::string headerMessage() const = 0;

    virtual void setProgressMessage(std::string const &message) = 0;

    virtual std::string progressMessage() const = 0;
  
    virtual void setProgress(int progress) = 0;
  
    virtual int progress() const = 0;

    virtual void setAborted(bool abort) = 0;

    virtual void abort() = 0;

    virtual void abortWithError(std::string const &msg) = 0;

    virtual bool isAborted() const = 0;

    virtual bool updateProgress(int progress) = 0;

    virtual bool updateProgressMessage(std::string const &message) = 0;

  protected:
    
    int _taskProgressMin, _taskProgressMax;

  };

}

#endif
