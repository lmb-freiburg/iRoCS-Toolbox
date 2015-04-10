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

#ifndef STREAMBUFTEXTEDITQT4_HH
#define STREAMBUFTEXTEDITQT4_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <streambuf>

#include <QtCore/QObject>
#include <QtGui/QTextEdit>

class StreamBufTextEditQt4 : public QObject, public std::streambuf
{
  
  Q_OBJECT
  
  public:
  
  explicit StreamBufTextEditQt4(QTextEdit *out);
  ~StreamBufTextEditQt4();
  
signals:
  
  void textAdded(QString const &text);
                                     
private slots:
  
  void appendTextToTextEdit(QString const &text);
  
private:
  
  StreamBufTextEditQt4::int_type overflow(int_type ch);
  
  StreamBufTextEditQt4(StreamBufTextEditQt4 const &);
  StreamBufTextEditQt4 &operator=(StreamBufTextEditQt4 const &);
  
  bool _insertTimeStamp;
  QTextEdit *p_out;
  
};

#endif
