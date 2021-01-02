#!/bin/bash 
# This script will run .dat files through evgen.exe from 50 to 99 

for i in {50..99}
do 
  echo "OPENING file NUMBER:" $i
  echo 
  ./evgen.exe -i ./grid_files/events_0.05_10k_$i.dat -x ./events/events -n $i 
  echo "DELETING CACHE CONTENT"
  rm ./cache/* 
done 
