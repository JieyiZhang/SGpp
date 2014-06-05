/******************************************************************************
* Copyright (C) 2012 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Florian Zipperle (florian.zipperle@tum.de)

#include "OperationMatrixLTwoDotExplicitPeriodic.hpp"
#include "base/exception/data_exception.hpp"
#include "base/grid/Grid.hpp"
#include <string.h>
#include <math.h>
#include <vector>

#include <iostream>

namespace sg {
  namespace pde {

  	OperationMatrixLTwoDotExplicitPeriodic::OperationMatrixLTwoDotExplicitPeriodic(sg::base::DataMatrix* m,
        sg::base::Grid* grid) :
      ownsMatrix_(false) {
      m_ = m;
      buildMatrix(grid);
    }

    OperationMatrixLTwoDotExplicitPeriodic::OperationMatrixLTwoDotExplicitPeriodic(sg::base::Grid* grid) :
      ownsMatrix_(true) {
      m_ = new sg::base::DataMatrix(grid->getStorage()->size(),
                                    grid->getStorage()->size());
      buildMatrix(grid);
    }

    void OperationMatrixLTwoDotExplicitPeriodic::buildMatrix(sg::base::Grid* grid) {
      size_t gridSize = grid->getStorage()->size();
      size_t gridDim = grid->getStorage()->dim();

      sg::base::DataMatrix level(gridSize, gridDim);
      sg::base::DataMatrix index(gridSize, gridDim);

      grid->getStorage()->getLevelIndexArraysForEval(level, index);

      for (size_t i = 0; i < gridSize; i++) {
        for (size_t j = i; j < gridSize; j++) {
          double res = 1;

          for (size_t k = 0; k < gridDim; k++) {
            double lik = level.get(i, k);
            double ljk = level.get(j, k);
            double iik = index.get(i, k);
            double ijk = index.get(j, k);

            if(lik == 1 || ljk == 1){ // level 0
              if(lik > 2 || ljk > 2){
            	  if(lik < ljk){
            		lik = 2;
            		iik = 1;

            		if(ijk < ljk/2){
            		  ijk = ljk/2 - ijk;
            		}else{
            		  ijk = ijk - ljk/2;
            		}
            	  } else {
            		ljk = 2;
            		ijk = 1;
            		if(iik < lik/2){
					  iik = lik/2 - iik;
					}else{
					  iik = iik - lik/2;
					}
            	  }
            	  //Use formula for different overlapping ansatz functions:
				  if (lik > ljk) { //Phi_i_k is the "smaller" ansatz function
				    double diff = (iik / lik) - (ijk / ljk); // x_i_k - x_j_k
				    double temp_res = fabs(diff - (1 / lik))
					  				+ fabs(diff + (1 / lik)) - fabs(diff);
				    temp_res *= ljk;
				    temp_res = (1 - temp_res) / lik;
				    res *= temp_res;
				  } else { //Phi_j_k is the "smaller" ansatz function
				    double diff = (ijk / ljk) - (iik / lik); // x_j_k - x_i_k
				    double temp_res = fabs(diff - (1 / ljk))
				  					+ fabs(diff + (1 / ljk)) - fabs(diff);
				    temp_res *= lik;
				    temp_res = (1 - temp_res) / ljk;
				    res *= temp_res;
				  }
              }else {
            	// if l1 == 0 and l2 == 0 => lik = 1 and ljk = 1 =>
            	// res = 1/3
            	// if l1 == 1 or l2 == 1 => lik = 2 or ljk = 2 =>
            	// res = 1/6
            	res = 1.0 / 3 / (lik * ljk);
            	break;
              }
            }else if (lik == ljk) {
              if (iik == ijk) { // case 4
                //Use formula for identical ansatz functions:
                res *= 2 / lik / 3;
              } else { //case 0
                //Different index, but same level => ansatz functions do not overlap:
                res = 0.;
                break;
              }
            } else {
              if (std::max((iik - 1) / lik, (ijk - 1) / ljk)
                  >= std::min((iik + 1) / lik, (ijk + 1) / ljk)) {
                //Ansatz functions do not not overlap:
                res = 0.;
                break;
              } else {
                //Use formula for different overlapping ansatz functions:
                if (lik > ljk) { //Phi_i_k is the "smaller" ansatz function
                  double diff = (iik / lik) - (ijk / ljk); // x_i_k - x_j_k
                  double temp_res = fabs(diff - (1 / lik))
                                    + fabs(diff + (1 / lik)) - fabs(diff);
                  temp_res *= ljk;
                  temp_res = (1 - temp_res) / lik;
                  res *= temp_res;
                } else { //Phi_j_k is the "smaller" ansatz function
                  double diff = (ijk / ljk) - (iik / lik); // x_j_k - x_i_k
                  double temp_res = fabs(diff - (1 / ljk))
                                    + fabs(diff + (1 / ljk)) - fabs(diff);
                  temp_res *= lik;
                  temp_res = (1 - temp_res) / ljk;
                  res *= temp_res;
                }
              }
            }
          }

          m_->set(i, j, res);
          m_->set(j, i, res);
        }
      }
    }

    OperationMatrixLTwoDotExplicitPeriodic::~OperationMatrixLTwoDotExplicitPeriodic() {
      if (ownsMatrix_)
        delete m_;
    }

    void OperationMatrixLTwoDotExplicitPeriodic::mult(sg::base::DataVector& alpha,
        sg::base::DataVector& result) {

      size_t nrows = m_->getNrows();
      size_t ncols = m_->getNcols();

      if (alpha.getSize() != ncols || result.getSize() != nrows) {
        throw sg::base::data_exception("Dimensions do not match!");
      }

      double* data = m_->getPointer();

      //Standard matrix multiplication:
      double temp = 0.;
      size_t acc = 0;

      for (size_t i = 0; i < nrows; i++) {
        for (size_t j = 0; j < ncols; j++) {
          temp += data[j + acc] * alpha[j];
        }

        result[i] = temp;
        temp = 0.;
        acc += ncols;
      }
    }

  } /* namespace pde */
} /* namespace sg */





