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
