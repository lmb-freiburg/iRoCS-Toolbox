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

#ifndef IROCS_DOUBLECONTROLELEMENT_HH
#define IROCS_DOUBLECONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ControlElement.hh"

#include <blitz/array.h>

class QDoubleSpinBox;
class QSlider;

namespace iRoCS {

  /*======================================================================*/
  /*!
   *  \class DoubleControlElement DoubleControlElement.hh <liblabelling_qt4/DoubleControlElement.hh>
   *  \brief The DoubleControlElement class implements the ControlElement
   *  interface and allows editing of floating point variables with double
   *  precision.
   */
   /*======================================================================*/
  class DoubleControlElement : public ControlElement {

    Q_OBJECT

  public:

    /*======================================================================*/
    /*!
     *   Create a new DoubleControlElement. The control element consists of
     *   a QDoubleSpinBox to edit the double value.
     *   At construction the spinbox has a range of (-infinity, infinity), a
     *   single step of 0.1 and a precision of 3 digits.
     *
     *   \param label  The Label that is shown in front of the control
     *   \param value  The initial value
     *   \param parent The parent widget.
     */
     /*======================================================================*/
    DoubleControlElement(
      QString const &label = "", double value = 0.0, QWidget *parent = NULL);

    /*======================================================================*/
    /*!
     *   Delete this DoubleControlElement.
     */
     /*======================================================================*/
    ~DoubleControlElement();

    /*======================================================================*/
    /*!
     *   Show/Hide an additional slider to edit the double value. The slider
     *   is setup horizontally to the left of the shown spin box.
     *
     *   \param enable The new visibility state of the slider
     */
     /*======================================================================*/
    void setSliderVisible(bool enable);

    /*======================================================================*/
    /*!
     *   Check whether this control element shows a slider on the left of the
     *   spin box.
     *
     *   \return \c true if the slider is visible, \c false otherwise
     */
     /*======================================================================*/
    bool sliderVisible() const;

    /*======================================================================*/
    /*!
     *   Set the value range for the control element.
     *
     *   \param min The minimum value for each vector component
     *   \param max The maximum value for each vector component
     */
     /*======================================================================*/
    void setRange(double min, double max);

    /*======================================================================*/
    /*!
     *   Set the single step that is performed when pushing the up/down buttons
     *   on the spinbox.
     *
     *   \param step The step lengths for up/down stepping of the spin box
     */
     /*======================================================================*/
    void setSingleStep(double step);

    /*======================================================================*/
    /*!
     *   Set the shown number of digits (precision) in the spinbox.
     *
     *   \param decimals The shown number of decimals
     */
     /*======================================================================*/
    void setDecimals(int decimals);

    /*======================================================================*/
    /*!
     *   If specified, the given text is shown if the spin box is at its
     *   minimum value instead of that value. If you pass a NULL (empty) string
     *   it will be reset to show the value again.
     *
     *   \param text The text to output instead of the spinbox's minimum value
     */
     /*======================================================================*/
    void setSpecialValueText(QString const &text);

    /*======================================================================*/
    /*!
     *   If specified, the given text is appended to the value in the spinbox.
     *
     *   \param text The text to append (e.g. unit)
     */
     /*======================================================================*/
    void setSuffix(QString const &text);

    /*======================================================================*/
    /*!
     *   Get the current value of the double precision floating point number this
     *   control is showing. The full precision value is returned. If edited
     *   the precision is truncated to the given number of digits.
     *
     *   \return The current value
     */
     /*======================================================================*/
    double value() const;

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
    void valueChanged(double value);

    public slots:

    /*======================================================================*/
    /*!
     *   Set the value of this DoubleControlElement. Values exceeding the
     *   spinbox range will be clipped.
     *
     *   \param value The new value
     */
     /*======================================================================*/
    void setValue(double value);

    /*======================================================================*/
    /*!
     *   Set the value of this ControlElement from a string representation.
     *
     *   \param value The new value as string
     */
     /*======================================================================*/
    void setValue(std::string const &value);

    private slots:

    void _setValue(double value);
    void updateSpinBox(int value);
    void updateSlider(double value);
    void emitValueChanged();

  private:

    // This is the actual full precision double
    double _value;

    // This is the spinbox showing the double in lower precision
    QDoubleSpinBox *p_spinbox;
    QSlider* p_slider;

  };

}

#endif
