/* ****************************************************************************
* Copyright (C) 2009 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
**************************************************************************** */
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de)

#ifndef CRANKNICOLSON_HPP
#define CRANKNICOLSON_HPP

#include <sgpp/base/application/ScreenOutput.hpp>
#include <sgpp/solver/ODESolver.hpp>

#include <sgpp/globaldef.hpp>


namespace SGPP {
  namespace solver {

    /**
     * This class implements the Crank-Nicolson method
     * for solving ordinary partial equations
     *
     * For solving the system of linear equations the
     * already implemented CG-method is used
     *
     * @version $HEAD$
     */
    class CrankNicolson : public ODESolver {
      private:
        /// Pointer to SGPP::base::ScreenOutput object
        SGPP::base::ScreenOutput* myScreen;

      public:
        /**
         * Std-Constructer
         *
         * @param nTimesteps number of maximum executed iterations
         * @param timestepSize the size of one timestep
         * @param screen possible pointer to a SGPP::base::ScreenOutput object
         */
        CrankNicolson(size_t nTimesteps, double timestepSize, SGPP::base::ScreenOutput* screen = NULL);

        /**
         * Std-Destructor
         */
        virtual ~CrankNicolson();

        virtual void solve(SLESolver& LinearSystemSolver, SGPP::pde::OperationParabolicPDESolverSystem& System, bool bIdentifyLastStep = false, bool verbose = false);
    };

  }
}

#endif /* CRANKNICOLSON_HPP */
