// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cstring>

#include <sgpp/globaldef.hpp>

#include <sgpp/optimization/operation/OptimizationOpFactory.hpp>
#include <sgpp/base/exception/factory_exception.hpp>

#include <sgpp/base/grid/type/LinearGrid.hpp>
#include <sgpp/base/grid/type/LinearTruncatedBoundaryGrid.hpp>
#include <sgpp/base/grid/type/LinearClenshawCurtisGrid.hpp>
#include <sgpp/base/grid/type/ModLinearGrid.hpp>
#include <sgpp/base/grid/type/BsplineGrid.hpp>
#include <sgpp/base/grid/type/BsplineTruncatedBoundaryGrid.hpp>
#include <sgpp/base/grid/type/BsplineClenshawCurtisGrid.hpp>
#include <sgpp/base/grid/type/ModBsplineGrid.hpp>
#include <sgpp/base/grid/type/WaveletGrid.hpp>
#include <sgpp/base/grid/type/WaveletTruncatedBoundaryGrid.hpp>
#include <sgpp/base/grid/type/ModWaveletGrid.hpp>

#include <sgpp/optimization/operation/hash/OperationMultipleHierarchisationBsplineBoundary.hpp>
#include <sgpp/optimization/operation/hash/OperationMultipleHierarchisationBsplineClenshawCurtis.hpp>
#include <sgpp/optimization/operation/hash/OperationMultipleHierarchisationModBspline.hpp>
#include <sgpp/optimization/operation/hash/OperationMultipleHierarchisationBspline.hpp>
#include <sgpp/optimization/operation/hash/OperationMultipleHierarchisationLinearBoundary.hpp>
#include <sgpp/optimization/operation/hash/OperationMultipleHierarchisationLinearClenshawCurtis.hpp>
#include <sgpp/optimization/operation/hash/OperationMultipleHierarchisationModLinear.hpp>
#include <sgpp/optimization/operation/hash/OperationMultipleHierarchisationLinear.hpp>
#include <sgpp/optimization/operation/hash/OperationMultipleHierarchisationWaveletBoundary.hpp>
#include <sgpp/optimization/operation/hash/OperationMultipleHierarchisationModWavelet.hpp>
#include <sgpp/optimization/operation/hash/OperationMultipleHierarchisationWavelet.hpp>

namespace SGPP {
  namespace op_factory {

    optimization::OperationMultipleHierarchisation*
    createOperationMultipleHierarchisation(base::Grid& grid) {
      if (std::strcmp(grid.getType(), "linear") == 0) {
        return new optimization::OperationMultipleHierarchisationLinear(
                 dynamic_cast<base::LinearGrid&>(grid));
      } else if (std::strcmp(grid.getType(), "linearTruncatedBoundary") == 0) {
        return new optimization::OperationMultipleHierarchisationLinearBoundary(
                 dynamic_cast<base::LinearTruncatedBoundaryGrid&>(grid));
      } else if (std::strcmp(grid.getType(), "linearClenshawCurtis") == 0) {
        return new optimization::OperationMultipleHierarchisationLinearClenshawCurtis(
                 dynamic_cast<base::LinearClenshawCurtisGrid&>(grid));
      } else if (std::strcmp(grid.getType(), "modlinear") == 0) {
        return new optimization::OperationMultipleHierarchisationModLinear(
                 dynamic_cast<base::ModLinearGrid&>(grid));
      } else if (std::strcmp(grid.getType(), "bspline") == 0) {
        return new optimization::OperationMultipleHierarchisationBspline(
                 dynamic_cast<base::BsplineGrid&>(grid));
      } else if (std::strcmp(grid.getType(), "bsplineTruncatedBoundary") == 0) {
        return new optimization::OperationMultipleHierarchisationBsplineBoundary(
                 dynamic_cast<base::BsplineTruncatedBoundaryGrid&>(grid));
      } else if (std::strcmp(grid.getType(), "bsplineClenshawCurtis") == 0) {
        return new optimization::OperationMultipleHierarchisationBsplineClenshawCurtis(
                 dynamic_cast<base::BsplineClenshawCurtisGrid&>(grid));
      } else if (std::strcmp(grid.getType(), "modBspline") == 0) {
        return new optimization::OperationMultipleHierarchisationModBspline(
                 dynamic_cast<base::ModBsplineGrid&>(grid));
      } else if (std::strcmp(grid.getType(), "wavelet") == 0) {
        return new optimization::OperationMultipleHierarchisationWavelet(
                 dynamic_cast<base::WaveletGrid&>(grid));
      } else if (std::strcmp(grid.getType(), "waveletTruncatedBoundary") == 0) {
        return new optimization::OperationMultipleHierarchisationWaveletBoundary(
                 dynamic_cast<base::WaveletTruncatedBoundaryGrid&>(grid));
      } else if (std::strcmp(grid.getType(), "modWavelet") == 0) {
        return new optimization::OperationMultipleHierarchisationModWavelet(
                 dynamic_cast<base::ModWaveletGrid&>(grid));
      } else {
        throw base::factory_exception(
          "OperationMultipleHierarchisation is not implemented for this grid type.");
      }
    }

  }
}