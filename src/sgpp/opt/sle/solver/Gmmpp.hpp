/* ****************************************************************************
* Copyright (C) 2014 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
**************************************************************************** */
// @author Julian Valentin (julian.valentin@stud.mathematik.uni-stuttgart.de)

#ifndef SGPP_OPT_SLE_SOLVER_GMMPP_HPP
#define SGPP_OPT_SLE_SOLVER_GMMPP_HPP

#include "opt/sle/solver/Solver.hpp"

#include <vector>

namespace sg
{
namespace opt
{
namespace sle
{
namespace solver
{

/**
 * Linear system solver using Gmm++ (iterative sparse solver).
 */
class Gmmpp : public Solver
{
public:
    /**
     * @param       system  system to be solved
     * @param       b       right-hand side
     * @param[out]  x       solution to the system
     * @return              whether all went well (false if errors occurred)
     */
    bool solve(system::System &system, const std::vector<double> &b, std::vector<double> &x) const;
};

}
}
}
}

#endif
