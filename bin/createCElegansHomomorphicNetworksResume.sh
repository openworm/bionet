if [ $# -eq 0 ]
then
./bionet -createHomomorphicNetworks -loadBehaviors CElegans_behaviors.txt -loadMorph morph.txt \
-numGenerations 1000 -behaveCutoff 50 -fitnessMotorList 1,2,3,4 \
-saveMorph morph.txt
else
./bionet -createHomomorphicNetworks -loadBehaviors CElegans_behaviors.txt -loadMorph morph.txt \
-numGenerations 1000 -behaveCutoff 50 -fitnessMotorList 1,2,3,4 \
-saveMorph morph.txt -randomSeed ${@}
fi

