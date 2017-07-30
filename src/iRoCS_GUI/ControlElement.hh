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

 /*======================================================================*/
 /*!
  *  \file ControlElement.hh
  *  \brief Abstract horizontally layouted control element consisting of a
  *    label, the control widget(s) and a checkbox that allows to
  *    activate/inactivate the element.
  */
  /*======================================================================*/

#ifndef IROCS_CONTROLELEMENT_HH
#define IROCS_CONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QWidget>

#include <string>

class QHBoxLayout;
class QLabel;
class QCheckBox;

namespace iRoCS {

  /*======================================================================*/
  /*!
   *  \class ControlElement ControlElement.hh <liblabelling_qt4/ControlElement.hh>
   *  \brief The ControlElement class specifies the interface for arbitrary
   *    input widgets with label and activation check box.
   */
   /*======================================================================*/
  class ControlElement : public QWidget {

    Q_OBJECT

  public:

    /*======================================================================*/
    /*!
     *   Create a new ControlElement. This sets up the layout and creates
     *   activation checkbox and label. The actual control must be added to
     *   the layout by the respective implementation.
     *
     *   \param label The Label that is shown in front of the control
     *   \param parent The parent widget.
     */
     /*======================================================================*/
    ControlElement(QString const &label = "", QWidget *parent = NULL);

    /*======================================================================*/
    /*!
     *   Delete this ControlElement
     */
     /*======================================================================*/
    virtual ~ControlElement();

    /*======================================================================*/
    /*!
     *   If desired you can show/hide a checkbox to manually activate
     *   the control by passing true/false to this method.
     *
     *   \param switchable If true is given a checkbox is shown in front of the
     *     control that allows to manually enable/disable the control.
     */
     /*======================================================================*/
    void setSwitchable(bool switchable);

    /*======================================================================*/
    /*!
     *   If the ControlElement is manually switchable \c true is returned.
     *
     *   \return \c true if the control element can be manually enabled/disabled
     *     via a visible checkbox \c false otherwise
     */
     /*======================================================================*/
    bool switchable() const;

    /*======================================================================*/
    /*!
     *   Set this ControlElement active or inactive. Active control elements
     *   allow editing, inactive controls are read-only. If the control element
     *   is switchable, this sets the state of the shown checkbox.
     *
     *   \param active The activation state of the control element
     */
     /*======================================================================*/
    void setActive(bool active);

    /*======================================================================*/
    /*!
     *   Get the activation state of this control element. Inactive controls
     *   are read-only. The returned state equals the checkbox state of the
     *   manual switch to activate/inactivate the control element if it is
     *   shown.
     *
     *   \param active The activation state of the control element
     */
     /*======================================================================*/
    bool isActive() const;

    /*======================================================================*/
    /*!
     *   Set the shown label of the ControlElement.
     *
     *   \param label The control element's new label
     */
     /*======================================================================*/
    void setLabelString(QString const &label);

    /*======================================================================*/
    /*!
     *   Get the shown label of the ControlElement.
     *
     *   \return The control element's label
     */
     /*======================================================================*/
    QString labelString() const;

    /*======================================================================*/
    /*!
     *   Get the QLabel of the ControlElement.
     *
     *   \return The control element's QLabel
     */
     /*======================================================================*/
    QWidget *label() const;


    /*======================================================================*/
    /*!
     *   Get the actual control widget of this ControlElement.
     *
     *   \return The widget that shows and allows editing of the ControlElement's
     *     value
     */
     /*======================================================================*/
    QWidget *controlWidget() const;

    /*======================================================================*/
    /*!
     *   Get a string representation of the currently set value in the
     *   ControlElement.
     *
     *   \return The value as string
     */
     /*======================================================================*/
    virtual std::string toStdString() const = 0;

  signals:

    /*======================================================================*/
    /*!
     *   This signal is emitted whenever the ControlElement changes its
     *   activation state. The change can be triggered by calling
     *   setActive(bool) or by toggling the state of the activation
     *   checkbox.
     *
     *   \param state The current activation state
     */
     /*======================================================================*/
    void activationStateChanged(bool state);

    /*======================================================================*/
    /*!
     *   This signal is emitted whenever the value of this control element
     *   changes. The change can be triggered by calling
     *   setValue(bool) or by editing the values in the control element.
     *   Text-based control elements should emit this signal only when
     *   editing is finished.
     */
     /*======================================================================*/
    void valueChanged();

    public slots:

    /*======================================================================*/
    /*!
     *   Set the value of this ControlElement from a string representation.
     *
     *   \param value The new value as string
     */
     /*======================================================================*/
    virtual void setValue(std::string const &value) = 0;

    protected slots:

    virtual void emitValueChanged();

  protected:

    /*======================================================================*/
    /*!
     *   Set the actual control widget of this ControlElement. This method
     *   should be called only once during construction of a concrete
     *   ControlElement. If the ControlElement already has a control widget,
     *   the old control widget will be removed from the layout but not deleted.
     *
     *   \param widget The widget that shows and allows editing of the
     *     ControlElement's value
     */
     /*======================================================================*/
    void setControlWidget(QWidget *widget);

  private:

    bool _switchable;

    QWidget *p_controlWidget;
    QHBoxLayout *p_layout;
    QWidget *p_switchableLabelWidget;
    QHBoxLayout *p_switchableLabelLayout;
    QCheckBox *p_switch;
    QLabel *p_label;


  };

}

#endif
