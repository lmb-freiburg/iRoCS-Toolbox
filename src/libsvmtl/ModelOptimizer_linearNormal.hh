/**************************************************************************
 *
 * Copyright (C) 2004-2015 Olaf Ronneberger, Florian Pigorsch, Jörg Mechnich,
 *                         Thorsten Falk
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

#ifndef MODELOPTIMIZER_LINEARNORMAL_HH
#define MODELOPTIMIZER_LINEARNORMAL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "BasicSVMAdapter.hh"

namespace svt
{

#ifndef ModelOptimizerlinearNormal
#define ModelOptimizerlinearNormal

template <typename FV>
class ModelOptimizer_linearNormal
{
    public:
	template <typename MODEL>
	void optimizeModel(MODEL& model);

	template <typename TWO_MODEL>
	FV* optimizeTwoClassModel(TWO_MODEL& model, int uid);

	std::vector<FV*> getReducedSV()
	{
	    return _reducedSV;
	}
    private:
	std::vector<FV*> _reducedSV;
};

#include "ModelOptimizer_linearNormal.icc"
#endif
}

#endif
