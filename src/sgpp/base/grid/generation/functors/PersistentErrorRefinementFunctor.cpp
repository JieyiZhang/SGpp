

#include "base/grid/generation/functors/PersistentErrorRefinementFunctor.hpp"

namespace sg {
namespace base {

PersistentErrorRefinementFunctor::PersistentErrorRefinementFunctor(
		DataVector* alpha, Grid* grid, size_t refinements_num, double threshold) :
		alpha(alpha), refinements_num(refinements_num), threshold(threshold), grid(
				grid), trainDataset(
		NULL), classes(NULL), errors(NULL), accum(NULL) {

}
PersistentErrorRefinementFunctor::~PersistentErrorRefinementFunctor() {
}

/*
 * current = \sum_{i=0}^{N} (r_i + y_i) * \phi_j(x_i)
 * accum[j] = BETA * accum[j] + current[j]
 * functor value = -alpha_j * accum[j]
 */
double PersistentErrorRefinementFunctor::operator()(GridStorage* storage,
		size_t seq) {

	const double BETA = 0.9;
	const size_t MIN_SUPPORT = 10;

	if (trainDataset == NULL || classes == NULL) {
		throw base::application_exception(
				"Training dataset or classes not set");
	}

	// Create and resize accum vector
	size_t numCoeff = alpha->getSize();

	if (accum == NULL) {
		accum = new sg::base::DataVector(numCoeff);
		accum->setAll(0.0);
	} else if (accum->getSize() != numCoeff) {
		accum->resizeZero(numCoeff);
	}

	size_t numData = trainDataset->getNrows();

	DataVector phi_x(numData);

	sg::base::DataVector singleAlpha(alpha->getSize());
	singleAlpha.setAll(0.0);
	singleAlpha.set(seq, 1.0);

	sg::op_factory::createOperationMultipleEval(*grid, trainDataset)->mult(
			singleAlpha, phi_x);

	if (phi_x.getNumberNonZero() < MIN_SUPPORT) {
		return -std::numeric_limits<double>::infinity(); // threshold is 0.0
	}

	// current = \sum_{i=0}^{N} (r_i + y_i) * \phi_j(x_i)

	double current = 0;

	for (size_t i = 0; i < numData; i++) {
		/* (r_i + y_i) * \phi_j(x_i) */
		current += (errors->get(i) + classes->get(i)) * phi_x[i];
	}

	accum->set(seq, accum->get(seq) * BETA + current);

	return -alpha->get(seq) * accum->get(seq);
}

double PersistentErrorRefinementFunctor::start() {
	return this->threshold;
}

size_t PersistentErrorRefinementFunctor::getRefinementsNum() {
	return this->refinements_num;
}

double PersistentErrorRefinementFunctor::getRefinementThreshold() {
	return this->threshold;
}

void PersistentErrorRefinementFunctor::setTrainDataset(
    DataMatrix* trainDataset_) {
	trainDataset = trainDataset_;
}

void PersistentErrorRefinementFunctor::setClasses( DataVector* classes_) {
	classes = classes_;
}
void PersistentErrorRefinementFunctor::setErrors( DataVector* errors_) {
  errors = errors_;
}

}
}
