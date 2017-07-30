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

#include "StreamBufTextEditQt4.hh"

#include <libArrayToolbox/ATBTiming.hh>

StreamBufTextEditQt4::StreamBufTextEditQt4(QTextEdit *out)
  : QObject(), std::streambuf(), _insertTimeStamp(true), p_out(out) {
  p_out->clear();

  // No buffer at all, directly write the characters to the output
  setp(NULL, NULL);

  connect(this, SIGNAL(textAdded(QString const &)),
    SLOT(appendTextToTextEdit(QString const &)));
}

StreamBufTextEditQt4::~StreamBufTextEditQt4() {}

StreamBufTextEditQt4::int_type StreamBufTextEditQt4::overflow(int_type ch) {
  if (ch != traits_type::eof()) {
    if (traits_type::to_char_type(ch) == '\n') _insertTimeStamp = true;
    else {
      if (_insertTimeStamp) {
        emit textAdded((atb::MyDateTime::prettyDate() + " - ").c_str());
        _insertTimeStamp = false;
      }
    }
    QString outString(
      std::string(1, traits_type::to_char_type(ch)).c_str());
    emit textAdded(outString);
    return traits_type::to_int_type(true);
  }
  return traits_type::eof();
}

void StreamBufTextEditQt4::appendTextToTextEdit(QString const &text) {
  p_out->moveCursor(QTextCursor::End);
  p_out->insertPlainText(text);
  p_out->ensureCursorVisible();
}

StreamBufTextEditQt4::StreamBufTextEditQt4(StreamBufTextEditQt4 const &)
  : QObject(), std::streambuf(), p_out(NULL) {}

StreamBufTextEditQt4 &StreamBufTextEditQt4::operator=(
  StreamBufTextEditQt4 const &) {
  return *this;
}

