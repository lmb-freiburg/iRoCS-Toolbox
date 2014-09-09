/*
 * lMath.hh
 *
 *  Created on: Nov 1, 2010
 *      Author: liu
 */

#ifndef LMATH_HH_
#define LMATH_HH_

#include <blitz/array.h>

#define SORT_eigenValueBy_ascendingAlgebra 0
#define SORT_eigenValueBy_descendingAlgebra 1
#define SORT_eigenValueBy_descendingABS 2

/* Calculation of eigenvalues (D) and eigenvectors (V) of 3x3 matrix A
 */
void eigen_decomposition(
    double A[][3], double V[][3], double d[], int sortMode);

template<typename DataT, int Dim>
void eig3DSym(
    blitz::Array<blitz::TinyVector<DataT,6>,3> &hessianMatrix,
    blitz::Array<blitz::TinyVector<DataT,Dim>,Dim> &hessianEigValues,
    blitz::Array<blitz::TinyVector<DataT,Dim>,Dim> &v1,
    blitz::Array<blitz::TinyVector<DataT,Dim>,Dim> &v2,
    blitz::Array<blitz::TinyVector<DataT,Dim>,Dim> &v3, int sortMode);

#include "lmath.icc"

#endif /* LMATH_HH_ */
