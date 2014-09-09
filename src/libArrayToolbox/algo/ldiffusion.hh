/*
 * ldiffusion.h
 *
 *  Created on: Oct 12, 2010
 *      Author: kun
 */

#ifndef LDIFFUSION_H_
#define LDIFFUSION_H_

#include "lParallel.hh"

#include <blitz/array.h>

int mindex3(int x, int y, int z, int sizx, int sizy);

template<typename Type, int Dim>
void anisotropicDiffusion3D_explicit(
    blitz::Array<Type,Dim> &image, blitz::Array<Type,Dim> &Dxx,
    blitz::Array<Type,Dim> &Dyy, blitz::Array<Type,Dim> &Dzz,
    blitz::Array<Type,Dim> &Dxy, blitz::Array<Type,Dim> &Dxz,
    blitz::Array<Type,Dim> &Dyz, double delta_t = 0.0625, int v = 1);

double CalculateDiffusionNewGreyValue(
    double *u, double *a, double *b, double *c, double *d, double *e,
    double *f, int x, int y, int z, int nx, int ny, int nz, int px, int py,
    int pz, double dt, int *dimsu);

#include "ldiffusion.icc"

#endif /* LDIFFUSION_H_ */
