#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "omp.h"
#include <immintrin.h>

#include "../../OperationMultipleEvalSubspace/combined/OperationMultipleEvalSubspaceCombinedParameters.hpp"
#include "../../OperationMultipleEvalSubspace/combined/OperationMultipleEvalSubspaceCombinedSubspaceNode.hpp"
#include <sgpp/datadriven/operation/OperationMultipleEvalSubspace/AbstractOperationMultipleEvalSubspace.hpp>

#include <sgpp/globaldef.hpp>


namespace SGPP {
namespace datadriven {

class OperationMultipleEvalSubspaceCombined: public AbstractOperationMultipleEvalSubspace {
private:

    SGPP::base::DataMatrix *paddedDataset;

    //size_t subspaceSize = -1;

    size_t maxGridPointsOnLevel;

    std::map<uint32_t, uint32_t> allLevelsIndexMap;

    size_t dim = -1;
    size_t maxLevel = 0;

    std::vector<SubspaceNodeCombined> allSubspaceNodes;
    uint32_t subspaceCount = -1;

    /// Pointer to the grid's gridstorage object
    //SGPP::base::GridStorage* storage = nullptr;
    uint32_t totalRegularGridPoints = -1;

#ifdef X86COMBINED_WRITE_STATS
    size_t refinementStep = 0;
    ofstream statsFile;
    string csvSep = "& ";
#endif

    void prepareSubspaceIterator();

    void listMultInner(size_t dim, const double * const datasetPtr, SGPP::base::DataVector &alpha, size_t dataIndexBase,
                       size_t end_index_data, SubspaceNodeCombined &subspace, double *levelArrayContinuous,
                       size_t validIndicesCount, size_t *validIndices, size_t *levelIndices,
                       //size_t *nextIterationToRecalcReferences, size_t nextIterationToRecalc,
                       double *evalIndexValuesAll, uint32_t *intermediatesAll);

    void uncachedMultTransposeInner(size_t dim, const double * const datasetPtr, size_t dataIndexBase,
                                    size_t end_index_data, SubspaceNodeCombined &subspace, double *levelArrayContinuous,
                                    size_t validIndicesCount, size_t *validIndices, size_t *levelIndices,
                                    //size_t *nextIterationToRecalcReferences,
                                    double *componentResults, double *evalIndexValuesAll, uint32_t *intermediatesAll);

    void setCoefficients(SGPP::base::DataVector &surplusVector);

    void unflatten(SGPP::base::DataVector &result);

    static uint32_t flattenIndex(size_t dim, std::vector<uint32_t> &maxIndices, std::vector<uint32_t> &index);

    void setSurplus(std::vector<uint32_t> &level, std::vector<uint32_t> &maxIndices, std::vector<uint32_t> &index,
                    double value);

    void getSurplus(std::vector<uint32_t> &level, std::vector<uint32_t> &maxIndices, std::vector<uint32_t> &index,
                    double &value, bool &isVirtual);

    uint32_t flattenLevel(size_t dim, size_t maxLevel, std::vector<uint32_t> &level);

public:

#include "../../OperationMultipleEvalSubspace/combined/OperationMultipleEvalSubspaceCombined_calculateIndexCombined.hpp"

    OperationMultipleEvalSubspaceCombined(SGPP::base::Grid &grid, SGPP::base::DataMatrix &dataset);

    ~OperationMultipleEvalSubspaceCombined();

    void prepare() override;

    void multTransposeImpl(SGPP::base::DataVector &alpha, SGPP::base::DataVector &result, const size_t start_index_data,
                           const size_t end_index_data) override;

    void multImpl(SGPP::base::DataVector &source, SGPP::base::DataVector &result, const size_t start_index_data,
                  const size_t end_index_data) override;

    SGPP::base::DataMatrix *padDataset(SGPP::base::DataMatrix &dataset);

    size_t getAlignment() override;

    std::string getImplementationName() override;

    size_t getPaddedDatasetSize() override;
};

}
}
