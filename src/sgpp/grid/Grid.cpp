/*****************************************************************************/
/* This file is part of sgpp, a program package making use of spatially      */
/* adaptive sparse grids to solve numerical problems                         */
/*                                                                           */
/* Copyright (C) 2008 Jörg Blank (blankj@in.tum.de)                          */
/* Copyright (C) 2009 Alexander Heinecke (Alexander.Heinecke@mytum.de)       */
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

#include "grid/Grid.hpp"
#include "grid/type/LinearGrid.hpp"
#include "grid/type/LinearBoundaryGrid.hpp"
#include "grid/type/LinearBoundaryUScaledGrid.hpp"
#include "grid/type/ModLinearGrid.hpp"
#include "grid/type/ModPolyGrid.hpp"
#include "grid/type/PolyGrid.hpp"

#include "grid/generation/SurplusRefinementFunctor.hpp"
#include "operation/OperationIdentity.hpp"

#include "exception/factory_exception.hpp"

#include <iostream>
#include <sstream>

namespace sg
{

Grid* Grid::createLinearGrid(size_t dim)
{
	return new LinearGrid(dim);
}

Grid* Grid::createLinearBoundaryGrid(size_t dim)
{
	return new LinearBoundaryGrid(dim);
}

Grid* Grid::createLinearBoundaryUScaledGrid(size_t dim)
{
	return new LinearBoundaryUScaledGrid(dim);
}

Grid* Grid::createModLinearGrid(size_t dim)
{
	return new ModLinearGrid(dim);
}

Grid* Grid::createPolyGrid(size_t dim, size_t degree)
{
	return new PolyGrid(dim, degree);
}

OperationMatrix* Grid::createOperationIdentity()
{
	return new OperationIdentity();
}

Grid* Grid::createModPolyGrid(size_t dim, size_t degree)
{
	return new ModPolyGrid(dim, degree);
}

Grid* Grid::unserialize(std::string& istr)
{
	std::istringstream istream;
	istream.str(istr);

	return Grid::unserialize(istream);
}

Grid* Grid::unserialize(std::istream& istr)
{
	std::string gridtype;
	istr >> gridtype;

	if(typeMap().count(gridtype) > 0)
	{
		// This is pretty esoteric. It calls a function pointer out of a map.
		return typeMap()[gridtype](istr);
	}
	else
	{
		throw factory_exception("factory_exeception unserialize: unkown gridtype");
	}

	return NULL;
}

std::map<std::string, Grid::Factory>& Grid::typeMap()
{
	// This is only executed once!
	static factoryMap* tMap = new factoryMap();
	if(tMap->size() == 0)
	{
		/*
		 * Insert factories here. This methods may NOT read the grid type.
		 * This map takes a string, function pointer pair.
		 */
		tMap->insert(std::make_pair("NULL",Grid::nullFactory));
		tMap->insert(std::make_pair("linear", LinearGrid::unserialize));
		tMap->insert(std::make_pair("linearBoundary", LinearBoundaryGrid::unserialize));
		tMap->insert(std::make_pair("linearBoundaryUScaled", LinearBoundaryUScaledGrid::unserialize));
		tMap->insert(std::make_pair("modlinear", ModLinearGrid::unserialize));
		tMap->insert(std::make_pair("poly", PolyGrid::unserialize));
		tMap->insert(std::make_pair("modpoly", ModPolyGrid::unserialize));
	}

	return *tMap;
}

/**
 * Factory for everything we don't know.
 */
Grid* Grid::nullFactory(std::istream&)
{
	throw factory_exception("factory_exeception unserialize: unsupported gridtype");
	return NULL;
}

Grid::Grid(std::istream& istr) : storage(NULL)
{
	int hasStorage;
	istr >> hasStorage;
	if(hasStorage == 1)
	{
		storage = new GridStorage(istr);
	}
}

Grid::Grid() : storage(NULL)
{
}

Grid::~Grid()
{
	if(storage != NULL)
	{
		delete storage;
	}
}

GridStorage* Grid::getStorage()
{
	return this->storage;
}

BoundingBox* Grid::getBoundingBox()
{
	return this->storage->getBoundingBox();
}

void Grid::serialize(std::string& ostr)
{
	std::ostringstream ostream;
	this->serialize(ostream);

	ostr = ostream.str();
}

void Grid::serialize(std::ostream& ostr)
{
	ostr << this->getType() << std::endl;
	if(storage != NULL)
	{
		ostr << "1" << std::endl;
		storage->serialize(ostr);
	}
	else
	{
		ostr << "0" << std::endl;
	}
}

void Grid::refine(DataVector* vector, int numOfPoints)
{
	//TODO: different refinemente Functors (khakhutv)
	this->createGridGenerator()->refine(new SurplusRefinementFunctor(vector, numOfPoints));
}

double Grid::eval(DataVector& alpha, DataVector& point){
	OperationEval* evalOp = this->createOperationEval();
	return evalOp->eval(alpha, point);
}

void Grid::insertPoint(size_t dim, unsigned int levels[], unsigned int indices[], bool isLeaf){
	//create HashGridIndex object for the point
	GridIndex pointIndex = new GridIndex(dim);
	for (unsigned int i=0; i<dim-1; i++){
		pointIndex.push(i, levels[i], indices[i]);
	}
	//insert last level/index and hash
	pointIndex.set(dim-1, levels[dim-1], indices[dim-1], isLeaf);
	//insert point to the GridStorage
	storage->insert(pointIndex);
}

}
