/******************************************************************************
* Copyright (C) 2011-2013 Technische Universitaet Muenchen                    *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de)

#ifndef OPERATIONMULTIPLEEVALVECTORIZED_HPP
#define OPERATIONMULTIPLEEVALVECTORIZED_HPP

#include "base/datatypes/DataVector.hpp"
#include "base/datatypes/DataMatrix.hpp"
#include "base/operation/OperationMatrix.hpp"

namespace sg
{
namespace parallel
{

/**
 * @brief Interface for multiplication with Matrices @f$B@f$ and @f$B^T@f$.
 *
 * If there are @f$N@f$ basis functions, @f$\{\varphi(\vec{x})\}_{i=1,\ldots,N}@f$ and @f$m@f$ data points
 *
 * This class defines an interface similar to OperationMultipleEval in order to support SIMD architectures
 * for datadriven task (multiple function evaluations, classification, regression). Target
 * architectures may be Intel SSE, Intel AVX, nVidia CUDA, OpenCL.
 */
class OperationMultipleEvalVectorized
{
protected:
	/// Pointer to the dataset that should be evaluated on the grid
	sg::base::DataMatrix* dataset_;
	/// Member to store the sparse grid's levels for better vectorization
	sg::base::DataMatrix* level_;
	/// Member to store the sparse grid's indices for better vectorization
	sg::base::DataMatrix* index_;
	/// Member to store for masks per grid point for better vectorization of modlinear operations
	sg::base::DataMatrix* mask_;
	/// Member to store offsets per grid point for better vecotrization of modlinear operations
    sg::base::DataMatrix* offset_;

public:
	/**
	 * Constructor
	 *
	 * @param dataset data set that should be evaluated on the sparse grid
	 */
	OperationMultipleEvalVectorized(sg::base::DataMatrix* dataset) {
		this->dataset_ = dataset;
		this->level_ = NULL;
		this->index_ = NULL;
		this->mask_ = NULL;
		this->offset_ = NULL;
	}

	/**
	 * Destructor
	 *
	 * cleans up level_ and index_ members
	 */
	virtual ~OperationMultipleEvalVectorized() {
		if (this->level_ != NULL)
			delete this->level_;

		if (this->index_ != NULL)
			delete this->index_;
			
		if (this->mask_ != NULL)
		    delete this->mask_;
		    
		if (this->offset_ != NULL)
		    delete this->offset_;
	}

	/**
	 * Multiplication of @f$B^T@f$ with vector @f$\alpha@f$
	 *
	 * IMPORTANT REMARK:
	 * In order to use this routine you have to keep following points in mind (for multVectorized and multTransposeVectorized):
 	 * 	- data MUST a have even number of points AND it must be transposed
 	 *  - result MUST have the same size as data points that should be evaluated
	 *
	 * @param alpha vector, to which @f$B@f$ is applied. Typically the coefficient vector
	 * @param result the result vector of the matrix vector multiplication
	 */
	virtual double multVectorized(sg::base::DataVector& alpha, sg::base::DataVector& result) = 0;

	/**
	 * Multiplication of @f$B@f$ with vector @f$\alpha@f$
	 *
	 * IMPORTANT REMARK:
	 * In order to use this routine you have to keep following points in mind (for multVectorized and multTransposeVectorized):
 	 * 	- data MUST a have even number of points AND it must be transposed
 	 *  - result MUST have the same size as data points that should be evaluated
	 *
	 * @param source vector, to which @f$B^T@f$ is applied. Typically the coefficient vector
	 * @param result the result vector of the matrix vector multiplication
	 */
	virtual double multTransposeVectorized(sg::base::DataVector& source, sg::base::DataVector& result) = 0;

	/**
	 * rebuilds the DataMatrix for Level and Index in Derivatives
	 * needed for vectorization.
	 */
	virtual void rebuildLevelAndIndex() = 0;
};

}
}

#endif /* OPERATIONMULTIPLEEVALVECTORIZED_HPP */
