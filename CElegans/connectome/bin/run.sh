if [ "$(expr substr $(uname -s) 1 6)" == "CYGWIN" ]
then
    java -classpath "./CElegansBionet.jar;../lib/jxl.jar" bionet.CElegansBionet "${@}"
else 
    java -classpath "./CElegansBionet.jar:../lib/jxl.jar" bionet.CElegansBionet "${@}"
fi


