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

#ifndef IROCS_MULTICHANNELMODELTREEWIDGET_HH
#define IROCS_MULTICHANNELMODELTREEWIDGET_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QTreeWidget>
#include "MultichannelModelObserver.hh"

#include <map>

class QTreeWidget;
class QTreeWidgetItem;
class QAction;

namespace iRoCS {

  /*======================================================================*/
  /*!
   *  \class MultichannelModelTreeWidget MultichannelModelTreeWidget.hh <liblabelling_qt4/MultichannelModelTreeWidget.hh>
   *  \brief The MultichannelModelTreeWidget class is a MultiChannelModel
   *    viewer and editor.
   *
   *  It implements the MultichannelModelObserver interface and is therefore
   *  notified if the underlying model changes. It is a QTreeWidget of the
   *  model in a hierarchical structure as it will be stored to an hdf5 file.
   */
   /*======================================================================*/
  class MultichannelModelTreeWidget
    : public QTreeWidget, public MultichannelModelObserver {

    Q_OBJECT

  public:

    /*======================================================================*/
    /*!
     *   Create a new MultichannelModelTreeWidget object.
     */
     /*======================================================================*/
    MultichannelModelTreeWidget(
      MultichannelModel &model, QWidget *parent = NULL);

    /*======================================================================*/
    /*!
     *   Delete this MultichannelModelTreeWidget and all children.
     */
     /*======================================================================*/
    ~MultichannelModelTreeWidget();

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
     *   This method updates the complete view. Call it for intialization or
     *   after extended model changes.
     */
     /*======================================================================*/
    void updateModel();

    /*======================================================================*/
    /*!
     *   Adjust the column widths so that the content is visible.
     *   Use this after changing channel names.
     */
     /*======================================================================*/
    void adjustColumnWidths();

  signals:

    void isEmptyChanged(bool isEmpty);
    void modifiedFlagChanged(bool modified);

    public slots:

    /*======================================================================*/
    /*!
     *   Delete all selected channels from the MultichannelModel.
     */
     /*======================================================================*/
    void deleteSelectedChannels();

    private slots:

    void updateModel(QTreeWidgetItem *item, int column);
    void handleEditRequest(QTreeWidgetItem *item, int column);
    void handleCustomContextMenuRequest(QPoint const &pos);
    void showChannelProperties(Channel *channel);

  private:

    void dropEvent(QDropEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

  };

}

#endif
