if [ $# -eq 0 ]
then
./bionet -createHomomorphicNetworks -loadBehaviors CElegans_behaviors.txt -loadMorph morphx.txt \
-numGenerations 2 -behaveCutoff 50 -saveMorph morphy.txt
else
./bionet -createHomomorphicNetworks -loadBehaviors CElegans_behaviors.txt -loadMorph morphx.txt \
-numGenerations 2 -behaveCutoff 50 -saveMorph morphy.txt -randomSeed ${@}
fi

