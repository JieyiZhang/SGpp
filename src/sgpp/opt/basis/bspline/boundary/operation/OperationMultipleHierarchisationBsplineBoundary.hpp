/* ****************************************************************************
* Copyright (C) 2014 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
**************************************************************************** */
// @author Julian Valentin (julian.valentin@stud.mathematik.uni-stuttgart.de)

#ifndef SGPP_OPT_BASIS_NOBOUNDARY_OPERATION_OPERATIONMULTIPLEHIERARCHISATIONBSPLINEBOUNDARY_HPP
#define SGPP_OPT_BASIS_NOBOUNDARY_OPERATION_OPERATIONMULTIPLEHIERARCHISATIONBSPLINEBOUNDARY_HPP

#include "opt/operation/OperationMultipleHierarchisation.hpp"
#include "opt/grid/BsplineTrapezoidBoundaryGrid.hpp"
#include "base/datatypes/DataVector.hpp"

namespace sg
{
namespace opt
{

/**
 * Hierarchisation operation for B-spline basis functions on Boundary grids.
 */
class OperationMultipleHierarchisationBsplineBoundary :
        public OperationMultipleHierarchisation
{
public:
    /**
     * Constructor.
     * 
     * @param storage   sparse grid
     */
    OperationMultipleHierarchisationBsplineBoundary(BsplineTrapezoidBoundaryGrid &grid) :
        grid(grid)
    {
    }
    
    /**
     * Virtual destructor.
     */
    virtual ~OperationMultipleHierarchisationBsplineBoundary()
    {
    }
    
    /**
     * @param[in,out] node_values   before: vector of function values at the grid points,
     *                              after: vector of hierarchical coefficients
     */
    virtual void doHierarchisation(std::vector<base::DataVector *> node_values);
    
    /**
     * @param[in,out] alpha         before: vector of hierarchical coefficients,
     *                              after: vector of function values at the grid points
     */
    virtual void doDehierarchisation(std::vector<base::DataVector *> alpha);
    
protected:
    /// storage of the sparse grid
    BsplineTrapezoidBoundaryGrid &grid;
};

}
}

#endif
