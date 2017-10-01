#include "geom.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>

using namespace std; 



/* returns 2 times the signed area of triangle abc. The area is positive if c
   is to the left of ab, and negative if c is to the right of ab
 */
int signed_area3D(point3d a, point3d b, point3d c, point3d p) 
{
  double volume, ax, ay, az, bx, by, bz, cx, cy, cz;

  ax = a.x - p.x;
  ay = a.y - p.y;
  az = a.z - p.z;

  bx = b.x - p.x;
  by = b.y - p.y;
  bz = b.z - p.z;

  cx = c.x - p.x;
  cy = c.y - p.y;
  cz = c.z - p.z;

  //Signed area found calculated by finding the determinant of the 4x4 matrix including all coordinate values.

  volume = (ax * (by * cz - bz *cy)) + (ay * (bz*cx - bx * cz)) + (az * (bx * cy - by * cx));
  if(volume > 0){return 1;}
  if(volume == 0){return 0;}
  return -1;
}

/* return 1 if p,q,r, t on same plane, and 0 otherwise */
int collinear(point3d p, point3d q, point3d r, point3d t) 
{
  if(signed_area3D(p,q,r,t) == 0)
  {
    return 1;
  } 
  return 0; 
}


/* return 1 if d is  strictly left of abc; 0 otherwise */
int left (point3d a, point3d b, point3d c, point3d d) 
{ 
  if(signed_area3D(a,b,c,d) > 0)
  {
    return 1;
  } 
  return 0;
}


/* compute and return the convex hull of the points */
vector<triangle3d> brute_force_hull(vector<point3d>& points) 
{  
  vector<triangle3d> resultFaces;

  int numFaces = 0;
  int numPoints = points.size();

  //For all possible triangles that could exist in the plane...
  for(int a = 0; a < numPoints; a++)
  {
    for(int b = 0; b < numPoints; b++)
    {
      for(int c = 0; c < numPoints; c++)
      {
        
	int leftCount = 0;        
        
        //Check if they are extreme. Do they form a face that all points fall on either side of.
	for(int d = 0; d < numPoints; d++)
	  {
	    if(a != b && a != c && a != d && b!=c && b!=d && c!=d){
	      if (left(points[a],points[b],points[c],points[d]) == 1){
		leftCount++;
	      }
	      if(collinear(points[a],points[b],points[c],points[d]) == 1){
		  leftCount++; 
		}
	    }
	}

        //If they are extreme, add the face to the convex hull.
	if(leftCount == numPoints-3)
	 {
	 
          triangle3d temp;
          temp.a = &points[a];
          temp.b = &points[b];
	  temp.c = &points[c];
	 
	  resultFaces.push_back(temp);
	  numFaces++;
	  }	
      }
    }
  }
  return resultFaces; 
}
