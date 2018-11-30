// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#pragma once
// #ifdef USE_EIGEN

#include <sgpp/optimization/activeSubspaces/ResponseSurface.hpp>

#include <iostream>

namespace sgpp {
namespace optimization {

/**
 * Reduced response surface on the active subspace of some objective function
 */
class ASResponseSurface : public ResponseSurface {
 public:
  /**
   * Constructor
   *
   * @param W1	the eigenvectors defining the active subspace
   */
  explicit ASResponseSurface(Eigen::MatrixXd W1) : ResponseSurface(), W1(W1) {}

  /**
   * Destructor
   */
  virtual ~ASResponseSurface() {}

  /**
   * create a regular interpolant by determining the coefficients through regression with the
   * evaluationPoints and the according functionValues. These are usually the ones calculated while
   * detecting the active subspace
   *
   * @param evaluationPoints	set of points
   * @param functionValues		the objective function evaluations at evaluationPoints
   * @param level				level of the regular interpolant
   */
  virtual void createRegularReducedSurfaceFromDetectionPoints(
      sgpp::base::DataMatrix evaluationPoints, sgpp::base::DataVector functionValues,
      size_t level) = 0;

 protected:
  // the eigenvectors defining the active subspace
  Eigen::MatrixXd W1;
};

}  // namespace optimization
}  // namespace sgpp

// #endif /* USE_EIGEN */
