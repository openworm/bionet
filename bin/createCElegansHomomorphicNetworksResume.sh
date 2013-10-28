if [ $# -eq 0 ]
then
./bionet -createHomomorphicNetworks -loadBehaviors CElegans_behaviors.txt -loadMorph morph.txt \
-numGenerations 1000 -behaveCutoff 50 -saveMorph morph.txt
else
./bionet -createHomomorphicNetworks -loadBehaviors CElegans_behaviors.txt -loadMorph morph.txt \
-numGenerations 1000 -behaveCutoff 50 -saveMorph morph.txt -randomSeed ${@}
fi

