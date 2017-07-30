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

#ifndef STRINGCONTROLELEMENT_HH
#define STRINGCONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ControlElement.hh"

class QLineEdit;

namespace iRoCS {

  /*======================================================================*/
  /*!
   *  \class StringControlElement StringControlElement.hh <liblabelling_qt4/StringControlElement.hh>
   *  \brief The StringControlElement class implements the ControlElement
   *  interface and allows editing of strings in a QLineEdit.
   */
   /*======================================================================*/
  class StringControlElement : public ControlElement {

    Q_OBJECT

  public:

    /*======================================================================*/
    /*!
     *   Create a new StringControlElement. The control element consists of
     *   a QLineEdit to edit the string.
     *
     *   \param label  The Label that is shown in front of the control
     *   \param value  The initial value
     *   \param parent The parent widget.
     */
     /*======================================================================*/
    StringControlElement(
      QString const &label = "", std::string const &value = std::string(),
      QWidget *parent = NULL);

    /*======================================================================*/
    /*!
     *   Delete this StringControlElement.
     */
     /*======================================================================*/
    ~StringControlElement();

    /*======================================================================*/
    /*!
     *   Get the current value of the string this control is showing.
     *
     *   \return The current value
     */
     /*======================================================================*/
    std::string value() const;

    /*======================================================================*/
    /*!
     *   Get the current value of the string this control is showing.
     *
     *   \return The current value
     */
     /*======================================================================*/
    std::string toStdString() const;

    public slots:

    /*======================================================================*/
    /*!
     *   Set the value of the control to the given string.
     *
     *   \param value The new string
     */
     /*======================================================================*/
    void setValue(const std::string& value);

    private slots:

    void emitValueChange();

  signals:

    void valueChanged(std::string const &);

  private:

    QLineEdit* p_lineedit;

  };

}

#endif
