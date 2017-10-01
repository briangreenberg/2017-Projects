
#include "geom.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

using namespace std; 

/* returns the signed area of triangle abc. The area is positive if c
   is to the left of ab, and negative if c is to the right of ab */

int signed_area2D(point2D a, point2D b, point2D c) 
{
  return (b.x - a.x)*(c.y-a.y) - (c.x-a.x)*(b.y-a.y);  
}

/* return 1 if p,q,r collinear, and 0 otherwise */
int collinear(point2D p, point2D q, point2D r) 
{
  int result = signed_area2D(p,q,r);
  if (result == 0) 
  {
       return 1;
  }
  return 0; 
}

/* Print the vector of points and angles */
void print(vector<vector<double> >& angles, vector<point2D> p){
  int i;
 
  for(i=0; i<angles.size();i++){
    printf("[%f, %3d, %3d] ",angles[i][0], p[angles[i][1]].x, p[angles[i][1]].y);
  }
  printf("\n");
}

/* return 1 if c is  strictly left of ab; 0 otherwise */
int left (point2D a, point2D b, point2D c) 
{
  int result = signed_area2D(a,b,c);
  if (result > 0)
  {
     return 1;
  }  
  return 0; 
}


void removeDuplicateAngles(vector<vector<double> >& A, vector<point2D> p)
{
  for(int i = 0; i < A.size()-1; i++)
  {
    while(A[i][0] == A[i+1][0])
    {
      point2D p1 = p[(int)A[i][1]];
      point2D p2 = p[(int)A[i+1][1]];
      if(p1.x < p2.x){A.erase(A.begin() + i);}
      else{A.erase(A.begin() + i + 1);}
    }
  }


}

/* Lomuto partition */
      int partition(vector<vector<double> >& A, int lo, int hi)
{
  vector<double> pivot;
  pivot = A[hi];
  int i = lo - 1;
  int j;
  for (j=lo; j<hi;j++)
  {
    if(A[j][0] <= pivot[0])
    {
      i = i + 1;
      vector<double> temp = A[i];
      A[i] = A[j];
      A[j] = temp;  
    }
  }
  vector<double> tem = A[i+1];
  A[i+1] = A[hi];
  A[hi] = tem;
  return i + 1;
}

/* Given a vector of points and angles, sort the points in radial order by their angles using a partition based quicksort */
      void quicksortByAngle(vector<vector<double> >& a, int lo , int hi)
{
  if (lo<hi)
  {
    int p = partition(a, lo, hi);
    quicksortByAngle(a, lo, p-1);
    quicksortByAngle(a, p+1, hi);
  }
}

/* Given a vector of points and a start point, return a vector of  the possible points along the convex hull sorted by a their angles with respect to the start point*/
vector<point2D> findTan(vector<point2D> p, point2D smallest)
{
  vector<vector<double> > angles;
  int i;
  vector<double> temp;
  vector<point2D> atMinX;

  //For all points that have different x and y coordinates: calculate their angles with respect to start point
  //Points that share an x or y value with start need to be handled separately
  for(i =0; i<p.size(); i++)
  {
    double a = ((double)p[i].y - (double)smallest.y);
    double b = (double(p[i].x) - (double)smallest.x);
    if (a !=0 && b !=0)
      {
       
       temp.push_back(a/b);
       temp.push_back((double)i);
       angles.push_back(temp);
       angles[angles.size() -1][0] = a/b;
       angles[angles.size() -1][1] = i;
     }

    if(smallest.x == p[i].x){
      atMinX.push_back(p[i]);
    }
  }
  
  //We must handle points that fall along the same x axis as the start point.
  //There can only be one point besides start that could be in the convex hull (the one farthest from the start). There is no need to add the other ones to the list of candidates
  int j;
  int indexOfl;
  point2D largestCollinear;
  largestCollinear = smallest;
  for(j=0;j < p.size(); j++)
  {
    if (smallest.y == p[j].y)
    {
       if (largestCollinear.x < p[j].x)
       { 
          largestCollinear = p[j];
          indexOfl = j;
       }
    } 
  }

 //If we find a point that has the same x as start, add it to the vector with an angle of 0 
 //It is critical we do not add multiple points with an angle of 0; luckily, there is only one that could possibly fall along the hull

 if (largestCollinear.x != smallest.x)
 {
   vector<double> specialAngle;
   specialAngle.push_back(0);
   specialAngle.push_back(smallest.x);
   specialAngle.push_back(smallest.y);
   angles.push_back(specialAngle);
   angles[angles.size()-1][0] = 0;
   angles[angles.size()-1][1] = indexOfl;
  }
  int hi = angles.size()-1;
  int lo = 0;
  
  //Now that we have all of the candidate points and their associated angles, we can call a lomuto quicksort to sort them in radial order
  quicksortByAngle(angles, lo, hi);
  removeDuplicateAngles(angles, p);

  //initialize a vector that will hold the final sorted results
  vector<point2D> results;

  //We know that of all points that have leftmost x coordinate, the highest and lowest point will certainly fall along the hull (They might be the same point)
  int k;
  point2D minXmaxY = smallest;  
  for(k=0; k<atMinX.size(); k++)
    {   
      if (atMinX[k].y > minXmaxY.y){minXmaxY = atMinX[k];}
  }
  // printf("%d", atMinX.size());
  //add minXminY at begining of result and minXmaxY at end of result
  results.push_back(smallest);

  int n;
  //add points into a sorted array by angle 
  for(n=0; n<angles.size(); n++)
  {   
    int index = (int) angles[n][1];
    results.push_back(p[index]);
  }
  printf("\n %3d, %3d\n", minXmaxY.x, minXmaxY.y);
  printf("\n %3d, %3d\n", smallest.x, smallest.y);
  results.push_back(minXmaxY);
  results.push_back(smallest);
  results.push_back(smallest);
  
  //Now we have a vector that stores all candidate points in radially sorted order. We can now attempt to find the hull
  return results;

}

/* compute the convex hull of the points in p; the points on the CH
   are returned as a list/vector 
*/
vector<point2D> find_hull(vector<point2D> p)
{
  vector<point2D> result;
  result.push_back(p[0]);
  result.push_back(p[1]);
  
  if(p.size() < 6){
    return result;
  }
  int i;;
  //Trace the candidate points until we find a reflex angle (one where left of the most recent three points != 1)
  //Whenever we find a reflex angle, pop points off until there are no reflex angles, then continue...
  for(i = 2; i < p.size()-1; i++)
  {
    point2D curr = p[i];
    point2D b = result[result.size() -1];
    point2D a = result[result.size() -2];
    if (left(a, b, curr) == 1)
    {
        result.push_back(curr);
    }
    else
    {
      while(left(result[result.size() -2], result[result.size() -1],curr) != 1)
      {
        result.pop_back();        
      }
      result.push_back(curr);
    }
  }
  return result;
}


/* Take an input of a vector of 2d points and return a vector of its convex hull. Call the functions; do the thing! */
vector<point2D> graham_scan(vector<point2D> p) 
{
  vector<point2D> result;
  vector<point2D> cand;
  cand = p;
  point2D minX = p[0];
  int i; 

  for (i=0; i< p.size(); i++){
     
     if (p[i].x == minX.x){
         if (p[i].y < minX.y) {
             minX = p[i];
          }
     }

     if(p[i].x < minX.x){
        minX = p[i];
     }     
  }
  
  vector<point2D> sortedPoints;
  
  sortedPoints = findTan(cand, minX);
  //print(sortedPoints);
   result = find_hull(sortedPoints);
  int m;
  for(m=0;m<result.size();m++){
    // printf("[%3d,%3d] ", result[m].x, result[m].y);
  }
  return result; 
}
