// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/finance/operation/hash/OperationHestonGLinear.hpp>

#include <sgpp/pde/basis/linear/noboundary/algorithm_sweep/PhiPhiDownBBLinear.hpp>
#include <sgpp/pde/basis/linear/noboundary/algorithm_sweep/PhiPhiUpBBLinear.hpp>

#include <sgpp/finance/basis/linear/noboundary/algorithm_sweep/XdPhiPhiDownBBLinear.hpp>
#include <sgpp/finance/basis/linear/noboundary/algorithm_sweep/XdPhiPhiUpBBLinear.hpp>

#include <sgpp/base/algorithm/sweep.hpp>

#include <sgpp/globaldef.hpp>

namespace SGPP {
namespace finance {

OperationHestonGLinear::OperationHestonGLinear(SGPP::base::GridStorage* storage,
                                               SGPP::base::DataVector& coef)
    : SGPP::pde::UpDownOneOpDim(storage, coef) {}

OperationHestonGLinear::~OperationHestonGLinear() {}

void OperationHestonGLinear::up(SGPP::base::DataVector& alpha, SGPP::base::DataVector& result,
                                size_t dim) {
  // phi * phi
  SGPP::pde::PhiPhiUpBBLinear func(this->storage);
  SGPP::base::sweep<SGPP::pde::PhiPhiUpBBLinear> s(func, *this->storage);

  s.sweep1D(alpha, result, dim);
}

void OperationHestonGLinear::down(SGPP::base::DataVector& alpha, SGPP::base::DataVector& result,
                                  size_t dim) {
  // phi * phi
  SGPP::pde::PhiPhiDownBBLinear func(this->storage);
  SGPP::base::sweep<SGPP::pde::PhiPhiDownBBLinear> s(func, *this->storage);

  s.sweep1D(alpha, result, dim);
}

void OperationHestonGLinear::upOpDim(SGPP::base::DataVector& alpha, SGPP::base::DataVector& result,
                                     size_t dim) {
  // x * dphi * phi
  XdPhiPhiUpBBLinear func(this->storage);
  SGPP::base::sweep<XdPhiPhiUpBBLinear> s(func, *this->storage);

  s.sweep1D(alpha, result, dim);
}

void OperationHestonGLinear::downOpDim(SGPP::base::DataVector& alpha,
                                       SGPP::base::DataVector& result, size_t dim) {
  // x * dphi * phi
  XdPhiPhiDownBBLinear func(this->storage);
  SGPP::base::sweep<XdPhiPhiDownBBLinear> s(func, *this->storage);

  s.sweep1D(alpha, result, dim);
}
}  // namespace finance
}  // namespace SGPP
