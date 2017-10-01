/* 
   Laura Toma
*/


#ifndef __geom_h
#define __geom_h
#include <vector>
using namespace std;

typedef struct _point2d {
  double x,y; 
} point2D;

typedef struct _lineSegment2D {
    point2D p1, p2;
} lineSegment2D;

typedef struct _stat {
  int x,y;
  int index;
  int succIndex;
  double cost;
  int theta;
} state;


typedef struct _rect2D  {
    point2D origin;
    float width, height;
} rect2D;
typedef struct _neibor {
  point2D curr;
  int currIndex;
  vector<point2D> neibors;
  vector<double> dist;
  double distance;
  vector<int> indices;
  int prev;
  
} neibor;

double signed_area2D(point2D a, point2D b, point2D c)
{
  return (b.x - a.x)*(c.y-a.y) - (c.x-a.x)*(b.y-a.y);
}

/* return 1 if p,q,r collinear, and 0 otherwise */
int collinear(point2D p, point2D q, point2D r)
{
  double result = signed_area2D(p,q,r);
  if (result == 0)
  {
       return 1;
  }
  return 0;
}

/* return 1 if c is  strictly left of ab; 0 otherwise */
int left (point2D a, point2D b, point2D c)
{
  double result = signed_area2D(a,b,c);
  if (result > 0)
  {
     return 1;
  }
  return 0;
}



//add any functions you might need to operate on these basic types

#endif
