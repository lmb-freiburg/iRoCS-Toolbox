/*
 * ltransform.h
 *
 *  Created on: Oct 27, 2010
 *      Author: liu
 */

#ifndef LTRANSFORM_H_
#define LTRANSFORM_H_

template<typename LabelT, int Dim>
void randomColorMapping(
    blitz::Array<LabelT,Dim> &L,
    blitz::Array<unsigned char,Dim+1> &randomColor);

#include "ltransform.icc"

#endif /* LTRANSFORM_H_ */
