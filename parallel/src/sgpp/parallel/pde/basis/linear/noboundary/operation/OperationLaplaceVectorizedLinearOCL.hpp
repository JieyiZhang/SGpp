/******************************************************************************
* Copyright (C) 2013 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de)
// @author Jacob Jepsen (jepsen@diku.dk)

#ifndef OPERATIONLAPLACEVECTORIZEDLINEAROCL_HPP
#define OPERATIONLAPLACEVECTORIZEDLINEAROCL_HPP

#include <sgpp/base/operation/OperationMatrix.hpp>
#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/base/grid/Grid.hpp>
#include <sgpp/parallel/pde/basis/common/OCLPDEKernels.hpp>

#include <sgpp/globaldef.hpp>


namespace SGPP {
  namespace parallel {

    /**
     * Implementation for linear functions of Laplace Operation, linear grids without boundaries
     *
     * @version $HEAD$
     */
    class OperationLaplaceVectorizedLinearOCL: public SGPP::base::OperationMatrix {
      private:
        SGPP::base::GridStorage* storage;
        SGPP::base::DataMatrix* level_;
        SGPP::base::DataMatrix* level_int_;
        SGPP::base::DataMatrix* index_;
        double* lcl_q;
        double* lcl_q_inv;
        SGPP::base::DataVector* lambda;
        OCLPDEKernels OCLPDEKernelsHandle;

      public:
        /**
         * Construtor of OperationLaplaceLinear
         *
         * @param storage Pointer to the grid's gridstorage obejct
         * @param lambda Vector which contains pre-factors for every dimension of the operator
         */
        OperationLaplaceVectorizedLinearOCL(SGPP::base::GridStorage* storage, SGPP::base::DataVector& lambda);

        /**
         * Construtor of OperationLaplaceLinear
         *
         * @param storage Pointer to the grid's gridstorage obejct
         */
        OperationLaplaceVectorizedLinearOCL(SGPP::base::GridStorage* storage);

        /**
         * Destructor
         */
        virtual ~OperationLaplaceVectorizedLinearOCL();

        virtual void mult(SGPP::base::DataVector& alpha, SGPP::base::DataVector& result);
    };

  }

}

#endif /* OPERATIONLAPLACEVECTORIZEDLINEAROCL_HPP */
