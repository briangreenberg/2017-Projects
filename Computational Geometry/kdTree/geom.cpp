#include "geom.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <algorithm>
using namespace std;
 
bool compareByX(point2D a, point2D b){
  if(a.x == b.x){
    return a.y < b.y;
  }

  return a.x < b.x;
}

bool compareByY(point2D a, point2D b){
  if(a.y == b.y){
    return a.x < b.x;
  }
  return a.y < b.y;
}
vector<point2D> sortByX(vector<point2D> points){
  std::sort(points.begin(), points.end(), compareByX);
  return points;
}
vector<point2D> sortByY(vector<point2D> points){
  std::sort(points.begin(), points.end(), compareByY);
  return points;
}
 
kdtree makeEmptyTree(void){
  kdtree empty;
  empty.count = 0;
  empty.height = 0;
  empty.root = NULL;
  return empty;
}


kdtree buildKdTree(vector<point2D> byX, vector<point2D> byY, int depth, point2D lowLeft, point2D upRight, vector<segment2D>& recs){
  kdtree tree = makeEmptyTree();
  tree.count = byX.size();
  
 
  vector<point2D> split_lo_byX;
  vector<point2D> split_lo_byY;
  vector<point2D> split_hi_byX;
  vector<point2D>split_hi_byY;

  char type;
  point2D  median;
  //base cases where one element remains
  if(byY.size() == 1){
    treeNode temp;
    temp.type = 'l';
    temp.p = byY[0];
    temp.left = NULL;
    temp.right = NULL;
    tree.root = &temp;
    tree.count = 1;
    tree.height = 0;
    return tree;
  }

  if (byX.size() == 1){
    treeNode temp;
    temp.type = 'l';
    temp.p = byX[0];
    temp.left = NULL;
    temp.right = NULL;
    tree.root = &temp;
    tree.count = 1;
    tree.height = 0;
    return tree;
  }
  //if vertical bisection
  else if (depth % 2 == 0){
    int  half_size = byX.size() / 2;
    //get median and position it to the left
    median = byX[half_size-1];
   
    //divide byX
    std::vector<point2D> split_lo(byX.begin(), byX.begin() + byX.size()/2);
    std::vector<point2D> split_hi(byX.begin() + byX.size()/2, byX.end());
    type = 'v';
    split_lo_byX = split_lo;
    split_hi_byX = split_hi;
   

    bool redud = false;
    //sort the half by y
    for(int i = 0; i < byY.size(); i++){
      if(byY[i].x == median.x){
	if(byY[i].y < median.y){
	  split_lo_byY.push_back(byY[i]);

	}
	//if median is redudant, only add it once
	else if (byY[i].y == median.y && !redud){
	  split_lo_byY.push_back(byY[i]);
	  redud = true;
	}
	else {
	  split_hi_byY.push_back(byY[i]);

	}
      }
     else if(byY[i].x < median.x){
	split_lo_byY.push_back(byY[i]);
      }
     else if (byY[i].x > median.x){
        split_hi_byY.push_back(byY[i]);
      }
    }
    //if right half is empty, add one element
    if(split_hi_byY.size() == 0){
      split_hi_byY.push_back(median);
    }
   
  }
  //if horizantal bisection
  else{
      int half_size = byY.size() / 2;
      
      median = byY[half_size-1];
     
      //split by y axis
      std::vector<point2D>  split_lo(byY.begin(), byY.begin() + byY.size()/2);
      std::vector<point2D> split_hi(byY.begin() + byY.size()/2, byY.end());
      type = 'h';
      median = byY[half_size-1];
      split_lo_byY = split_lo;
      split_hi_byY = split_hi;
    
      bool redud = false;
      for(int i = 0; i < byX.size(); i++){
	if(byX[i].y == median.y){
	  if(byX[i].x < median.x){
	    split_lo_byX.push_back(byX[i]);
	  }
	  //only add the redundant element once
	  else if (byX[i].x == median.x && !redud){
	    split_lo_byX.push_back(byX[i]);
	    redud = true;
	  }
	  else {
	    split_hi_byX.push_back(byX[i]);
	  }
	}

	else if(byX[i].y < median.y){
          split_lo_byX.push_back(byX[i]);
        }
        else if (byX[i].y > median.y){
          split_hi_byX.push_back(byX[i]);
        }
      }
      if(split_hi_byX.size() == 0){
	split_hi_byX.push_back(median);
      }
      
    }

  //construct rectangle
  point2D lnewlowLeft;
  point2D rnewlowLeft;

  point2D lnewupRight;
  point2D rnewupRight;
  //if horizontal cut

  if (type == 'h'){
    lnewlowLeft = lowLeft;
    lnewupRight.x = upRight.x;
    lnewupRight.y = median.y;
   
    rnewlowLeft.x = lowLeft.x;
    rnewlowLeft.y = median.y;
    rnewupRight = upRight;

  }

  else if (type == 'v'){
    lnewlowLeft = lowLeft;
    lnewupRight.x = median.x;
    lnewupRight.y = upRight.y;

    rnewlowLeft.x = median.x;
    rnewlowLeft.y = lowLeft.y;
    rnewupRight = upRight;
  }
  
  //save for drawing
  //add rectangle to the vector of rectangles
  segment2D tempRec;

   tempRec.lowLeft = lowLeft;
   tempRec.upRight = upRight;
   recs.push_back(tempRec);
  
  kdtree vleft = buildKdTree(split_lo_byX, split_lo_byY, depth + 1, rnewlowLeft, rnewupRight, recs);
 
  kdtree vright = buildKdTree(split_hi_byX, split_hi_byY, depth + 1, lnewlowLeft, lnewupRight, recs);
 
  
  treeNode node;
  node.left = vleft.root;
  node.right = vright.root;
  node.type = type;
  node.p = median;
  tree.root = &node;
  tree.height = (int)log2(byX.size());
  return tree;
}

void printTree(kdtree tree){
  int height = tree.height;
  int count = tree.count;
  
  printf("Height = %d  Count = %d \n", height, count);

  
}

kdtree build(vector<point2D> points, point2D upper, point2D lower, vector<segment2D>& emptyRec){
  //prepare arguments

  vector<point2D> byX = sortByX(points);
 
 
  vector<point2D> byY = sortByY(points);
  

  //build kdtree
  return buildKdTree(byX, byY, 0,upper, lower, emptyRec);
}
