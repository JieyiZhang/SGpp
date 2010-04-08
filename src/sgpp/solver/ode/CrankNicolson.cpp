/*****************************************************************************/
/* This file is part of sgpp, a program package making use of spatially      */
/* adaptive sparse grids to solve numerical problems                         */
/*                                                                           */
/* Copyright (C) 2009-2010 Alexander Heinecke (Alexander.Heinecke@mytum.de)  */
/*                                                                           */
/* sgpp is free software; you can redistribute it and/or modify              */
/* it under the terms of the GNU Lesser General Public License as published  */
/* by the Free Software Foundation; either version 3 of the License, or      */
/* (at your option) any later version.                                       */
/*                                                                           */
/* sgpp is distributed in the hope that it will be useful,                   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU Lesser General Public License for more details.                       */
/*                                                                           */
/* You should have received a copy of the GNU Lesser General Public License  */
/* along with sgpp; if not, write to the Free Software                       */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
/* or see <http://www.gnu.org/licenses/>.                                    */
/*****************************************************************************/

#include "grid/common/DirichletUpdateVector.hpp"
#include "solver/ode/CrankNicolson.hpp"
#include "solver/sle/BiCGStab.hpp"

namespace sg
{

CrankNicolson::CrankNicolson(size_t nTimesteps, double timestepSize, size_t iMaxCG, double epsilonCG, ScreenOutput* screen) : ODESolver(nTimesteps, timestepSize), maxCGIterations(iMaxCG), epsilonCG(epsilonCG), myScreen(screen)
{
	this->residuum = 0.0;
}

CrankNicolson::~CrankNicolson()
{
}

void CrankNicolson::solve(OperationODESolverSystem& System, bool verbose)
{
	size_t allIter = 0;
    BiCGStab myCG(this->maxCGIterations, this->epsilonCG);

	for (size_t i = 0; i < this->nMaxIterations; i++)
	{
		DataVector rhs(System.getGridCoefficientsForCG()->getSize());
		rhs.setAll(0.0);

		// generate right hand side
		System.generateRHS(*System.getGridCoefficientsForCG(), rhs);

	    // Do some adjustments on the boundaries if needed
		System.startTimestep(*System.getGridCoefficients());

		// solve the system of the current timestep
	    myCG.solve(System, *System.getGridCoefficientsForCG(), rhs, true, false, -1.0);
	    allIter += myCG.getNumberIterations();
	    if (verbose == true)
	    {
	    	if (myScreen == NULL)
	    	{
	    		std::cout << "Final residuum " << myCG.residuum << "; with " << myCG.getNumberIterations() << " Iterations (Total Iter.: " << allIter << ")" << std::endl;
	    	}
	    }
	    if (myScreen != NULL)
    	{
    		std::stringstream soutput;
    		soutput << "Final residuum " << myCG.residuum << "; with " << myCG.getNumberIterations() << " Iterations (Total Iter.: " << allIter << ")";

    		if (i < this->nMaxIterations-1)
    		{
    			myScreen->update((size_t)(((double)(i+1)*100.0)/((double)this->nMaxIterations)), soutput.str());
    		}
    		else
    		{
    			myScreen->update(100, soutput.str());
    		}
    	}

	    // Do some adjustments on the boundaries if needed
		//SystemMatrix.finishTimestep(alpha);
	}

	// write some empty lines to console
    if (myScreen != NULL)
	{
    	myScreen->writeEmptyLines(2);
	}
}

}
