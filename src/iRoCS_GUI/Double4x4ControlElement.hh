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

#ifndef IROCS_DOUBLE4X4CONTROLELEMENT_HH
#define IROCS_DOUBLE4X4CONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ControlElement.hh"

#include <blitz/array.h>

#include <libArrayToolbox/TypeTraits.hh>

class QDoubleSpinBox;

namespace iRoCS {

  /*======================================================================*/
  /*!
   *  \class Double4x4ControlElement Double4x4ControlElement.hh <liblabelling_qt4/Double4x4ControlElement.hh>
   *  \brief The Double4x4ControlElement class implements the ControlElement
   *  interface and allows editing of blitz::TinyMatrix<double,4,4> variables.
   */
   /*======================================================================*/
  class Double4x4ControlElement : public ControlElement {

    Q_OBJECT

  public:

    /*======================================================================*/
    /*!
     *   Create a new Double4x4ControlElement. The control element consists of
     *   16 QDoubleSpinBox widgets laid out in a 4x4 array to edit the
     *   components of a 4x4 matrix.
     *   At construction each spinbox has a range of (-infinity, infinity), a
     *   single step of 0.1 and a precision of 3 digits.
     *
     *   \param label  The Label that is shown in front of the control
     *   \param value  The initial value
     *   \param parent The parent widget
     */
     /*======================================================================*/
    Double4x4ControlElement(
      QString const &label = "",
      blitz::TinyMatrix<double, 4, 4> const &value =
      (atb::traits< blitz::TinyMatrix<double, 4, 4> >::one),
      QWidget *parent = NULL);

    /*======================================================================*/
    /*!
     *   Delete this Double4x4ControlElement
     */
     /*======================================================================*/
    ~Double4x4ControlElement();

    /*======================================================================*/
    /*!
     *   Set the value range for the 16 vector components.
     *
     *   \param min The minimum value for each vector component
     *   \param max The maximum value for each vector component
     */
     /*======================================================================*/
    void setRange(
      blitz::TinyMatrix<double, 4, 4> const &min,
      blitz::TinyMatrix<double, 4, 4> const &max);

    /*======================================================================*/
    /*!
     *   Set the single step that is performed when pushing the up/down buttons
     *   on the spinboxes.
     *
     *   \param step The step lengths for up/down stepping of the spin boxes
     */
     /*======================================================================*/
    void setSingleStep(blitz::TinyMatrix<double, 4, 4> const &step);

    /*======================================================================*/
    /*!
     *   Set the shown number of digits (precision) in the spinboxes.
     *
     *   \param decimals The shown number of decimals
     */
     /*======================================================================*/
    void setDecimals(blitz::TinyMatrix<int, 4, 4> const &decimals);

    /*======================================================================*/
    /*!
     *   Get the current value of the 4x4 double matrix this
     *   control is showing. The full precision matrix is returned. If edited
     *   the precision is truncated to the given number of digits.
     *
     *   \return The current values of the matrix
     */
     /*======================================================================*/
    blitz::TinyMatrix<double, 4, 4> const &value() const;

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
    void valueChanged(blitz::TinyMatrix<double, 4, 4> const &value);

    public slots:

    /*======================================================================*/
    /*!
     *   Set the value of this Double4x4ControlElement. Values exceeding the
     *   spinbox ranges will be clipped.
     *
     *   \param value The new value
     */
     /*======================================================================*/
    void setValue(blitz::TinyMatrix<double, 4, 4> const &value);

    /*======================================================================*/
    /*!
     *   Set the value of this ControlElement from a string representation.
     *
     *   \param value The new value as string
     */
     /*======================================================================*/
    void setValue(std::string const &value);

    private slots:

    void setValue(double value);
    void emitValueChanged();

  private:

    // These are the actual full precision doubles
    blitz::TinyMatrix<double, 4, 4> _value;

    // These are the spinboxes showing the doubles in lower precision
    blitz::TinyMatrix<QDoubleSpinBox*, 4, 4> _spinbox;

  };

}

#endif
