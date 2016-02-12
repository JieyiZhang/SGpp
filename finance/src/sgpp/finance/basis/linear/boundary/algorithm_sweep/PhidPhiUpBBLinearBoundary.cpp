// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/finance/basis/linear/boundary/algorithm_sweep/PhidPhiUpBBLinearBoundary.hpp>

#include <sgpp/globaldef.hpp>

namespace SGPP {
namespace finance {

PhidPhiUpBBLinearBoundary::PhidPhiUpBBLinearBoundary(SGPP::base::GridStorage* storage)
    : PhidPhiUpBBLinear(storage) {}

PhidPhiUpBBLinearBoundary::~PhidPhiUpBBLinearBoundary() {}

void PhidPhiUpBBLinearBoundary::operator()(SGPP::base::DataVector& source,
                                           SGPP::base::DataVector& result, grid_iterator& index,
                                           size_t dim) {
  // get boundary values
  float_t fl = 0.0;
  float_t fr = 0.0;

  // the following computations are independent from a bounding box
  if (!index.hint()) {
    index.resetToLevelOne(dim);

    if (!this->storage->end(index.seq())) {
      rec(source, result, index, dim, fl, fr);
    }

    index.resetToLeftLevelZero(dim);
  }

  size_t seq_left;
  size_t seq_right;

  // left boundary
  seq_left = index.seq();

  // right boundary
  index.resetToRightLevelZero(dim);
  seq_right = index.seq();

  // check boundary conditions
  if (this->boundingBox->hasDirichletBoundaryLeft(dim)) {
    result[seq_left] = 0.0;  // source[seq_left];
  } else {
    // up
    //////////////////////////////////////
    result[seq_left] = fl;

    result[seq_left] += source[seq_right] * (-0.5);
  }

  if (this->boundingBox->hasDirichletBoundaryRight(dim)) {
    result[seq_right] = 0.0;  // source[seq_right];
  } else {
    result[seq_right] = fr;
  }

  index.resetToLeftLevelZero(dim);
}

}  // namespace finance
}  // namespace SGPP
