/**************************************************************************
 *
 * This file belongs to the iRoCS Toolbox.
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

/*======================================================================*/
/*!
 *  \file PluginInterface.hh
 *  \brief labelling plugin interface.
 */
/*======================================================================*/

#ifndef PLUGININTERFACE_HH
#define PLUGININTERFACE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <map>
#include <string>

#include <QtCore/QtPlugin>
#include <QtCore/QString>

class LabellingMainWidget;

/*======================================================================*/
/*!
 *  \class PluginInterface PluginInterface.hh <liblabelling_qt4/PluginInterface.hh>
 *  \brief The abstract plugin interface all plugins must inherit from.
 */
/*======================================================================*/
class PluginInterface
{

public:
  
/*======================================================================*/
/*!
 *  Plugin destructor.
 *
 *    Plugins must clean-up all ressources here.
 */
/*======================================================================*/
  virtual ~PluginInterface()
        {}
  
/*======================================================================*/
/*!
 *  Get the plugin name.
 *
 *  Sub-classes must implement this method and return a short plugin title
 *  string. The given string will be shown in the Plugins menu.
 */
/*======================================================================*/
  virtual QString name() const = 0;

/*======================================================================*/
/*!
 *  Set the LabellingMainWidget this plugin will act on.
 *
 *  Sub-classes must implement this method. Typically they just store the
 *  given pointer s.t. it can be used in the run() method.
 */
/*======================================================================*/
  virtual void setLabellingMainWidget(LabellingMainWidget* parent) = 0;

/*======================================================================*/
/*!
 *  Set plugin-specific parameters.
 *
 *  Sub-classes must implement this method. The given map contains
 *  attribute-value pairs that have to be parsed and remapped to
 *  plugin-specific parameters. The purpose of this method is to allow
 *  writing of macros in a later version. Currently it is not used
 *  by labelling, and therefore the implementation can be left empty
 *  without loss of function.
 *
 *  \todo Implement macro-like pipeline execution
 */
/*======================================================================*/
  virtual void setParameters(
      const std::map<std::string,std::string>& parameters) = 0;

/*======================================================================*/
/*!
 *  Run the plugin.
 *
 *  Sub-classes must implement this method. It contains the plugin body
 *  which can be a simple sequence of commands or more complicated
 *  interactive steps.
 *
 *  Typically it first opens a dialog asking the user to provide parameters
 *  for the plugin. Then it does the work and returns.
 *
 *  For long-running jobs a separate worker thread should be started that
 *  processes the data. Currently the GUI thread must be put to sleep
 *  during execution. Labelling is not thread-safe, therefore it is also a
 *  good idea to disable active control elements during processing especially
 *  to avoid execution of multiple plugins in parallel which will almost
 *  certainly crash labelling. This is done using
 *  LabellingMainWidget::setUserInteractionEnabled(bool). Long-running
 *  plugins should show their progress using
 *  LabellingMainWidget::progressReporter(). It is good practize to call
 *  QCoreApplication::processEvents() every 10 milliseconds to keep the
 *  GUI responsive.
 *
 *  If the plugin creates new items like channels, markers or the like
 *  make sure that the thread affinity for all created objects is passed to
 *  the GUI thread before the worker thread ends. If possible generate
 *  such structures in the GUI thread directly.
 */
/*======================================================================*/
  virtual void run() = 0;


/*======================================================================*/
/*!
 *  Abort the plugin.
 *
 *  If the worker thread uses the iRoCS::ProgressReporter of
 *  LabellingMainWidget this method should set its state to aborted using
 *  iRoCS::ProgressReporter::abort(). If the run() method regularly polls
 *  the progress reporter state it can react accordingly and gracefully
 *  end the thread and restore the old state or perform other actions.
 *  Progress reporting is using a thread-safe SIGNAL-SLOT connection,
 *  therefore all progress reporting and reactions on abort can be done in
 *  the worker thread.
 */
/*======================================================================*/
  virtual void abort() = 0;

};

Q_DECLARE_INTERFACE(PluginInterface,
                    "lmbsoft.labelling_qt4.PluginInterface/1.0")

#endif
