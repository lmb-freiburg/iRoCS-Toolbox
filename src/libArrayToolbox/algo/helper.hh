/*
 * helper.h
 *
 *  Created on: Oct 10, 2010
 *      Author: kun
 */

#ifndef HELPER_H_
#define HELPER_H_

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <cstdlib>

#include <vector>
#include <blitz/array.h>

#include <libArrayToolbox/ATBTiming.hh>

#define LOG std::cerr << atb::MyDateTime::prettyDate() << " " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"

#define lmax(a,b)  (a > b ? a: b)
#define lmin(a,b)  (a < b ? a: b)

template<typename Type>
void randomSample(
    std::vector<Type>& dataset, std::vector<Type>& samples,
    int numberOfSamples);

template<typename Type>
void downSample(
    std::vector<Type>& dataset, std::vector<Type>& samples, float downSample);

template<typename Type, typename Type2, typename Type3>
void keepInRange3D(Type& input, Type2 lb, Type3 ub);

template<typename Type, typename Type2>
void keepHigherThan3D(Type& input, Type2 lb);

template<typename Type, typename Type2>
void keepLowerThan3D(Type& input, Type2 ub);

template<typename Type, typename Type2, typename Type3>
bool testInRange3D(Type& input, Type2 lb, Type3 ub);

#include "helper.icc"

#endif /* HELPER_H_ */
