/* ****************************************************************************
* Copyright (C) 2014 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
**************************************************************************** */
// @author Julian Valentin (julian.valentin@stud.mathematik.uni-stuttgart.de)

#ifndef SGPP_OPT_FUNCTION_TEST_SCHWEFEL_HPP
#define SGPP_OPT_FUNCTION_TEST_SCHWEFEL_HPP

#include "opt/function/test/Test.hpp"

namespace sg
{
namespace opt
{
namespace function
{
namespace test
{

/**
 * Schwefel test function.
 * 
 * Definition:
 * \f$f(\vec{x}) := -\sum_{t=1}^d x_t \sin \sqrt{|x_t|}\f$,
 * \f$\vec{x} \in [-500, 500]^d\f$,
 * \f$\vec{x}_{\text{opt}} = (420.9687, \dotsc, 420.9687)^{\mathrm{T}}\f$,
 * \f$f_{\text{opt}} = -418.9829d\f$
 * (domain scaled to \f$[0, 1]^d\f$)
 */
class Schwefel : public Test
{
public:
    /**
     * Constructor.
     * 
     * @param d     dimension of the domain
     */
    Schwefel(size_t d) : Test(d)
    {
    }
    
    /**
     * Evaluates the test function.
     * 
     * @param x     point \f$\vec{x} \in [0, 1]^d\f$
     * @return      \f$f(\vec{x})\f$
     */
    double evalUndisplaced(const std::vector<double> &x)
    {
        double result = 0.0;
        
        for (size_t t = 0; t < d; t++)
        {
            const double xt = 1000.0 * x[t] - 500.0;
            result -= xt * std::sin(std::sqrt(std::abs(xt)));
        }
        
        return result;
    }
    
    /**
     * Returns minimal point and function value of the test function.
     * 
     * @param[out] x    minimal point \f$\vec{x}_{\text{opt}} \in [0, 1]^d\f$
     * @return          minimal function value \f$f_{\text{opt}} = f(\vec{x}_{\text{opt}})\f$
     */
    double getOptimalPointUndisplaced(std::vector<double> &x)
    {
        x = std::vector<double>(d, 0.920968746359982027311844365);
        return evalUndisplaced(x);
    }
    
    /**
     * @return clone of the object
     */
    virtual tools::SmartPointer<Objective> clone()
    {
        return tools::SmartPointer<Objective>(new Schwefel(*this));
    }
};

}
}
}
}

#endif
