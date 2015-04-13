/**************************************************************************
 *
 * Copyright (C) 2015 Kun Liu, Thorsten Falk
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

/*
 * lParallel.hh
 *
 *  Created on: Nov 26, 2010
 *      Author: liu
 */

#ifndef LPARALLEL_HH_
#define LPARALLEL_HH_

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <libArrayToolbox/TypeTraits.hh>

template<typename Type, int Dim>
void pClear(blitz::Array<Type,Dim> &a);

template<typename Type, int Dim>
void pClip(blitz::Array<Type,Dim> &a, Type L, Type H);

template<typename Typea, typename Typeb, int Dim>
void pCopy(blitz::Array<Typea,Dim> const &a, blitz::Array<Typeb,Dim> &b);

template<typename Typea, typename Typeb, typename Typec, int Dim>
void pMul(
    blitz::Array<Typea,Dim> const &a, blitz::Array<Typeb,Dim> const &b,
    blitz::Array<Typec,Dim> &c);

template<typename Typea, typename Typeb, int Dim>
void
pScale(blitz::Array<Typea,Dim> &a, Typeb b);

template<typename Typea, typename Typeb, int Dim>
void pMul(blitz::Array<Typea,Dim> &a, blitz::Array<Typeb,Dim> const &b);

template<typename Typea, typename Typeb, int Dim>
void pABS(blitz::Array<Typea,Dim> const &a, blitz::Array<Typeb,Dim> &b);

template<typename Typea, int Dim>
void pExp(blitz::Array<Typea,Dim> &a, double c);

template<typename Typea, int Dim>
void pEigBack(
    blitz::Array<blitz::TinyVector<Typea,Dim>,Dim> const &e,
    blitz::Array<blitz::TinyVector<Typea,Dim>,Dim> const &v1,
    blitz::Array<blitz::TinyVector<Typea,Dim>,Dim> const &v2,
    blitz::Array<blitz::TinyVector<Typea,Dim>,Dim> const &v3,
    blitz::Array<Typea,Dim> &a, blitz::Array<Typea,Dim> &workspace,
    int d1, int d2);

template<typename Typea, typename Typeb, int Dim>
void pPow(blitz::Array<Typea,Dim> &a, Typeb b);

template<typename Typea, int Dim>
void pSquare(blitz::Array<Typea,Dim> &a);

template<typename Typea, typename Typeb, int Dim>
void pAdd(blitz::Array<Typea,Dim> &a, blitz::Array<Typeb,Dim> const &b);

#include "lParallel.icc"

#endif /* LPARALLEL_HH_ */
