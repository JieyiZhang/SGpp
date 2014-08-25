/* ****************************************************************************
* Copyright (C) 2014 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
**************************************************************************** */
// @author Julian Valentin (julian.valentin@stud.mathematik.uni-stuttgart.de)

#include "opt/grid/BsplineTrapezoidBoundaryGrid.hpp"
#include "base/grid/generation/TrapezoidBoundaryGridGenerator.hpp"

namespace sg
{
namespace opt
{

BsplineTrapezoidBoundaryGrid::BsplineTrapezoidBoundaryGrid(std::istream &istr) :
    base::Grid(istr),
    degree(1 << 16)
{
    istr >> degree;
}

BsplineTrapezoidBoundaryGrid::BsplineTrapezoidBoundaryGrid(size_t dim, size_t degree) :
    degree(degree)
{
    this->storage = new base::GridStorage(dim);
}

BsplineTrapezoidBoundaryGrid::~BsplineTrapezoidBoundaryGrid()
{
}

const char *BsplineTrapezoidBoundaryGrid::getType()
{
    return "BsplineTrapezoidBoundary";
}

size_t BsplineTrapezoidBoundaryGrid::getDegree()
{
    return this->degree;
}

base::Grid *BsplineTrapezoidBoundaryGrid::unserialize(std::istream &istr)
{
    return new BsplineTrapezoidBoundaryGrid(istr);
}

void BsplineTrapezoidBoundaryGrid::serialize(std::ostream &ostr)
{
    this->base::Grid::serialize(ostr);
    ostr << degree << std::endl;
}

base::GridGenerator *BsplineTrapezoidBoundaryGrid::createGridGenerator()
{
    return new base::TrapezoidBoundaryGridGenerator(this->storage);
}

}
}
