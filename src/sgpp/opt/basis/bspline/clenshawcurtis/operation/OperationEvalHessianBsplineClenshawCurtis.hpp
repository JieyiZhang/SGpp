/* ****************************************************************************
* Copyright (C) 2014 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
**************************************************************************** */
// @author Julian Valentin (julian.valentin@stud.mathematik.uni-stuttgart.de)

#ifndef SGPP_OPT_BASIS_BSPLINE_CLENSHAWCURTIS_OPERATION_OPERATIONEVALHESSIANBSPLINECLENSHAWCURTIS_HPP
#define SGPP_OPT_BASIS_BSPLINE_CLENSHAWCURTIS_OPERATION_OPERATIONEVALHESSIANBSPLINECLENSHAWCURTIS_HPP

#include "opt/operation/OperationEvalHessian.hpp"
#include "base/grid/GridStorage.hpp"
#include "opt/basis/bspline/clenshawcurtis/BsplineClenshawCurtisBasis.hpp"
#include "base/datatypes/DataVector.hpp"
#include "base/datatypes/DataMatrix.hpp"

namespace sg
{
namespace opt
{

/**
 * Operation for evaluating B-spline linear combinations on Clenshaw-Curtis grids, their gradients
 * and their Hessians.
 */
class OperationEvalHessianBsplineClenshawCurtis : public OperationEvalHessian
{
public:
    /**
     * Constructor.
     * 
     * @param storage       storage of the sparse grid
     * @param degree        B-spline degree
     * @param cosine_table  cosine table for faster cosine evaluation (optional)
     */
    OperationEvalHessianBsplineClenshawCurtis(base::GridStorage *storage, size_t degree,
                                              const tools::CosineTable *cosine_table = NULL) :
        storage(storage),
        base(degree, cosine_table)
    {
    }
    
    /**
     * Virtual destructor.
     */
    virtual ~OperationEvalHessianBsplineClenshawCurtis()
    {
    }
    
    /**
     * @param       alpha       coefficient vector
     * @param       point       evaluation point
     * @param[out]  gradient    gradient vector of linear combination
     * @param[out]  hessian     Hessian matrix of linear combination
     * @return                  value of linear combination
     */
    virtual double evalHessian(base::DataVector &alpha, const std::vector<double> &point,
                               base::DataVector &gradient, base::DataMatrix &hessian);
    
protected:
    /// storage of the sparse grid
    base::GridStorage *storage;
    /// 1D B-spline basis
    SBsplineClenshawCurtisBase base;
};

}
}

#endif
