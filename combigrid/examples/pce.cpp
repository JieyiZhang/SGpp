// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/combigrid/functions/MonomialFunctionBasis1D.hpp>
#include <sgpp/combigrid/functions/OrthogonalPolynomialBasis1D.hpp>
#include <sgpp/combigrid/operation/CombigridOperation.hpp>
#include <sgpp/combigrid/operation/CombigridTensorOperation.hpp>
#include <sgpp/combigrid/operation/Configurations.hpp>
#include <sgpp/combigrid/operation/multidim/AveragingLevelManager.hpp>
#include <sgpp/combigrid/operation/multidim/WeightedRatioLevelManager.hpp>
#include <sgpp/combigrid/serialization/TreeStorageSerializationStrategy.hpp>
#include <sgpp/combigrid/storage/FunctionLookupTable.hpp>
#include <sgpp/combigrid/utils/Stopwatch.hpp>
#include <sgpp/combigrid/utils/Utils.hpp>

#include <cmath>

#include <iostream>
#include <vector>

double f(sgpp::base::DataVector const &v) {
  // return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
  // return v[0] * v[0] + v[1] * v[1];
  return exp(3 * v[0] * v[0] + v[1]) * atan(10 * v[2]) + sin(3 * v[1] + v[2]);
}

int main() {
  for (size_t q = 0; q <= 8; ++q) {
    size_t d = 3;

    auto functionBasis = std::make_shared<sgpp::combigrid::OrthogonalPolynomialBasis1D>(
        sgpp::combigrid::OrthogonalPolynomialBasisType::LEGENDRE);
    auto func = sgpp::combigrid::MultiFunction(f);
    auto op = sgpp::combigrid::CombigridTensorOperation::createLinearLejaPolynomialInterpolation(
        functionBasis, d, func);

    std::cout << "q = " << q << "\n";

    sgpp::combigrid::Stopwatch stopwatch;
    auto tensorResult = op->evaluate(q, std::vector<sgpp::combigrid::FloatTensorVector>());
    /*std::cout
        << sgpp::combigrid::TreeStorageSerializationStrategy<sgpp::combigrid::FloatScalarVector>(d)
               .serialize(tensorResult.getValues())
        << "\n";*/

    stopwatch.log();
    std::cout << "E(u) = " << tensorResult.get(sgpp::combigrid::MultiIndex(d, 0)) << std::endl;
    std::cout << "Var(u) = " << std::pow(tensorResult.norm(), 2) << std::endl;
  }
}
