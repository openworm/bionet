// Bionet sinusoidal movement (locomotion) behavior.

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <EasyGL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "behavior.hpp"
#include "gettime.h"


using namespace std;

// Usage.
char *Usage[] =
{
   (char *)"Usage:",
   (char *)"",
   (char *)"Run:",
   (char *)"",
   (char *)"CElegansBehavior",
   (char *)"",
   (char *)"Create network behavior:",
   (char *)"",
   (char *)"CElegansBehavior",
   (char *)"  -behaviorLength <sensory-motor sequence length>",
   (char *)"  [-motorOutputDelay <delay for motor output> (for signal propagation)]",
   (char *)"  [-initialPhase <initial sinusoid phase>]",
   (char *)"  -saveBehaviors <behaviors file name>",
   (char *)"",
   (char *)"Load and run network behavior:",
   (char *)"",
   (char *)"CElegansBehavior",
   (char *)"   -loadBehaviors <behaviors file name>",
   NULL
};

void printUsage()
{
   for (int i = 0; Usage[i] != NULL; i++)
   {
      fprintf(stderr, (char *)"%s\n", Usage[i]);
   }
}


void printUsageError(char *error)
{
   fprintf(stderr, (char *)"Error: %s\n", error);
   printUsage();
}


// Connectome dimensions.
#define NUM_SENSORS    86
#define NUM_MOTORS     97
#define NUM_NEURONS    396

// Behavior load/save.
int  MotorSequenceLength = -1;
char *BehaviorsLoadFile  = NULL;
char *BehaviorsSaveFile  = NULL;
vector<vector<float> > VentralMotorSequence;
vector<vector<float> > DorsalMotorSequence;
int   MotorSequenceIndex = 0;
int   MotorOutputDelay   = -1;
int   MotorDelayCount    = 0;
float InitialPhase       = 0.0f;
void loadBehaviors();
void saveBehaviors();

// Connectome cell index.
struct CellIndex
{
   char *name;
   int  index;
};

// Sensor indices.
struct CellIndex SensorIndices[] =
{
   { (char *)"ALML", 8 },
   { (char *)"ALMR", 9 }
};

// Muscle indices.
struct CellIndex MuscleIndices[] =
{
   { (char *)"MDL01", 48 },
   { (char *)"MDL02", 49 },
   { (char *)"MDL03", 53 },
   { (char *)"MDL04", 50 },
   { (char *)"MDL05",  0 },
   { (char *)"MDL06", 74 },
   { (char *)"MDL07", 16 },
   { (char *)"MDL08",  2 },
   { (char *)"MDL09", 20 },
   { (char *)"MDL10", 18 },
   { (char *)"MDL11", 24 },
   { (char *)"MDL12", 22 },
   { (char *)"MDL13", 28 },
   { (char *)"MDL14", 26 },
   { (char *)"MDL15", 32 },
   { (char *)"MDL16", 30 },
   { (char *)"MDL17", 36 },
   { (char *)"MDL18", 34 },
   { (char *)"MDL19",  6 },
   { (char *)"MDL20",  4 },
   { (char *)"MDL21",  8 },
   { (char *)"MDL22",  9 },
   { (char *)"MDL23", 10 },
   { (char *)"MDL24", 11 },
   { (char *)"MDR01", 51 },
   { (char *)"MDR02", 52 },
   { (char *)"MDR03", 55 },
   { (char *)"MDR04", 71 },
   { (char *)"MDR05",  1 },
   { (char *)"MDR06", 75 },
   { (char *)"MDR07", 17 },
   { (char *)"MDR08",  3 },
   { (char *)"MDR09", 21 },
   { (char *)"MDR10", 19 },
   { (char *)"MDR11", 25 },
   { (char *)"MDR12", 23 },
   { (char *)"MDR13", 29 },
   { (char *)"MDR14", 27 },
   { (char *)"MDR15", 33 },
   { (char *)"MDR16", 31 },
   { (char *)"MDR17", 37 },
   { (char *)"MDR18", 35 },
   { (char *)"MDR19",  7 },
   { (char *)"MDR20",  5 },
   { (char *)"MDR21", 12 },
   { (char *)"MDR22", 13 },
   { (char *)"MDR23", 14 },
   { (char *)"MDR24", 15 },
   { (char *)"MVL01", 54 },
   { (char *)"MVL02", 58 },
   { (char *)"MVL03", 44 },
   { (char *)"MVL04", 73 },
   { (char *)"MVL05", 65 },
   { (char *)"MVL06", 69 },
   { (char *)"MVL07", 66 },
   { (char *)"MVL08", 70 },
   { (char *)"MVL09", 62 },
   { (char *)"MVL10", 42 },
   { (char *)"MVL11", 39 },
   { (char *)"MVL12", 38 },
   { (char *)"MVL13", 63 },
   { (char *)"MVL14", 40 },
   { (char *)"MVL15", 80 },
   { (char *)"MVL16", 81 },
   { (char *)"MVL17", 86 },
   { (char *)"MVL18", 84 },
   { (char *)"MVL19", 88 },
   { (char *)"MVL20", 89 },
   { (char *)"MVL21", 92 },
   { (char *)"MVL22", 60 },
   { (char *)"MVL23", 93 },
   { (char *)"MVL24", -1 }, // missing
   { (char *)"MVR01", 56 },
   { (char *)"MVR02", 59 },
   { (char *)"MVR03", 57 },
   { (char *)"MVR04", 45 },
   { (char *)"MVR05", 64 },
   { (char *)"MVR06", 67 },
   { (char *)"MVR07", 72 },
   { (char *)"MVR08", 68 },
   { (char *)"MVR09", 76 },
   { (char *)"MVR10", 43 },
   { (char *)"MVR11", 78 },
   { (char *)"MVR12", 77 },
   { (char *)"MVR13", 79 },
   { (char *)"MVR14", 41 },
   { (char *)"MVR15", 82 },
   { (char *)"MVR16", 83 },
   { (char *)"MVR17", 87 },
   { (char *)"MVR18", 85 },
   { (char *)"MVR19", 90 },
   { (char *)"MVR20", 91 },
   { (char *)"MVR21", 61 },
   { (char *)"MVR22", 94 },
   { (char *)"MVR23", 95 },
   { (char *)"MVR24", 96 }
};

// Dimensions.
#define WINDOW_WIDTH     550
#define WINDOW_HEIGHT    550
float WindowWidth      = (float)WINDOW_WIDTH;
float WindowHeight     = (float)WINDOW_HEIGHT;
float TouchHeightRatio = 0.1f;
float RunHeightRatio   = 0.1f;

// Functions.
void display();
void drawCircle(float x, float y, float radius, bool solid = false);
void reshape(int, int);
void idle();
void keyInput(unsigned char key, int x, int y);
void mouseClicked(int button, int state, int x, int y);
void mouseDragged(int x, int y);
void mouseMoved(int x, int y);
void renderBitmapString(float, float, void *, char *);

// Controls.
class EventsHandler : public GUIEventListener
{
public:
   virtual void actionPerformed(GUIEvent& evt);
};
EventsHandler Handler;
GUICheckBox   *TouchCheck;
GUICheckBox   *PauseCheck;
GUIFrame      *GuiFrame = NULL;
bool          Run;
bool          Step;

// Body sinusoid.
int   BodyJoints     = 12;
float BodyAmplitude  = 0.05f;
float BodyPeriod     = 1.0f;
float BodyPhase      = 0.0f;
float BodyPhaseDelta = 0.01f;
float BodyRadius     = 0.02f;

// Body animation timer.
#define MS_ANIMATION_TIMER    100
TIME msAnimationTimer = INVALID_TIME;

/*
 *  Available fonts:
 *  GLUT_BITMAP_8_BY_13
 *  GLUT_BITMAP_9_BY_15
 *  GLUT_BITMAP_TIMES_ROMAN_10
 *  GLUT_BITMAP_TIMES_ROMAN_24
 *  GLUT_BITMAP_HELVETICA_10
 *  GLUT_BITMAP_HELVETICA_12
 *  GLUT_BITMAP_HELVETICA_18
 */
#define SMALL_FONT    GLUT_BITMAP_8_BY_13
#define FONT          GLUT_BITMAP_9_BY_15
#define BIG_FONT      GLUT_BITMAP_TIMES_ROMAN_24
#define LINE_SPACE    15

// Main.
int main(int argc, char *argv[])
{
   bool gotInitialPhase = false;

   for (int i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "-behaviorLength") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         MotorSequenceLength = atoi(argv[i]);
         if (MotorSequenceLength < 0)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-motorOutputDelay") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         MotorOutputDelay = atoi(argv[i]);
         if (MotorOutputDelay < 0)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-initialPhase") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         InitialPhase = (float)atof(argv[i]);
         if (InitialPhase < 0.0f)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         gotInitialPhase = true;
         continue;
      }
      if (strcmp(argv[i], "-saveBehaviors") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         BehaviorsSaveFile = argv[i];
         continue;
      }
      if (strcmp(argv[i], "-loadBehaviors") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         BehaviorsLoadFile = argv[i];
         continue;
      }
      if ((strcmp(argv[i], "-h") == 0) ||
          (strcmp(argv[i], "-help") == 0) ||
          (strcmp(argv[i], "--h") == 0) ||
          (strcmp(argv[i], "--help") == 0) ||
          (strcmp(argv[i], "-?") == 0) ||
          (strcmp(argv[i], "--?") == 0))
      {
         printUsage();
         return(0);
      }
      printUsageError((char *)"invalid option");
      return(1);
   }
   if (BehaviorsSaveFile != NULL)
   {
      if (BehaviorsLoadFile != NULL)
      {
         printUsageError((char *)"incompatible options");
         return(1);
      }
      if (MotorSequenceLength == -1)
      {
         printUsageError((char *)"invalid option");
         return(1);
      }
   }
   else
   {
      if (MotorSequenceLength != -1)
      {
         printUsageError((char *)"invalid option");
         return(1);
      }
      if (MotorOutputDelay != -1)
      {
         printUsageError((char *)"invalid option");
         return(1);
      }
      if (gotInitialPhase)
      {
         printUsageError((char *)"invalid option");
         return(1);
      }
      if (BehaviorsLoadFile != NULL)
      {
         loadBehaviors();
      }
   }
   if (MotorOutputDelay == -1)
   {
      MotorOutputDelay = 0;
   }

   // Initialize.
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
   glutInitWindowSize((int)WindowWidth, (int)WindowHeight);
   glutCreateWindow("C. elegans");
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutIdleFunc(idle);
   glutKeyboardFunc(keyInput);
   glutMouseFunc(mouseClicked);
   glutMotionFunc(mouseDragged);
   glutPassiveMotionFunc(mouseMoved);
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0.0f, WindowWidth, 0.0f, WindowHeight);
   glScalef(1.0f, -1.0f, 1.0f);
   glTranslatef(0.0f, -WindowHeight, 0.0f);
   msAnimationTimer = gettime();
   GLeeInit();
   MediaPathManager::registerPath("../resource/");
   GuiFrame = new GUIFrame();
   assert(GuiFrame != NULL);
   GuiFrame->GUIPanel::loadXMLSettings("GUILayout.xml");
   GuiFrame->setGUIEventListener(&Handler);
   TouchCheck = (GUICheckBox *)GuiFrame->getWidgetByCallbackString("touch");
   TouchCheck->setAlphaFadeScale(1000.0);
   PauseCheck = (GUICheckBox *)GuiFrame->getWidgetByCallbackString("pause");
   PauseCheck->setAlphaFadeScale(1000.0);
   Run  = false;
   Step = false;

   // Start up.
   glutReshapeWindow(WindowWidth, WindowHeight);
   glutMainLoop();
   return(0);
}


// Load behaviors.
void loadBehaviors()
{
   int   i, j, k;
   float m;

   vector<float>      dorsalMagnitudes, ventralMagnitudes;
   vector<Behavior *> behaviors;
   Behavior           *behavior;

   if (!Behavior::loadBehaviors(BehaviorsLoadFile, behaviors))
   {
      fprintf(stderr, (char *)"Cannot load behavior file %s\n", BehaviorsLoadFile);
      exit(1);
   }
   if (behaviors.size() != 1)
   {
      fprintf(stderr, (char *)"Behavior file %s contains %d behaviors; must contain 1 behavior\n", BehaviorsLoadFile, behaviors.size());
      exit(1);
   }
   behavior            = behaviors[0];
   MotorSequenceLength = (int)behavior->motorSequence.size();
   for (i = 0; i < MotorSequenceLength; i++)
   {
      dorsalMagnitudes.clear();
      ventralMagnitudes.clear();
      for (j = 0; j < BodyJoints; j++)
      {
         k = j * 2;
         m = (behavior->motorSequence[i][MuscleIndices[k].index] +
              behavior->motorSequence[i][MuscleIndices[k + 1].index] +
              behavior->motorSequence[i][MuscleIndices[k + 24].index] +
              behavior->motorSequence[i][MuscleIndices[k + 25].index]) / 4.0f;
         dorsalMagnitudes.push_back(m);
         if (j < BodyJoints - 1)
         {
            m = (behavior->motorSequence[i][MuscleIndices[k + 48].index] +
                 behavior->motorSequence[i][MuscleIndices[k + 49].index] +
                 behavior->motorSequence[i][MuscleIndices[k + 72].index] +
                 behavior->motorSequence[i][MuscleIndices[k + 73].index]) / 4.0f;
         }
         else
         {
            m = (behavior->motorSequence[i][MuscleIndices[k + 48].index] +
                 behavior->motorSequence[i][MuscleIndices[k + 72].index] +
                 behavior->motorSequence[i][MuscleIndices[k + 73].index]) / 3.0f;
         }
         ventralMagnitudes.push_back(m);
      }
      DorsalMotorSequence.push_back(dorsalMagnitudes);
      VentralMotorSequence.push_back(ventralMagnitudes);
   }
}


// Save behaviors.
void saveBehaviors()
{
   int   i, j, k;
   float m;

   vector<float>      dorsalMagnitudes, ventralMagnitudes;
   vector<Behavior *> behaviors;
   Behavior           *behavior;

   behavior = new Behavior();
   assert(behavior != NULL);
   behavior->sensorSequence.resize(MotorSequenceLength);
   behavior->motorSequence.resize(MotorSequenceLength);
   for (i = 0; i < MotorSequenceLength; i++)
   {
      behavior->sensorSequence[i].resize(NUM_SENSORS, 0.0f);
      behavior->sensorSequence[i][SensorIndices[0].index] = 1.0f;
      behavior->sensorSequence[i][SensorIndices[1].index] = 1.0f;
      behavior->motorSequence[i].resize(NUM_MOTORS, 0.0f);
      dorsalMagnitudes  = DorsalMotorSequence[i];
      ventralMagnitudes = VentralMotorSequence[i];
      for (j = 0; j < BodyJoints; j++)
      {
         k = j * 2;
         m = dorsalMagnitudes[j];
         behavior->motorSequence[i][MuscleIndices[k].index]      = m;
         behavior->motorSequence[i][MuscleIndices[k + 1].index]  = m;
         behavior->motorSequence[i][MuscleIndices[k + 24].index] = m;
         behavior->motorSequence[i][MuscleIndices[k + 25].index] = m;
         m = ventralMagnitudes[j];
         behavior->motorSequence[i][MuscleIndices[k + 48].index] = m;
         if (j < BodyJoints - 1)
         {
            behavior->motorSequence[i][MuscleIndices[k + 49].index] = m;
         }
         behavior->motorSequence[i][MuscleIndices[k + 72].index] = m;
         behavior->motorSequence[i][MuscleIndices[k + 73].index] = m;
      }
   }
   behaviors.push_back(behavior);
   if (!Behavior::saveBehaviors(BehaviorsSaveFile, behaviors))
   {
      fprintf(stderr, (char *)"Cannot save behavior file %s\n", BehaviorsSaveFile);
      exit(1);
   }
}


// Pi.
#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif
float M_PI_X2 = M_PI * 2.0f;

// 2D point.
class Point2D
{
public:
   float x;
   float y;

   Point2D()
   {
      x = y = 0.0f;
   }


   Point2D(float x, float y)
   {
      this->x = x;
      this->y = y;
   }
};


// Display.
void display()
{
   int   i, j;
   bool  isTouched;
   float bodyOffset, bodyHeight, d;
   float angle, xdelta, ydelta, radius;

   vector<Point2D> centers, dorsals, ventrals;
   vector<float>   ventralMagnitudes, dorsalMagnitudes;
   Point2D         point;
   char            buf[50];

   glClear(GL_COLOR_BUFFER_BIT);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // Partitions.
   glColor3f(1.0f, 1.0f, 1.0f);
   bodyOffset = WindowHeight * TouchHeightRatio;
   bodyHeight = WindowHeight - bodyOffset - (WindowHeight * RunHeightRatio);
   glBegin(GL_LINES);
   d = bodyOffset * 0.95f;
   glVertex2f(WindowWidth * 0.25f, d);
   glVertex2f(WindowWidth * 0.75f, d);
   glVertex2f(WindowWidth * 0.25f, 0.0f);
   d = WindowHeight * (1.0f - RunHeightRatio) * 1.005f;
   glVertex2f(WindowWidth * 0.25f, d);
   glVertex2f(WindowWidth * 0.75f, 0.0f);
   glVertex2f(WindowWidth * 0.75f, d);
   glVertex2f(0.0f, d);
   glVertex2f(WindowWidth, d);
   glEnd();

   // Controls.
   GuiFrame->render((float)gettime());

   // Body.
   isTouched = TouchCheck->isChecked();
   if (isTouched)
   {
      xdelta = M_PI_X2 / (float)(BodyJoints - 1);
   }
   else
   {
      xdelta = 0.0f;
   }
   if (isTouched && BehaviorsLoadFile)
   {
      dorsalMagnitudes  = DorsalMotorSequence[MotorSequenceIndex];
      ventralMagnitudes = VentralMotorSequence[MotorSequenceIndex];
   }
   radius  = WindowWidth * BodyRadius;
   point.y = bodyOffset + radius;
   ydelta  = (bodyHeight - (radius * 2.0f)) / (float)(BodyJoints - 1);
   for (i = 0, angle = 0.0f; i < BodyJoints; i++, angle += xdelta)
   {
      if (isTouched && (MotorDelayCount == MotorOutputDelay))
      {
         if (BehaviorsLoadFile)
         {
            point.x = dorsalMagnitudes[i] - ventralMagnitudes[i];
         }
         else
         {
            point.x = sin(BodyPeriod * (angle - BodyPhase));
            if (point.x > 0.0f)
            {
               dorsalMagnitudes.push_back(point.x);
               ventralMagnitudes.push_back(0.0f);
            }
            else if (point.x < 0.0f)
            {
               dorsalMagnitudes.push_back(0.0f);
               ventralMagnitudes.push_back(-point.x);
            }
            else
            {
               dorsalMagnitudes.push_back(0.0f);
               ventralMagnitudes.push_back(0.0f);
            }
         }
      }
      else
      {
         point.x = 0.0f;
         dorsalMagnitudes.push_back(0.0f);
         ventralMagnitudes.push_back(0.0f);
      }
      point.x *= (BodyAmplitude * WindowWidth);
      point.x += (WindowWidth * 0.5f);
      centers.push_back(point);
      point.x += (WindowWidth * BodyRadius);
      dorsals.push_back(point);
      point.x -= (WindowWidth * BodyRadius * 2.0f);
      ventrals.push_back(point);
      point.y += ydelta;
   }
   if (Run && BehaviorsSaveFile)
   {
      DorsalMotorSequence.push_back(dorsalMagnitudes);
      VentralMotorSequence.push_back(ventralMagnitudes);
   }
   glColor3f(0.5f, 0.5f, 0.5f);
   for (i = 0, j = (int)dorsals.size() - 1; i < j; i++)
   {
      glBegin(GL_POLYGON);
      point = dorsals[i];
      glVertex2f(point.x, point.y);
      point = dorsals[i + 1];
      glVertex2f(point.x, point.y);
      point = ventrals[i + 1];
      glVertex2f(point.x, point.y);
      point = ventrals[i];
      glVertex2f(point.x, point.y);
      glEnd();
   }
   radius = WindowWidth * (BodyRadius * 0.9f);
   point  = centers[0];
   drawCircle(point.x, point.y, radius, true);
   point = centers[centers.size() - 1];
   drawCircle(point.x, point.y, radius, true);
   glColor3f(1.0f, 1.0f, 1.0f);
   glBegin(GL_LINE_STRIP);
   for (i = 0, j = (int)centers.size(); i < j; i++)
   {
      point = centers[i];
      glVertex2f(point.x, point.y);
   }
   glEnd();
   for (i = 0, j = (int)centers.size(); i < j; i++)
   {
      point = centers[i];
      glColor3f(1.0f, 1.0f, 1.0f);
      drawCircle(point.x, point.y, radius);
      glColor3f(dorsalMagnitudes[i], 0.0f, 0.0f);
      drawCircle((WindowWidth * 0.75f) + (radius * 1.5f), point.y, radius, true);
      glColor3f(1.0f, 1.0f, 1.0f);
      sprintf(buf, (char *)"%0.2f", dorsalMagnitudes[i]);
      renderBitmapString((WindowWidth * 0.75f) - (radius * 5.0f), point.y + (radius * 0.4f), FONT, buf);
      glColor3f(ventralMagnitudes[i], 0.0f, 0.0f);
      drawCircle((WindowWidth * 0.25f) - (radius * 1.5f), point.y, radius, true);
      glColor3f(1.0f, 1.0f, 1.0f);
      sprintf(buf, (char *)"%0.2f", ventralMagnitudes[i]);
      renderBitmapString((WindowWidth * 0.25f) + (radius), point.y + (radius * 0.4f), FONT, buf);
      switch (i)
      {
      case 0:
         sprintf(buf, (char *)"1,2");
         renderBitmapString((WindowWidth * 0.25f) - (radius * 11.5f), point.y + (radius * 0.4f), FONT, buf);
         renderBitmapString((WindowWidth * 0.75f) + (radius * 3.5f), point.y + (radius * 0.4f), FONT, buf);
         break;

      case 1:
         sprintf(buf, (char *)"3,4");
         renderBitmapString((WindowWidth * 0.25f) - (radius * 11.5f), point.y + (radius * 0.4f), FONT, buf);
         renderBitmapString((WindowWidth * 0.75f) + (radius * 3.5f), point.y + (radius * 0.4f), FONT, buf);
         break;

      case 2:
         sprintf(buf, (char *)"5,6");
         renderBitmapString((WindowWidth * 0.25f) - (radius * 11.5f), point.y + (radius * 0.4f), FONT, buf);
         renderBitmapString((WindowWidth * 0.75f) + (radius * 3.5f), point.y + (radius * 0.4f), FONT, buf);
         break;

      case 3:
         sprintf(buf, (char *)"7,8");
         renderBitmapString((WindowWidth * 0.25f) - (radius * 11.5f), point.y + (radius * 0.4f), FONT, buf);
         renderBitmapString((WindowWidth * 0.75f) + (radius * 3.5f), point.y + (radius * 0.4f), FONT, buf);
         break;

      case 4:
         sprintf(buf, (char *)"9,10");
         renderBitmapString((WindowWidth * 0.25f) - (radius * 11.5f), point.y + (radius * 0.4f), FONT, buf);
         renderBitmapString((WindowWidth * 0.75f) + (radius * 3.5f), point.y + (radius * 0.4f), FONT, buf);
         break;

      case 5:
         sprintf(buf, (char *)"11,12");
         renderBitmapString((WindowWidth * 0.25f) - (radius * 11.5f), point.y + (radius * 0.4f), FONT, buf);
         renderBitmapString((WindowWidth * 0.75f) + (radius * 3.5f), point.y + (radius * 0.4f), FONT, buf);
         break;

      case 6:
         sprintf(buf, (char *)"13,14");
         renderBitmapString((WindowWidth * 0.25f) - (radius * 11.5f), point.y + (radius * 0.4f), FONT, buf);
         renderBitmapString((WindowWidth * 0.75f) + (radius * 3.5f), point.y + (radius * 0.4f), FONT, buf);
         break;

      case 7:
         sprintf(buf, (char *)"15,16");
         renderBitmapString((WindowWidth * 0.25f) - (radius * 11.5f), point.y + (radius * 0.4f), FONT, buf);
         renderBitmapString((WindowWidth * 0.75f) + (radius * 3.5f), point.y + (radius * 0.4f), FONT, buf);
         break;

      case 8:
         sprintf(buf, (char *)"17,18");
         renderBitmapString((WindowWidth * 0.25f) - (radius * 11.5f), point.y + (radius * 0.4f), FONT, buf);
         renderBitmapString((WindowWidth * 0.75f) + (radius * 3.5f), point.y + (radius * 0.4f), FONT, buf);
         break;

      case 9:
         sprintf(buf, (char *)"19,20");
         renderBitmapString((WindowWidth * 0.25f) - (radius * 11.5f), point.y + (radius * 0.4f), FONT, buf);
         renderBitmapString((WindowWidth * 0.75f) + (radius * 3.5f), point.y + (radius * 0.4f), FONT, buf);
         break;

      case 10:
         sprintf(buf, (char *)"21,22");
         renderBitmapString((WindowWidth * 0.25f) - (radius * 11.5f), point.y + (radius * 0.4f), FONT, buf);
         renderBitmapString((WindowWidth * 0.75f) + (radius * 3.5f), point.y + (radius * 0.4f), FONT, buf);
         break;

      case 11:
         sprintf(buf, (char *)"23,24");
         renderBitmapString((WindowWidth * 0.25f) - (radius * 11.5f), point.y + (radius * 0.4f), FONT, buf);
         renderBitmapString((WindowWidth * 0.75f) + (radius * 3.5f), point.y + (radius * 0.4f), FONT, buf);
         break;
      }
   }
   renderBitmapString(10, 15, FONT, (char *)"MV R/L");
   renderBitmapString((int)(WindowWidth * 0.75f) + 10, 15, FONT, (char *)"MD R/L");

   if (BehaviorsLoadFile)
   {
      sprintf(buf, "Load: %d/%d", MotorSequenceIndex + 1, MotorSequenceLength);
      renderBitmapString(10, WindowHeight - 15, FONT, buf);
   }
   else if (BehaviorsSaveFile)
   {
      sprintf(buf, "Save: %d/%d", MotorSequenceIndex + 1, MotorSequenceLength);
      renderBitmapString(10, WindowHeight - 15, FONT, buf);
   }

   glutSwapBuffers();
   glFlush();

   if (Run)
   {
      Run = false;

      // Advance phase.
      BodyPhase += (M_PI_X2 * BodyPhaseDelta);
      if (BodyPhase >= M_PI_X2)
      {
         BodyPhase = 0.0f;
      }
      if (MotorDelayCount < MotorOutputDelay)
      {
         BodyPhase = InitialPhase;
         MotorDelayCount++;
      }

      // Behavior complete?
      MotorSequenceIndex++;
      if (BehaviorsLoadFile)
      {
         if (MotorSequenceIndex == MotorSequenceLength)
         {
#ifdef WIN32
            Sleep(5000);
#else
            sleep(5);
#endif
            exit(0);
         }
      }
      else if (BehaviorsSaveFile)
      {
         if (MotorSequenceIndex == MotorSequenceLength)
         {
            saveBehaviors();
#ifdef WIN32
            Sleep(5000);
#else
            sleep(5);
#endif
            exit(0);
         }
      }
   }
}


// Draw a circle.
void drawCircle(float x, float y, float radius, bool solid)
{
   if (solid)
   {
      glBegin(GL_POLYGON);
   }
   else
   {
      glBegin(GL_LINE_LOOP);
   }
   float rx = radius;
   float ry = radius;
   glVertex2f(x + rx, y);
   int   sides = 20;
   float ad    = M_PI_X2 / (float)sides;
   float a     = ad;
   for (int i = 1; i < sides; i++, a += ad)
   {
      glVertex2f(x + (rx * cos(a)), y + (ry * sin(a)));
   }
   glEnd();
}


// Window reshape.
void reshape(int width, int height)
{
   glViewport(0, 0, width, height);
   WindowWidth  = (float)width;
   WindowHeight = (float)height;
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0.0f, WindowWidth, 0.0f, WindowHeight);
   glScalef(1.0f, -1.0f, 1.0f);
   glTranslatef(0.0f, -WindowHeight, 0.0f);
   GuiFrame->setDimensions(WindowWidth, WindowHeight);
   GuiFrame->forceUpdate(true);
   glutPostRedisplay();
}


// Idle.
void idle()
{
   bool isTouched = TouchCheck->isChecked();

   if (!PauseCheck->isChecked() || Step)
   {
      Step = false;
      if (isTouched)
      {
         // Time to run?
         TIME t = gettime();
         if ((t - msAnimationTimer) >= MS_ANIMATION_TIMER)
         {
            msAnimationTimer = t;
            Run = true;
         }
      }
   }
   if (!isTouched)
   {
      // Reset.
      BodyPhase          = InitialPhase;
      MotorSequenceIndex = 0;
      MotorDelayCount    = 0;
      if (BehaviorsSaveFile)
      {
         DorsalMotorSequence.clear();
         VentralMotorSequence.clear();
      }
   }
   glutPostRedisplay();
}


// Keyboard input.
void keyInput(unsigned char key, int x, int y)
{
}


// Mouse callbacks.
void mouseClicked(int button, int state, int x, int y)
{
   if (button != GLUT_LEFT_BUTTON)
   {
      return;
   }
   MouseEvent event = MouseEvent(MB_BUTTON1, x, y, GuiFrame->getHeight() - y);
   GuiFrame->checkMouseEvents(event, (state == GLUT_DOWN) ? ME_CLICKED : ME_RELEASED);
}


void mouseDragged(int x, int y)
{
   MouseEvent event = MouseEvent(MB_UNKNOWN_BUTTON, x, y, GuiFrame->getHeight() - y);

   GuiFrame->checkMouseEvents(event, ME_DRAGGED);
}


void mouseMoved(int x, int y)
{
   MouseEvent event = MouseEvent(MB_UNKNOWN_BUTTON, x, y, GuiFrame->getHeight() - y);

   GuiFrame->checkMouseEvents(event, ME_MOVED);
}


// GUI event handler.
void EventsHandler::actionPerformed(GUIEvent& evt)
{
   const std::string& callbackString   = evt.getCallbackString();
   GUIRectangle       *sourceRectangle = evt.getEventSource(),
   *parent        = sourceRectangle ? sourceRectangle->getParent() : NULL;
   int widgetType = sourceRectangle->getWidgetType();

   if (widgetType == WT_CHECK_BOX)
   {
      GUICheckBox *checkbox = (GUICheckBox *)sourceRectangle;

      // Touch?
      if (callbackString == (char *)"touch")
      {
         return;
      }

      // Pause?
      if (callbackString == (char *)"pause")
      {
         return;
      }
   }

   if (widgetType == WT_BUTTON)
   {
      GUIButton *button = (GUIButton *)sourceRectangle;

      if (callbackString == "step")
      {
         if (button->isClicked())
         {
            Step = true;
         }
         return;
      }
   }
}


// Print string on screen at specified location.
void renderBitmapString(float x, float y, void *font, char *string)
{
   char *c;

   glRasterPos2f(x, y);
   for (c = string; *c != '\0'; c++)
   {
      glutBitmapCharacter(font, *c);
   }
}
