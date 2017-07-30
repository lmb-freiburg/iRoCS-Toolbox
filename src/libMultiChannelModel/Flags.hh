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
 * THE CONTENTS OF THIS FILE ARE ORIGINALLY TAKEN FROM THE QT SOURCES
 * (see license below)
 *
 * Adapted from:
 *
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
 *
 **************************************************************************/

#ifndef IROCS_FLAGS_HH
#define IROCS_FLAGS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#ifdef _WIN32
#if defined(MultiChannelModel_EXPORTS)
#define MULTICHANNELMODELDLL_API __declspec(dllexport)
#elif defined(MultiChannelModel_USE_DLL)
#define MULTICHANNELMODELDLL_API __declspec(dllimport)
#else
#define MULTICHANNELMODELDLL_API
#endif
#else
#define MULTICHANNNELMODELDLL_API
#endif

namespace iRoCS {

  class MULTICHANNELMODELDLL_API Flag {

    int value;

  public:

    Flag(int flagValue);
    operator int() const;

  };

  class IncompatibleFlag {

    int value;

  public:

    explicit IncompatibleFlag(int flagValue);
    operator int() const;

  };

  template<typename Enum>
  class Flags {

    typedef void **Zero;
    int value;

  public:

    typedef Enum enum_type;

    Flags(Flags const &f)
      : value(f.value) {}

    Flags(Enum f)
      : value(f) {}

    Flags(Zero = 0)
      : value(0) {}

    Flags(Flag f)
      : value(f) {}

    Flags &operator=(Flags const &f) {
      value = f.value;
      return *this;
    }

    Flags &operator&=(int mask) {
      value &= mask;
      return *this;
    }

    Flags &operator&=(unsigned int mask) {
      value &= mask;
      return *this;
    }

    Flags &operator|=(Flags f) {
      value |= f.value;
      return *this;
    }

    Flags &operator|=(Enum f) {
      value |= f;
      return *this;
    }

    Flags &operator^=(Flags f) {
      value ^= f.value;
      return *this;
    }

    Flags &operator^=(Enum f) {
      value ^= f;
      return *this;
    }

    operator int() const {
      return value;
    }

    Flags operator|(Flags f) const {
      return Flags(Enum(value | f.value));
    }

    Flags operator|(Enum f) const {
      return Flags(Enum(value | f));
    }

    Flags operator^(Flags f) const {
      return Flags(Enum(value ^ f.value));
    }

    Flags operator^(Enum f) const {
      return Flags(Enum(value ^ f));
    }

    Flags operator&(int mask) const {
      return Flags(Enum(value & mask));
    }

    Flags operator&(unsigned int mask) const {
      return Flags(Enum(value & mask));
    }

    Flags operator&(Enum f) const {
      return Flags(Enum(value & f));
    }

    Flags operator~() const {
      return Flags(Enum(~value));
    }

    bool operator!() const {
      return !value;
    }

    bool testFlag(Enum f) const {
      return (value & f) == f && (f != 0 || value == int(f));
    }

  };

}

#define IROCS_DECLARE_OPERATORS_FOR_FLAGS(FlagsName) \
inline iRoCS::Flags<FlagsName::enum_type> operator|(FlagsName::enum_type f1, FlagsName::enum_type f2) \
{ return iRoCS::Flags<FlagsName::enum_type>(f1) | f2; }                 \
inline iRoCS::Flags<FlagsName::enum_type> operator|(FlagsName::enum_type f1, iRoCS::Flags<FlagsName::enum_type> f2) \
{ return f2 | f1; } \
inline iRoCS::IncompatibleFlag operator|(FlagsName::enum_type f1, int f2)      \
{ return iRoCS::IncompatibleFlag(int(f1) | f2); }

#endif
