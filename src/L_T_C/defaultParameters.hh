/*
 * defaultParameters.h
 *
 *  Created on: Nov 1, 2010
 *      Author: liu
 */

#ifndef DEFAULTPARAMETERS_H_
#define DEFAULTPARAMETERS_H_

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#define MyDataDims						3
#define MyDataType						float
#define MyLabelT                                                  int

//#define DEFAULT_dataFileName 			"/misc/lmbraid9/liu/Data/Test/part1PI72.h5"
#define DEFAULT_dataFileName                  "/misc/lmbraid9/liu/Data/Cellwall/annotated_PI72_root02a.h5"
#define DEFAULT_filePosition                  "/misc/lmbraid9/liu/Data/Test"
#define DEFAULT_datasetName				"/t0/channel0"
//#define DEFAULT_datasetName                     "/crop"
#define DEFAULT_outFileNameBase			"/misc/lmbraid9/liu/Data/Test/testCWatershed2"


//for GAC
#define DEFAULT_kappa1					0.1
#define DEFAULT_delta_t1				0.25
#define DEFAULT_n_iters1				2
#define DEFAULT_countour_weight			3
#define DEFAULT_expansion_weight		1
#define DEFAULT_kappa2					3.0/255
#define DEFAULT_delta_t2				5
#define DEFAULT_n_iters2				40

//for watershed
#define DEFAULT_kappa3					0.05
#define DEFAULT_delta_t3				0.5
#define DEFAULT_n_iters3				9
#define DEFAULT_sigma3					1
#define DEFAULT_thresholdOnL1			-0.0015
#define DEFAULT_alpha3					0.0015
#define DEFAULT_morphSize3				2
#define DEFAULT_thresholdOnVolume                       8000

#endif /* DEFAULTPARAMETERS_H_ */
