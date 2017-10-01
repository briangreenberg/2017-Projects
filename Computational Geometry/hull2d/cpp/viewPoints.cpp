/* view1.c 

Laura Toma

What it does:  

Draws a set of points in the default 2D projection.  

Includes a tentative function for printing and drawing a list of-
points (assumed to be a convex hull). These functions were not 
debugged so use them at your own risk.

This code is written in C.  You will need to update this code to work
with your own (presumably C++) data structures. Feel free to change
this code as much as you need to.

*/

#include "geom.h"
#include "rtimer.h"

#include <unistd.h>
#include <time.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#include <vector>

using namespace std; 


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

int testCase = 0;


/* forward declarations of functions */
void display(void);
void keypress(unsigned char key, int x, int y);
void main_menu(int value);




/* global variables */
const int WINDOWSIZE = 500; 

//the array of n points; note: this variable needs to be global
//because it needs to be rendered
vector<point2D>  points;

int n;  //desired number of points 


//the convex hull, stored as a list. note: this variable needs to be
//global because it needs to be rendered
vector<point2D>  hull; 



void initialize_points_circle() {

  //clear the vector just to be safe 
  points.clear(); 

  double  step = 2* M_PI/n; 
  int rad = 100; 

  int i; 
  point2D p; 
  for (i=0; i<n; i++) {
    p.x = WINDOWSIZE/2+ rad*cos(i*step); 
    p.y = WINDOWSIZE/2+ rad*sin(i*step); 
    points.push_back(p); 
  }
}

void initialize_points_horizontal_line() {

  //clear the vector just to be safe 
  points.clear(); 
  
  int i; 
  point2D p;
  
  for (i=0; i<n; i++) {
    p.x = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE)); 
    p.y =  WINDOWSIZE/2;
 
    points.push_back(p); 
  }
  /*
  for (int j=0; j<10; j++){
    p.x = j*10;
    p.y = WINDOWSIZE/2;
    points.push_back(p);
  }
  */
}




/* ****************************** */
/* initialize  the array of points stored in global variable points[] with random points */
void initialize_points_random() {
  
  //clear the vector just to be safe 
  points.clear(); 

  int i; 
  point2D p; 
  for (i=0; i<n; i++) {
    p.x = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE)); 
    p.y =  (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
    points.push_back(p); 
  }
}


/* ****************************** */
/* initialize the array of points stored in global variable points[]
   with random points shaped like a star */
void initialize_points_star() {
  
  //clear the vector just to be safe 
  points.clear(); 
  
  int i; 
  point2D p; 
  for (i=0; i<n; i++) {
    if (i%2 == 0) {
      
      p.x = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE)); 
      p.y =  random() % ((int)(.7*WINDOWSIZE))  / 5;
      p.y += (int)((1-.7/5)*WINDOWSIZE/2);
    };
    if (i%2 == 1)  {
      
      p.x = random() % ((int)(.7*WINDOWSIZE)) / 5; 
      p.x +=  (int)((1-.7/5)*WINDOWSIZE/2);
      p.y =  (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
    }
   
    points.push_back(p); 
    
  }//for i

}

void initialize_points_down_sloping_diagonal_line() {
  // assert(points);
  
	int i;
	for (i=0; i<n; i++) {
		points[i].x = i * (.7*WINDOWSIZE/n) + 20;
		points[i].y = -points[i].x + WINDOWSIZE;
	}
}

void initialize_points_semicircle()
{
  points.clear();
  double  step = 2* M_PI/n;
  int rad = 100;

  point2D p;
  int i;
  for(i = 0; i < n/2; i++)
  {
    p.x = WINDOWSIZE/2+ rad*cos(i*step); 
    p.y = WINDOWSIZE/2+ rad*sin(i*step); 
    points.push_back(p); 
  }
  
  for (i=0; i<n; i++) 
  {
    p.x = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE)); 
    p.y =  WINDOWSIZE/2;
    points.push_back(p); 
  }
}

void initialize_points_outlier() {
 
  //clear the vector just to be safe                                                                                                     \
                                                                                                                                          
  points.clear();
 
 
  //creates random points                                                                                                                 
  int i;
  point2D p;
  for (i=0; i<n; i++) {
    p.x = (int)(.7*WINDOWSIZE)/2 + random() % ((int)(.3*WINDOWSIZE));
    p.y =  (int)(.7*WINDOWSIZE)/2 + random() % ((int)(.3*WINDOWSIZE));
    points.push_back(p);
  }
 
 
  //randomly chooses a side 1 to 4 to put the outlier near                                                                                
  //and then randomly generates the appropriate x or y value                                                                              
  //near that side                                                                                                                        
  int side = random() % 4 + 1;
 
  if (side == 1){
    p.x = (int)(.9*WINDOWSIZE);
    p.y = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
    points.push_back(p);
  }
 
  if (side == 2){
    p.x= (int)(.1*WINDOWSIZE);
    p.y= (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
    points.push_back(p);
  }
 
 
  if (side == 3){
    p.y= (int)(.9*WINDOWSIZE);
    p.x= (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
    points.push_back(p);
  }
 
  if (side == 4){
    p.y= (int)(.1*WINDOWSIZE);
    p.x= (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
    points.push_back(p);
  }
 
}

void initialize_points_person () {
    
    points.clear();
    point2D p;
    
    srand(time(NULL));
    
    int theRand = random() % 7;
    int randomHeight = random() % 250;
   
    if (randomHeight < 20){
        randomHeight + 80;
    }  
 
 
    for (int i = 0; i < n; i++){
        //p.x = WINDOWSIZE/5 + (int)(rand() % (int)(WINDOWSIZE * (.6)));
        double xdub = rand() % WINDOWSIZE;
        p.x = (int)xdub;
        p.y = (WINDOWSIZE/2) + randomHeight*sin((theRand)*xdub*0.0174533);
        points.push_back(p);   
    }
}


void initialize_illuminati() 
{
//clearing the vector 
points.clear();
//creating the larger circle: 
double step = 2 * M_PI/((n-3)/2);
int rad = 80; 

int i; 
point2D p; 
for (i = 0; i < (n-3)/2; i++) {
p.x = WINDOWSIZE/2 + rad*cos(i*step);
p.y = WINDOWSIZE/2 + rad*sin(i*step); 
points.push_back(p);
}
//creating the smaller circle: 
rad = 50; 
for (i = 0; i < (n-3)/2; i++) {
p.x = WINDOWSIZE/2 + rad*cos(i*step);
p.y = WINDOWSIZE/2 + rad*sin(i*step); 
points.push_back(p);
}
//pushing back the three points of the triangle ILLUMINATI! 
p.x = WINDOWSIZE/8; 
p.y = WINDOWSIZE/8;
points.push_back(p);
p.x = WINDOWSIZE * 7 / 8;
p.y = WINDOWSIZE/8;
points.push_back(p);

p.x = WINDOWSIZE/2;
p.y = WINDOWSIZE *7 / 8;
points.push_back(p);
}

void initialize_points_stairs() {
  // assert(points);
  points.clear();
    int i;
    for (i=0; i<n; i++) {
        points[i].x = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
        points[i].y = WINDOWSIZE - points[i].x + random() % ((int)(.05*WINDOWSIZE));
    }
 
}

void initialize_points_circle_over_square() {

    //clear the vector just to be safe
    points.clear();

    int i;
    point2D p;

    int squareN = n / 2;
    int circleN = n - squareN;

    // initialize the square first
    for (i=0; i<squareN/4 - 1; i++) {
        p.x = (int) random() % (int)(WINDOWSIZE * 0.4) + (int) (WINDOWSIZE * 0.3);
        p.y =  (int)(WINDOWSIZE * .7);
        points.push_back(p);
    }

    for (i=0; i<squareN/4 - 1; i++) {
        p.x = (int) random() % (int)(WINDOWSIZE * 0.4) + (int) (WINDOWSIZE * 0.3);
        p.y =  (int)(WINDOWSIZE * .3);
        points.push_back(p);
    }

    for (i=0; i<squareN/4 - 1; i++) {
        p.y = (int) random() % (int)(WINDOWSIZE * 0.4) + (int) (WINDOWSIZE * 0.3);
        p.x =  (int)(WINDOWSIZE * .7);
        points.push_back(p);
    }

    for (i=0; i<squareN/4 - 1; i++) {
        p.y = (int) random() % (int)(WINDOWSIZE * 0.4) + (int) (WINDOWSIZE * 0.3);
        p.x =  (int)(WINDOWSIZE * .3);
        points.push_back(p);
    }

    p.x =  (int)(WINDOWSIZE * .3);
    p.y =  (int)(WINDOWSIZE * .3);
    points.push_back(p);

    p.x =  (int)(WINDOWSIZE * .3);
    p.y =  (int)(WINDOWSIZE * .7);
    points.push_back(p);

    p.x =  (int)(WINDOWSIZE * .7);
    p.y =  (int)(WINDOWSIZE * .3);
    points.push_back(p);

    p.x =  (int)(WINDOWSIZE * .7);
    p.y =  (int)(WINDOWSIZE * .7);
    points.push_back(p);

    // initialize the circle
    double  step = 2* M_PI/(circleN); 
    int rad = 125; 

    for (i=0; i<circleN; i++) {
        p.x = WINDOWSIZE/2 + rad*cos(i*step); 
        p.y = WINDOWSIZE/2 + rad*sin(i*step); 
        points.push_back(p); 
    }
}

void initialize_points_pokeball() {

    //clear the vector just to be safe
    points.clear();

    int i;
    point2D p;

    int outerCircleN = n/2;
    int mediumCircleN = outerCircleN/2;
    int innerCircleN = mediumCircleN/2;
    int lineN = n - outerCircleN - mediumCircleN - innerCircleN;
    int leftLineN = lineN/2;
    int rightLineN = lineN - leftLineN;
    int bigRad = 160;
    int mediumRad = 80;
    int smallRad = 40;

    // initialize the line
    for (i = 0; i < leftLineN; i++) {
        p.x = (WINDOWSIZE/2 - bigRad) + random() % (bigRad - mediumRad); 
        p.y =  WINDOWSIZE/2; 
        points.push_back(p); 
    }

    for (i = 0; i < rightLineN; i++) {
        p.x = (WINDOWSIZE/2 + mediumRad) + random() % (bigRad - mediumRad);
        p.y = WINDOWSIZE/2;
        points.push_back(p);
    }

    // initialize the circle
    double bigStep = 2 * M_PI/(outerCircleN);  

    for (i=0; i<outerCircleN; i++) {
        p.x = WINDOWSIZE/2 + bigRad*cos(i*bigStep); 
        p.y = WINDOWSIZE/2 + bigRad*sin(i*bigStep); 
        points.push_back(p); 
    }

    double mediumStep = 2 * M_PI/(mediumCircleN);

    for (i=0; i < mediumCircleN; i++) {
        p.x = WINDOWSIZE/2 + mediumRad*cos(i*mediumStep); 
        p.y = WINDOWSIZE/2 + mediumRad*sin(i*mediumStep); 
        points.push_back(p); 
    }

    double smallStep = 2 * M_PI/(innerCircleN);

    for (i=0; i < innerCircleN; i++) {
        p.x = WINDOWSIZE/2 + smallRad*cos(i*smallStep); 
        p.y = WINDOWSIZE/2 + smallRad*sin(i*smallStep); 
        points.push_back(p); 
    }
}

/* four point edges that create a triangle */
void initialize_points_triangle() {
   // clear the vector to be safe
   points.clear();
   point2D p;   
   int i = 0;

   // edge boundary
   int height = (int)(.1*WINDOWSIZE)/2 + random() % ((int)(.5*WINDOWSIZE));
   int width = (int)(.1*WINDOWSIZE)/2 + random() % ((int)(.5*WINDOWSIZE));

   for (i=0; i<n; i++) {
   	p.x = WINDOWSIZE/2 + height/2;
   	p.y = WINDOWSIZE/2 - width/4;
   	points.push_back(p);
   	p.x = WINDOWSIZE/2 + height/2;
   	p.y = WINDOWSIZE/2 + (3*width)/4;
   	points.push_back(p);
   	p.x = WINDOWSIZE/2 - height/2;
   	p.y = WINDOWSIZE/2 - (3*width)/4;
   	points.push_back(p);
   }
}



/*Alligns all points in a vertical line*/
void initialize_points_vertical_line() {
  points.clear();
 // assert(points);
 
  int i;
  for (i=0; i<n; i++) {
    points[i].x =  WINDOWSIZE/2;
    points[i].y = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
  }
}

void initialize_points_rectangle() {

  //clear the vector just to be safe
  points.clear();

  int i;
  point2D p;

  for (i=0; i<n/4 - 1; i++) {
    p.x = (int) random() % (int)(WINDOWSIZE * 0.4) + (int) (WINDOWSIZE * 0.3);
    p.y =  (int)(WINDOWSIZE * .7);
    points.push_back(p);
  }

  for (i=0; i<n/4 - 1; i++) {
    p.x = (int) random() % (int)(WINDOWSIZE * 0.4) + (int) (WINDOWSIZE * 0.3);
    p.y =  (int)(WINDOWSIZE * .3);
    points.push_back(p);
  }

  for (i=0; i<n/4 - 1; i++) {
    p.y = (int) random() % (int)(WINDOWSIZE * 0.4) + (int) (WINDOWSIZE * 0.3);
    p.x =  (int)(WINDOWSIZE * .7);
    points.push_back(p);
  }

  for (i=0; i<n/4 - 1; i++) {
    p.y = (int) random() % (int)(WINDOWSIZE * 0.4) + (int) (WINDOWSIZE * 0.3);
    p.x =  (int)(WINDOWSIZE * .3);
    points.push_back(p);
  }

  p.x =  (int)(WINDOWSIZE * .3);
  p.y =  (int)(WINDOWSIZE * .3);
  points.push_back(p);

  p.x =  (int)(WINDOWSIZE * .3);
  p.y =  (int)(WINDOWSIZE * .7);
  points.push_back(p);

  p.x =  (int)(WINDOWSIZE * .7);
  p.y =  (int)(WINDOWSIZE * .3);
  points.push_back(p);

  p.x =  (int)(WINDOWSIZE * .7);
  p.y =  (int)(WINDOWSIZE * .7);
  points.push_back(p);
}

void run_test_case(int caseNum)
{
  points.clear();

  switch(caseNum){
  
  case 1:
    initialize_points_person();
    break;
  case 2:
    //Our test case!
    initialize_points_semicircle();
    break;
  case 3:
    initialize_illuminati();
    break;
  case 4:
    //initialize_points_stairs(); This one gave us a segfault
    initialize_points_star();
    break;
  case 5:
    initialize_points_circle_over_square();
    break;
  case 6:
    initialize_points_pokeball();
    break;
  case 7:
    initialize_points_triangle();
    break;
  case 8:
    initialize_points_circle_over_square();
    break;
  case 9:
    initialize_points_rectangle();
    break;
  }

  hull = graham_scan(points);
}


/* ****************************** */
/* print the array of points stored in global variable points[]*/
void print_points() {
  
  int i; 
  printf("points: ");
  for (i=0; i< points.size(); i++) {
    printf("[%3d,%3d] ", points[i].x, points[i].y);
  }
  printf("\n");
  fflush(stdout);  //flush stdout, weird sync happens when using gl thread
}

/* ****************************** */
//print the list of points in global variable  hull; 
void print_hull () {
  
  printf("convex hull: ");
  //...
 }



/* ****************************** */
int main(int argc, char** argv) {

  //read number of points from user
  if (argc!=2) {
    printf("usage: viewPoints <nbPoints>\n");
    exit(1); 
  }
  n = atoi(argv[1]); 
  printf("you entered n=%d\n", n);
  assert(n >0); 

 
  //initialize_points_random();
  initialize_points_star();

  print_points();

  Rtimer rt1; 
  rt_start(rt1); 
  hull = graham_scan(points); 
  rt_stop(rt1); 

  //print the hull 
  print_hull(); 
  //print the timing 
  char buf [1024]; 
  rt_sprint(buf,rt1);
  printf("finding convex hull with graham scan:  %s\n\n", buf);
  fflush(stdout); 

 
  

  /* initialize GLUT  */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(WINDOWSIZE, WINDOWSIZE);
  glutInitWindowPosition(100,100);
  glutCreateWindow(argv[0]);

  /* register callback functions */
  glutDisplayFunc(display); 
  glutKeyboardFunc(keypress);

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
/* draw the array of points stored in global variable points[] 
   each point is drawn as a small square 
  
*/
void draw_points(){

  const int R= 1;
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  //set color 
  glColor3fv(yellow);   
  
  int i;
  for (i=0; i<n; i++) {
    //draw a small square centered at (points[i].x, points[i].y)
    glBegin(GL_POLYGON);
    glVertex2f(points[i].x -R,points[i].y-R);
    glVertex2f(points[i].x +R,points[i].y-R);
    glVertex2f(points[i].x +R,points[i].y+R);
    glVertex2f(points[i].x -R,points[i].y+R);
    glEnd();
  }

}





/* ****************************** */
/* draw the list of points stored in global variable hull; the points
   are expected to be in order (ccw or cw) and consecutive points are
   connected by a line
*/
void draw_hull(){

  //set color 
  glColor3fv(red);   
  
  if (hull.size() >0) {
    int i; 
    for (i=0; i< hull.size()-1; i++) {
      
      //draw a line fromcrt to next 
      glBegin(GL_LINES);
      glVertex2f(hull[i].x, hull[i].y); 
      glVertex2f(hull[i+1].x, hull[i+1].y); 
      glEnd();
    }
    
    //draw a line from the last point to the first point 
    //fill in code here 

    
  }//if (hull not empty)
}


/* ****************************** */
void display(void) {

  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity(); //clear the matrix


  /* the default GL window is [-1,1]x[-1,1] with the origin in the
     center the points are in the range (0,0) to (WINSIZE,WINSIZE), so
     they need to be mapped to [-1,1]x [-1,1] */
  glScalef(2.0/WINDOWSIZE, 2.0/WINDOWSIZE, 1.0);  
  glTranslatef(-WINDOWSIZE/2, -WINDOWSIZE/2, 0); 
   draw_points();
  draw_hull(); 

  /* execute the drawing commands */
  glFlush();
}









/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
     
   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);
 
   glMatrixMode(GL_PROJECTION);  // To operpeate on the Projection matrix
   glLoadIdentity();             // Reset
   gluOrtho2D(0.0, (GLdouble) width, 0.0, (GLdouble) height); 
}

void initialize_points_criss_cross()
{
//clear the vector just to be safe
  points.clear();
  int i;
  point2D p;
  for (i=0; i<n/2; i++) {
    p.x = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
    p.y = p.x;
    points.push_back(p);
    int xCoord = p.x;
    p.x = WINDOWSIZE - 1 - (xCoord);
    points.push_back(p);
  }
}




/* ****************************** */
void keypress(unsigned char key, int x, int y) {
  switch(key) {
  case 'q':
    exit(0);
    break;

  case '1':
    initialize_points_random();
    hull = graham_scan(points);
    glutPostRedisplay();
   
    break; 

 case '2': 
   initialize_points_star();
   hull = graham_scan(points);
    glutPostRedisplay();
    break; 

 case '3': 
    initialize_points_circle();
    hull = graham_scan(points);
    glutPostRedisplay();
    break; 

 case '4': 
    initialize_points_horizontal_line();
    hull = graham_scan(points);
    glutPostRedisplay();
    break; 

  case '5':
    testCase++;
    if (testCase > 9 ){testCase = 1;}
    run_test_case(testCase);
    hull = graham_scan(points);
    glutPostRedisplay();
  } 
}
