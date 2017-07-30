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

#ifndef IROCS_MULTICHANNELMODELOBSERVER_HH
#define IROCS_MULTICHANNELMODELOBSERVER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <cstddef>

namespace iRoCS
{

  class MultichannelModel;
  class Channel;

/*======================================================================*/
/*!
 *  \class MultichannelModelObserver MultichannelModelObserver.hh <liblabelling_qt4/MultichannelModelObserver.hh>
 *  \brief The MultichannelModelObserver class specifies the interface
 *    that must be implemented by classes that shall be notified at model
 *    changes.
 *
 *  This class interfaces the pure C++ MultichannelModel class to QT
 *  view- and control widgets. The class is pure virtual, therefore it
 *  must be sub-classed and corresponding functionality implemented.
 */
/*======================================================================*/
  class MultichannelModelObserver
  {
    
  public:

/*======================================================================*/
/*! 
 *   Create a new MultichannelModelObserver object. 
 */
/*======================================================================*/
    MultichannelModelObserver(MultichannelModel &model);

/*======================================================================*/
/*! 
 *   Destroy this MultichannelModelObserver.
 *
 *   If the observer is registered to a model at time of destruction,
 *   it will be unregistered from the MultichannelModel first.
 */
/*======================================================================*/
    virtual ~MultichannelModelObserver();
    
/*======================================================================*/
/*! 
 *   This method is called <b>after</b> a channel was added to the
 *   MultichannelModel.
 *
 *   An implementation should insert a corresponding entry to the Observer's
 *   view.
 *
 *   If updates for this observer are disabled, calling this method is a
 *   noop.
 *
 *   \param index A pointer to the Channel that was added to the model
 */
/*======================================================================*/
    virtual void updateAddChannel(Channel *channel) = 0;

/*======================================================================*/
/*! 
 *   This method is called <b>before</b> a channel is deleted from the
 *   MultichannelModel.
 *
 *   An implementation must remove the corresponding entry from the
 *   Observer's view. Immediately after execution of this method the
 *   Channel is deleted, so make sure that the Observer is not able to
 *   accidentally access it later.
 *
 *   If updates for this observer are disabled, calling this method is a
 *   noop.
 *
 *   \param index A pointer to the Channel that will be removed from the
 *     model
 */
/*======================================================================*/
    virtual void updateRemoveChannel(Channel *channel) = 0;

/*======================================================================*/
/*! 
 *   This method is called when the modified flag of the observed
 *   MultichannelModel changes. Corresponding views should be updated
 *   accordingly.
 */
/*======================================================================*/
    virtual void updateModified(bool modified) = 0;

/*======================================================================*/
/*! 
 *   This method updates the complete view. Call it for intialization or
 *   after extended model changes.
 */
/*======================================================================*/
    virtual void updateModel() = 0;
    
  protected:

    MultichannelModel &_model;
    
  };

}

#endif
