Create C Elegans bionet sinusoidal movement (wriggling) behavior.

Usage:

Run:

CElegansBody

Create network behavior:

CElegansBody
  -behaviorLength <sensory-motor sequence length>
  [-motorOutputDelay <delay for motor output> (for signal propagation)]
  [-initialPhase <initial sinusoid phase>]
  -saveBehavior <behavior file name>

Load and run network behavior:

CElegansBody
   -loadBehavior <behavior file name>