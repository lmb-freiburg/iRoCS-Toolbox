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

#ifndef IROCSPROGRESSREPORTERSTREAM_HH
#define IROCSPROGRESSREPORTERSTREAM_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ProgressReporter.hh"
#include <iostream>
#include <sstream>

namespace iRoCS
{
  
  class PROGRESSREPORTERDLL_API ProgressReporterStream : public ProgressReporter
  {

  public:
  
    ProgressReporterStream(
        std::ostream& os = std::cout,
        int progress = 0, int progressMin = 0, int progressMax = 100,
        std::string const &headerMessage = std::string(),
        std::string const &progressMessage = std::string());

    ~ProgressReporterStream();

    void setProgressMin(int progressMin);

    int progressMin() const;

    void setProgressMax(int progressMax);

    int progressMax() const;

    void setHeaderMessage(std::string const &message);

    std::string headerMessage() const;

    void setProgressMessage(std::string const &message);

    std::string progressMessage() const;
  
    void setProgress(int progress);
  
    int progress() const;

    void setAborted(bool abort);

    void abort();

    void abortWithError(std::string const &msg);

    bool isAborted() const;

    bool updateProgress(int progress);

    bool updateProgressMessage(std::string const &message);

/*======================================================================*/
/*! 
 *   Appends the given message to the selected output stream.
 *
 *   \param msg The message to append (may be also stream control commands)
 */
/*======================================================================*/
    template<typename DataT>
    ProgressReporterStream &operator<<(DataT const &msg);

  private:
  
    std::ostream& _os;

    int _progress, _progressMin, _progressMax;
    std::string _headerMessage, _progressMessage;
    bool _aborted;

  };

  template<typename DataT>
  ProgressReporterStream &ProgressReporterStream::operator<<(DataT const &msg)
  {
#ifdef _OPENMP
#pragma omp critical (_PROGRESSMESSAGE_IS_CURRENTLY_UPDATING_)
#endif
    {
      std::ostringstream os;
      os << msg;
      _progressMessage += os.str();
      _os << msg << std::flush;
    }
    return *this;
  }

}

#endif
