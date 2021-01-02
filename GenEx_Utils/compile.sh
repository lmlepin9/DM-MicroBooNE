#Script to compile GenExLight 

g++ evgen.cxx -I$(root-config --incdir) --std=c++14 -I ./TDecayTFoam $(root-config --libs) -lFoam -lEG ./TDecayTFoam/TDecay.o -o evgen.exe
