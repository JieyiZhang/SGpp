// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/datadriven/application/LearnerDensityBasedReg.hpp>
#include <sgpp/datadriven/DatadrivenOpFactory.hpp>
#include <sgpp/base/operation/BaseOpFactory.hpp>
#include <sgpp/pde/operation/PdeOpFactory.hpp>
#include <sgpp/datadriven/algorithm/DensitySystemMatrix.hpp>
#include <sgpp/base/exception/application_exception.hpp>
#include <sgpp/solver/sle/ConjugateGradients.hpp>
#include <sgpp/solver/sle/BiCGStab.hpp>
#include <sgpp/base/tools/GridPrinter.hpp>
#include <sgpp/base/grid/generation/functors/SurplusRefinementFunctor.hpp>

#include <sgpp/globaldef.hpp>

#include <string>

namespace sgpp {
namespace datadriven {

LearnerDensityBasedReg::LearnerDensityBasedReg(
  sgpp::datadriven::RegularizationType& regularization,
  double border) :
  LearnerBase(true), CMode_(regularization), C_(NULL), maxValue_(0.), minValue_(
    0.), border_(border) {
}

LearnerDensityBasedReg::~LearnerDensityBasedReg() {
  if (C_ != NULL)
    delete C_;
}

sgpp::datadriven::DMSystemMatrixBase* LearnerDensityBasedReg::createDMSystem(
  sgpp::base::DataMatrix& trainDataset, double lambda) {
  // Is not used
  return NULL;
}

LearnerTiming LearnerDensityBasedReg::train(sgpp::base::DataMatrix&
    trainDataset,
    sgpp::base::DataVector& classes,
    const sgpp::base::RegularGridConfiguration& GridConfig,
    const sgpp::solver::SLESolverConfiguration& SolverConfigRefine,
    const sgpp::solver::SLESolverConfiguration& SolverConfigFinal,
    const sgpp::base::AdpativityConfiguration& AdaptConfig,
    bool testAccDuringAdapt, const double lambda) {
  LearnerTiming result;

  size_t dim = trainDataset.getNcols();
  size_t m = trainDataset.getNrows();

  if (m != classes.getSize()) {
    throw base::application_exception(
      "LearnerBase::train: length of classes vector does not match to dataset!");
  }

  result.timeComplete_ = 0.0;
  result.timeMultComplete_ = 0.0;
  result.timeMultCompute_ = 0.0;
  result.timeMultTransComplete_ = 0.0;
  result.timeMultTransCompute_ = 0.0;
  result.timeRegularization_ = 0.0;
  result.GFlop_ = 0.0;
  result.GByte_ = 0.0;

  execTime_ = 0.0;
  GFlop_ = 0.0;
  GByte_ = 0.0;

  double oldAcc = 0.0;

  sgpp::solver::SLESolver* myCG;

  if (SolverConfigRefine.type_ == sgpp::solver::SLESolverType::CG) {
    myCG = new sgpp::solver::ConjugateGradients(
      SolverConfigRefine.maxIterations_, SolverConfigRefine.eps_);
  } else if (SolverConfigRefine.type_ == sgpp::solver::SLESolverType::BiCGSTAB) {
    myCG = new sgpp::solver::BiCGStab(SolverConfigRefine.maxIterations_,
                                      SolverConfigRefine.eps_);
  } else {
    throw base::application_exception(
      "LearnerDensityBasedReg::train: An unsupported SLE solver type was chosen!");
  }

  sgpp::base::SGppStopwatch* myStopwatch = new sgpp::base::SGppStopwatch();

  if (isVerbose_)
    std::cout << "Starting Learning...." << std::endl;

  // Normalize "class" vector and append it to the train data matrix to create a m x (n+1) matrix
  // for estimation of Pr[x, t]:
  sgpp::base::DataVector classes_copy(classes);
  classes_copy.minmax(&minValue_, &maxValue_);
  classes_copy.normalize(border_);
  sgpp::base::DataMatrix densityMatrix(m, dim + 1);
  double* densityData = densityMatrix.getPointer();
  double* trainData = trainDataset.getPointer();
  double* classesData = classes_copy.getPointer();
  size_t acc = 0;

  for (size_t i = 0; i < m; i++) {
    size_t j = 0;

    for (; j < dim; j++) {
      densityData[i * (dim + 1) + j] = trainData[acc + j];
    }

    densityData[i * (dim + 1) + j] = classesData[i];
    acc += dim;
  }

  // Adjust grid configuration to the higher dimension:
  sgpp::base::RegularGridConfiguration config_adap;
  config_adap.dim_ = GridConfig.dim_ + 1;
  config_adap.level_ = GridConfig.level_;
  config_adap.type_ = GridConfig.type_;

  // Construct Grid
  if (alpha_ != NULL)
    delete alpha_;

  if (grid_ != NULL)
    delete grid_;

  if (isTrained_ == true)
    isTrained_ = false;

  InitializeGrid(config_adap);

  // check if grid was created
  if (grid_ == NULL)
    return result;

  for (size_t i = 0; i < AdaptConfig.numRefinements_ + 1; i++) {
    if (isVerbose_)
      std::cout << std::endl << "Doing refinement: " << i << std::endl;

    myStopwatch->start();

    // Do Refinements
    if (i > 0) {
      sgpp::base::SurplusRefinementFunctor myRefineFunc(*alpha_, AdaptConfig.noPoints_);
      grid_->getGenerator().refine(myRefineFunc);

      alpha_->resize(grid_->getSize());

      // DMSystem->rebuildLevelAndIndex();   not implemented

      if (isVerbose_)
        std::cout << "New Grid Size: " << grid_->getSize() << std::endl;
    } else {
      if (isVerbose_)
        std::cout << "Grid Size: " << grid_->getSize() << std::endl;
    }

    // regularization term:

    // Clean up, if needed
    if (C_ != NULL)
      delete C_;

    if (this->CMode_ == sgpp::datadriven::RegularizationType::Laplace) {
      C_ = sgpp::op_factory::createOperationLaplace(*grid_).release();
    } else if (this->CMode_ == sgpp::datadriven::RegularizationType::Identity) {
      C_ = sgpp::op_factory::createOperationIdentity(*grid_).release();
    } else {
      // should not happen
    }

    sgpp::datadriven::DensitySystemMatrix DMatrix(*grid_, densityMatrix, *C_,
        lambda);
    sgpp::base::DataVector rhs(grid_->getSize());
    DMatrix.generateb(rhs);

    if (i == AdaptConfig.numRefinements_) {
      myCG->setMaxIterations(SolverConfigFinal.maxIterations_);
      myCG->setEpsilon(SolverConfigFinal.eps_);
    }

    myCG->solve(DMatrix, *alpha_, rhs, false, false, -1.0);

    if (isVerbose_) {
      std::cout << "Needed Iterations: " << myCG->getNumberIterations()
                << std::endl;
      std::cout << "Final residuum: " << myCG->getResiduum() << std::endl;
    }

    if (testAccDuringAdapt) {
      double acc = getAccuracy(trainDataset, classes);

      if (isVerbose_)
        std::cout << "MSE (train): " << acc << std::endl;

      if ((i > 0) && (oldAcc <= acc)) {
        if (isVerbose_)
          std::cout << "The grid is becoming worse --> stop learning"
                    << std::endl;

        break;
      }

      oldAcc = acc;
    }

    execTime_ += myStopwatch->stop();

    // use post-processing to determine Flops and time
    if (i < AdaptConfig.numRefinements_) {
      postProcessing(trainDataset, SolverConfigRefine.type_,
                     myCG->getNumberIterations());
    } else {
      postProcessing(trainDataset, SolverConfigFinal.type_,
                     myCG->getNumberIterations());
    }
  }

  result.timeComplete_ = execTime_;

  isTrained_ = true;

  if (isVerbose_) {
    std::cout << "Finished Training!" << std::endl << std::endl;
    std::cout << "Training took: " << execTime_ << " seconds" << std::endl
              << std::endl;
  }

  delete myStopwatch;
  delete myCG;

  return result;
}

sgpp::base::DataVector LearnerDensityBasedReg::predict(
  sgpp::base::DataMatrix& testDataset) {
  sgpp::base::DataVector res(testDataset.getNrows());

  double delta = (maxValue_ - minValue_) / (1 - 2 * border_);

  size_t dim = testDataset.getNcols();
  size_t m = testDataset.getNrows();
  sgpp::base::DataVector point(dim);

  for (size_t i = 0; i < m; i++) {
    testDataset.getRow(i, point);

    sgpp::base::Grid* tempGrid = grid_;
    sgpp::base::Grid* lastGrid = NULL;
    sgpp::base::DataVector* lastAlpha = alpha_;

    // Conditionalize for all dimensions, but the last one:
    for (size_t j = 0; j < dim; j++) {
      sgpp::base::DataVector* tempAlpha = new sgpp::base::DataVector(1);
      sgpp::op_factory::createOperationDensityConditional(*tempGrid)->
          doConditional(*lastAlpha, tempGrid, *tempAlpha, 0, point.get(j));

      if (j > 0) {
        delete lastAlpha;
        delete lastGrid;
      }

      lastGrid = tempGrid;
      lastAlpha = tempAlpha;
    }

    // Compute conditional expectation:
    double value_normalized =
        sgpp::op_factory::createOperationFirstMoment(*lastGrid)->doQuadrature(*lastAlpha);

    res.set(i, ((value_normalized - border_) * delta) + minValue_);

    delete lastAlpha;
    delete lastGrid;
  }

  return res;
}

void LearnerDensityBasedReg::dumpDensityAtPoint(sgpp::base::DataVector& point,
    std::string fileName, unsigned int resolution) {
  size_t dim = point.getSize();
  sgpp::base::Grid* tempGrid = grid_;
  sgpp::base::Grid* lastGrid = NULL;
  sgpp::base::DataVector* lastAlpha = alpha_;

  // Conditionalize for all dimensions, but the last one:
  for (size_t j = 0; j < dim; j++) {
    sgpp::base::DataVector* tempAlpha = new sgpp::base::DataVector(1);
    sgpp::op_factory::createOperationDensityConditional(*tempGrid)->doConditional(
        *lastAlpha, tempGrid, *tempAlpha, 0, point.get(j));

    if (j > 0) {
      delete lastAlpha;
      delete lastGrid;
    }

    lastGrid = tempGrid;
    lastAlpha = tempAlpha;
  }

  sgpp::base::GridPrinter myPlotter(*lastGrid);
  myPlotter.printGrid(*lastAlpha, fileName, resolution);
}

}  // namespace datadriven
}  // namespace sgpp

