"""Lego.py
   Methods to create a "lego" style plot using the surface
   plot methods from matplotlib's Axes3D class
"""

import numpy

def makeVectorRedundant(xs, repeat):
    t = numpy.zeros(repeat * xs.shape[0])
    for i in xrange(repeat):
        t[i::repeat] = xs
    return t

def printVals(X,Y,Z):
    print X,"\n"
    print Y,"\n"
    print Z,"\n"


def prepareLegoData(xlims, ylims, zvals):
    """Prepare 3D histogram data to be used with matplotlib's Axes3D/Axes3DI.

    usage example:

    >>> nx, ny = 3, 5
    >>> X, Y, Z = prepareLegoData(numpy.arange(nx), numpy.arange(ny),
    ... numpy.random.rand(nx, ny))
    >>> fig = pylab.figure()
    >>> ax = matplotlib.axes3d.Axes3DI(fig)
    >>> ax.plot_surface(X, Y, Z, rstride=2, cstride=2)
    >>> pylab.show()

    @param xlims: N+1 array with the bin limits in x direction
    @param ylims: M+1 array with the bin limits in y direction
    @param zvals: a 2D array with shape (N, M) with the bin entries,
        example::
        --> y-index (axis 1)
        |       z_0_0  z_0_1  ...
        |       z_1_0  z_1_1  ...
        V        ...
        x-index (axis 0)
    @returns: a tuple containing X, Y, Z 2D-arrays for Axes3D plotting methods
    """
    xlims = numpy.array(xlims)
    ylims = numpy.array(ylims)
    zvals = numpy.array(zvals)

    assert xlims.shape[0] - 1 == zvals.shape[0]
    assert ylims.shape[0] - 1 == zvals.shape[1]

    # use a higher redundancy for surface_plot
    # must be a multiple of 2!
    repeat = 4

    X, Y = numpy.meshgrid(makeVectorRedundant(xlims, repeat),
                          makeVectorRedundant(ylims, repeat))
    Z = numpy.zeros(X.shape)

    # enumerate the columns of th zvals
    for yi, zvec in enumerate(zvals):
        t = makeVectorRedundant(zvec, repeat)
        for off in xrange(1, repeat+1):
            Z[repeat/2:-repeat/2, repeat*yi + off] = t
#    printVals(X,Y,Z)
    return X, Y, Z


def test_lego():
    import pylab
    from mpl_toolkits.mplot3d import axes3d

    X, Y, Z = prepareLegoData([0,1,2], [0,1,2],[[1, 3], [2, 4]])

#    X, Y, Z = prepareLegoData(numpy.arange(3), numpy.arange(3),
#            numpy.array([[1, 3], [2, 4]]))

    #X, Y, Z = prepareLegoData(numpy.arange(4), numpy.arange(5),
    #         numpy.array([[1.0, 1.5, 1.3],
    #                     [1.7, 2.1, 0.8],
    #                     [2.1, 2.0, 0.7],
    #                     [2.5, 1.9, 0.5]])
    #         )

    nx, ny = 10, 8
    z = numpy.random.rand(nx, ny)
#    X, Y, Z = prepareLegoData(numpy.arange(nx + 1), numpy.arange(ny + 1), z)
#    print X,"\n"
#    print Y,"\n"
#    print Z,"\n"
    ax = axes3d.Axes3D(pylab.figure())
    #ax.plot_surface(X, Y, Z, rstride=2, cstride=2)
    ax.plot_surface(X, Y, Z, rstride=2, cstride=2, color='w', edgecolors='k')
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('#')


    pylab.show()

# test_lego()
if __name__ == '__main__':
    test_lego()
