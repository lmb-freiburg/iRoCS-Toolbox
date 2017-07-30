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

#ifndef IROCS_DOUBLE3CONTROLELEMENT_HH
#define IROCS_DOUBLE3CONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ControlElement.hh"

#include <blitz/array.h>

class QDoubleSpinBox;

namespace iRoCS {

  /*======================================================================*/
  /*!
   *  \class Double3ControlElement Double3ControlElement.hh <liblabelling_qt4/Double3ControlElement.hh>
   *  \brief The Double3ControlElement class implements the ControlElement
   *  interface and allows editing of blitz::TinyVector<double,3> variables.
   */
   /*======================================================================*/
  class Double3ControlElement : public ControlElement {

    Q_OBJECT

  public:

    /*======================================================================*/
    /*!
     *   Create a new Double3ControlElement. The control element consists of
     *   three QDoubleSpinBox widgets to edit the components of a 3D vector.
     *   At construction each spinbox has a range of (-infinity, infinity), a
     *   single step of 0.1 and a precision of 3 digits.
     *
     *   \param label  The Label that is shown in front of the control
     *   \param value  The initial value
     *   \param parent The parent widget.
     */
     /*======================================================================*/
    Double3ControlElement(
      QString const &label = "",
      blitz::TinyVector<double, 3> const &value = 0.0,
      QWidget *parent = NULL);

    /*======================================================================*/
    /*!
     *   Delete this Double3ControlElement
     */
     /*======================================================================*/
    ~Double3ControlElement();

    /*======================================================================*/
    /*!
     *   Set the value range for the three vector components.
     *
     *   \param min The minimum value for each vector component
     *   \param max The maximum value for each vector component
     */
     /*======================================================================*/
    void setRange(
      blitz::TinyVector<double, 3> const &min,
      blitz::TinyVector<double, 3> const &max);

    /*======================================================================*/
    /*!
     *   Set the single step that is performed when pushing the up/down buttons
     *   on the spinboxes.
     *
     *   \param step The step lengths for up/down stepping of the spin boxes
     */
     /*======================================================================*/
    void setSingleStep(blitz::TinyVector<double, 3> const &step);

    /*======================================================================*/
    /*!
     *   Set the shown number of digits (precision) in the spinboxes.
     *
     *   \param decimals The shown number of decimals
     */
     /*======================================================================*/
    void setDecimals(blitz::TinyVector<int, 3> const &decimals);

    /*======================================================================*/
    /*!
     *   Get the current value of the three-element double vector this
     *   control is showing. The full precision vector is returned. If edited
     *   the precision is truncated to the given number of digits.
     *
     *   \return The current values of the vector
     */
     /*======================================================================*/
    blitz::TinyVector<double, 3> const &value() const;

    /*======================================================================*/
    /*!
     *   Get a string representation of the currently set value in the
     *   ControlElement.
     *
     *   \return The value as string
     */
     /*======================================================================*/
    std::string toStdString() const;

  signals:

    /*======================================================================*/
    /*!
     *   This signal is emitted whenever the value of this control element
     *   changes. The change can be triggered by calling
     *   setValue(bool) or by editing the values in the control element.
     */
     /*======================================================================*/
    void valueChanged(blitz::TinyVector<double, 3> const &value);

    public slots:

    /*======================================================================*/
    /*!
     *   Set the value of this Double3ControlElement. Values exceeding the
     *   spinbox ranges will be clipped.
     *
     *   \param value The new value
     */
     /*======================================================================*/
    void setValue(const blitz::TinyVector<double, 3>& value);

    /*======================================================================*/
    /*!
     *   Set the value of this ControlElement from a string representation.
     *
     *   \param value The new value as string
     */
     /*======================================================================*/
    void setValue(const std::string& value);

    private slots:

    void setValue(double value);
    void emitValueChanged();

  private:

    // These are the actual full precision doubles
    blitz::TinyVector<double, 3> _value;

    // These are the spinboxes showing the doubles in lower precision
    blitz::TinyVector<QDoubleSpinBox*, 3> _spinbox;

  };

}

#endif
