#!/bin/bash

echo
echo "======== cd to CONDOR_DIR_INPUT ========"
cd $CONDOR_DIR_INPUT

echo
echo "======== ls ========"
ls

echo
echo "======== UNTARRING... ========"
tar xvfz local_install_dark_tridents.tar.gz -C ./ > /dev/null

echo
echo "======== Done untarring. ls ========"
ls

echo "======== SETUP G4, ROOT, ETC ========"
source setup_evgen_grid.sh

echo
echo "======== UPDATE MACRO WITH RUN NUMBER ========"
SEED=$((RUN+PROCESS))
sed -i 's/\${seed}/'$SEED'/g' parameter_uboone_grid.dat
OUTFILE="events_${MA}_${SIG}_${PROCESS}_cut.dat"
SUMFILE="summary_${MA}_${SIG}_${PROCESS}_cut.dat"
sed -i 's/\${outFile}/'$OUTFILE'/g' parameter_uboone_grid.dat
sed -i 's/\${sumFile}/'$SUMFILE'/g' parameter_uboone_grid.dat

echo "PROCESS=$PROCESS"
echo "SEED=$SEED"
echo "OUTFILE=$OUTFILE"
echo "SUMFILE=$SUMFILE"

echo
echo "======== PARAMETER FILE CONTENT ========"
cat parameter_uboone_grid.dat

echo
echo "======== EXECUTING BdNMC ========"
echo "./BdNMC/bin/BDNMC parameter_uboone_grid.dat"
./BdNMC/bin/BDNMC parameter_uboone_grid.dat


echo
echo "======== EXECUTING BdNMC ========"
echo "./evgen_anyssa.exe -i $OUTFILE -x events -o events_uboone_0.05_test.root -h hepevt_uboone_0.05_test.txt"
./evgen_anyssa.exe -i BdNMC/$OUTFILE -x events -m ${MA} -o events_${MA}_${SIG}_${PROCESS}_cut.root -h hepevt_${MA}_${SIG}_${PROCESS}_cut.txt

echo
echo "Moving output to CONDOR_DIR_DARKTRIDENT"

mv BdNMC/events_*.dat $CONDOR_DIR_DARKTRIDENT
mv BdNMC/summary_*.dat $CONDOR_DIR_DARKTRIDENT

mv *.root $CONDOR_DIR_DARKTRIDENT
mv *.txt $CONDOR_DIR_DARKTRIDENT

