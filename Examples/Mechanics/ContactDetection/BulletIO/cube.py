#!/usr/bin/env python

#
# Example of one object under gravity with one contactor and a ground
#

from Siconos.Mechanics.ContactDetection import Contactor
from Siconos.Mechanics.ContactDetection.Bullet import IO

# Creation of the hdf5 file for input/output
with IO.Hdf5() as io:

    # Definition of a cube as a convex shape
    io.insertConvexShape('Cube', [
        (-1.0, 1.0, -1.0),
        (-1.0, -1.0, -1.0),
        (-1.0, -1.0, 1.0),
        (-1.0, 1.0, 1.0),
        (1.0, 1.0, 1.0),
        (1.0, 1.0, -1.0),
        (1.0, -1.0, -1.0),
        (1.0, -1.0, 1.0)])

    # Alternative to the previous convex shape definition.
    # io.insertPrimitiveShape('Cube1', 'Box', (2, 2, 2))

    # Definition of the ground shape
    io.insertPrimitiveShape('Ground', 'Box', (100, 100, .5))

    # Definition of a non smooth law. As no group ids are specified it
    # is between contactors of group id 0.
    io.insertNewtonImpactFrictionNSL('contact', mu=0.3)

    # The cube object made with an unique Contactor : the cube shape.
    # As a mass is given, it is a dynamic system involved in contact
    # detection and in the simulation.  With no group id specified the
    # Contactor belongs to group 0
    io.insertObject('cube', [Contactor('Cube')], position=[0, 0, 2],
                    velocity=[100, 0, 0, 20, 20, 20],
                    mass=.1)

    # the ground object made with the ground shape. As the mass is
    # not given, it is a static object only involved in contact
    # detection.
    io.insertObject('ground', [Contactor('Ground')],
                    position=[0, 0, 0])


# Run the simulation from the inputs previously defined and add
# results to the hdf5 file. The visualisation of the output may be done
# with the vview command.
with IO.Hdf5(mode='r+') as io:

    io.run()
