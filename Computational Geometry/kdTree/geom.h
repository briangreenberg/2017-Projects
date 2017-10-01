#ifndef __geom_h
#define __geom_h

#include <cstddef>
#include <vector>

using namespace std;
typedef struct _point2d {
  double x,y; 
} point2D;


typedef struct _segment2d {
  point2D lowLeft; 
  point2D upRight; 
} segment2D;


typedef struct _treeNode treeNode;
struct _treeNode {
     point2D p; /* If this is a leaf node,  p represents the point stored in this leaf. 
                  If this is not a leaf node,  p represents the horizontal or vertical line
                  stored in this node. For a vertical line, p.y is
                  ignored. For a horizontal line, p.x is ignored
                */

     char type; /* this can be 'h' (horizontal) or 'v' (vertical), or 'l' (leaf)
                    depending whether the node splits with a horizontal line or  vertical line.
                    Technically this should be an enum.
                */
     treeNode  *left, *right; /* left/below and right/above children. */
};

typedef struct _kdtree{
   treeNode* root; 
  
   int count; //number of nodes  in the tree

   int height; //height of tree
  
} kdtree; 

bool compareByX(point2D a, point2D b);
bool compareByY(point2D a, point2D b);
vector<point2D> sortByX(vector<point2D> points);
vector<point2D> sortByY(vector<point2D> points);
kdtree makeEmptyTree(void);
kdtree buildKdTree(vector<point2D> byX, vector<point2D> byY, int depth, point2D lowLeft, point2D upRight, vector<segment2D>& recs);
void printTree(kdtree tree);
kdtree build(vector<point2D> points,point2D upper, point2D lower, vector<segment2D>& emptyRec);
#endif
