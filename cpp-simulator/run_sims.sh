#!/bin/bash

echo -e "Welcome to campus simulator\n Enter the Intervention option that has been set in the simulator : \n -- 0 : No intervention \n -- 1 : Case Isolation \n -- 2 : Class Isolation \n -- 3 : Shutdown \n -- 4 : Selective Shutdown \n --5 : Evacuation \n --  6: Custom Intervention \n"
read INTERVENTION_ID

echo -e "Enter the seed infection value: \n"
read SEED_VALUE

#echo -e "Enter number of runs of simulator: \n"
#read NUM_RUNS

#PARSE_PATH="./staticInst"
#RUN_PATH="./cpp-simulator"
#cd PARSE_PATH
#python3 campus_parse_and_instantiate.py 
#cd RUN_PATH


INTERVENTION_ARRAY=(no_intervention case_isolation class_isolation lockdown selective_shutdown evacuation custom_intervention)
#Change the INTERVENTION_ARRAY index based on the name of the intervention scenario
#no_intervention - index 0 
#case_isolation - index 1
#class_isolation - index 2
#lockdown - index 3
INTERVENTION_NAME=${INTERVENTION_ARRAY[${INTERVENTION_ID}]}

SRCNAME="../staticInst/data/campus_outputs"

#Folder (PRTNAME) name should be changed for every intervention - should be the same as intervention name
PRTNAME="../staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME" 

num_affected="../staticInst/data/campus_outputs/num_affected.csv"
num_cases="../staticInst/data/campus_outputs/num_cases.csv"
num_fatalities="../staticInst/data/campus_outputs/num_fatalities.csv"
num_recovered="../staticInst/data/campus_outputs/num_recovered.csv"
disease_label_stats="../staticInst/data/campus_outputs/disease_label_stats.csv"

#make clean
make -f Makefile_np all

mkdir $PRTNAME

for sim in {1..10}
do 
./drive_simulator --input_directory ../staticInst/data/campus_data --output_directory ../staticInst/data/campus_outputs --SEED_FIXED_NUMBER  --INIT_FIXED_NUMBER_INFECTED 100 --NUM_DAYS 120 --ENABLE_TESTING --testing_protocol_filename testing_protocol_001.json 
echo 
echo "----------------------------------------$sim RUN DONE------------------------------------------"

#Folder names should be same as PRTNAME & should be for every intervention - should be the same as intervention name  
DST_NUM_AFFECTED="../staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME/num_affected_$sim.csv"
DST_NUM_CASES="../staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME/num_cases_$sim.csv"
DST_NUM_FATALITIES="../staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME/num_fatalities_$sim.csv"
DST_NUM_RECOVERED="../staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME/num_recovered_$sim.csv"
DST_DISEASE_LABEL_STATS="../staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME/disease_label_stats_$sim.csv"


cp $num_affected $DST_NUM_AFFECTED 
cp $num_cases $DST_NUM_CASES
cp $num_fatalities $DST_NUM_FATALITIES
cp $num_recovered $DST_NUM_RECOVERED$DST_NUM_TESTED_POSITIVE
cp $disease_label_stats $DST_DISEASE_LABEL_STATS
done
