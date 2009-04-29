#############################################################################
# This file is part of pysgpp, a program package making use of spatially    #
# adaptive sparse grids to solve numerical problems                         #
#                                                                           #
# Copyright (C) 2009 Alexander Heinecke (Alexander.Heinecke@mytum.de)       #
#                                                                           #
# pysgpp is free software; you can redistribute it and/or modify            #
# it under the terms of the GNU Lesser General Public License as published  #
# by the Free Software Foundation; either version 3 of the License, or      #
# (at your option) any later version.                                       #
#                                                                           #
# pysgpp is distributed in the hope that it will be useful,                 #
# but WITHOUT ANY WARRANTY; without even the implied warranty of            #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             #
# GNU Lesser General Public License for more details.                       #
#                                                                           #
# You should have received a copy of the GNU Lesser General Public License  #
# along with pysgpp; if not, write to the Free Software                     #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA #
# or see <http://www.gnu.org/licenses/>.                                    #
#############################################################################


from optparse import OptionParser
import sys
from tools import *
from pysgpp import *
from math import sqrt
import random
import re
import numpy as np

from array import array

try:
    import psyco
    psyco.full()
    print "Using psyco"
except:
    pass

#-------------------------------------------------------------------------------
## Builds the data vector that hold the coefficients fo either the ansatzfuctions
# or the node base
# re.sub(regex, replacement, subject)
# @param filename name of the ARFF file that contains the coefficients
# @return a instance of a DataVector that stores coefficients
def buildCoefficientVectorFromFile(filename):
    data = readDataARFF(filename);
    
    dim = len(data["data"])
    coeff = DataVector(len(data["data"][0]), dim)
    
    # i iterates over the data points, d over the dimension of one data point
    for i in xrange(len(data["data"][0])):
        for d in xrange(dim):
            coeff[i*dim + d] = data["data"][d][i]
    
    return coeff


#-------------------------------------------------------------------------------
## Opens a file and returns the stored data
# @param filename of file to open
# @return the data stored in the file
def openFile(filename):
    try:
        data = readDataARFF(filename)
    except:
        print ("An error occured while reading " + filename + "!")
        sys.exit(1)
        
    if data.has_key("classes") == False:
        print ("No classes found in the given File " + filename + "!")
        sys.exit(1)
        
    return data    

    
#-------------------------------------------------------------------------------
## Builds the training data vector
# 
# @param data a list of lists that contains the points a the training data set, coordinate-wise
# @return a instance of a DataVector that stores the training data
def buildTrainingVector(data):
    dim = len(data["data"])
    training = DataVector(len(data["data"][0]), dim)
    
    # i iterates over the data points, d over the dimension of one data point
    for i in xrange(len(data["data"][0])):
        for d in xrange(dim):
            training[i*dim + d] = data["data"][d][i]
    
    return training
    
    
#-------------------------------------------------------------------------------
## Wrapper for buildCoefficientVectorFromFile
# @param filename name of the ARFF file that contains the node base coefficients
# @return a instance of a DataVector that stores node base coefficients
def buildNodevalueVector(filename):
    return buildCoefficientVectorFromFile(filename)
    
    
#-------------------------------------------------------------------------------
## Wrapper for buildCoefficientVectorFromFile
# @param filename name of the ARFF file that contains the ansatzfunction coefficients
# @return a instance of a DataVector that stores ansatzfunction coefficients    
def buildAlphaVector(filename):
    return buildCoefficientVectorFromFile(filename)
    

#-------------------------------------------------------------------------------
## Writes reference points to a given file
# @param points string containing the evaluation potins
# @param dim the dimension of the function
# @param function string that describes the function to evaluate
# @param fout filehandle to result file, containing the coordinates and value
# @param foutvalue filehandle to resultfile, containing only the value
def printRefPoint(points, dim, function, fout, foutvalue):
    p = None
    
    p = points.split()
    pc = evalFunction(function, p)
    for y in xrange(dim):
        fout.write("%s " % p[y])
    
    fout.write("%s " % pc)    
    fout.write("\n")
    foutvalue.write("%s " % pc)    
    foutvalue.write("\n")
    
    return 
    
    
#-------------------------------------------------------------------------------
## Recursive generation of the function's coordinate string
# @param dim_rem remaining dimensions
# @param dim dimension of function
# @param points current coordinate string
# @param function string that describes the function to evaluate
# @param resolution number of supporting points
# @param fout filehandle to result file, containing the coordinates and value
# @param foutvalue filehandle to resultfile, containing only the value
def recGenPrintRefVector(dim_rem, dim, points, function, resolution, fout, foutvalue):
    if dim_rem == 0:
        points_save = points
        for x in xrange(resolution):
            points = str(float(x) / (resolution - 1)) + " " + points_save
            printRefPoint(points, dim, function, fout, foutvalue)
            
        fout.write("\n")
        #foutvalue.write("\n")
    else:
        points_save = points
        for x in xrange(resolution):
            points = str(float(x) / (resolution - 1)) + " "+ points_save + str(float(x) / (resolution - 1))
            recGenPrintRefVector(dim_rem-1, dim, points, function, resolution, fout, foutvalue)
        
    return


#-------------------------------------------------------------------------------   
## This allows to evaluate a multidimensional function
# @param filename filename of file with coordinates and function values
# @param filenameValue filename of file with function values only
# @param function string that describes the function to evaluate
# @param resolution number of evaluating points
# @param dim dimension of function
def printRefNDFunction(filename, filenameValue, function, resolution, dim):
    points = ""
    fout = file(filename, "w")
    foutvalue = file(filenameValue, "w")
    
    recGenPrintRefVector(dim-1, dim, points, function, resolution, fout, foutvalue)
    
    fout.close()
    foutvalue.close()
    return
    
    
#-------------------------------------------------------------------------------
## Here a evaluate point on a Sparse Grid is written to a file
# @param p DataVector containing the coordinates of the point
# @param grid reference to the Sparse Grid
# @param alpha hierarchical surplus of the Sparse Grid's Ansatzfunctions 
# @param fout filehandle to result file, containing the coordinates and value
# @param foutvalue filehandle to resultfile, containing only the value
def printPoint(p, grid, alpha, fout, foutvalue):
    pc = grid.createOperationEval().eval(alpha, p)
    for y in xrange(grid.getStorage().dim()):
        fout.write("%s " % p[y])
    
    fout.write("%s " % pc)
    fout.write("\n")
    foutvalue.write("%s " % pc)
    foutvalue.write("\n") 
    
    return 
    
    
#-------------------------------------------------------------------------------
## Evaluates a mutlidimensional function on a Sparse Grid
# @param dim_rem remaining dimensions
# @param p DataVector containing the coordinates of the point
# @param grid reference to the Sparse Grid
# @param alpha hierarchical surplus of the Sparse Grid's Ansatzfunctions 
# @param resolution number of evaluating points
# @param fout filehandle to result file, containing the coordinates and value
# @param foutvalue filehandle to resultfile, containing only the value
def recGenPrintVector(dim_rem, p, grid, alpha, resolution, fout, foutvalue):
    if dim_rem == 0:
        for x in xrange(resolution):
            p[dim_rem] = float(x) / (resolution - 1)
            printPoint(p, grid, alpha, fout, foutvalue)
            
        fout.write("\n")
        #foutvalue.write("\n")
    else:
        for x in xrange(resolution):
            p[dim_rem] = float(x) / (resolution - 1)
            recGenPrintVector(dim_rem-1, p, grid, alpha, resolution, fout, foutvalue)
        
    return


#-------------------------------------------------------------------------------    
## This allows to evaluate a multidimensional function
# @param filename filename of file with coordinates and function values
# @param filenameValue filename of file with function values only
# @param grid reference to the Sparse Grid
# @param alpha hierarchical surplus of the Sparse Grid's Ansatzfunctions 
# @param resolution number of evaluating points
def printNDFunction(filename, filenameValue, grid, alpha, resolution):
    dim = grid.getStorage().dim()
    p = DataVector(1,dim)
    fout = file(filename, "w")
    foutvalue = file(filenameValue, "w")
    
    recGenPrintVector(dim-1, p, grid, alpha, resolution, fout, foutvalue)
    
    fout.close()
    foutvalue.close()
    return


#-------------------------------------------------------------------------------
## Compares two files and calculates the maximum difference between the values
# @param file1 the first file
# @param file2 the second file
# @return the maximum detected difference between two components
def compareResultFiles(file1, file2):
    error = 0.0
    
    vala = 0.0
    valb = 0.0
    
    fa = open(file1, 'r')
    fb = open(file2, 'r')
    vala = fa.readline().strip()
    valb = fb.readline().strip()
    while len(vala) > 0:
        if abs(abs(float(vala))-abs(float(valb))) > error:
            error = abs(abs(float(vala))-abs(float(valb))) 
        
        vala = fa.readline().strip()
        valb = fb.readline().strip()

    fa.close()
    fb.close()
    
    return error


#-------------------------------------------------------------------------------
## hierarchisation of the node base values on a grid
# @param node_values DataVector that holds the coefficients of the function's node base
# @param grid the grid matching to the node_vector
def doHierarchisation(node_values, grid):   
    tmp =  DataVector(grid.getStorage().size(), 1)
    
    for i in xrange(len(node_values)):
        tmp[i] = node_values[i]
    
    # create operation: hierarchisation
    hierarchisation = grid.createOperationHierarchisation()
    
    # execute hierarchisation
    hierarchisation.doHierarchisation(tmp)    

    return tmp


#-------------------------------------------------------------------------------
## hierarchisation of the node base values on a grid
# @param alpha DataVector that holds the coefficients of the sparse grid's ansatzfunctions
# @param grid thee grid matching to the alpha vector
def doDehierarchisation(alpha, grid):
    tmp =  DataVector(grid.getStorage().size(), 1)
    
    for i in xrange(len(alpha)):
        tmp[i] = alpha[i]
         
    # create operation: hierarchisation
    hierarchisation = grid.createOperationHierarchisation()
    
    # execute hierarchisation
    hierarchisation.doDehierarchisation(tmp)
    
    return tmp
    
    
#-------------------------------------------------------------------------------    
## evalutes a given function
# @param function a string the gives the function; x1...xn must be the names of the placeholders
# @param points sorted list of the coordinates (x1...xn) of evaluation point
# @return returns the function value at points
def evalFunction(function, points):
    for i in xrange(len(points)):
        function = re.sub("x" + str(i+1), points[i], function)
            
    return eval(function)    
    

#-------------------------------------------------------------------------------
## tests the correctness of the hierarchisation and dehierachisation
# @param node1 the vector of the node base values before hierarchisation and dehierarchisation
# @param node2 the vector of the node base values after hierarchisation and dehierarchisation
# @return maximum error during the transformations
def testHierarchisationResults(node1, node2):
    error = 0.0
    
    for i in xrange(len(node1)):
        if abs(abs(node1[i])-abs(node2[i])) > error:
            error = abs(abs(node1[i])-abs(node2[i]))
            
    return error
    
    
#-------------------------------------------------------------------------------
## Generates the Laplace Matrix for a given grid using numpy
# @param factory the grid object
# @param verbose default:False prints some additional information
def generateCMatrix(factory, verbose=False):
    from pysgpp import DataVector
    storage = factory.getStorage()
    
    laplace = factory.createOperationLaplace()
    
    # create vector
    alpha = DataVector(storage.size())
    erg = DataVector(storage.size())
    col = 0

    # create stiffness matrix
    m = np.zeros( (storage.size(), storage.size()) )

    for i in xrange(storage.size()):
        # apply unit vectors
        alpha.setAll(0)
        alpha[i] = 1
        laplace.mult(alpha, erg)
        
        #Sets the column in m
        for j in xrange(storage.size()):
            m[j,col] = erg[j]
            
        col = col + 1

    return m

#-------------------------------------------------------------------------------
## Generates the BBT DM Matrix for a given grid using numpy
# @param factory the grid object
# @param training DataVector to contains the used training data
# @param verbose default:False prints some additional information
def generateBBTMatrix(factory, training, verbose=False):
    from pysgpp import DataVector
    storage = factory.getStorage()
       
    b = factory.createOperationB()
    
    alpha = DataVector(storage.size())
    temp = DataVector(training.getSize())
    
    erg = DataVector(alpha.getSize())
    
    col = 0
    
    # create B matrix
    m = np.zeros( (storage.size(), storage.size()) )
    
    for i in xrange(storage.size()):
        # apply unit vectors
        temp.setAll(0.0)
        erg.setAll(0.0)
        alpha.setAll(0.0)
        alpha[i] = 1.0
        b.multTranspose(alpha, training, temp)
        b.mult(temp, training, erg)
        
        #Sets the column in m
        for j in xrange(storage.size()):
            m[j,col] = erg[j]

        col = col + 1
        
    return m

#-------------------------------------------------------------------------------
## Generates the BT DM Matrix for a given grid using numpy
# @param factory the grid object
# @param training DataVector to contains the used training data
# @param verbose default:False prints some additional information
def generateBTMatrix(factory, training, verbose=False):
    from pysgpp import DataVector
    storage = factory.getStorage()
       
    b = factory.createOperationB()
    
    alpha = DataVector(storage.size())
    temp = DataVector(training.getSize())
        
    col = 0
    
    # create BT matrix
    m = np.zeros( (training.getSize(), storage.size()) )
    
    for i in xrange(storage.size()):
        # apply unit vectors
        temp.setAll(0.0)
        alpha.setAll(0.0)
        alpha[i] = 1.0
        b.multTranspose(alpha, training, temp)
        
        #Sets the column in m
        for j in xrange(training.getSize()):
            m[j,col] = temp[j]

        col = col + 1
        
    return m

#-------------------------------------------------------------------------------
## Generates the BT DM Matrix for a given grid only using python arrays
# @param factory the grid object
# @param training DataVector to contains the used training data
# @param verbose default:False prints some additional information
def generateBTMatrixPython(factory, training, verbose=False):
    from pysgpp import DataVector
    storage = factory.getStorage()
       
    b = factory.createOperationB()
    
    alpha = DataVector(storage.size())
    temp = DataVector(training.getSize())
    
    # create BT matrix
    m = DataVector(training.getSize(), storage.size())
    
    for i in xrange(storage.size()):
        # apply unit vectors
        temp.setAll(0.0)
        alpha.setAll(0.0)
        alpha[i] = 1.0
        b.multTranspose(alpha, training, temp)
        
        #Sets the column in m       
        m.setColumn(i, temp)
        
    return m

#-------------------------------------------------------------------------------
## Generates the BBT DM Matrix for a given grid only using python arrays
# @param factory the grid object
# @param training DataVector to contains the used training data
# @param verbose default:False prints some additional information
def generateBBTMatrixPython(factory, training, verbose=False):
    storage = factory.getStorage()
       
    b = factory.createOperationB()
    
    alpha = DataVector(storage.size())
    erg = DataVector(alpha.getSize())
    temp = DataVector(training.getSize())
    
    # create B matrix
    m = DataVector(storage.size(), storage.size())
    for i in xrange(storage.size()):
        # apply unit vectors
        temp.setAll(0.0)
        erg.setAll(0.0)
        alpha.setAll(0.0)
        alpha[i] = 1.0
        b.multTranspose(alpha, training, temp)
        b.mult(temp, training, erg)
        #Sets the column in m
        m.setColumn(i, erg)
        
    return m


#-------------------------------------------------------------------------------
## Reads a Reference matrix and returns it
# @param storage storage of the grid
# @param filename the filename of the file that contains the reference data
# @return returns a python array with the reference matrix
def readReferenceMatrix(storage, filename):
    from pysgpp import DataVector
    # read reference matrix
    try:
        fd = gzOpen(filename, 'r')
    except IOError, e:
        fd = None
        
    if not fd:
        fd = gzOpen('../tests/' + filename, 'r')
        
    dat = fd.read().strip()
    fd.close()
    dat = dat.split('\n')
    dat = map(lambda l: l.strip().split(None), dat)

    #print len(dat)
    #print len(dat[0])
    m_ref = DataVector(len(dat), len(dat[0]))
    for i in xrange(len(dat)):
        for j in xrange(len(dat[0])):
            #print float(dat[i][j])
            m_ref[i*len(dat[0]) + j] = float(dat[i][j])

    return m_ref


#-------------------------------------------------------------------------------
## Reads a DataVector
# @param filename the filename of the file that contains the data
# @return returns a data Vector
def readDataVector(filename):
    
    try:
        fin = tools.gzOpen(filename, 'r')
    except IOError, e:
        fin = None
        
    if not fin:
        fin = tools.gzOpen('tests/' + filename, 'r')
    
    data = []
    classes = []
    hasclass = False

    # get the different section of ARFF-File
    for line in fin:
        sline = line.strip().lower()
        if sline.startswith("%") or len(sline) == 0:
            continue

        if sline.startswith("@data"):
            break
        
        if sline.startswith("@attribute"):
            value = sline.split()
            if value[1].startswith("class"):
                hasclass = True
            else:
                data.append([])
    
    #read in the data stored in the ARFF file
    for line in fin:
        sline = line.strip()
        if sline.startswith("%") or len(sline) == 0:
            continue

        values = sline.split(",")
        if hasclass:
            classes.append(float(values[-1]))
            values = values[:-1]
        for i in xrange(len(values)):
            data[i].append(float(values[i]))
            
    # cleaning up and return
    fin.close()
    return {"data":data, "classes":classes, "filename":filename}

#-------------------------------------------------------------------------------
## Compares, if two BBT matrices are "almost" equal.
# Has to handle the problem that the underlying grid was ordered
# differently. Uses heuristics, e.g. whether the diagonal elements
# and row and column sums match.
# and prints the result for each element
# @param m1 the first matrix 
# @param m2 the second matrix
def compareBBTMatrices(m1, m2):
    # check dimensions
 
    n = m1.getSize()

    # check diagonal
    values = []
    for i in range(n):
        values.append(m1[i*n + i])
    values.sort()
    values_ref = []
    for i in range(n):
        values_ref.append(m2[i*n + i])
    values_ref.sort()

    for i in range(n):
        print values_ref[i], values[i]

    # check row sum
    v = DataVector(n)
    values = []
    for i in range(n):
        m1.getRow(i,v)
        values.append(v.sum())
    values.sort()
    values_ref = []
    for i in range(n):
        m2.getRow(i,v)
        values_ref.append(v.sum())
    values_ref.sort()
    for i in range(n):
        print values_ref[i], values[i]

    # check col sum
    v = DataVector(n)
    values = []
    for i in range(n):
        m1.getColumn(i,v)
        values.append(v.sum())
    values.sort()
    values_ref = []
    for i in range(n):
        m2.getColumn(i,v)
        values_ref.append(v.sum())
    values_ref.sort()
    for i in range(n):
        print values_ref[i], values[i]

#-------------------------------------------------------------------------------
##Compares, if two BT matrices are "almost" equal.
# Has to handle the problem that the underlying grid was ordered
# differently. Uses heuristics, e.g. whether the 
# row and column sums match.
# and prints the result for each element
# @param m1 the first matrix 
# @param m2 the second matrix
def compareBTMatrices(m1, m2): 
    n = m1.getSize() # lines
    m = m1.getDim()  # columns

    # check row sum
    v = DataVector(m)
    values = []
    for i in range(n):
        m1.getRow(i,v)
        values.append(v.sum())
    values.sort()
    values_ref = []
    for i in range(n):
        m2.getRow(i,v)
        values_ref.append(v.sum())
    values_ref.sort()
    for i in range(n):
        print values_ref[i], values[i]

    # check col sum
    v = DataVector(n)
    values = []
    for i in range(m):
        m1.getColumn(i,v)
        values.append(v.sum())
    values.sort()
    values_ref = []
    for i in range(m):
        m2.getColumn(i,v)
        values_ref.append(v.sum())
    values_ref.sort()
    for i in range(m):
        print values_ref[i], values[i]

#-------------------------------------------------------------------------------
## Writes matrix stored in numpy format into a file
# @param filename file's filename to which the data is written
# @param matrix the matrix that should be stored
# @param n number of rows
# @param m number of columns
def writeMatrixToFile(filename, matrix, n, m):
    fout = file(filename, "w")
        
    for i in range(n):
        for j in range(m):
            fout.write(str(matrix[i*m+j]) + " " )
            
        fout.write("\n")
        
    fout.close()
    
    return

