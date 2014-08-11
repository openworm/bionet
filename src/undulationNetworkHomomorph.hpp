// Undulation network homomorph.

#ifndef __UNDULATION_NETWORK_HOMOMORPH_HPP__
#define __UNDULATION_NETWORK_HOMOMORPH_HPP__

#include "networkHomomorph.hpp"
#ifdef FFT_UNDULATION_EVAL
#include <fftw3.h>
#endif

// Undulation behavior network homomorph.
class UndulationNetworkHomomorph : public NetworkHomomorph
{
public:

   // Constructors.
   UndulationNetworkHomomorph(int undulationMovements, Network *homomorph,
                              MutableParm& synapseWeightsParm,
                              vector<vector<pair<int, int> > > *motorConnections,
                              Random *randomizer, int tag = (-1));
   UndulationNetworkHomomorph(FilePointer *fp, int undulationMovements,
                              vector<vector<pair<int, int> > > *motorConnections,
                              Random *randomizer);

   // Destructor.
   ~UndulationNetworkHomomorph();

   // Optimize synapses.
   void optimize(int synapseOptimizedPathLength);

   // Fitness.
   float fitness;

   // Evaluate undulation behavior fitness.
   int undulationMovements;
   void evaluate();

   // Clone.
   UndulationNetworkHomomorph *clone(int tag = (-1));

   // Load.
   void load(FilePointer *fp);

   // Save.
   void save(FilePointer *fp);

   // Print.
   void print(bool printNetwork = false);

   // Connectome cell index.
   struct CellIndex
   {
      char *name;
      int  index;
   };

   // Sensor indices.
   static const struct CellIndex sensorIndices[];

   // Muscle indices.
   static const struct CellIndex muscleIndices[];

   // Body joint.
   struct BodyJoint
   {
      int dorsalMuscles[4];
      int ventralMuscles[4];
   };

   // Body joints.
   static const int              NUM_BODY_JOINTS = 12;
   static const struct BodyJoint bodyJoints[NUM_BODY_JOINTS];

#ifdef FFT_UNDULATION_EVAL
   // Fourier transform.
   double       *activations, *bodyActivations, *jointActivations;
   fftw_complex *bodyDFT, *jointDFT;
   fftw_plan    bodyPlan, jointPlan;
#endif
};
#endif
