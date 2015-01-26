/******************************************************************************
* Copyright (C) 2013 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de)
// @author Jacob Jepsen (jepsen@diku.dk)

#include <sgpp/base/grid/type/LinearGrid.hpp>
#include <sgpp/base/grid/generation/GridGenerator.hpp>

#include <sgpp/parallel/pde/basis/linear/noboundary/operation/OperationLaplaceVectorizedLinearOCL.hpp>

#include <sgpp/globaldef.hpp>


namespace SGPP {
  namespace parallel {

    OperationLaplaceVectorizedLinearOCL::OperationLaplaceVectorizedLinearOCL(SGPP::base::GridStorage* storage, SGPP::base::DataVector& lambda) : storage(storage) {
      this->lambda = new SGPP::base::DataVector(lambda);
      this->OCLPDEKernelsHandle = OCLPDEKernels();
      this->level_ = new SGPP::base::DataMatrix(storage->size(), storage->dim());
      this->level_int_ = new SGPP::base::DataMatrix(storage->size(), storage->dim());
      this->index_ = new SGPP::base::DataMatrix(storage->size(), storage->dim());
      lcl_q = new double[this->storage->dim()];
      lcl_q_inv = new double[this->storage->dim()];

      storage->getLevelIndexArraysForEval(*(this->level_), *(this->index_));
      storage->getLevelForIntegral(*(this->level_int_));
    }

    OperationLaplaceVectorizedLinearOCL::OperationLaplaceVectorizedLinearOCL(SGPP::base::GridStorage* storage) : storage(storage) {
      this->lambda = new base::DataVector(storage->dim());
      this->lambda->setAll(1.0);
      this->OCLPDEKernelsHandle = OCLPDEKernels();
      this->level_ = new SGPP::base::DataMatrix(storage->size(), storage->dim());
      this->level_int_ = new SGPP::base::DataMatrix(storage->size(), storage->dim());
      this->index_ = new SGPP::base::DataMatrix(storage->size(), storage->dim());
      lcl_q = new double[this->storage->dim()];
      lcl_q_inv = new double[this->storage->dim()];

      storage->getLevelIndexArraysForEval(*(this->level_), *(this->index_));
      storage->getLevelForIntegral(*(this->level_int_));
    }


    OperationLaplaceVectorizedLinearOCL::~OperationLaplaceVectorizedLinearOCL() {
      delete this->level_;
      delete this->level_int_;
      delete this->index_;
      delete[] lcl_q;
      delete[] lcl_q_inv;
      this->OCLPDEKernelsHandle.CleanUpGPU();
    }

    void OperationLaplaceVectorizedLinearOCL::mult(SGPP::base::DataVector& alpha, SGPP::base::DataVector& result) {
      result.setAll(0.0);

      // fill q array
      for (size_t d = 0; d < this->storage->dim(); d++) {
        SGPP::base::BoundingBox* boundingBox = this->storage->getBoundingBox();
        lcl_q[d] = boundingBox->getIntervalWidth(d);
        lcl_q_inv[d] = 1.0 / boundingBox->getIntervalWidth(d);
      }

      this->OCLPDEKernelsHandle.
      RunOCLKernelLaplaceInner(alpha, result, lcl_q, lcl_q_inv,
                               this->level_->getPointer(),
                               this->index_->getPointer(),
                               this->level_int_->getPointer(),
                               lambda->getPointer(),
                               storage->size(),
                               storage->dim(),
                               storage);
    }

  }

}
