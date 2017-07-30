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

#include "StringControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>

#include <iostream>

namespace iRoCS {

  StringControlElement::StringControlElement(
    QString const &label, std::string const &value, QWidget* parent)
    : ControlElement(label, parent), p_lineedit(NULL) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<StringControlElement::StringControlElement "
      << "this=\"" << this << "\" labelString=\"" << label.toStdString()
      << "\" value=\"" << value << "\">" << std::endl;
#endif
    QWidget *controls = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout(controls);
    layout->setContentsMargins(0, 0, 0, 0);
    p_lineedit = new QLineEdit;
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "p_linedit = new QLineEdit (" << p_lineedit << ")"
      << std::endl;
#endif
    p_lineedit->setText(value.c_str());
    layout->addWidget(p_lineedit);
    connect(p_lineedit, SIGNAL(editingFinished()), SLOT(emitValueChange()));
    setControlWidget(controls);
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</StringControlElement::StringControlElement>" << std::endl;
#endif
  }

  StringControlElement::~StringControlElement() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<StringControlElement::~StringControlElement "
      << "this=\"" << this << "\" labelString=\""
      << labelString().toStdString() << "\" value=\"" << value()
      << "\" p_lineedit=\"" << p_lineedit
      << "\"></StringControlElement::~StringControlElement>"
      << std::endl;
#endif
  }

  std::string StringControlElement::value() const {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<StringControlElement::value "
      << "this=\"" << this << "\"" << std::flush;
    std::cerr << " labelString=\""
      << labelString().toStdString() << "\"" << std::flush;
    std::cerr << " value=\""
      << p_lineedit->text().toStdString() << "\"" << std::flush;
    std::cerr << " p_lineedit=\"" << p_lineedit
      << "\"></StringControlElement::value>"
      << std::endl;
#endif
    return p_lineedit->text().toStdString();
  }

  std::string StringControlElement::toStdString() const {
    return value();
  }

  void StringControlElement::setValue(const std::string& value) {
    p_lineedit->setText(value.c_str());
  }

  void StringControlElement::emitValueChange() {
    emit ControlElement::valueChanged();
    emit valueChanged(value());
  }

}
