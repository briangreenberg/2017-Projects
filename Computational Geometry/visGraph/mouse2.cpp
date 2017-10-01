/* mouse1.c 

Laura Toma
What it does:

Shows how to use the mouse in OpenGL.  First the mouse is registered
via a callback function. Once registered, this function will be called
on any mouse event in the window.  The user can use this function to
respond to mouse events. This example will print the coordinates of
the point where the mouse is clicked, and will draw a small blue disk
at the point where the mouse is pressed.

*/

#include "geom.h" 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <algorithm> 
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#include <vector> 

using namespace std; 

typedef struct _node{
  point2D p;
  vector <point2D> visibleNodes;
} node;

GLfloat red[3] = {1.0, 0.0, 0.0};
GLfloat green[3] = {0.0, 1.0, 0.0};
GLfloat blue[3] = {0.0, 0.0, 1.0};
GLfloat black[3] = {0.0, 0.0, 0.0};
GLfloat white[3] = {1.0, 1.0, 1.0};
GLfloat gray[3] = {0.5, 0.5, 0.5};
GLfloat yellow[3] = {1.0, 1.0, 0.0};
GLfloat magenta[3] = {1.0, 0.0, 1.0};
GLfloat cyan[3] = {0.0, 1.0, 1.0};

GLint fillmode = 0;

/* forward declarations of functions */
void display(void);
void keypress(unsigned char key, int x, int y);
void mousepress(int button, int state, int x, int y);
void timerfunc(); 
point2D intersection(point2D p1, point2D p2, point2D p3, point2D p4);
void initialize_polygon(); 
void print_polygon(vector<point2D> poly); 
void draw_polygon(vector<point2D> poly);
int isInside(point2D pointA,int p);
int popLow(vector<int>& pri);
void computeShortestPath();
/* our coordinate system is (0,0) x (WINDOWSIZE,WINDOWSIZE) where the
   origin is the lower left corner */


/* global variables */
const int WINDOWSIZE = 500; 
int direction = 1;
bool computed = false;
//the current polygon 
vector<point2D>  poly;
vector<point2D> shortestPath;
vector< vector<point2D> > polys;
vector <node> visibilityGraph;
vector<neibor> vg;
//coordinates of last mouse click
double mouse_x=10, mouse_y=10, begin_x=50, begin_y=50, end_x=500, end_y=500;
int poly_init_mode = 0;
//initialized to a point outside the window
/*
void endInSight(point2D p)
{
  point2D temp1, temp2, end;
  end.x = end_x;
  end.y = end_y;
  for(int i=0; i < poly.size() - 1; i++){
    temp1 = poly[i];
    temp2 = poly[i+1];
    if(intersection(temp1,temp2, p, end).x != -1){
      return;
    }
  }
  vector<point2D>  poly;
  poly.push_back(p);poly.push_back(end);
  draw_polygon(poly);
  printf("End in sight! \n");
}
*/
bool notSame(int a, int ai, int b, int bi, int c, int ci, int d, int di){
  //check a and b are not the same
  if(a == b && ai == bi){return false;}
  //check a is not c or d
  if(a == c && ai == ci){return false;}
  if(a == d && ai == di){return false;}
  //check b is not c or d
  if(b == c && bi == ci){return false;}
  if(b == d && bi == di){return false;}
  //printf("sasasa \n");
  //printf("%d, %d, %d, %d, %d, %d, %d, %d\n", a, ai, b, bi, c, ci, d, di);
  return true;
}
double calcDist(point2D a, point2D b){
  return sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y));
}
point2D getMid(point2D a, point2D b){
  //get diagnol
  point2D c;
  c.x = (a.x + b.x)/2;
  c.y = (a.y + b.y)/2;
  return c;
}
int isLineOutside(int pol, int a, int b){
  //if they are the same point retuen
  if(a == b){
    return -1;
  }

  //if they are neibors, include
  //check degenerates
  int size = polys[pol].size()-1;
  if((a == 0 && b == size) || (b == 0 && a == size)){return 1;}
  if((a == b+1) || (b == a+1)){return 1;}

  //check if the edge intersects the polygon
  point2D start = polys[pol][a];
  point2D end = polys[pol][b];
  point2D c;
  point2D d;
  point2D ret;
  
  for(int i = 0; i < size; i++){
    c = polys[pol][i];
    d = polys[pol][i+1];
    if(a != i && a != i+1 && b != i && b != i+1){
      ret = intersection(start,end,c,d);
      if(ret.x != -1 || ret.y != -1){
	return -1;
      }
    }
    
  }
  //check the last point
  c = polys[pol][size];
  d = polys[pol][0];
  if(a != size && a != 0 && b != size && b != 0){
    ret = intersection(start,end,c,d);
    if(ret.x != -1 || ret.y != -1){
      return -1;
    }
  }
 
  //check if the mid point is inside
  point2D mid = getMid(start,end);
  printf("%f %f %f ,\n", mid.x, (start.x+ end.x)/2);
  int inside = isInside(mid,pol);
  if(inside == 1){
    return -1;
  }
  return 1;
  
}
void computeVG(){
  //compute vg for robot
  point2D a;
  point2D b;
  point2D c;
  point2D d;
  point2D ret;
  vector<point2D> visiblePoints;
  vector<double> weights;
  neibor temp;
  double dist;
  vector<int> neiborIndices;
  int index = -1;
  int indexA = -1;
  for(int i = 0; i < polys.size(); i++){
    for(int j = 0; j  <polys[i].size(); j++){
      //get the point
      indexA++;
      a = polys[i][j];
      index = -1;
      //find edges to all other points
      for(int k = 0; k < polys.size(); k++){
	for(int l = 0; l < polys[k].size(); l++){
	  //connect to form an edge
	  index++;
	  
	  if(k != i){
	    b = polys[k][l];
	    
	    bool notInside = true;
	    //check if they are the same polygon
	    bool checked = false;
	    //check this edge with all the polygons for intersection
	    bool validEdge = true;
	  
	    for(int m = 0; m < polys.size(); m++){
	      for(int n = 0; n <polys[m].size(); n++){
		int t = n+1;
		if(t >= polys[m].size()){
		  t = 0;
		}
		c = polys[m][n];
		d = polys[m][t];
		//check self intersection
		if(k == i){
		  point2D mid = getMid(a,b);
		  int inside = isInside(mid,k);
		  if(inside == 1){validEdge = false;}
		  
		}
		if(notSame(i,j,k,l,m,n,m,t)){
		  //x pr intf("ujings");
		  checked = true;
		  //check that edge does not go inside just for the same polygon
		  if(k == i){
		    //checked = true;
		    point2D mid = getMid(a,b);
		    //check if mid is inside
		    int inside = isInside(mid,k);
		    //if inside, disqualify the point
		     if(inside == 1){validEdge = false;}
		  }
		  ret = intersection(a,b,c,d);
		  
		  //this edge intersecs with something, not valid
		  if(!(ret.x == -1 && ret.y == -1)){
		    validEdge = false;
		  }
		}
	      }
	    }
	    if(validEdge && checked){
	      //printf("dfdfdfd\n");
	      //if did not find intersection, add the edge
	      visiblePoints.push_back(b);
	      neiborIndices.push_back(index);
	      
	      //calculate distance
	      dist = calcDist(a,b);
	      weights.push_back(dist);
	    }
	  
	    
	    }else if (k == i){
	    b = polys[k][l];
	    int isValid = isLineOutside(k,j,l);
	    if(isValid == 1){
	      visiblePoints.push_back(b);
	      neiborIndices.push_back(index);
	      dist = calcDist(a,b);
	      weights.push_back(dist);
	    }
	  
      	  }
	
	}
      } 
	  
      //get rid of points that are inside
     
      //create this points with its edge
      if(visiblePoints.size() != 0){
	temp.curr = a;
	temp.distance = 999999999;
	temp.indices = neiborIndices;
	temp.currIndex = indexA;
	temp.neibors = visiblePoints;
	temp.dist = weights;
	vg.push_back(temp);
	
      }
      visiblePoints.clear();
      weights.clear();
      neiborIndices.clear();
    }
  }
  
}

point2D intersection(point2D p1, point2D p2, point2D p3, point2D p4) {
  double x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
  double y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;
  point2D ret;
  ret.x = -1;
  ret.y = -1;

  double d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
  // If d is zero, there is no intersection
  if (d == 0) return ret;
  
  // Get the x and y
  double pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
  double x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
  double y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;
  
  // Check if the x and y coordinates are within both lines
  if ( x < min(x1, x2) || x > max(x1, x2) ||  x < min(x3, x4) || x > max(x3, x4) ) return ret;
  if ( y < min(y1, y2) || y > max(y1, y2) ||  y < min(y3, y4) || y > max(y3, y4) ) return ret;
 
  // Return the point of intersection
  
  ret.x = x;
  ret.y = y;
  // printf("Intersect at (x=%d, y=%d)\n", (int)ret.x, (int)ret.y);
  return ret;
}

void draw_circle(double x, double y){
  glColor3fv(blue);

  if(x == begin_x){glColor3fv(green);}
  if(x == end_x){glColor3fv(red);}
  int precision = 100;
  double r = 4; 
  double theta = 0;
  glBegin(GL_POLYGON);
  for(int i = 0; i < precision; i++){
    theta = i * 2 * M_PI/precision;
    glVertex2f(x + r*cos(theta), y + r*sin(theta));
  }
  glEnd();
}

int isSimple(){
  point2D inter;
  point2D a,b,c,d;
  int i,j;
  if(poly.size() < 3){return 0;}
  for (i = 0; i < (int)poly.size()-1; i++){
    a = poly[i];
    b = poly[i+1];
    for (j = 0; j < (int)poly.size()-1; j++){
      if( i != j && i != j+1 && i+1 != j && i+1 != j+1){      
	c = poly[j];
	d = poly[j+1];
	inter = intersection(a,b,c,d);
	if(!(inter.x == -1 && inter.y == -1)){return 0;}
      }
    }
  }

  //check the last edge between first vertex and last vertex
  a = poly[0];
  b = poly[poly.size()-1];
  for (j = 1; j < (int)poly.size()-2; j++){
      c = poly[j];
      d = poly[j+1];
      inter = intersection(a,b,c,d);
      if(!(inter.x == -1 && inter.y == -1)){return 0;}
  }
  return 1;
}

int isInside(point2D pointA,int p){
  point2D pointB, pointC, pointD, inter;
  int i, count=0;
  pointB.y = 500;
  pointB.x = 500;
  for (i = 0; i < polys[p].size(); i++){
    pointC = polys[p][i];
    if(i >= polys[p].size()-1){
      pointD = polys[p][0];
    }else{
      pointD = polys[p][i+1];
    }
    inter = intersection(pointA, pointB, pointC, pointD);
    if (!(inter.x == -1 && inter.y == -1)){count++;}
  }
  
  
  
  
  if(count % 2 == 0){
    return 0;
  }
  return 1;
}

void computeShortestPath(){

  vg[0].distance = 0;
  
  vector<int> priorityQ;
  priorityQ.push_back(0);
  int size = priorityQ.size();
  int curr = 0;
  double currDist;
  while(size != 0){
    
    // printf("%d \n", priorityQ.size() );
    int index = priorityQ[0];
    int realIndex = 0;
    double min = vg[index].distance;
    for(int i = 0; i < priorityQ.size(); i++){

      if(min > vg[priorityQ[i]].distance){
	min = vg[priorityQ[i]].distance;
	index = priorityQ[i];
	realIndex = i;
      }

    }
    priorityQ.erase(priorityQ.begin()+realIndex);
    curr =  index;

    //get neighbors of current node
    for(int i = 0; i < vg[curr].indices.size();i++){
      
      currDist = vg[curr].distance + vg[curr].dist[i];
      int indexPoint = vg[curr].indices[i];

      //printf("%d %d\n", currDist, vg[index].distance);
      if(currDist < vg[indexPoint].distance){
	vg[indexPoint].distance = currDist;
	vg[indexPoint].prev = curr;
	priorityQ.push_back(indexPoint);
	
      }
    }
    size = priorityQ.size();
  }
  for(int q = 0; q < vg.size(); q++){
    //printf("%f\n", vg[q].distance);

  }
  
  int pathIndex = vg.size()-1;
  shortestPath.push_back(vg[pathIndex].curr);
  while(pathIndex != 0)
  {
    //printf("%d\n", vg[pathIndex].prev);
   pathIndex = vg[pathIndex].prev;
   shortestPath.push_back(vg[pathIndex].curr);
   }   
}
int popLow(vector<int>& pri){
  int index = pri[0];
  int realIndex = 0;
  double min = vg[index].distance;
  for(int i = 0; i < pri.size(); i++){
    
    if(min > vg[pri[i]].distance){
      min = vg[pri[i]].distance;
      index = pri[i];
      realIndex = i;
    }
    
  }
  pri.erase(pri.begin()+realIndex);
  return index;
}
//This function determines if the obstacles are overlapping
//if that is the case, we remove the last polygon insterted

bool overlapPoly(){
  //for all the polygons check if there vertices intersect
  if(polys.size() <= 1){
    //return false;
  }
  //check last poly with the other polys
  int last = polys.size();
  point2D a;
  point2D b;
  point2D c;
  point2D d;
  point2D inter;
  for (int i = 0; i < poly.size();i++){
    a = poly[i];
    //handle looping around
    if(i >= poly.size()-1){
      b = poly[0];
    }else{
      b = poly[i+1];
    }
    
    //check intersections with other polygons
    for(int j = 0; j < polys.size(); j++){
      for(int k = 0; k < polys[j].size(); k++){
	c = polys[j][k];
	//handle looping around 
	if(k >= polys[j].size()-1){
	  d = polys[j][0];
	}else{
	  d = polys[j][k+1];
	}
	inter = intersection(a,b,c,d);
	if(!(inter.x == -1 && inter.y == -1)){return true;}
	
       
      }
    }
  }
  return false;
}
/* 
Usage

void glutMouseFunc(void (*func)(int button, int state, int x, int y));

Description

glutMouseFunc sets the mouse callback for the current window. When a
user presses and releases mouse buttons in the window, each press and
each release generates a mouse callback. The button parameter is one
of GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON. For
systems with only two mouse buttons, it may not be possible to
generate GLUT_MIDDLE_BUTTON callback. For systems with a single mouse
button, it may be possible to generate only a GLUT_LEFT_BUTTON
callback. The state parameter is either GLUT_UP or GLUT_DOWN
indicating whether the callback was due to a release or press
respectively. The x and y callback parameters indicate the window
relative coordinates when the mouse button state changed. If a
GLUT_DOWN callback for a specific button is triggered, the program can
assume a GLUT_UP callback for the same button will be generated
(assuming the window still has a mouse callback registered) when the
mouse button is released even if the mouse has moved outside the
window.
*/

void mousepress(int button, int state, int x, int y) {


  if(state == GLUT_DOWN) {
    
    mouse_x = x;
    mouse_y = y;
    //(x,y) are in wndow coordinates, where the origin is in the upper
    //left corner; our reference system has the origin in lower left
    //corner, this means we have to reflect y
    mouse_y = WINDOWSIZE - mouse_y; 
    //if this is the first point, 
    // printf("mouse click at (x=%d, y=%d)\n", (int)mouse_x, (int)mouse_y);
    
    if (poly_init_mode ==1) {
      if(poly.size() != 0 && mouse_x <= poly[0].x + 4 
	 && mouse_x >= poly[0].x -4 && mouse_y <= poly[0].y+4 && mouse_y >= poly[0].y -4){
	poly_init_mode = 0;
	polys.push_back(poly);
      }else{
	point2D p;
	p.x = mouse_x;
	p.y = mouse_y;
	poly.push_back(p);
      }
    }else if(poly_init_mode == 0 && poly.size() != 0){
      /*//draw visiblePoly
      point2D guard;
      guard.x = mouse_x;
      guard.y = mouse_y;
      printf("mouse click at (x=%d, y=%d)\n", (int)mouse_x, (int)mouse_y);
      int inside = isInside(guard);
      int simple = isSimple();
       if(inside == 1 && simple == 1){
	 // visiblePoly.clear();
	 printf("");
       }else if(simple != 1){
	 poly.clear();
	 }*/
    }
    
  }
  glutPostRedisplay();
}


/* ****************************** */
/* initialize  polygon stored in global variable poly  */
//Note: use our local coordinate system (0,0) to (WINSIZE,WINSIZE),
//with the origin in the lower left corner.
void initialize_polygon() {
  
  //clear the vector, in case something was there 
  poly.clear(); 

  int n = 10; //size of polygon 
  double rad = 100; 
  double step = 2 * M_PI / n;
  point2D p; 
  for (int i=0; i<n; i++) {

    p.x = WINDOWSIZE/2 + rad * cos (i * step); 
    p.y = WINDOWSIZE/2 + rad * sin (i * step); 


    //insert the segment in the array of segments 
    poly.push_back (p);
    ;
    
  } //for i
  polys.push_back(poly);
}

/* ************************************************** */
void print_polygon(vector<point2D> poly) {

  for (unsigned int i=0; i<poly.size()-1; i++) {
    printf("edge %f: [(%f,%f), (%f,%f)]\n",
	   i, poly[i].x, poly[i].y, poly[i+1].x, poly[i+1].y);
  }
  //print last edge from last point to first point 
  int last = poly.size()-1; 
    printf("edge %f: [(%f,%f), (%f,%f)]\n",
	   last, poly[last].x, poly[last].y, poly[0].x, poly[0].y);

}




/* ****************************** */
int main(int argc, char** argv) {

  //initialize_polygon();
  //print_polygon(poly);

  /* initialize GLUT  */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(WINDOWSIZE, WINDOWSIZE);
  glutInitWindowPosition(100,100);
  glutCreateWindow(argv[0]);

  /* register callback functions */
  glutDisplayFunc(display); 
  glutKeyboardFunc(keypress);
  glutMouseFunc(mousepress); 
  //  glutIdleFunc(timerfunc); 

  /* init GL */
  /* set background color black*/
  glClearColor(0, 0, 0, 0);   
  /* here we can enable depth testing and double buffering and so
     on */
  
  /* give control to event handler */
  glutMainLoop();
  return 0;
}


/* ****************************** */
/* draw the polygon */

void draw_polygon(vector<point2D> poly){
  
  if (poly.size() == 0) return; 

  //set color 
  glColor3fv(yellow);   
  
  int i;
  for (i=0; i<poly.size()-1; i++) {
    glBegin(GL_LINES);
    glVertex2f(poly[i].x, poly[i].y); 
    glVertex2f(poly[i+1].x, poly[i+1].y);
    glEnd();
  }
  //render last segment between last point and forst point 
  int last=poly.size()-1; 
    glBegin(GL_LINES);
    glVertex2f(poly[last].x, poly[last].y); 
    glVertex2f(poly[0].x, poly[0].y);
    glEnd();
}

void drawVG(){
  if(vg.size() == 0){
    return;
  }
  glColor3fv(red);
  point2D a;
  point2D b;
  
  for(int i = 0; i< vg.size();i++){
    a = vg[i].curr;
    for(int j = 0; j < vg[i].neibors.size(); j++){
      b = vg[i].neibors[j];
      glBegin(GL_LINES);
      glVertex2f(a.x, a.y);
      glVertex2f(b.x, b.y);
      glEnd();
      
    }
  }
}

void drawSP(){
  if(shortestPath.size() == 0){
    return;
  }
  glColor3fv(cyan);
  point2D a;
  point2D b;
  for(int i = 0; i< shortestPath.size()-1;i++){
    a = shortestPath[i];
    b = shortestPath[i+1];
    glBegin(GL_LINES);
    glVertex2f(a.x, a.y);
    glVertex2f(b.x, b.y);
    glEnd();
  }
}



/* ****************************** */
void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity(); //clear the matrix

  /* The default GL window is [-1,1]x[-1,1] with the origin in the
     center. 
     
     Our system of coordinates (in which we generate our points) is
     (0,0) to (WINSIZE,WINSIZE), with the origin in the lower left
     corner.

     We need to map the points to [-1,1] x [-1,1]  

     Assume we are the local coordinate system. 

     First we scale down to [0,2] x [0,2] */ 
  glScalef(2.0/WINDOWSIZE, 2.0/WINDOWSIZE, 1.0);  
  /* Then we translate so the local origin goes in the middle of teh
     window to (-WINDOWSIZE/2, -WINDOWSIZE/2) */
  glTranslatef(-WINDOWSIZE/2, -WINDOWSIZE/2, 0); 
  
  //now we draw in our local coordinate system (0,0) to
  //(WINSIZE,WINSIZE), with the origin in the lower left corner.
  
  draw_polygon(poly);
  drawVG();
  
  for(int i=0; i< (int)polys.size(); i++){
    draw_polygon(polys[i]);
  }
  drawSP();
  //draw a circle where the mouse was last clicked. Note that this
  //point is stored as a global variable and is modified by the mouse handler function 
  draw_circle(mouse_x, mouse_y); 
  draw_circle(begin_x, begin_y); 
  draw_circle(end_x, end_y); 
  
  /* execute the drawing commands */
  glFlush();
}



/* ****************************** */
void keypress(unsigned char key, int x, int y) {
  int simp;
  switch(key) {
  case 'q':	
    exit(0);
    break;
  case 'b':
    begin_x=mouse_x, begin_y=mouse_y;
    glutPostRedisplay(); 
    break;
  case 's': 
    //start re-initializeing polygon 
    poly.clear();
   
    mouse_x = mouse_y=50; 
    poly_init_mode = 1;
    //simp = isSimple();
    glutPostRedisplay();
    break;
    
  case 'f':
    end_x=mouse_x, end_y=mouse_y;
    
    glutPostRedisplay(); 
    break;
  case 'c':
    polys.clear();
    vg.clear();
    poly.clear();
    shortestPath.clear();
    glutPostRedisplay();
    break;
  case 'p':
    computeShortestPath();                                                                                                                        
    glutPostRedisplay();
    computed = true;
    break;
  case 'z':
    
    vg.clear();
    poly.clear();
    poly_init_mode=0;
    point2D p;
    p.x = begin_x;p.y = begin_y;
    poly.push_back(p);
    polys.insert(polys.begin(),poly);
    poly.clear();
    p.x = end_x;p.y = end_y;
    poly.push_back(p);
    polys.push_back(poly);
    shortestPath.clear();
    poly.clear();
    computeVG();
    // computeShortestPath();
    glutPostRedisplay();
    //printf("%d  %d\n", polys[1].size(),polys.size());
    //print_polygon(shortestPath);
    //computed = true;
    
    break;

  case 'e': 
    poly_init_mode=0;
    //polys.push_back(poly);
    bool inValid = overlapPoly();
    //do not add the polygon if it overlaps with previous polygons
    if(inValid == true){
      printf("LAST POLYGON OVERLAPS \n");
      //remove the last polygon
      //polys.erase(polys.begin()+polys.size()-1);
    }else{
      polys.push_back(poly);
    }
    poly.clear();
    glutPostRedisplay();

    break;
   


  }
}


/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
     
   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);
 
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset
   gluOrtho2D(0.0, (GLdouble) width, 0.0, (GLdouble) height); 
}





void timerfunc() {
  
    
  glutPostRedisplay(); 

}
