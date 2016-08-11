/* Copyright (C) 2008-today The SG++ project

 * This file is part of the SG++ project. For conditions of distribution and
 * use, please see the copyright notice provided with SG++ or at
 * sgpp.sparsegrids.org
 *
 * regressionPipeline.cpp
 *
 *  Created on: 01.06.2016
 *      Author: Michael Lettrich
 */

#include <sgpp/datadriven/datamining/builder/DataSourceBuilder.hpp>
#include <sgpp/datadriven/datamining/configuration/DataMiningConfigurationLeastSquares.hpp>
#include <sgpp/datadriven/datamining/modules/crossValidation/CrossValidation.hpp>
#include <sgpp/datadriven/datamining/modules/crossValidation/RandomShufflingFunctor.hpp>
#include <sgpp/datadriven/datamining/modules/crossValidation/ShufflingFunctor.hpp>
#include <sgpp/datadriven/datamining/modules/dataSource/DataSource.hpp>
#include <sgpp/datadriven/datamining/modules/fitting/ModelFittingLeastSquares.hpp>
#include <sgpp/datadriven/datamining/modules/scoring/MSE.hpp>
#include <sgpp/datadriven/datamining/modules/scoring/SimpleSplittingScorer.hpp>
#include <sgpp/globaldef.hpp>

#include <iostream>
#include <memory>
#include <string>

using sgpp::datadriven::DataSourceBuilder;
using sgpp::datadriven::DataSource;
using sgpp::datadriven::Dataset;
using sgpp::datadriven::ModelFittingLeastSquares;
using sgpp::datadriven::DataMiningConfigurationLeastSquares;
using sgpp::datadriven::MSE;
using sgpp::datadriven::SimpleSplittingScorer;
using sgpp::datadriven::CrossValidation;
using sgpp::datadriven::RandomShufflingFunctor;
using sgpp::base::GridType;

int main(int argc, char **argv) {
  // input
  std::string path;
  if (argc != 2) {
    std::cout << "No or bad path given, aborting" << std::endl;
    exit(-1);
  } else {
    path = std::string(argv[1]);
  }

  auto dataSource = std::unique_ptr<DataSource>(DataSourceBuilder().withPath(path).assemble());
  std::cout << "reading input file: " << path << std::endl;
  auto dataset = std::unique_ptr<Dataset>(dataSource->getNextSamples());

  // regression
  auto config = std::make_shared<DataMiningConfigurationLeastSquares>();
  // set grid dim
  auto gridConfig = config->getGridConfig();
  gridConfig.level_ = 2;
  gridConfig.type_ = GridType::ModLinear;
  gridConfig.dim_ = dataset->getDimension();
  config->setGridConfig(gridConfig);
  config->setLambda(10e-1);

  std::cout << "starting 5 fold cross validation with seed 42" << std::endl;
  auto model = std::make_shared<ModelFittingLeastSquares>(config);
  auto metric = std::make_shared<MSE>();
  auto shuffling = std::make_shared<RandomShufflingFunctor>();
  auto stdDeviation = std::make_shared<double>();
  CrossValidation crossValidation(metric, shuffling, 42);
  double score = crossValidation.calculateScore(*model, *dataset, 5, stdDeviation);

  std::cout << "Score = " << score << " with stdDeviation " << *stdDeviation << std::endl;


  return 0;
}