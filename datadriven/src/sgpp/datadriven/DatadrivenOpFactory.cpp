/******************************************************************************
 * Copyright (C) 2009 Technische Universitaet Muenchen                         *
 * This file is part of the SG++ project. For conditions of distribution and   *
 * use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
 ******************************************************************************/
// @author Valeriy Khakhutskyy (khakhutv@in.tum.de), Dirk Pflueger (pflueged@in.tum.de)
#include <sgpp/datadriven/DatadrivenOpFactory.hpp>

#include <cstring>

#include <sgpp/datadriven/operation/OperationMultiEvalStreaming/OperationMultiEvalStreaming.hpp>
#include <sgpp/base/exception/factory_exception.hpp>

#include <sgpp/base/grid/type/PolyGrid.hpp>
#include <sgpp/base/grid/type/ModPolyGrid.hpp>
#include <sgpp/base/grid/type/PrewaveletGrid.hpp>
#include <sgpp/base/grid/type/ModBsplineGrid.hpp>

#include <sgpp/datadriven/basis/linear/noboundary/operation/OperationTestLinear.hpp>
#include <sgpp/datadriven/basis/linear/boundary/operation/OperationTestLinearBoundary.hpp>
#include <sgpp/datadriven/basis/modbspline/operation/OperationTestModBspline.hpp>
#include <sgpp/datadriven/basis/modlinear/operation/OperationTestModLinear.hpp>
#include <sgpp/datadriven/basis/poly/operation/OperationTestPoly.hpp>
#include <sgpp/datadriven/basis/modpoly/operation/OperationTestModPoly.hpp>
#include <sgpp/datadriven/basis/modwavelet/operation/OperationTestModWavelet.hpp>
#include <sgpp/datadriven/basis/prewavelet/operation/OperationTestPrewavelet.hpp>
#include <sgpp/datadriven/basis/linearstretched/boundary/operation/OperationTestLinearStretchedBoundary.hpp>
#include <sgpp/datadriven/basis/linearstretched/noboundary/operation/OperationTestLinearStretched.hpp>
#include <sgpp/datadriven/basis/linear/noboundary/operation/OperationDensityMarginalizeLinear.hpp>
#include <sgpp/datadriven/basis/linear/noboundary/operation/OperationDensityMargTo1DLinear.hpp>
#include <sgpp/datadriven/basis/linear/noboundary/operation/OperationDensitySampling1DLinear.hpp>
#include <sgpp/datadriven/basis/linear/noboundary/operation/OperationDensitySamplingLinear.hpp>
#include <sgpp/datadriven/basis/linear/noboundary/operation/OperationDensityRejectionSamplingLinear.hpp>
#include <sgpp/datadriven/basis/linear/noboundary/operation/OperationDensityConditionalLinear.hpp>
#include <sgpp/datadriven/basis/linear/noboundary/operation/OperationRosenblattTransformationLinear.hpp>
#include <sgpp/datadriven/basis/linear/noboundary/operation/OperationRosenblattTransformation1DLinear.hpp>
#include <sgpp/datadriven/basis/linear/noboundary/operation/OperationInverseRosenblattTransformationLinear.hpp>
#include <sgpp/datadriven/basis/linear/boundary/operation/OperationRegularizationDiagonalLinearBoundary.hpp>

#include <sgpp/datadriven/operation/OperationMultipleEvalSubspace/combined/OperationMultipleEvalSubspaceCombined.hpp>
#include <sgpp/datadriven/operation/OperationMultipleEvalSubspace/simple/OperationMultipleEvalSubspaceSimple.hpp>

#include <sgpp/base/operation/BaseOpFactory.hpp>

#include <sgpp/globaldef.hpp>


namespace SGPP {

namespace op_factory {

datadriven::OperationTest* createOperationTest(base::Grid& grid) {
    if (strcmp(grid.getType(), "linear") == 0) {
        return new datadriven::OperationTestLinear(grid.getStorage());
    } else if (strcmp(grid.getType(), "linearBoundary") == 0
               || strcmp(grid.getType(), "linearTrapezoidBoundary") == 0) {
        return new datadriven::OperationTestLinearBoundary(grid.getStorage());
    } else if (strcmp(grid.getType(), "modBspline") == 0) {
        return new datadriven::OperationTestModBspline(grid.getStorage(), ((base::ModBsplineGrid*) &grid)->getDegree());
    } else if (strcmp(grid.getType(), "modlinear") == 0) {
        return new datadriven::OperationTestModLinear(grid.getStorage());
    } else if (strcmp(grid.getType(), "poly") == 0) {
        return new datadriven::OperationTestPoly(grid.getStorage(), ((base::PolyGrid*) &grid)->getDegree());
    } else if (strcmp(grid.getType(), "modpoly") == 0) {
        return new datadriven::OperationTestModPoly(grid.getStorage(), ((base::ModPolyGrid*) &grid)->getDegree());
    } else if (strcmp(grid.getType(), "modWavelet") == 0) {
        return new datadriven::OperationTestModWavelet(grid.getStorage());
    } else if (strcmp(grid.getType(), "prewavelet") == 0) {
        return new datadriven::OperationTestPrewavelet(grid.getStorage());
    } else if (strcmp(grid.getType(), "linearStretched") == 0) {
        return new datadriven::OperationTestLinearStretched(grid.getStorage());
    } else if (strcmp(grid.getType(), "linearStretchedTrapezoidBoundary") == 0) {
        return new datadriven::OperationTestLinearStretchedBoundary(grid.getStorage());
    }

    else
        throw base::factory_exception("OperationTest is not implemented for this grid type.");
}

base::OperationMatrix* createOperationRegularizationDiagonal(base::Grid& grid, int mode, double k) {
    if (strcmp(grid.getType(), "linear") == 0 || strcmp(grid.getType(), "linearBoundary") == 0
            || strcmp(grid.getType(), "linearTrapezoidBoundary") == 0 || strcmp(grid.getType(), "modlinear") == 0) {
        return new datadriven::OperationRegularizationDiagonalLinearBoundary(grid.getStorage(), mode, k);
    } else
        throw base::factory_exception("OperationRegularizationDiagonal is not implemented for this grid type.");
}

datadriven::OperationDensityMarginalize* createOperationDensityMarginalize(base::Grid& grid) {
    if (strcmp(grid.getType(), "linear") == 0)
        return new datadriven::OperationDensityMarginalizeLinear(&grid);
    else
        throw base::factory_exception("OperationDensityMarginalize is not implemented for this grid type.");
}

datadriven::OperationDensityMargTo1D* createOperationDensityMargTo1D(base::Grid& grid) {
    if (strcmp(grid.getType(), "linear") == 0)
        return new datadriven::OperationDensityMargTo1DLinear(&grid);
    else
        throw base::factory_exception("OperationDensityMargTo1D is not implemented for this grid type.");
}

datadriven::OperationDensitySampling1D* createOperationDensitySampling1D(base::Grid& grid) {
    if (strcmp(grid.getType(), "linear") == 0)
        return new datadriven::OperationDensitySampling1DLinear(&grid);
    else
        throw base::factory_exception("OperationDensitySampling1D is not implemented for this grid type.");
}

datadriven::OperationDensitySampling* createOperationDensitySampling(base::Grid& grid) {
    if (strcmp(grid.getType(), "linear") == 0)
        return new datadriven::OperationDensitySamplingLinear(&grid);
    else
        throw base::factory_exception("OperationDensitySampling is not implemented for this grid type.");
}

datadriven::OperationDensityRejectionSampling* createOperationDensityRejectionSampling(base::Grid& grid) {
    if (strcmp(grid.getType(), "linear") == 0)
        return new datadriven::OperationDensityRejectionSamplingLinear(&grid);
    else
        throw base::factory_exception("OperationDensityRejectionSampling is not implemented for this grid type.");
}

datadriven::OperationDensityConditional* createOperationDensityConditional(base::Grid& grid) {
    if (strcmp(grid.getType(), "linear") == 0)
        return new datadriven::OperationDensityConditionalLinear(&grid);
    else
        throw base::factory_exception("OperationDensityConditional is not implemented for this grid type.");
}

datadriven::OperationRosenblattTransformation* createOperationRosenblattTransformation(base::Grid& grid) {
    if (strcmp(grid.getType(), "linear") == 0)
        return new datadriven::OperationRosenblattTransformationLinear(&grid);
    else
        throw base::factory_exception("OperationRosenblattTransformation is not implemented for this grid type.");
}

datadriven::OperationTransformation1D* createOperationRosenblattTransformation1D(base::Grid& grid) {
    if (strcmp(grid.getType(), "linear") == 0)
        return new datadriven::OperationRosenblattTransformation1DLinear(&grid);
    else
        throw base::factory_exception("OperationRosenblattTransformation1D is not implemented for this grid type.");
}

datadriven::OperationInverseRosenblattTransformation* createOperationInverseRosenblattTransformation(base::Grid& grid) {
    if (strcmp(grid.getType(), "linear") == 0)
        return new datadriven::OperationInverseRosenblattTransformationLinear(&grid);
    else
        throw base::factory_exception(
            "OperationInverseRosenblattTransformation is not implemented for this grid type.");
}

base::OperationMultipleEval *createOperationMultipleEval(base::Grid &grid, base::DataMatrix &dataset,
        SGPP::datadriven::OperationMultipleEvalConfiguration configuration) {
    if (configuration.type == SGPP::datadriven::OperationMultipleEvalType::DEFAULT) {
        return createOperationMultipleEval(grid, dataset);
    }


    if (strcmp(grid.getType(), "linear") == 0) {
        switch (configuration.type) {
        case datadriven::OperationMultipleEvalType::DEFAULT:
        case datadriven::OperationMultipleEvalType::STREAMING:
            if (configuration.subType != SGPP::datadriven::OperationMultipleEvalSubType::DEFAULT) {
                throw base::factory_exception("OperationMultiEval is not implemented for this implementation subtype.");
            }
            return new datadriven::OperationMultiEvalStreaming(grid, dataset);
            break;
        case datadriven::OperationMultipleEvalType::SUBSPACELINEAR:
            switch (configuration.subType) {
            case SGPP::datadriven::OperationMultipleEvalSubType::DEFAULT:
            case SGPP::datadriven::OperationMultipleEvalSubType::COMBINED:
                return new datadriven::OperationMultipleEvalSubspaceCombined(grid, dataset);
                break;
            case SGPP::datadriven::OperationMultipleEvalSubType::SIMPLE:
                return new datadriven::OperationMultipleEvalSubspaceSimple(grid, dataset);
                break;
            default:
                throw base::factory_exception("OperationMultiEval is not implemented for this implementation subtype.");
                break;
            }
            break;
        default:
            throw base::factory_exception("OperationMultiEval is not implemented for this implementation type.");
            break;
        }

    } else {
        throw base::factory_exception("OperationMultiEval is not implemented for this grid type.");
    }
}

}
}

