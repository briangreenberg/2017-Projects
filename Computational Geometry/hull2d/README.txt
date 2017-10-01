README:

Implementation of the graham scan algorithm to find the convex hull of a vector of points represented as x,y
coordinate pairs.

To Test on random points: 
- cd into the cpp directory
- run the makefile with "make"
- run viewpoints x to draw the convex hull of x randomly generated points.

To run test Cases:

Default test cases will be run by pressing 1,2,3, and 4 respectively.
Custom Test cases can be cycled through by pressing 5.

Known Bugs:

Diagonally alligned points with respect to the leftmost reference point sometimes cause error.
Having tests where multiple points are stored with the same angle sometimes cause a segfault error.

We were able to successfully able to run all test cases besides the diagonal line examples:
- diagonal sloping line
- stairs

There are also very rare segfault errors which occur nondetermistically with random points probably due to the diagonal segfault error.

Lab 2 Submission by Brian and Osborn!

