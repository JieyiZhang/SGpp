##############################################################################
# This file is part of pysgpp, a program package making use of spatially    #
# adaptive sparse grids to solve numerical problems                         #
#                                                                           #
# Copyright (C) 2007 Joerg Blank (blankj@in.tum.de)                         #
# Copyright (C) 2007 Richard Roettger (roettger@in.tum.de)                  #
# Copyright (C) 2008 Dirk Plueger (pflueged@in.tum.de)                      #
# Copyright (C) 2009 Alexander Heinecke (Alexander.Heinecke@mytum.de)       #
# Copyright (C) 2009 Valeriy Khakhutskyy (khakhutv@in.tum.de)               #
#                                                                           #
# pysgpp is free software; you can redistribute it and/or modify            #
# it under the terms of the GNU General Public License as published by      #
# the Free Software Foundation; either version 3 of the License, or         #
# (at your option) any later version.                                       #
#                                                                           #
# pysgpp is distributed in the hope that it will be useful,                 #
# but WITHOUT ANY WARRANTY; without even the implied warranty of            #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             #
# GNU Lesser General Public License for more details.                       #
#                                                                           #
# You should have received a copy of the GNU General Public License         #
# along with pysgpp; if not, write to the Free Software                     #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA #
# or see <http://www.gnu.org/licenses/>.                                    #
#############################################################################
#from twisted import im

import unittest

#correct the syspath, so python looks for packages in the root directory of SGpp
import sys, os
pathname = os.path.dirname(__file__)
pathsgpp = os.path.abspath(pathname) + '/../../..'
if pathsgpp not in sys.path: sys.path.append(pathsgpp)

from bin.learner.folding import RandomFoldingPolicy
from bin.data.DataContainer import DataContainer
from bin.pysgpp import DataVector


##
# @package tests.tbin.test_RandomFoldingPolicy
# Contains class test_RandomFoldingPolicy::TestRandomFoldingPolicy with unittests for @link bin.learner.folding.RandomFoldingPolicy.RandomFoldingPolicy RandomFoldingPolicy @endlink

##
# Class with unittests for @link bin.learner.folding.RandomFoldingPolicy.RandomFoldingPolicy RandomFoldingPolicy @endlink
#
# @ingroup tests
#
# @test Unittests for @link bin.learner.folding.RandomFoldingPolicy.RandomFoldingPolicy RandomFoldingPolicy @endlink
class TestRandomFoldingPolicy(unittest.TestCase):

    
    ## Set up the variables
    def setUp(self):
        self.size = 11
        self.level = 10
        self.seed = 42
        points = DataVector(self.size, 1)
        values = DataVector(self.size, 1)
        for i in xrange(self.size):
            points[i] = i
            values[i] = i
        self.dataContainer = DataContainer(points, values)
        self.policy = RandomFoldingPolicy(self.dataContainer, self.level, self.seed)
    
    
    ##
    # Tests the function @link bin.learner.folding.FoldingPolicy.FoldingPolicy.next() RandomFoldingPolicy.next() @endlink    
    def testNext(self):
        self.assertEqual(self.level, len(self.policy.dataFold))
        for l in self.policy:
            sizeTrain = l.getTrainDataset().getPoints().getSize()
            sizeValidation = l.getTestDataset().getPoints().getSize()
            self.assertEqual(self.size, sizeTrain + sizeValidation)
        
        
if __name__=="__main__":
    unittest.main() 