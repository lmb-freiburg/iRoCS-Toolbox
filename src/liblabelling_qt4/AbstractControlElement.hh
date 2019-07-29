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

/*======================================================================*/
/*!
 *  \file AbstractControlElement.hh
 *  \brief Abstract horizontally layouted control element consisting of a
 *    label, the control widget(s) and a checkbox that allows to
 *    enable and disable the element.
 */
/*======================================================================*/

#ifndef ABSTRACTCONTROLELEMENT_HH
#define ABSTRACTCONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QWidget>

#include <string>

class QHBoxLayout;
class QLabel;
class QCheckBox;

class AbstractControlElement : public QWidget
{

  Q_OBJECT

public:

  AbstractControlElement(
      const QString& label, QWidget* parent = 0);
  virtual ~AbstractControlElement();

  void setSwitchable(const bool switchable);
  bool switchable() const;

  void setActive(const bool active);
  bool isActive() const;

  QString label() const;
  void setLabel(const QString& label);

  virtual std::string toStdString() const = 0;

  QWidget *labelWidget();
  QWidget *controlWidget();

signals:

  void activationStateChanged(bool);
  void valueChanged();

public slots:

  virtual void setValue(const std::string& value) = 0;

protected:

  QHBoxLayout *p_labelLayout;
  QHBoxLayout *p_controlLayout;

  QCheckBox* p_switch;
  QLabel* p_label;

private:

  QWidget *p_labelWidget;
  QWidget *p_controlWidget;

};

#endif
