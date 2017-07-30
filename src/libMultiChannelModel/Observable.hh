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

#ifndef IROCS_OBSERVABLE_HH
#define IROCS_OBSERVABLE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <set>

namespace iRoCS {

  /*======================================================================*/
  /*!
   *  \class Observable Observable.hh <liblabelling_qt4/Observable.hh>
   *  \brief The Observable class specifies the interface for general objects
   *    that can be observed.
   */
   /*======================================================================*/
  class Observable {

  protected:

    /*======================================================================*/
    /*!
     *   Create a new Observable.
     */
     /*======================================================================*/
    Observable();

    /*======================================================================*/
    /*!
     *   Delete this observable.
     */
     /*======================================================================*/
    virtual ~Observable();

  };

}

#endif
