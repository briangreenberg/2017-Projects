/* kdtree.cpp

Brian Greenberg and Osborn Ongimor


Draws a mondrian-style rendering of a kdtree given a set of points

*/

#include "geom.h"
#include <unistd.h>
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

vector<segment2D> emptyRec;

/* forward declarations of functions */
void display(void);
void keypress(unsigned char key, int x, int y);
void main_menu(int value);
void initializePointsX(int n);
void initiallize_points_circle();
void initialize_points_diag();
void initialize_points_grid();
void initialize_points_cross();

/* global variables */
const int WINDOWSIZE = 500; 

//the array of n points; note: this variable needs to be global
//because it needs to be rendered
vector<point2D>  points;

int n;  //desired number of points 


//the convex hull, stored as a list. note: this variable needs to be
//global because it needs to be rendered

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
/* print the array of points stored in global variable points[]*/
void print_points() {
  
  int i; 
  printf("points: ");
  for (i=0; i< int(points.size()); i++) {
    printf("[%f , %f] ", points[i].x, points[i].y);
  }
  fflush(stdout);  //flush stdout, weird sync happens when using gl thread
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

 
  initialize_points_random();
  //print the timing
  //prepare arguments
  point2D upper;
  upper.x = 0;
  upper.y = 0;
  point2D lower;
  lower.x = WINDOWSIZE;
  lower.y = WINDOWSIZE;

  kdtree t = build(points, lower, upper, emptyRec);
  printTree(t);

  
   
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

  const int R= 5;
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

void drawRec(segment2D rec){
  point2D a1 = rec.lowLeft;
  point2D b2 = rec.upRight;
  point2D a2;
  a2.x = b2.x;
  a2.y = a1.y;

  point2D b1;
  b1.x = a1.x;
  b1.y = b2.y;
  GLfloat a1x = a1.x;
  GLfloat a1y = a1.y;
  GLfloat a2x = a2.x;
  GLfloat a2y = a2.y;
  GLfloat b1x = b1.x;
  GLfloat b1y = b1.y;
  GLfloat b2x = b2.x;
  GLfloat b2y = b2.y;
  
  
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  
  int rando = rand()%6 + 1;
  switch(rando) {
  case 1:
    glColor3fv(blue);
    break;
  case 2:
    glColor3fv(red);
    break; 
  case 3:
    glColor3fv(yellow);
    break;
  default:
    glColor3fv(white);
    break; 
  }
  
  //draw the rectangle
  
  glBegin(GL_POLYGON);
  glVertex2f(a1x, a1y);
  glVertex2f(a2x, a2y);
  glVertex2f(b2x, b2y);
  glVertex2f(b1x, b1y);
  glEnd();
  
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  //draw the rectangle
  glColor3fv(blue);
  glBegin(GL_POLYGON);
  glVertex2f(a1x, a1y);
  glVertex2f(a2x, a2y);
  glVertex2f(b2x, b2y);
  glVertex2f(b1x, b1y);
  
  glEnd();
                                                                                                       


}

void draw_recs(){
  glColor3fv(yellow);   
  int i;
  for (i=0; i<emptyRec.size(); i++) {
    drawRec(emptyRec[i]);
  }
  /*
  segment2D temp;
  point2D a;
  a.x = 0;
  a.y = 0;
  point2D b;
  b.x = WINDOWSIZE/2;
  b.y = WINDOWSIZE/2;
  temp.upperLeft = b;
  temp.lowerRight = a;
  drawRec(temp);
  */

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
  draw_recs();
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

/* ****************************** */
void keypress(unsigned char key, int x, int y) {
  point2D upper;
  upper.x = 0;
  upper.y = 0;
  point2D lower;
  lower.x = WINDOWSIZE;
  lower.y = WINDOWSIZE;
  kdtree t;

  switch(key) {
  case 'q':
    exit(0);
    break;

  case 'r':
    initialize_points_random();
    t.root = NULL;
    emptyRec.clear();
    t = build(points, lower, upper, emptyRec);
    glutPostRedisplay();
    break;
  case '1':
    initializePointsX(n);
    t = build(points, lower, upper, emptyRec);
    glutPostRedisplay();
    break; 
  case '2':
    initiallize_points_circle();
    t = build(points, lower, upper, emptyRec);
    glutPostRedisplay();
    break;
  case '3':
    initialize_points_random();
    t = build(points, lower, upper, emptyRec);
    glutPostRedisplay();
    break;
  case '4':
    initialize_points_diag();
    t = build(points, lower, upper, emptyRec);
    glutPostRedisplay();
    break;
  case '5':
    // initialize_points_grid();
    // t = build(points, lower, upper, emptyRec);
    // glutPostRedisplay();
    break;
  case '6':
    initialize_points_cross();
    t = build(points, lower, upper, emptyRec);
    glutPostRedisplay();
    break;

  default:
    break;
  } 
}






















////////////////////////// TEST CASES!


void initializePointsX(int n) {
  for(int i = 0; i < n; ++i)
    {
      point2D newPoint;
      newPoint.x = -1 + (2 * (double)rand() / (double)RAND_MAX);
      newPoint.y = newPoint.x;
      points.push_back(newPoint);
    }
  for(int i = 0; i < n; ++i)
    {
      point2D newPoint;
      newPoint.x = -1 + (2 * (double)rand() / (double)RAND_MAX);
      newPoint.y = -1 * newPoint.x;
      points.push_back(newPoint);
    }
  return;
}

//Erik and Grace code
void initiallize_points_circle(){
  points.clear();
  double radius = WINDOWSIZE / 5;
  point2D p;
  for (int i = 0; i < n; i++){
    p.x = radius * cos (2*M_PI / n * i) + WINDOWSIZE / 2;
    p.y = radius * sin (2*M_PI / n * i) + WINDOWSIZE / 2;
    // points[i] = p;
    points.push_back(p);
  }
}

// Bo Bleckel & Henry Daniels Koch
// Initialize the points in a downward sloping diagonal
// Works best with n <= 100
void initialize_points_diag() {
  points.clear();
  
  point2D point;
  for (int i = 0; i < n; i++) {
    double x = i * (WINDOWSIZE/n);
    double y = (n-i) * (WINDOWSIZE/n);
    point.x = x;
    point.y = y;
    
        points.push_back(point);
  }
}

// Jack Ward
// Initialize the `points` vector to a grid.
/*
void initialize_points_grid() {
  // clear the vector
  points.clear();
  double window  = (double) WINDOWSIZE;
  double padding = window / 8;
  double width   = window - 2 * padding;
  int    side    = sqrt(n);
  double spacing = width / (double) side;
  
  for (int row = 0; row < side; row++) {
    for (int col = 0; col < side; col++) {
      // col * spacing + padding
      double x = fma(col, spacing, padding);
      // row * spacing + padding
      double y = fma(row, spacing, padding);
      points.push_back({x, y});
    }
  }
}
*/
void initialize_points_cross(){
  points.clear();
  
  point2D p;
  for(int i =0; i < n; i++){
    p.x = WINDOWSIZE/n*i;
    p.y = WINDOWSIZE/2;
    points.push_back(p);
  }
  
  point2D b;
  for (int j = 0; j < n; j++){
    b.x = WINDOWSIZE/2;
    b.y = WINDOWSIZE/n *j;
    points.push_back(b);
  }
  
}
