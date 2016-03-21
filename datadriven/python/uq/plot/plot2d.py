from pysgpp.extensions.datadriven.uq.operations import evalSGFunction
from pysgpp import DataVector, DataMatrix

import numpy as np
import matplotlib.pyplot as plt
from pysgpp.extensions.datadriven.uq.operations.sparse_grid import evalSGFunctionMulti


def plotDensity2d(U, n=50, addContour=True):
    xlim, ylim = U.getBounds()

    x = np.linspace(xlim[0], xlim[1], n)
    y = np.linspace(ylim[0], ylim[1], n)
    Z = np.ones((n, n))

    xv, yv = np.meshgrid(x, y, sparse=False, indexing='xy')
    for i in xrange(len(x)):
        for j in xrange(len(y)):
            Z[j, i] = U.pdf([xv[j, i], yv[j, i]])

    # np.savetxt('density2d.csv', z.reshape(n * n, 3), delimiter=' ')

    plt.imshow(Z, interpolation='bicubic', aspect='auto',
               extent=[xlim[0], xlim[1], ylim[0], ylim[1]])

    plt.jet()
    cbar = plt.colorbar()
    cbar.ax.set_ylabel(r'$\hat{f}(\xi_1, \xi_2)$')

    if addContour:
        cs = plt.contour(xv, yv, Z, colors='black')
        plt.clabel(cs, inline=1, fontsize=18)

def plotSGDE2d(U, n=100):
    gs = U.grid.getStorage()
    x = [0.0] * gs.size()
    y = [0.0] * gs.size()

    for i in xrange(gs.size()):
        x[i] = gs.get(i).getCoord(0)
        y[i] = gs.get(i).getCoord(1)

    neg_x = []
    neg_y = []
    neg_z = []
    xlim, ylim = U.getBounds()

    xv, yv = np.meshgrid(x, y, sparse=False, indexing='xy')
    for i in xrange(len(x)):
        for j in xrange(len(y)):
            value = U.pdf([xv[j, i], yv[j, i]])

            if value < 0 and abs(value) > 1e-14:
                neg_x.append(xv[j, i])
                neg_y.append(yv[j, i])
                neg_z.append(value)

    # plot image of density
    plotDensity2d(U)

    # plot grid points
#     plt.plot(x, y, linestyle=' ', marker='o', color="white")

    # plot data points
#     plt.plot(U.trainData.array()[:, 0], U.trainData.array()[:, 1],
#              linestyle=' ', marker='x', color='orange')

    # plot negative areas
    if len(neg_z) > 0:
        plt.plot(neg_x, neg_y, linestyle=' ', marker='o', color='red')
        plt.title("N=%i, [%g, %g]" % (U.grid.getSize(), min(neg_z), max(neg_z)))
    else:
        plt.title("N=%i" % U.grid.getSize())
    plt.xlim(0, 1)
    plt.ylim(0, 1)


def plotFunction2d(f, addContour=True, n=101,
                   xlim=[0, 1], ylim=[0, 1]):
    x = np.linspace(xlim[0], xlim[1], n)
    y = np.linspace(ylim[0], ylim[1], n)
#     X, Y = np.meshgrid(x, y)
    Z = np.ones(n * n).reshape(n, n)

    xv, yv = np.meshgrid(x, y, sparse=False, indexing='xy')
    for i in xrange(len(x)):
        for j in xrange(len(y)):
            Z[j, i] = f(xv[j, i], yv[j, i])

#     for i in xrange(len(X)):
#         for j, (xi, yi) in enumerate(zip(X[i], Y[i])):
#             Z[i, j] = f(xi, 1 - yi)

    plt.imshow(Z[::-1, :], interpolation='bilinear',
               extent=(xlim[0], xlim[1], xlim[0], xlim[1]))

    plt.jet()
    plt.colorbar()

    if addContour:
        cs = plt.contour(xv, yv, Z, colors='black')
#         cs = plt.contour(X, 1 - Y, Z, colors='black')
        plt.clabel(cs, inline=1, fontsize=18)

    return


def plotSG2d(grid, alpha, addContour=True, n=100,
             show_negative=False, show_grid_points=False):
    gs = grid.getStorage()

    gpxp = []
    gpyp = []

    gpxn = []
    gpyn = []

    for i in xrange(gs.getSize()):
        if alpha[i] > 0:
            gpxp.append(gs.get(i).getCoord(0))
            gpyp.append(gs.get(i).getCoord(1))
        else:
            gpxn.append(gs.get(i).getCoord(0))
            gpyn.append(gs.get(i).getCoord(1))

    x = np.linspace(0, 1, n)
    y = np.linspace(0, 1, n)
    xv, yv = np.meshgrid(x, y, sparse=False, indexing='xy')
    Z = np.ones(n * n).reshape(n, n)

    neg_x = []
    neg_y = []
    neg_z = []

    A = DataMatrix(n * n, 2)
    p = DataVector(2)
    # do vectorized evaluation
    k = 0
    for i in xrange(len(x)):
        for j in xrange(len(y)):
            p[0] = xv[i, j]
            p[1] = xy[i, j]
            A.setRow(k, p)
            k += 1

    res = evalSGFunctionMulti(grid, alpha, A)

    k = 0
    for i in xrange(len(x)):
        for j in xrange(len(y)):
            Z[i, j] = res[k]
            if Z[i, j] < 0 and abs(Z[i, j]) > 1e-13:
                neg_x.append(xv[i, j])
                neg_y.append(xy[i, j])
                neg_z.append(res[k])
            k += 1

    plt.imshow(Z, interpolation='bilinear', extent=(0, 1, 0, 1))

    if len(neg_z) > 0 and show_negative:
        plt.plot(neg_x, neg_y, linestyle=' ', marker='o', color='red')
        plt.title("[%g, %g]" % (min(neg_z), max(neg_z)))

    # plot surpluses
    if show_grid_points:
        plt.plot(gpxp, gpyp, "^ ", color="white")
        plt.plot(gpxn, gpyn, "v ", color="red")

    plt.jet()
    plt.colorbar()

    if addContour:
        cs = plt.contour(X, 1 - Y, Z, colors='black')
        plt.clabel(cs, inline=1, fontsize=18)

    return res


def plotGrid2d(grid, alpha=None):
    gs = grid.getStorage()
    gps = {'p': np.zeros([0, 2]),
           'n': np.zeros([0, 2])}
    p = DataVector(2)
    for i in xrange(gs.size()):
        gs.get(i).getCoords(p)
        if alpha is None or alpha[i] >= 0:
            gps['p'] = np.vstack((gps['p'], p.array()))
        else:
            gps['n'] = np.vstack((gps['n'], p.array()))

    # plot the grid points
    plt.plot(gps['p'][:, 0], gps['p'][:, 1], "^ ", color='red')
    plt.plot(gps['n'][:, 0], gps['n'][:, 1], "v ", color='red')
    plt.xlim(0, 1)
    plt.ylim(0, 1)


def plotSamples2d(samples):
    X = np.zeros(len(samples))
    Y = np.zeros(len(samples))
    for i, sample in enumerate(samples):
        X[i], Y[i] = sample.getActiveProbabilistic()
    plt.plot(X, Y, linestyle=' ', marker='o')
