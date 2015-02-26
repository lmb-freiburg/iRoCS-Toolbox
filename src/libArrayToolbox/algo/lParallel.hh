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
