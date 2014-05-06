/*
 * Fourier Transform analysis of C. elegans undulation motor activation.
 *
 * The body is modeled as a linear span of equally spaced articulated joints, flexed
 * on a common plane by opposing muscle groups that are activated by dorsal and ventral
 * motor neurons.
 *
 * Undulation is a sinusoidal motor activation pattern of the joints that changes
 * over time. This is represented by a two dimensional array: space X time.
 *
 * The Fourier Transform of the motor activation array produces a frequency spectrum
 * of sinusoidal patterns in space and time.
 */

#include "mutableParm.hpp"
#include <fftw3.h>
#include <math.h>
#include <vector>
#include <numeric>
using namespace std;

// Usage.
char *Usage[] =
{
   (char *)"Usage:",
   (char *)"",
   (char *)"undulationFFT",
   (char *)"   [-body_joints <number of body joints>]",
   (char *)"   [-motor_sequence_length <length of motor sequence>]",
   (char *)"   [-unsynch_joint_activations (unsynchronize joint activations)]",
   (char *)"   [-fixed_joint_activations(fixed joint activations)]",
   (char *)"   [-random_activations (randomize all activations)]",
   (char *)"   [-random_seed <random seed>]",
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


bool UnsynchJointActivations = false;
bool FixedJointActivations   = false;
bool RandomActivations       = false;

// Undulation parameters:
int         BODY_JOINTS           = 12;
int         MOTOR_SEQUENCE_LENGTH = 25;
MutableParm PeriodParm(2.5f, 10.0f, 0.1f, 1.0f);
MutableParm AmplitudeParm(0.5f, 1.0f, .05f, 1.0f);
MutableParm PhaseParm(0.0f, 6.28f, 0.39f, 1.0f);
MutableParm SpeedParm(0.09f, 0.39f, 0.09f, 1.0f);

// Undulation motor activation sequence.
vector<vector<float> > DorsalMotorSequence;
vector<vector<float> > VentralMotorSequence;

Random *Randomizer = NULL;

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

// Create undulation motor activation sequence.
void createMotorSequence()
{
   int   i, j;
   float angle, x, p;

   vector<float> dorsalMagnitudes, ventralMagnitudes;
   vector<float> randomPhaseAdvances;
   float         M_PI_X2 = M_PI * 2.0f;

   float period    = PeriodParm.value;
   float amplitude = AmplitudeParm.value;
   float phase     = PhaseParm.value;
   if (UnsynchJointActivations)
   {
      for (i = 0; i < BODY_JOINTS; i++)
      {
         randomPhaseAdvances.push_back((float)Randomizer->RAND_INTERVAL(0.0, M_PI_X2));
      }
   }
   float speed = SpeedParm.value;
   if (FixedJointActivations)
   {
      speed = 0.0f;
   }
   float xdelta = M_PI_X2 / (float)(BODY_JOINTS - 1);
   DorsalMotorSequence.clear();
   VentralMotorSequence.clear();
   for (i = 0; i < MOTOR_SEQUENCE_LENGTH; i++, phase += speed)
   {
      dorsalMagnitudes.clear();
      ventralMagnitudes.clear();
      for (j = 0, angle = 0.0f; j < BODY_JOINTS; j++, angle += xdelta)
      {
         p = phase;
         if (UnsynchJointActivations)
         {
            p += randomPhaseAdvances[j];
         }
         x = sin((M_PI_X2 / period) * (angle - p)) * amplitude;
         if (RandomActivations)
         {
            x = (float)(Randomizer->RAND_PROB());
            if (Randomizer->RAND_BOOL())
            {
               x = -x;
            }
         }
         if (x > 0.0f)
         {
            dorsalMagnitudes.push_back(x);
            ventralMagnitudes.push_back(0.0f);
         }
         else if (x < 0.0f)
         {
            dorsalMagnitudes.push_back(0.0f);
            ventralMagnitudes.push_back(-x);
         }
         else
         {
            dorsalMagnitudes.push_back(0.0f);
            ventralMagnitudes.push_back(0.0f);
         }
      }
      DorsalMotorSequence.push_back(dorsalMagnitudes);
      VentralMotorSequence.push_back(ventralMagnitudes);
   }
}


// Transform motor sequence.
void transformMotorSequence()
{
   double       *motors, *bodyMotors, *jointMotors;
   fftw_complex *bodyDFT, *jointDFT;
   fftw_plan    bodyPlan, bodyPlanInv, jointPlan, jointPlanInv;
   double       bodyScale  = 1.0 / BODY_JOINTS;
   double       jointScale = 1.0 / MOTOR_SEQUENCE_LENGTH;
   int          i, j, k;
   double       a, r, im, m, max;

   vector<double> values;
   double         sum, mean, sq_sum, stdev, bodyMean, jointMean, bodyMax, jointMax, stdevFitness, rangeFitness;

   motors      = (double *)fftw_malloc(sizeof(double) * (MOTOR_SEQUENCE_LENGTH * BODY_JOINTS));
   bodyMotors  = (double *)fftw_malloc(sizeof(double) * BODY_JOINTS);
   bodyDFT     = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * (BODY_JOINTS / 2 + 1));
   bodyPlan    = fftw_plan_dft_r2c_1d(BODY_JOINTS, bodyMotors, bodyDFT, FFTW_MEASURE);
   bodyPlanInv = fftw_plan_dft_c2r_1d(BODY_JOINTS, bodyDFT, bodyMotors, FFTW_MEASURE);

   printf("Motor activations:\n");
   for (i = 0; i < MOTOR_SEQUENCE_LENGTH; i++)
   {
      for (j = 0; j < BODY_JOINTS; j++)
      {
         a = (double)(DorsalMotorSequence[i][j] - VentralMotorSequence[i][j]);
         motors[j + (BODY_JOINTS * i)] = a;
         printf("%0.3f ", a);
      }
      printf("\n");
   }

   printf("\nTransforms:\n");
   printf("\nBody:\n");
   bodyMean = 0.0;
   bodyMax = 0.0;
   for (i = 0, k = BODY_JOINTS / 2 + 1; i < MOTOR_SEQUENCE_LENGTH; i++)
   {
      printf("\nSequence %d:\nInput:\n", i);
      for (j = 0; j < BODY_JOINTS; j++)
      {
         bodyMotors[j] = motors[j + (BODY_JOINTS * i)];
         printf("%0.3f ", bodyMotors[j]);
      }
      fftw_execute(bodyPlan);
      printf("\nFrequency,Magnitude\n");
      values.clear();
	  max = 0.0;
      for (j = 0; j < k; j++)
      {
         r  = bodyDFT[j][0];
         im = bodyDFT[j][1];
         m  = sqrt((r * r) + (im * im));
         if (j > 0)
         {
            values.push_back(m);
			if (j == 1 || m > max)
			{
				max = m;
			}
         }
         printf("%d,%0.3f\n", j, m);
      }
      fftw_execute(bodyPlanInv);
      printf("Inverse:\n");
      for (j = 0; j < BODY_JOINTS; j++)
      {
         printf("%0.3f ", bodyMotors[j] * bodyScale);
      }
      printf("\n");
      if (values.size() > 0)
      {
         sum       = std::accumulate(values.begin(), values.end(), 0.0);
         mean      = sum / values.size();
		 bodyMax += (max - mean);
         sq_sum    = std::inner_product(values.begin(), values.end(), values.begin(), 0.0);
         stdev     = std::sqrt(sq_sum / values.size() - mean * mean);
         bodyMean += stdev;
      }
   }
   if (MOTOR_SEQUENCE_LENGTH > 0)
   {
      bodyMean /= (double)MOTOR_SEQUENCE_LENGTH;
	  bodyMax /= (double)MOTOR_SEQUENCE_LENGTH;
   }

   jointMotors  = (double *)fftw_malloc(sizeof(double) * MOTOR_SEQUENCE_LENGTH);
   jointDFT     = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * (MOTOR_SEQUENCE_LENGTH / 2 + 1));
   jointPlan    = fftw_plan_dft_r2c_1d(MOTOR_SEQUENCE_LENGTH, jointMotors, jointDFT, FFTW_MEASURE);
   jointPlanInv = fftw_plan_dft_c2r_1d(MOTOR_SEQUENCE_LENGTH, jointDFT, jointMotors, FFTW_MEASURE);

   printf("\nTransforms:\n");
   printf("\nJoints:\n");
   jointMean = 0.0;
   jointMax = 0.0;
   for (i = 0, k = MOTOR_SEQUENCE_LENGTH / 2 + 1; i < BODY_JOINTS; i++)
   {
      printf("\nJoint %d:\nInput:\n", i);
      for (j = 0; j < MOTOR_SEQUENCE_LENGTH; j++)
      {
         jointMotors[j] = motors[i + (BODY_JOINTS * j)];
         printf("%0.3f ", jointMotors[j]);
      }
      fftw_execute(jointPlan);
      printf("\nFrequency,Magnitude\n");
      values.clear();
	  max = 0.0;
      for (j = 0; j < k; j++)
      {
         r  = jointDFT[j][0];
         im = jointDFT[j][1];
         m  = sqrt((r * r) + (im * im));
         if (j > 0)
         {
            values.push_back(m);
			if (j == 1 || m > max)
			{
				max = m;
			}
         }
         printf("%d,%0.3f\n", j, m);
      }
      fftw_execute(jointPlanInv);
      printf("Inverse:\n");
      for (j = 0; j < MOTOR_SEQUENCE_LENGTH; j++)
      {
         printf("%0.3f ", jointMotors[j] * jointScale);
      }
      printf("\n");
      if (values.size() > 0)
      {
         sum        = std::accumulate(values.begin(), values.end(), 0.0);
         mean       = sum / values.size();
		 jointMax += (max - mean);
         sq_sum     = std::inner_product(values.begin(), values.end(), values.begin(), 0.0);
         stdev      = std::sqrt(sq_sum / values.size() - mean * mean);
         jointMean += stdev;
      }
   }
   if (BODY_JOINTS > 0)
   {
      jointMean /= (double)BODY_JOINTS;
	  jointMax /= (double)BODY_JOINTS;
   }
   stdevFitness = (bodyMean + jointMean) / 2.0;
   rangeFitness = (bodyMax + jointMax) / 2.0;
   printf("body stdev mean=%f, joint stdev mean=%f, stdev fitness=%f, range fitness=%f\n", bodyMean, jointMean, stdevFitness, rangeFitness);

   fftw_destroy_plan(bodyPlan);
   fftw_destroy_plan(bodyPlanInv);
   fftw_free(bodyMotors);
   fftw_free(bodyDFT);
   fftw_destroy_plan(jointPlan);
   fftw_destroy_plan(jointPlanInv);
   fftw_free(jointMotors);
   fftw_free(jointDFT);
   fftw_free(motors);
}


int main(int argc, char *argv[])
{
   RANDOM randomSeed = 4517;

   for (int i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "-body_joints") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         BODY_JOINTS = atoi(argv[i]);
         if (BODY_JOINTS < 0)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-motor_sequence_length") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         MOTOR_SEQUENCE_LENGTH = atoi(argv[i]);
         if (MOTOR_SEQUENCE_LENGTH < 0)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-unsynch_joint_activations") == 0)
      {
         UnsynchJointActivations = true;
         continue;
      }
      if (strcmp(argv[i], "-fixed_joint_activations") == 0)
      {
         FixedJointActivations = true;
         continue;
      }
      if (strcmp(argv[i], "-random_activations") == 0)
      {
         RandomActivations = true;
         continue;
      }
      if (strcmp(argv[i], "-random_seed") == 0)
      {
         i++;
         if (i >= argc)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         randomSeed = atoi(argv[i]);
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
   if (UnsynchJointActivations && (FixedJointActivations || RandomActivations))
   {
      printUsageError((char *)"conflicting options");
      return(1);
   }
   if (FixedJointActivations && (UnsynchJointActivations || RandomActivations))
   {
      printUsageError((char *)"conflicting options");
      return(1);
   }
   if (RandomActivations && (UnsynchJointActivations || FixedJointActivations))
   {
      printUsageError((char *)"conflicting options");
      return(1);
   }

   Randomizer = new Random(randomSeed);
   PeriodParm.initValue(Randomizer);
   AmplitudeParm.initValue(Randomizer);
   PhaseParm.initValue(Randomizer);
   SpeedParm.initValue(Randomizer);
   printf("Parameters:\n");
   printf("Period:\n"); 
   PeriodParm.print();
   printf("Amplitude:\n");
   AmplitudeParm.print();
   printf("Phase:\n");
   PhaseParm.print();
   printf("Speed:\n");
   SpeedParm.print();
   printf("\n");

   // Create motor activation sequence.
   createMotorSequence();

   // Transform sequence.
   transformMotorSequence();

   delete Randomizer;

   return(0);
}
