/* ****************************************************************************
* Copyright (C) 2009 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
**************************************************************************** */
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de) Chao qi (qic@in.tum.de)  Stefanie Schraufstetter (schraufs@in.tum.de)

#ifndef XPHIPHIUPBBLINEAR_HPP
#define XPHIPHIUPBBLINEAR_HPP

#include <sgpp/base/grid/GridStorage.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>

#include <sgpp/globaldef.hpp>


namespace SGPP {
  namespace finance {



    /**
     * Implementation of sweep operator (): 1D Up for
     * Bilinearform \f$\int_{x} x \phi(x) \phi(x) dx\f$
     */
    class XPhiPhiUpBBLinear {
      protected:
        typedef SGPP::base::GridStorage::grid_iterator grid_iterator;

        /// Pointer to SGPP::base::GridStorage object
        SGPP::base::GridStorage* storage;
        /// Pointer to the bounding box Obejct
        SGPP::base::BoundingBox* boundingBox;

      public:
        /**
         * Constructor
         *
         * @param storage the grid's SGPP::base::GridStorage object
         */
        XPhiPhiUpBBLinear(SGPP::base::GridStorage* storage);

        /**
         * Destructor
         */
        virtual ~XPhiPhiUpBBLinear();

        /**
         * This operations performs the calculation of up in the direction of dimension <i>dim</i>
         * on a grid with fix Dirichlet 0 boundary conditions
         *
         * @param source SGPP::base::DataVector that contains the gridpoint's coefficients (values from the vector of the laplace operation)
         * @param result SGPP::base::DataVector that contains the result of the up operation
         * @param index a iterator object of the grid
         * @param dim current fixed dimension of the 'execution direction'
         */
        virtual void operator()(SGPP::base::DataVector& source, SGPP::base::DataVector& result, grid_iterator& index, size_t dim);

      protected:

        /**
         * recursive function for the calculation of Up without bounding Box support
         *
         * @param source SGPP::base::DataVector that contains the coefficients of the ansatzfunction
         * @param result SGPP::base::DataVector in which the result of the operation is stored
         * @param index reference to a griditerator object that is used navigate through the grid
         * @param dim the dimension in which the operation is executed
         * @param fl function value on the left boundary, reference parameter
         * @param fr function value on the right boundary, reference parameter
         */
        void rec(SGPP::base::DataVector& source, SGPP::base::DataVector& result, grid_iterator& index, size_t dim, double& fl, double& fr);

        /**
         * recursive function for the calculation of Up with Bounding Box Support
         *
         * @param source SGPP::base::DataVector that contains the coefficients of the ansatzfunction
         * @param result SGPP::base::DataVector in which the result of the operation is stored
         * @param index reference to a griditerator object that is used navigate through the grid
         * @param dim the dimension in which the operation is executed
         * @param fl function value on the left boundary, reference parameter
         * @param fr function value on the right boundary, reference parameter
         * @param q interval width in the current dimension <i>dim</i>
         * @param t interval offset in current dimension <i>dim</i>
         */
        void recBB(SGPP::base::DataVector& source, SGPP::base::DataVector& result, grid_iterator& index, size_t dim, double& fl, double& fr, double q, double t);
    };

    // namespace detail

  } // namespace SGPP
}

#endif /* XPHIPHIUPBBLINEAR_HPP */
