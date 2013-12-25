Bionet sinusoidal movement (locomotion) behavior.

Usage:

Run:

CElegansBehavior

Create network behavior:

CElegansBehavior
  -behaviorLength <sensory-motor sequence length>
  [-motorOutputDelay <delay for motor output> (for signal propagation)]
  [-initialPhase <initial sinusoid phase>]
  -saveBehavior <behavior file name>

Load and run network behavior:

CElegansBehavior
   -loadBehavior <behavior file name>