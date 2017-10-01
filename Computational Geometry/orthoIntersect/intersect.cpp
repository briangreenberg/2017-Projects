/* 

intersect.cpp

Laura Toma

What it does:  

Draws a set of horizontal and vertical line segments in the default 2D
projection. Then it pretends to compute their intersections using the
line sweep algorithm, and simulates the sweep line moving from left to
right.

*/

#include "geom.h"
#include "rtimer.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <algorithm>
#include<set>

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


/* forward declarations of functions */
void display(void);
void keypress(unsigned char key, int x, int y);
void timerfunc(); 
void initialize_segments_random(); 
void initialize_segments_horizontal();
void initialize_segments_helix(); 
void initialize_segments_zero();
void initialize_segments_worst_case();
void init_segs_hash();
void initialize_segments_perpendicular_lines();

void print_segments(); 
//renders the sweep line 
void draw_sweep_line();
//renders the active structure
void draw_active_structure();
//renders the intersection points 
void draw_intersection_points();





/* global variables */
const int WINDOWSIZE = 500; 

//we got two test cases so far: random and horizontal; add more!
int init_case = 0; 
const int NB_TEST_CASES = 2; 

//NOTE: all the structures below need to be global so that they can be rendered

//number of segments requested by user 
int n; 
int valX;
int valY;
int val2X;
int val2Y;
//the array of  segments
vector<segment2D>  segments;

//the active structure that stores the segments intersecting the sweep line 
//vector<segment2D> as;
vector<segment2D> as;

//the intersections points of the segments 
vector<point2D> intpoints; 


//the events. do we need this??
vector<sweepEvent> events; 

//std::set<segment2D> activeStructure;


//current position of sweep line; this is used to animate the sweep line moving 
int sweep_line_x = 0; 

/* ************************************************** */
//fills global variable "segments" with n segments 
void initialize_segments() {

  switch (init_case)  {
      
    case 0: 
      initialize_segments_random(); 
      break;
      
    case 1: 
      initialize_segments_horizontal(); 
      break; 
      
    default: 
      initialize_segments_random(); 
    }

  init_case = (init_case+1) % NB_TEST_CASES;
  return; 
}

/* ************************************************** */
//fills global variable "segments" with n horizontal segments 
void initialize_segments_horizontal() {

  int i; 
  point2D a,b;
  segment2D s; 

  //clear the vector of segments 
  segments.clear(); 

  //a long horizontal segment 
  a.x = 1; 
  a.y = WINDOWSIZE/2; 
  b.x = WINDOWSIZE - 10; 
  b.y = a.y; 

  s.start = a; s.end = b; 
  segments.push_back(s);  

  //n-1 vertical segments 
  for (i=0; i<n-1; i++) {
    
    a.x = i*WINDOWSIZE/n; 
    a.y = WINDOWSIZE/2 - random() % ((int)(.4*WINDOWSIZE)); 
    b.x = a.x; 
    b.y = WINDOWSIZE/2 + random() % ((int)(.4*WINDOWSIZE)); 
    s.start = a; s.end = b; 
    segments.push_back(s); 
  }

}

/* ****************************** */
//fills global variable "segments" with n random segments, half horizontal and half vertical 
void initialize_segments_random() {
  
  //clear the vector 
  segments.clear(); 

  int i; 
  point2D a, b; 
  segment2D s; 
  for (i=0; i<n; i++) {
    if (random()%2 == 0) {
      //horizontal segment
      a.x = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE)); 
      a.y =  (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
      b.y = a.y; 
      b.x = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE)); 

      if (a.x < b.x) {
	s.start = a; s.end = b; 
      } else {
	s.start = b; s.end = a; 
      } 
 
    } else {
      //vertical segment 
      a.x = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE)); 
      b.x = a.x; 
      a.y = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE)); 
      b.y = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE)); 

      if (a.y < b.y) {
	s.start = a; s.end = b; 
      } else {
	s.start = b; s.end = a; 
      }
    }

    //insert the segment in the array of segments 
    segments.push_back (s); 
  } //for i
}




/* ************************************************** */
void print_segments() {

  for (int i=0; i<int(segments.size()); i++) {
    printf("segment %d: [(%d,%d), (%d,%d)]\n",
	   i, segments[i].start.x, segments[i].start.y, segments[i].end.x, segments[i].end.y);

  }
}
int typeEvent(sweepEvent a){
  if(vertical(a)){return 2;}
  if(endOfSegment(a)){return 3;}
  if(startOfSegment(a)){return 1;}
}
bool compareByX(sweepEvent a, sweepEvent b){
  if( a.x != b.x && a.x < b.x){
    return true;
  }
  int typeA = typeEvent(a);
  int typeB = typeEvent(b);
  
  return typeA < typeB;
}

//Initialize the list of events from the segments
void initialize_events()
{
  
  for(int i=0; i< int(segments.size()); i++)
  {
  
    segment2D temp = segments[i];
    
    //If horizontal - add the start point and end of the line to the events list
    if(temp.start.y == temp.end.y)
    {
 
      sweepEvent tempEvent;
      tempEvent.x = temp.start.x;
      tempEvent.segment = temp;
      events.push_back(tempEvent);
      
      tempEvent.x = temp.end.x;
      tempEvent.segment = temp;
      events.push_back(tempEvent);
    } 
    //If vertical - add the y line to the events list
    else
    {
      sweepEvent tempEvent;
      tempEvent.x = temp.start.x;
      tempEvent.segment = temp;
      events.push_back(tempEvent);
    }
  }
  printf("A \n");
  //sort the events by x                                                                   
  std::sort(events.begin(), events.end(), compareByX);
  printf("ufala2");
}

bool startOfSegment(sweepEvent a){
  if (a.x == a.segment.start.x){
    if(a.segment.start.y == a.segment.end.y){
      return true;
    }
  }
  return false;
}

bool endOfSegment(sweepEvent a){
  if(a.x == a.segment.end.x){
    if(a.segment.start.y == a.segment.end.y){
      return true;
    }
  }
  return false;
}

bool vertical(sweepEvent a){
  if(a.segment.start.x == a.segment.end.x){
    return true;
  }
  return false;
}

bool compareSegmentByY(segment2D a, segment2D b){
  return a.start.y < b.start.y;
}

bool equalSeg(const segment2D a){
  if(a.start.x == valX && a.start.y == valY && a.end.x == val2X && a.end.y == val2Y){
    return false;
  }
  return false;
}
void delet(segment2D a){
  segment2D b;

  for(int i = 0; i < as.size(); i++){
    b = as[i];
    if(a.start.x == b.start.x && a.start.y == b.start.y && a.end.x == b.end.x && a.end.y == b.end.y){
      as.erase(as.begin()+i);
    }
  }

}
//Check if there are any intersections to be found at the current x sweepline
void computeIntersections(int currX){
  if (events.size() == 0){return;}
  while(currX == events[0].x){
    printf("slsls\n");
    int i = 0;
    /*
    if(startOfSegment(events[i])){
      as.push_back(events[i].segment);
      std::sort(as.begin(), as.end(), compareSegmentByY);
      glutPostRedisplay();
    }
    if(endOfSegment(events[i])){
      valX = events[i].segment.start.x;
      valY = events[i].segment.start.y;
      val2X = events[i].segment.end.x;;
      val2Y = events[i].segment.end.y;;
      delet(events[i].segment);
      //as.erase(remove_if(as.begin(),as.end(), equalSeg), as.end());
      //as.erase(std::remove(as.begin(), as.end(),events[i].segment), as.end());
      glutPostRedisplay();
      }*/
    if (vertical(events[i])){  
      std::vector<segment2D>::iterator itlow,itup;
      //segment down;
      //down.start.y = events[i].segment -1;
      //down.end.y
      itlow  = std::lower_bound(as.begin(),as.end(), events[i].segment);
      //segment for upper bound
      segment2D up;
      up.start = events[i].segment.end;
      up.end = events[i].segment.end;
      up.start.y++;
      up.end.y++;
      printf("%d\n", as.size());
      itup = std::upper_bound(as.begin(),as.end(),up);
      segment2D t = *itlow;
      t = *itup;
      
      for(std::vector<segment2D>::iterator index = itlow; index != itup; index++){
	segment2D temp = *index;
	//create intersection
	point2D intersection;
	intersection.x = events[i].x;
	intersection.y = temp.start.y;
	
	//add intersection to intPoints.
	intpoints.push_back(intersection);
	
      }
      int min = events[i].segment.start.y;
      int max = events[i].segment.end.y;
      for(int w = 0; w < as.size(); w++){
	int curr = as[w].start.y;
	if(curr >=min && curr <= max){
	  point2D inter;
	  inter.x = events[i].x;
	  inter.y = curr;
	  intpoints.push_back(inter);
	}
      }
    }
    events.erase(events.begin());
    if(events.size()==0){
      return;
    }
   else if(startOfSegment(events[i])){
      as.push_back(events[i].segment);
      segment2D temp;
      temp.start.x = events[i].segment.end.x;
      temp.start.y = events[i].segment.end.y;
      temp.end.x = events[i].segment.end.x;
      temp.end.y = events[i].segment.end.y;
      as.push_back(temp);
      std::sort(as.begin(), as.end(), compareSegmentByY);
      glutPostRedisplay();
    }
   else if(endOfSegment(events[i])){
      valX = events[i].segment.start.x;
      valY = events[i].segment.start.y;
      val2X = events[i].segment.end.x;;
      val2Y = events[i].segment.end.y;;
      delet(events[i].segment);
      //as.erase(remove_if(as.begin(),as.end(), equalSeg), as.end());                                       
      //as.erase(std::remove(as.begin(), as.end(),events[i].segment), as.end());                            
      glutPostRedisplay();
    }

  }
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

  //the default is to initialize the segments randomly 
  //initialize_segments_random();
  initialize_segments_perpendicular_lines();
  print_segments();
  initialize_events();
  
  /* initialize GLUT  */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(WINDOWSIZE, WINDOWSIZE);
  glutInitWindowPosition(100,100);
  glutCreateWindow(argv[0]);

  /* register callback functions */
  glutDisplayFunc(display); 
  glutKeyboardFunc(keypress);
  glutIdleFunc(timerfunc);  //<--------note we got an idle function, we'll use it to animate 

  /* init GL */
  /* set background color black*/
  glClearColor(0, 0, 0, 0);   

  
  /* give control to event handler */
  glutMainLoop();
  return 0;
}




/* draw the segments stored in global variable segments
   NOTE: We draw in the local coordinate system (0,0) to (WINSIZE,WINSIZE)
*/
void draw_segments(){

  //set color 
  glColor3fv(yellow);   
  
  int i;
  for (i=0; i<int(segments.size()); i++) {
    glBegin(GL_LINES);
    glVertex2f(segments[i].start.x, segments[i].start.y); 
    glVertex2f(segments[i].end.x, segments[i].end.y);
    glEnd();
  }
}



/* 
   draw the sweep line
   NOTE: We draw in the local coordinate system (0,0) to (WINSIZE,WINSIZE)
*/
void draw_sweep_line() {
  
  //sweep line color 
  glColor3fv(red); 
  
  //the current position of sweep line is sweep_line_x; assume it's a
  //segment from y=0 to y=windowsize;
  glBegin(GL_LINES); 
  glVertex2f(sweep_line_x, 0); 
  glVertex2f(sweep_line_x, WINDOWSIZE); 
  glEnd();
}


/* draw a segment with current color
   NOTE: We draw in the local coordinate system (0,0) to (WINSIZE,WINSIZE)
*/
void draw_segment(segment2D s) {
  glBegin(GL_LINES);
  glVertex2f(s.start.x, s.start.y); 
  glVertex2f(s.end.x, s.end.y);
  glEnd();
}


/* draw all the elements in the active structure 
   NOTE: we draw in the local coordinate system (0,0) to (WINSIZE,WINSIZE)
*/
void draw_active_structure() {
 

  glColor3fv(red);

  for(std::vector<segment2D>::iterator index = as.begin(); index != as.end(); ++index){
   
    segment2D temp = *index;
    glBegin(GL_LINES);
    glVertex2f(temp.start.x, temp.start.y);
    glVertex2f(temp.end.x, temp.end.y);
    glEnd();

  }
  //printf("Boo!\n");  
}



/* draw all the elements in intpoints 
   NOTE: we draw in the local coordinate system  (0,0) to (WINSIZE,WINSIZE)
*/
void draw_intersection_points() {
  
  for(int i = 0; i < int(intpoints.size()); i++){
    point2D point = intpoints[i];
    glColor3fv(cyan);
    glBegin(GL_LINES);
    glVertex2f(point.x - 5, point.y - 5);
    glVertex2f(point.x + 5, point.y + 5);
    glEnd();
    glColor3fv(cyan);
    glBegin(GL_LINES);
    glVertex2f(point.x - 5, point.y + 5);
    glVertex2f(point.x + 5, point.y - 5);
    glEnd();

  }
}




/* ****************************** */
void display(void) {

  //clear everything, start from scratch 
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity(); 


  /* The default GL window is [-1,1]x[-1,1] with the origin in the center. 
     
     The points are in the range (0,0) to (WINSIZE,WINSIZE). This is our local coordinate system. 

     We first transform our local coordinate system to [-1,1] x [-1,1]
  */
  glScalef(2.0/WINDOWSIZE, 2.0/WINDOWSIZE, 1.0);  
  glTranslatef(-WINDOWSIZE/2, -WINDOWSIZE/2, 0); 


  //draw my scene in the local coordinate system (0,0) to (WINSIZE,WINSIZE)
  draw_segments();
  draw_active_structure(); 
  draw_intersection_points(); 
  draw_sweep_line(); 


  /* execute the drawing commands */
  glFlush();
}



/* ****************************** */
void keypress(unsigned char key, int x, int y) {  
  if (key == 'r'){
    intpoints.clear();
    as.clear();
    events.clear();
    initialize_events();
    sweep_line_x = 0;
    return;
  }

  intpoints.clear();
  segments.clear();
  as.clear();
  events.clear();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  switch(key) {
  case 'q':
    exit(0);
    break;
  case 'i': 
    initialize_segments_random();
    break; 
  case '1':
    initialize_segments_helix();
    break;   
  case '2':
    initialize_segments_zero();
    break;   
  case '3':
    initialize_segments_worst_case();
    break;
  case '4':
    init_segs_hash();
    break;
  case '5':
    initialize_segments_perpendicular_lines();
    break;
  default:
    initialize_segments_random();
  }
  initialize_events();
  sweep_line_x = 0;
}

//Test Case: Grid with many intersections
void initialize_segments_worst_case() {
    //clear the vector
    segments.clear();

    int i;
    int offset = 100;
    int length = 300;
    int pad = length / (n/2);
    point2D a, b;
    segment2D s;

    // printf("pad = %d\n", pad);

    int count1 = 0, count2 = 0;

    for (i = 0; i < n; i++) {

        if (i % 2 == 0) {
            // horizontal segment
            a.x = offset;
            a.y = offset + pad * count1;
            b.x = offset + length;
            b.y = offset + pad * count1;
            s.start = a;
            s.end = b;
            count1++;
        }
        else {
            // vertical segment
            a.x = offset + pad * count2;
            a.y = offset;
            b.x = offset + pad * count2;
            b.y = offset + length;
            s.start = a;
            s.end = b;
            count2++;
        }

        // insert the segment in the array of segments
        segments.push_back(s);
    }
}

//Test Case: Grid with many intersections
void initialize_segments_perpendicular_lines() {
    //clear the vector
    segments.clear();

    int i;
    int offset = 100;
    int length = 300;
    int pad = length / (n/2);
    point2D a, b;
    segment2D s;

    // printf("pad = %d\n", pad);

    int count1 = 0;
    int count2 = 0;

    for (i = 0; i < n; i++) 
    {
        if (i % 2 == 0) {
            // horizontal segment
            a.x = offset;
            a.y = offset + pad * count1;
            b.x = offset + length;
            b.y = offset + pad * count1;
            s.start = a;
            s.end = b;
            count1++;
        }
        else if(i == n-1) {
            // vertical segment
            a.x = offset + pad * count2;
            a.y = offset;
            b.x = offset + pad * count2;
            b.y = offset + length;
            s.start = a;
            s.end = b;
            count2++;
        }
	segments.push_back(s);
    }
}

//Test case: no intersects
void initialize_segments_zero() {
  // Clear the vector
  segments.clear();
  
  int i;
  const int MARGIN = 80;
  const int PADDING = 4;
  point2D a, b;
  segment2D s;

  // Horizontal segments
  for (i = PADDING; i < n/2; i++) {
    a.x = MARGIN;
    a.y = MARGIN + PADDING * i + PADDING;
    b.x = MARGIN + PADDING * i;
    b.y = MARGIN + PADDING * i + PADDING;

    s.start = a;
    s.end = b;

    segments.push_back(s);
  }

  // Vertical segments
  for (i = PADDING; i < n/2; i++) {
    a.x = MARGIN + PADDING * i + PADDING;
    a.y = MARGIN;
    b.x = MARGIN + PADDING * i + PADDING;
    b.y = MARGIN + PADDING * i;
    
    s.start = a;
    s.end = b;

    segments.push_back(s);
  }

}

void init_segs_hash() {
        int i;
        point2D a, b;
        segment2D s;
        int j = 0;
        for (i = 0; i < n; i++) {
                if (i % 3 == 0) {
                        a.x = 40 + j;
                        a.y = 20;
                        b.x = 40 + j;
                        b.y = 480;
                        j = j + 1;
                }
                if (i % 3 == 1) {
                        a.x = 38;
                        b.x = 462;
                        a.y = 200 + j;
                        b.y = 200 + j;
                        j++;
                }
                if (i % 3 == 2) {
                        a.x = 260 - j;
                        a.y = 20;
                        b.y = 480;
                        b.x = 260 - j;
                        j++;
                }
                s.start = a;
                s.end = b;
                segments.push_back(s);
        }
}

//Test case: helix
void initialize_segments_helix(){
	//clear the vector
    segments.clear();
    int pad = WINDOWSIZE/(n/2 + 2);
    int length = pad * 2;
    point2D a, b;
    segment2D s;
    // printf("pad = %d\n", pad);
    int count1 = 1, count2 = 1;
    for (int i = 0; i < n; i++) {
        if (i % 2 == 0) {
            // horizontal segment
           	a.y = pad * (count1 + 1);
           	b.y = pad * (count1 + 1);
           	a.x = pad * count1;
           	b.x = a.x + length;
           	count1++;
        }
        else {
            // vertical segment
            a.x = pad * (count2 + 1);
            b.x = pad * (count2 + 1);
            a.y = pad * count2;
            b.y = a.y + length;
            count2++;
        }
		s.start = a;
		s.end = b;
        // insert the segment in the array of segments
        segments.push_back(s);
    }
}

void timerfunc() 
{
  //Move the line forward and check for intersections along the line
  sweep_line_x++;
  computeIntersections(sweep_line_x);

  //redisplay
  glutPostRedisplay(); 
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
