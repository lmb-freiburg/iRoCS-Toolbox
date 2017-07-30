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

#ifndef IROCS_MULTICHANNELMODELCONTROLWIDGETSTACK_HH
#define IROCS_MULTICHANNELMODELCONTROLWIDGETSTACK_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QStackedWidget>
#include "MultichannelModelObserver.hh"

namespace iRoCS {

  class ChannelControlWidget;

  /*======================================================================*/
  /*!
   *  \class MultichannelModelControlWidgetStack MultichannelModelControlWidgetStack.hh <liblabelling_qt4/MultichannelModelControlWidgetStack.hh>
   *  \brief The MultichannelModelControlWidgetStack class is a MultiChannelModel
   *    viewer and editor.
   *
   *  It implements the MultichannelModelObserver interface and is therefore
   *  notified if the underlying model changes. It is a QStackedWidget
   *  containing ChannelControlWidget instances.
   */
   /*======================================================================*/
  class MultichannelModelControlWidgetStack
    : public QStackedWidget, public MultichannelModelObserver {

    Q_OBJECT

  public:

    /*======================================================================*/
    /*!
     *   Create a new MultichannelModelControlWidgetStack object.
     */
     /*======================================================================*/
    MultichannelModelControlWidgetStack(
      MultichannelModel &model, QWidget *parent = NULL);

    /*======================================================================*/
    /*!
     *   Destroy this MultichannelModelControlWidgetStack.
     *
     *   If the observer is registered to a model at time of destruction,
     *   it will be unregistered from the MultichannelModel first.
     */
     /*======================================================================*/
    ~MultichannelModelControlWidgetStack();

    /*======================================================================*/
    /*!
     *   This method is called <b>after</b> a channel was added to the
     *   MultichannelModel.
     *
     *   An implementation should insert a corresponding entry to the Observer's
     *   view.
     *
     *   \param index A pointer to the Channel that was added to the model
     */
     /*======================================================================*/
    void updateAddChannel(Channel *channel);

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
     *   \param index A pointer to the Channel that will be removed from the
     *     model
     */
     /*======================================================================*/
    void updateRemoveChannel(Channel *channel);

    /*======================================================================*/
    /*!
     *   This method is called when the modified flag of the observed
     *   MultichannelModel changes. Corresponding views should be updated
     *   accordingly.
     */
     /*======================================================================*/
    void updateModified(bool modified);

    /*======================================================================*/
    /*!
     *   Use this method to force a full revalidation of the MultichannelModel.
     *   This method is called when the model pointer changes but you can also
     *   call it directly to force a reinitialization of the Observer.
     */
     /*======================================================================*/
    void updateModel();

  };

}

#endif
