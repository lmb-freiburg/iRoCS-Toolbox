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

#ifndef IROCS_CHANNELCONTROLWIDGET_HH
#define IROCS_CHANNELCONTROLWIDGET_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <libMultiChannelModel/ChannelObserver.hh>
#include <QtGui/QWidget>

#include <blitz/array.h>

namespace iRoCS {

  class Channel;
  class MultichannelModelControlWidgetStack;
  class StringControlElement;
  class DoubleControlElement;
  class BoolControlElement;
  class Double3ControlElement;
  class Double4x4ControlElement;

  /*======================================================================*/
  /*!
   *  \class ChannelControlWidget ChannelControlWidget.hh <liblabelling_qt4/ChannelControlWidget.hh>
   *  \brief The ChannelControlWidget class shows channel properties in a
   *    properties form layout.
   *
   *  For annotation channels it also contains an editable marker list/table.
   *  It is mainly intended as verbose editing facility for channel metadata
   *  and annotations.
   */
   /*======================================================================*/
  class ChannelControlWidget : public QWidget, public ChannelObserver {

    Q_OBJECT

  protected:

    /*======================================================================*/
    /*!
     *   Create a new ChannelControlWidget showing editable channel meta-data
     *   and in the case of annotation channels the marker list.
     *
     *   \param parent The parent widget
     */
     /*======================================================================*/
    ChannelControlWidget(
      Channel &channel,
      MultichannelModelControlWidgetStack *controlStack = NULL,
      bool advancedMode = false);

  public:

    /*======================================================================*/
    /*!
     *   Delete this ChannelControlWidget.
     */
     /*======================================================================*/
    virtual ~ChannelControlWidget();

    /*======================================================================*/
    /*!
     *   Get the current verbosity state of thisChannelControlElement.
     *   In advanced mode all channel meta-data including read-only elements
     *   are shown.
     *
     *   \return The current verbosity state
     */
     /*======================================================================*/
    bool advancedModeEnabled() const;

    /*======================================================================*/
    /*!
     *   This method is called when the name of the observed Channel changes.
     *   Updates the QLineEdit showing the name
     */
     /*======================================================================*/
    virtual void updateName();

    /*======================================================================*/
    /*!
     *   This method is called when the filename of the observed Channel changes.
     *   Updates the QLineEdit showing the filename
     */
     /*======================================================================*/
    virtual void updateFilename();

    /*======================================================================*/
    /*!
     *   This method is called when the transparency of the observed Channel
     *   changes. Updates the value of the corresponding control element.
     */
     /*======================================================================*/
    virtual void updateAlpha();

    /*======================================================================*/
    /*!
     *   This method is called when the visibility of the observed Channel
     *   changes. Checks/Unchecks the isVisible checkbox.
     */
     /*======================================================================*/
    virtual void updateVisibilityFlag();

    /*======================================================================*/
    /*!
     *   This method is called when the modified flag of the observed Channel
     *   changes. Checks/Unchecks the isModified checkbox.
     */
     /*======================================================================*/
    virtual void updateModifiedFlag();

    /*======================================================================*/
    /*!
     *   This method is called when the voxel extents in micrometers of the
     *   observed Channel change. Updates the element size control.
     */
     /*======================================================================*/
    virtual void updateElementSizeUm();

    /*======================================================================*/
    /*!
     *   This method is called when the voxel extents in micrometers of the
     *   observed Channel change. Updates the transformation control.
     */
     /*======================================================================*/
    virtual void updateTransformation();

    /*======================================================================*/
    /*!
     *   Factory method to produce a new ChannelControlWidget of type
     *   corresponding to the given channel type.
     *
     *   \param channel The Channel to create a ChannelControlWidget for
     *   \param controlStack The MultichannelModelControlWidgetStack to create a
     *     ChannelControlWidget for
     *   \param advancedMode Toggles advanced mode view
     *
     *   \return A pointer to the new ChannelControlWidget
     */
     /*======================================================================*/
    static ChannelControlWidget *instance(
      Channel &channel,
      MultichannelModelControlWidgetStack *controlStack = NULL,
      bool advancedMode = false);

    private slots:

    // Events to Channel
    void setName(std::string const &name) const;
    void setAlpha(double alpha) const;
    void setVisibility(bool visible) const;
    void setElementSizeUm(
      blitz::TinyVector<double, 3> const &elementSizeUm) const;
    void setTransformation(
      blitz::TinyMatrix<double, 4, 4> const &trandformation) const;

  private:

    bool _advancedMode;

    StringControlElement *p_nameControl;
    StringControlElement *p_filenameControl;
    DoubleControlElement *p_alphaControl;
    BoolControlElement *p_visibleControl;
    BoolControlElement *p_modifiedControl;
    Double3ControlElement *p_elementSizeUmControl;
    Double4x4ControlElement *p_transformationControl;

  };

}

#endif
