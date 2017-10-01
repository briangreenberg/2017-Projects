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
#include <iostream>
#include <cstdio>
#include <ctime>
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
bool intersection(point2D p1, point2D p2, point2D p3, point2D p4);
void initialize_polygon();  
void draw_polygon(vector<point2D> poly);
int isInside(point2D pointA,int p);
int popLow(vector<int>& pri);
void computeGrid();
bool isFree(int i,int j,int t);
void rotateBot(vector<point2D>& r, int theta, int midX, int midY);
bool crossesPolygon(vector<point2D> r, int midX, int midY);
void translateBot(vector<point2D>& r, int x, int y);
void print_polygon(vector<point2D> poly);
void runAstar();


/* our coordinate system is (0,0) x (WINDOWSIZE,WINDOWSIZE) where the
   origin is the lower left corner */


/* global variables */
const int WINDOWSIZE = 500; 
int direction = 1;
int ogChanged = 0;
int timer = 0;
double duration;
clock_t start;

//the current polygon 
vector<point2D>  poly;
vector< vector<point2D> > polys;
vector<neibor> vg;
vector<point2D> robot;
vector<point2D> robotRect;
int movement = 0; 
vector<state> path;
vector<point2D> animatedBot;
int occupancyGrid[WINDOWSIZE][WINDOWSIZE];
int scoreGrid[WINDOWSIZE][WINDOWSIZE];
point2D final;


vector<point2D> shortestPath;

//coordinates of last mouse click
double mouse_x=10, mouse_y=10, end_x=WINDOWSIZE, end_y=WINDOWSIZE;
int poly_init_mode = 0;
int robot_init_mode = 0;
int final_init_mode = 0;
//initialized to a point outside the window



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
  point2D c;
  c.x = (a.x + b.x)/2;
  c.y = (a.y + b.y)/2;
  return c;
}

int equal(point2D a, point2D b){
  if (a.x == b.x && a.y == b.y){return 1;}
  return 0;
}

bool intersection(point2D p1, point2D p2, point2D p3, point2D p4) {

  double x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
  double y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;
  //if(equal(p1,p2) || equal(p1,p3) || equal(p2,p3) || equal(p3,p4) || equal(p1,p4)) return ret;
  double denom = x1 *(y4-y3)+x2*(y3-y4) + x4*(y2-y1) + x3*(y1-y2);
  if(denom == 0){
    return false;
  }

  double num = x1 *(y4-y3) + x3*(y1-y4) + x4*(y3-y1);
  double s = num/denom;
  num = -(x1*(y3-y2) + x2*(y1-y3) + x3*(y2-y1));
  double t = num/denom;

  if((0.0<=s) && (s<=1.0) && (0.0<= t) && (t<=1.0)){
    return true;
  }
  return false;
}

void draw_circle(double x, double y,int color,double radius ){
  if(color == 1){
    glColor3fv(blue);
  }else if (color == 2){
    glColor3fv(magenta);
  }else if(color == 3){glColor3fv(yellow);}
    else{
      glColor3fv(green);
    }
  if(x == end_x && y == end_y){glColor3fv(yellow);}
  int precision = 100;
  double r = radius; 
  double theta = 0;
  glBegin(GL_POLYGON);
  for(int i = 0; i < precision; i++){
    theta = i * 2 * M_PI/precision;
    glVertex2f(x + r*cos(theta), y + r*sin(theta));
  }
  glEnd();
}
void draw_shortest(){
  if(shortestPath.size() == 0){
    return;
  }
  for(int k = 0; k < (int)shortestPath.size(); k++){
    draw_circle(shortestPath[k].x, shortestPath[k].y,2,1);
  }
}
void draw_points(){
  for(int k = 0; k < (int)path.size(); k++){
    draw_circle(path[k].x, path[k].y,1,1);
  }
}
int isSimple(){
  bool inter;
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
	if(inter){return 0;}
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
      if(inter){return 0;}
  }
  return 1;
}

int isInside(point2D pointA,int p){
  point2D pointB, pointC, pointD;
  int i, count=0;
  bool inter;
  pointB.y = WINDOWSIZE;
  pointB.x = WINDOWSIZE;
  for (i = 0; i < (int)polys[p].size(); i++){
    pointC = polys[p][i];
    if(i >= (int)polys[p].size()-1){
      pointD = polys[p][0];
    }else{
      pointD = polys[p][i+1];
    }
    inter = intersection(pointA, pointB, pointC, pointD);
    if (inter){count++;}
  }
   inter = intersection(pointA, pointB, pointC, pointD);  if (count % 2 == 0){
    return 0;
  }
  return 1;
}


void mousepress(int button, int state, int x, int y) {

  point2D p;
  if(state == GLUT_DOWN) {
    mouse_x = x;
    mouse_y = y;
    //(x,y) are in wndow coordinates, where the origin is in the upper
    //left corner; our reference system has the origin in lower left
    //corner, this means we have to reflect y
    mouse_y = WINDOWSIZE - mouse_y; 
    //if this is the first point, 
    printf("mouse click at (x=%d, y=%d)\n", (int)mouse_x, (int)mouse_y);
    if (poly_init_mode ==1) {
      if(poly.size() != 0 && mouse_x <= poly[0].x + 4 
	 && mouse_x >= poly[0].x -4 && mouse_y <= poly[0].y+4 && mouse_y >= poly[0].y -4){
	poly_init_mode = 0;
	polys.push_back(poly);
      }else{
	p.x = mouse_x;
	p.y = mouse_y;
	poly.push_back(p);
      }
    }
    else if (final_init_mode == 1){
      final.x = mouse_x;
      final.y = mouse_y;
      end_x = mouse_x;
      end_y = mouse_y;
      final_init_mode = 0;
    }
    else if(robot_init_mode == 1){
      if(robot.size() == 1){
	p.x = mouse_x;
	p.y = mouse_y;
	robot.push_back(p);
	robot_init_mode = 0;
      }else{
	point2D p;
	p.x = mouse_x;
	p.y = mouse_y;
	robot.push_back(p);
      }
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
  polys.clear();
}

/* ****************************** */
int main(int argc, char** argv) {
  /* initialize GLUT  */
  //initialize empty grid
  int MAX_INT = WINDOWSIZE * WINDOWSIZE;
                                                                                                
  for(int i = 0; i < WINDOWSIZE; i++){
    for(int j = 0; j < WINDOWSIZE; j++){
      occupancyGrid[i][j] = -1;
      scoreGrid[i][j] = MAX_INT;
    }
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(WINDOWSIZE, WINDOWSIZE);
  glutInitWindowPosition(100,100);
  glutCreateWindow(argv[0]);

  /* register callback functions */
  glutDisplayFunc(display); 
  glutKeyboardFunc(keypress);
  glutMouseFunc(mousepress); 
  glutIdleFunc(timerfunc); 

  /* init GL */
  /* set background color black*/
  glClearColor(100, 100, 100, 100);   
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
  glColor3fv(black);   
  int i;
  for (i=0; i<(int)poly.size()-1; i++) {
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
void draw_robot(){
  if (robot.size() < 2 ) return; 
  //set color 
  glColor3fv(blue);   
  glBegin(GL_LINES);
  glVertex2f(robot[0].x, robot[0].y); 
  glVertex2f(robot[1].x, robot[0].y);
  glVertex2f(robot[1].x, robot[0].y);
  glVertex2f(robot[1].x, robot[1].y);
  glVertex2f(robot[1].x, robot[1].y);
  glVertex2f(robot[0].x, robot[1].y);
  glVertex2f(robot[0].x, robot[1].y);
  glVertex2f(robot[0].x, robot[0].y);
  glEnd();
  //render last segment between last point and first point 

}
void initializeGrid(){
  for(int i = 0; i < WINDOWSIZE; i++){
    for(int j = 0; j < WINDOWSIZE; j++){
      occupancyGrid[i][j] = -1;
    }
  }


}
void computeGrid()
{
  //initialize empty grid
  for(int i = 0; i < WINDOWSIZE; i++){
    for(int j =0; j < WINDOWSIZE; j++){
      occupancyGrid[i][j] = -1;
      scoreGrid[i][j] = WINDOWSIZE*WINDOWSIZE;
    }
  }
  for(int i = 0; i < WINDOWSIZE; i++){
    for(int j = 0; j < WINDOWSIZE; j++){
      for(int theta = 0; theta < 360; theta = theta + 10){
	//cout << i << ' ' << j << ' ' << theta << '\n';
	if(isFree(i,j, theta)){
	  j++;
	  occupancyGrid[i][j] = theta;
	  theta -= 10;
	  continue;
	}
	//else{occupancyGrid[i][j][theta] = 0;}
      }
    }
  }
  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  std::cout<<"Time for computing occupancy grid... "<< duration <<'\n';
  start = clock();
  //run A* to compute shortest path
  runAstar();
  //get the path
  int succIndex = path[path.size()-1].succIndex;
  state curr;
  point2D temp;
  
  while (succIndex != -1){
    curr = path[succIndex];
    temp.x = curr.x;
    temp.y = curr.y;
    shortestPath.push_back(temp);
    succIndex = curr.succIndex;
  }
  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  std::cout<<"Time for computing shortest path with A*... "<< duration <<'\n';
}

//return true if plaving the (origin of the) robot at state s 
//results in no intersection  with any obstacle
bool isFree(int i,int j,int t){
  vector<point2D> robotSquare;
  point2D temp;
  robotSquare.push_back(robot[0]);
  temp.x = robot[1].x;
  temp.y = robot[0].y;
  robotSquare.push_back(temp);  
  robotSquare.push_back(robot[1]);
  temp.x = robot[0].x;
  temp.y = robot[1].y;
  robotSquare.push_back(temp);
  //robotSquare.push_back(robot[0]);
  translateBot(robotSquare, i,j);
  rotateBot(robotSquare, t, i, j);
  if(crossesPolygon(robotSquare,i,j)){
    //cout << "Not free at..." << i << ' ' << j << ' ' << t << '\n';
    return false;
  }
  else{
    //cout << "Free at..." << i << ' ' << j << ' ' << t << '\n';
    return true;
  }
				  
}

void translateBot(vector<point2D>& r, int x, int y){
  //get the mid of the robot
  //following how they are constructed, get the avarage of the cordinates
  int robMidX = (robot[0].x + robot[1].x)/2;
  int robMidY = (robot[0].y + robot[1].y)/2;
  
  //get the vector to the new mid at x and y
  int difX = x - robMidX;
  int difY = y - robMidY;
  //go through all the vertices and move them by the vector
  for(int i = 0; i<(int)r.size(); i++){
    r[i].x += difX;
    r[i].y += difY;
  }
}

void rotateBot(vector<point2D>& r, int theta, int midX, int midY){
  //convert theta to radians
  double rad = theta * M_PI/180;
  int mX = (r[0].x + r[1].x)/2;
  int mY = (r[0].y + r[3].y)/2;
  
  //translate the robot to origin
  //subract mid x from x components of cordinates and do the same for y
  for(int i = 0; i < (int)r.size(); i++){
    r[i].x = r[i].x - mX;
    r[i].y = r[i].y - mY;
  }
  //get the new verteices after rotation given by a formula
  //then translate back to midx and midy
  double xnew;
  double ynew;
  for(int j = 0; j < (int)r.size(); j++){
    xnew= mX + (r[j].x * cos(rad) - r[j].y * sin(rad));
    ynew = mY + (r[j].x * sin(rad) + r[j].y * cos(rad));
    r[j].x = xnew;
    r[j].y = ynew;
  }
  
}

bool crossesPolygon(vector<point2D> r, int midX, int midY){
  point2D a,b,c,d;
  point2D mid;
  bool ret;
  mid.x = midX;
  mid.y = midY;
  //if rotated robot is out bounds 
 
  for(int x = 0; x  < (int)r.size(); x++){

   
    c = r[x];
    if(c.x < 0 || c.x > WINDOWSIZE || c.y < 0|| c.y > WINDOWSIZE){
      return true;
    }
  }
  //check if robot is completely inside an obstacle
  for (int m = 0; m < (int)polys.size(); m++){
    int inside = isInside(mid, m);
    //if mid point is inside then return true
    if(inside == 1){
      return true;
    }
  }
  
  //check if the robot's edges intersect with any obstcle.
  for (int n = 0; n < (int)r.size(); n++){
    //for an edge of the robot
    a = r[n];
    if(n >= (int)r.size()-1){
      b = r[0];
    }else{
      b = r[n+1];
    }
   
    for (int p = 0; p < (int)polys.size(); p++){
      for (int q = 0; q < (int)polys[p].size(); q++){
	c = polys[p][q];
	if(q >= (int)polys[p].size()-1){
	  d = polys[p][0];
	}else{
	  d = polys[p][q+1];
	}
	ret = intersection(a,b,c,d);
	//if it intersects
	if(ret){
	  return true;
	}
	
      }
    }

  }
  // if we get here then there was no intersection
  return false;
}

bool cmp (const state& a, const state& b){
  return a.cost < b.cost;
}
state popMin(vector<state>& p){
  int minIndex = 0;
  state min = p[0];
  for(int i = 0; i < (int)p.size(); i++){
    if((scoreGrid[p[i].x][p[i].y] + fabs(end_x - p[i].x) + fabs(end_y - p[i].y))  < (scoreGrid[min.x][min.y] + fabs(end_x-min.x)+ fabs(end_y-min.y))){
      min = p[i];
      minIndex = i;
    }
  }
  p.erase(p.begin() + minIndex);
  return min;
  

}
double dist(state a){
  double distx, disty;
  distx = pow((end_x - a.x),2.0);
  disty = pow((end_y - a.y),2.0);
  return sqrt(distx + disty);
 
}
void runAstar(){
  vector<state> prioQ;
  state start;
  start.x = (robot[0].x + robot[1].x)/2;
  start.y = (robot[0].y + robot[1].y)/2;
  start.index = 0;
  start.succIndex = -1;
  start.cost = 0;
  start.theta = 0;
  scoreGrid[start.x][start.y] = 0;
  //push start into the priority queue
  prioQ.push_back(start);
  // std::make_heap(prioQ.begin(), prioQ.end());
  bool reached = false;
  state curr;
  vector<state> succ;
  state left;
  state right;
  state up;
  state down;
  //as along as we have not reached the final destination
  while (reached == false){
    //std::pop_heap(prioQ.begin(), prioQ.end(), cmp);
    curr = popMin(prioQ);    
    //set the index of current state
    curr.index = path.size();
    path.push_back(curr);
    //glutPostRedisplay();
    //for all the successors
    succ.clear();
    //set successors succ index to curr index
    left.x = curr.x -1; left.y = curr.y;left.succIndex = curr.index;left.cost = curr.cost + 1;
    right.x = curr.x + 1;right.y = curr.y;right.succIndex = curr.index;right.cost =curr.cost + 1;
    up.x = curr.x; up.y = curr.y + 1;up.succIndex = curr.index;up.cost =curr.cost + 1;
    down.x = curr.x; down.y = curr.y -1;down.succIndex = curr.index;down.cost =curr.cost + 1;

    succ.push_back(left);
    succ.push_back(right);
    succ.push_back(up);
    succ.push_back(down);
    
    for(int i = 0; i < 4; i++){
      //if out of bounds, continue
      if(succ[i].x < 0 || succ[i].x > WINDOWSIZE || succ[i].y < 0 || succ[i].y > WINDOWSIZE){
	continue;
      }
      //if found the destination we are done
      if (succ[i].x == end_x && succ[i].y == end_y){
	path.push_back(succ[i]);
	reached = true;
	return;
      }

      //implement h() = 0;
      //always add edges
      //check that state is free
      if((occupancyGrid[succ[i].x][succ[i].y] != -1)){
	
	int currCost = scoreGrid[curr.x][curr.y];
	int nextCost = scoreGrid[succ[i].x][succ[i].y];
	if((succ[i].index != curr.succIndex) && (currCost + 1 < nextCost)){// && scoreGrid[curr.x][curr.y] + 1 < scoreGrid[succ[i].x][succ[i].y]){
	  //insert to priority queue
	  //cout<<succ[i].x<<","<<succ[i].y<< ","<<succ[i].cost<<endl;
	  curr.theta = occupancyGrid[succ[i].x][succ[i].y];
	  prioQ.push_back(succ[i]); //std::push_heap(prioQ.begin(), prioQ.end(),cmp);
	  scoreGrid[succ[i].x][succ[i].y] = currCost + 1; 
	}
      }
    }
    if(prioQ.size() == 0){cout << "NO PATH FOUND!!! \n"; return;}
  }
  
  
  
  

}

void print_polygon(vector<point2D> poly) {

  for (unsigned int i=0; i<poly.size()-1; i++) {
    printf("(%f,%f), (%f,%f)]\n",
          poly[i].x, poly[i].y, poly[i+1].x, poly[i+1].y);
  }
  //print last edge from last point to first point
  int last = poly.size()-1;
    printf("[(%f,%f), (%f,%f)]\n",
           poly[last].x, poly[last].y, poly[0].x, poly[0].y);

}

void drawVG(){
  if(vg.size() == 0){
    return;
  }
  glColor3fv(red);
  point2D a;
  point2D b;
  
  for(int i = 0; i< (int)vg.size();i++){
    a = vg[i].curr;
    for(int j = 0; j < (int)vg[i].neibors.size(); j++){
      b = vg[i].neibors[j];
      glBegin(GL_LINES);
      glVertex2f(a.x, a.y);
      glVertex2f(b.x, b.y);
      glEnd();
    }
  }
}

void drawSP(){
  if((int)shortestPath.size() == 0){
    return;
  }
  glColor3fv(cyan);
  point2D a;
  point2D b;
  for(int i = 0; i < (int)shortestPath.size()-1;i++){
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
  //draw_points();
  draw_polygon(poly);
  draw_robot();
  draw_shortest();
  for(int i=0; i< (int)polys.size(); i++){
    draw_polygon(polys[i]);
  }
  draw_polygon(animatedBot);
  //draw a circle where the mouse was last clicked. Note that this
  //point is stored as a global variable and is modified by the mouse handler function 
  draw_circle(mouse_x, mouse_y,0,5);  
  draw_circle(end_x, end_y,3,5); 
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
  case 's': 
    //start re-initializeing polygon 
    poly.clear();
    mouse_x = mouse_y=50;
    robot_init_mode = 0;
    poly_init_mode = 1;    
    glutPostRedisplay();
    break;
  case 'r':
    robot.clear();
    robot_init_mode = 1;
    poly_init_mode = 0;
    poly.clear();
    glutPostRedisplay();
    break;
  case 'f':
    final_init_mode = 1;
    poly_init_mode=0;
    robot_init_mode = 0;
    poly.clear();
    glutPostRedisplay(); 
    break;
  case 'c':
    polys.clear();
    vg.clear();
    poly.clear();
    shortestPath.clear();
    initializeGrid();
    path.clear();
    animatedBot.clear();
    glutPostRedisplay();
    break;
  case 'm':
    movement++;
    if(movement == 2){movement = 0;}
    glutPostRedisplay();
    break;
  case 'n':
    timer = 0;
    movement = 0;
    break;
  case 'z':
    shortestPath.clear();
    path.clear();
    start = clock();
    computeGrid();
    std::reverse(shortestPath.begin(),shortestPath.end());
    timer = 0;
    glutPostRedisplay();
    break;
  case 'e': 
    simp = isSimple();
    if(simp == 0){
      poly.clear();
    }
    poly_init_mode=0;
    polys.push_back(poly);
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
void animateBot(int x, int y){
  int theta = occupancyGrid[x][y];
  vector<point2D> robotSquare;
  point2D temp;
  robotSquare.push_back(robot[0]);
  temp.x = robot[1].x;
  temp.y = robot[0].y;
  robotSquare.push_back(temp);
  robotSquare.push_back(robot[1]);
  temp.x = robot[0].x;
  temp.y = robot[1].y;
  robotSquare.push_back(temp);
  translateBot(robotSquare, x,y);
  rotateBot(robotSquare, theta, x, y);
  animatedBot = robotSquare;
}

void timerfunc(){
  if(movement == 0){return;}
  if(shortestPath.size() != 0){
    timer+= 4;
    if(timer < (int)shortestPath.size()){
      int x,y;
      x = shortestPath[timer].x;
      y = shortestPath[timer].y;
      animateBot(x,y);
    }
  }
  glutPostRedisplay(); 

}

double getDistance(point2D a, point2D b){
  double distx, disty;
  distx = pow((b.x - a.x),2.0);
  disty = pow((b.y - a.y),2.0);
  return sqrt(distx + disty);
}

double weight(lineSegment2D line){
  return getDistance(line.p1,line.p2);
}
