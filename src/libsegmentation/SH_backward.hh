/***************************************************************************
  **************************************************************************
  
                           S2kit 1.0

          A lite version of Spherical Harmonic Transform Kit

   Peter Kostelec, Dan Rockmore
   {geelong,rockmore}@cs.dartmouth.edu
  
   Contact: Peter Kostelec
            geelong@cs.dartmouth.edu
  
   Copyright 2004 Peter Kostelec, Dan Rockmore

   This file is part of S2kit.

   S2kit is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   S2kit is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with S2kit; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   See the accompanying LICENSE file for details.
  
  ************************************************************************
  ************************************************************************/


/*

Source code to test FORWARD spherical harmonic transform
using the seminaive and naive algorithms coded up during October, 1995.
It will compute the spherical coefficients of the function whose
sample values are given in the named input file, and will write
those coefficients in the named output file.

In its current state, assuming that will seminaive at ALL orders.
If you wish to change this, modify the CUTOFF variable in this file, i.e.
cutoff = at what order to switch from semi-naive to naive algorithm

WILL PRECOMPUTE IN MEMORY EVERYTHING BEFORE DOING TRANSFORM!

Sample call

 test_s2_semi_memo_for sampleFile outputFile bw [output_format]

 test_s2_semi_memo_for y31_bw8.dat y31_coef.dat 8

 output_format is an optional argument:
 = 0 -> coefficients in "code" order,
        i.e. suitable for test_s2_semi_memo_inv

 = 1 -> coefficients in prettier "human" order,
        i.e. if f_{l,m} is the coefficient of degree l, order m,
	then the coefficients will be arranged this way:
	f_{0,0},
	f_{1,-1}, f_{1,0}, f_{1,1},
	f_{2,-2}, f_{2,-1}, f_{2,0}, f_{2,1}, f_{2,2},
	...

The default output format is "code" order. To help you out,
the function

 seanindex(m, l, bw)

defined in FST_semi_memo.c, returns the array index of
the coefficient f_{l,m} (so you know where it goes). Since we're
talking in C here, the indexing begins at 0.

The format of the input sample file will be an interleaved
real/imaginary parts of the function samples arranged in
"latitude-major" format, i.e. the function will be sampled
in this order:

  (theta_0, phi_0)
  (theta_0, phi_1)
  (theta_0, phi_2)
  ...
  (theta_0, phi_{bw-1})
  (theta_1, phi_0)  
  (theta_1, phi_1)
  ...
  (theta_{bw-1}, phi_{bw-1})

  where theta_k = pi*(2*j+1)/(4*bw)
        phi_j = 2*pi*k/(2*bw)

The format of the output depends on whether or not it's human
or code-ordered. For code-ordered, it's interleaved real/imaginary.
For human-ordered, it's verbose and it's one coefficient per line,
e.g. l = 2  m = 1  2.3 + 6 I


*/

#ifndef SH_BACKWARD_HH
#define SH_BACKWARD_HH

#include <fftw3.h>

extern "C" {
#include <lmbs2kit/makeweights.h>
#include <lmbs2kit/cospmls.h>
#include <lmbs2kit/FST_semi_memo.h>
#include <lmbs2kit/csecond.h>
#include <lmbs2kit/primitive.h>
#include <lmbs2kit/naive_synthesis.h>
#include <lmbs2kit/seminaive.h>
#include <lmbs2kit/pmls.h>
}

#include <map>

/**************************************************/
/**************************************************/
class SH_backward
{

private:
  
  int i, size ;
  int l, m, dummy;
  int cutoff, order ;
  int rank, howmany_rank ;
  double *rdata, *idata ;
  double *rcoeffs, *icoeffs ;
  double *weights ;
  double *seminaive_naive_tablespace, *workspace,
      *trans_seminaive_naive_tablespace;
  double **seminaive_naive_table, **trans_seminaive_naive_table;
  double tstart, tstop;
  fftw_plan idctPlan, ifftPlan ;
  fftw_iodim dims[1], howmany_dims[1];
  int _bw;
  static std::map<int,SH_backward*> SH_backward_cache;

  class CacheCleaner
  {
    CacheCleaner();
    void init();
    ~CacheCleaner();
    friend class SH_backward;
  };
  
  SH_backward(const SH_backward&);
  SH_backward(int bw);

 public:
  
  static SH_backward &instance(int bw);
      
  ~SH_backward();

  int sh_semi_memo_back(double const *indata, double *outdata);

};

#endif
