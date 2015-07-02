// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#ifndef OPERATIONMULTIPLEEVALPOLYBOUNDARY_HPP
#define OPERATIONMULTIPLEEVALPOLYBOUNDARY_HPP

#include "OperationMultipleEval.hpp"
#include <sgpp/base/grid/Grid.hpp>
#include <sgpp/base/operation/hash/common/basis/PolyBoundaryBasis.hpp>

#include <sgpp/globaldef.hpp>

namespace SGPP {
namespace base {

/**
 * This class implements OperationMultipleEval for a grids with poly basis ansatzfunctions
 */
class OperationMultipleEvalPolyBoundary: public OperationMultipleEval {
public:
    /**
     * Constructor
     *
     * @param storage the grid's GridStorage object
     * @param degree the polynom's max. degree
     * @param dataset Dataset
     */
    OperationMultipleEvalPolyBoundary(Grid& grid, size_t degree,
            DataMatrix& dataset) :
            OperationMultipleEval(grid, dataset), base(degree) {
        this->storage = grid.getStorage();
    }

    /**
     * Destructor
     */
    virtual ~OperationMultipleEvalPolyBoundary() {
    }

    virtual void mult(DataVector& alpha, DataVector& result);
    virtual void multTranspose(DataVector& source, DataVector& result);

protected:
    /// Pointer to GridStorage object
    GridStorage* storage;
    /// Poly Basis object
    SPolyBoundaryBase base;
};

}
}

#endif /* OPERATIONMULTIPLEEVALPOLYBOUNDARY_HPP */