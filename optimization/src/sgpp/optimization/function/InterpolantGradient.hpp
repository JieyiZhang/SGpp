// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#ifndef SGPP_OPTIMIZATION_FUNCTION_INTERPOLANTGRADIENT_HPP
#define SGPP_OPTIMIZATION_FUNCTION_INTERPOLANTGRADIENT_HPP

#include <sgpp/globaldef.hpp>

#include <sgpp/optimization/function/ObjectiveGradient.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/operation/hash/OperationNaiveEvalGradient.hpp>

namespace SGPP {
  namespace optimization {

    /**
     * Sparse grid interpolant gradient as an objective gradient.
     *
     * @see Interpolant
     */
    class InterpolantGradient : public ObjectiveGradient {
      public:
        /**
         * Constructor.
         * Do not destruct the grid before the InterpolantGradient object!
         *
         * @param grid  sparse grid
         * @param alpha coefficient vector
         */
        InterpolantGradient(base::Grid& grid, const base::DataVector& alpha) :
          ObjectiveGradient(grid.getStorage()->dim()),
          grid(grid),
          opEvalGradient(op_factory::createOperationNaiveEvalGradient(grid)),
          alpha(alpha) {
        }

        /**
         * Evaluation of the function and its gradient.
         *
         * @param      x        evaluation point \f$\vec{x} \in [0, 1]^d\f$
         * @param[out] gradient gradient
         *                      \f$\nabla f(\vec{x}) \in \mathbb{R}^d\f$
         * @return              \f$f(\vec{x})\f$
         */
        inline float_t eval(const base::DataVector& x,
                            base::DataVector& gradient) {
          // copy x, necessary due to non-existing const correctness
          // in SGPP::base
          base::DataVector y(x);
          return opEvalGradient->evalGradient(alpha, y, gradient);
        }

        /**
         * @param[out] clone pointer to cloned object
         */
        virtual void clone(std::unique_ptr<ObjectiveGradient>& clone) const {
          clone = std::unique_ptr<ObjectiveGradient>(
                    new InterpolantGradient(grid, alpha));
        }

      protected:
        /// sparse grid
        base::Grid& grid;
        /// pointer to evaluation operation
        std::unique_ptr<base::OperationNaiveEvalGradient> opEvalGradient;
        /// coefficient vector
        base::DataVector alpha;
    };

  }
}

#endif /* SGPP_OPTIMIZATION_FUNCTION_INTERPOLANTGRADIENT_HPP */
