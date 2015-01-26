/* ****************************************************************************
* Copyright (C) 2011 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
**************************************************************************** */
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de), Sarpkan Selcuk (Sarpkan.Selcuk@mytum.de)

#ifndef HIERARCHISATIONLINEARSTRETCHEDBOUNDARY_HPP
#define HIERARCHISATIONLINEARSTRETCHEDBOUNDARY_HPP

#include <sgpp/base/grid/GridStorage.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>

#include <sgpp/base/basis/linearstretched/noboundary/algorithm_sweep/HierarchisationLinearStretched.hpp>

#include <sgpp/globaldef.hpp>


namespace SGPP {
  namespace base {



    /**
     * Class that implements the hierarchisation on a linear sparse grid with boundaries. Therefore
     * the ()operator has to be implement in order to use the sweep algorithm for
     * the grid traversal
     */
    class HierarchisationLinearStretchedBoundary: public HierarchisationLinearStretched {
      public:
        /**
         * Constructor, must be bind to a grid
         *
         * @param storage the grid storage object of the the grid, on which the hierarchisation should be executed
         */
        HierarchisationLinearStretchedBoundary(GridStorage* storage);

        /**
         * Destructor
         */
        virtual ~HierarchisationLinearStretchedBoundary();

        /**
         * Implements operator() needed by the sweep class during the grid traversal. This function
         * is applied to the whole grid.
         *
         * For level zero it's assumed, that both ansatz-functions do exist: 0,0 and 0,1
         * If one is missing this code might produce some bad errors (segmentation fault, wrong calculation
         * result)
         * So please assure that both functions do exist!
         *
         * @param source this DataVector holds the node base coefficients of the function that should be applied to the sparse grid
         * @param result this DataVector holds the linear base coefficients of the sparse grid's ansatz-functions
         * @param index a iterator object of the grid
         * @param dim current fixed dimension of the 'execution direction'
         */
        virtual void operator()(DataVector& source, DataVector& result, grid_iterator& index, size_t dim);
    };

    // namespace detail

  } // namespace SGPP
}

#endif /* HIERARCHISATIONLINEARSTRETCHEDBOUNDARY_HPP */
