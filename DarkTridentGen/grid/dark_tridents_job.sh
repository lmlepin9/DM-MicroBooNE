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
OUTFILE="events_${MA}_${DM_TYPE}_${PROCESS}_cut.dat"
OUTROOT="events_${MA}_${DM_TYPE}_${PROCESS}_cut.root"
SUMFILE="summary_${MA}_${DM_TYPE}_${PROCESS}_cut.dat"
sed -i 's/\${seed}/'$SEED'/g' parameter_uboone_grid.dat
sed -i 's/\${decay_type}/'$DM_TYPE'/g' parameter_uboone_grid.dat
sed -i 's/\${outFile}/'$OUTFILE'/g' parameter_uboone_grid.dat
sed -i 's/\${sumFile}/'$SUMFILE'/g' parameter_uboone_grid.dat
sed -i 's/\${outroot}/'$OUTROOT'/g' parameter_uboone_grid.dat 

echo "PROCESS=$PROCESS"
echo "SEED=$SEED"
echo "OUTFILE=$OUTFILE"
echo "SUMFILE=$SUMFILE"
echo "ROOTFILE=$OUTROOT"

echo
echo "======== PARAMETER FILE CONTENT ========"
cat parameter_uboone_grid.dat

echo
echo "======== EXECUTING BdNMC ========"
echo "./BdNMC/bin/BDNMC parameter_uboone_grid.dat"
./BdNMC/bin/BDNMC parameter_uboone_grid.dat

echo "======= Listing files ========"

ls 

echo
#echo "======== SKIP evgen for now ========="
echo "======== EXECUTING GenExLight ========"
#echo "./evgen_anyssa.exe -i $OUTFILE -x events -o events_uboone_0.05_test.root -h hepevt_uboone_0.05_test.txt"
#./evgen_anyssa.exe -i BdNMC/$OUTFILE -x events -m ${MA} -o events_${MA}_${SIG}_${PROCESS}_cut.root -h hepevt_${MA}_${SIG}_${PROCESS}_cut.txt
./evgen.exe -i events_${MA}_${DM_TYPE}_${PROCESS}_cut.root -x ./xsec/cross_section_${MA}_${DM_TYPE}.root -m root -t ${DM_TYPE} -o hepevt_${MA}_${DM_TYPE}_${PROCESS}

echo
echo "Moving output to CONDOR_DIR_DARKTRIDENT"

mv BdNMC/*.dat $CONDOR_DIR_DARKTRIDENT
mv BdNMC/*.root $CONDOR_DIR_DARKTRIDENT

mv *.root $CONDOR_DIR_DARKTRIDENT
mv *.txt $CONDOR_DIR_DARKTRIDENT

