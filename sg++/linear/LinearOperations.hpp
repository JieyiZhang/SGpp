/*
This file is part of sg++, a program package making use of spatially adaptive sparse grids to solve numerical problems

Copyright (C) 2008 Joerg Blank (blankj@in.tum.de)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LINEAROPERATIONS_HPP_
#define LINEAROPERATIONS_HPP_

#include "Operations.hpp"

namespace sg
{


class OperationBLinear : public OperationB
{
public:
	OperationBLinear(GridStorage* storage) : storage(storage) {}
	virtual ~OperationBLinear() {}

	virtual void mult(DataVector& alpha, DataVector& data, DataVector& result);
	virtual void multTranspose(DataVector& alpha, DataVector& data, DataVector& result);

protected:
	GridStorage* storage;
};


class OperationEvalLinear : public OperationEval
{
public:
	OperationEvalLinear(GridStorage* storage) : storage(storage) {}
	virtual ~OperationEvalLinear() {}

	virtual double eval(DataVector& alpha, std::vector<double>& point);
	virtual double test(DataVector& alpha, DataVector& data, DataVector& classes);

protected:
	GridStorage* storage;

};

}

#endif /*LINEAROPERATIONS_HPP_*/
