#!/bin/bash

echo -e "Welcome to campus simulator\n Enter the Intervention option that has been set in the simulator : \n -- 0 : No intervention \n -- 1 : Case Isolation \n -- 2 : Class Isolation \n -- 3 : Shutdown \n -- 4 : Custom Intervention \n"
read INTERVENTION_ID

echo -e "Enter the seed infection value: \n"
read SEED_VALUE

#PARSE_PATH="./staticInst"
#RUN_PATH="./cpp-simulator"
#cd PARSE_PATH
#python3 campus_parse_and_instantiate.py 
#cd RUN_PATH


INTERVENTION_ARRAY=(no_intervention case_isolation class_isolation lockdown custom_intervention)
#Change the INTERVENTION_ARRAY index based on the name of the intervention scenario
#no_intervention - index 0 
#case_isolation - index 1
#class_isolation - index 2
#lockdown - index 3
INTERVENTION_NAME=${INTERVENTION_ARRAY[${INTERVENTION_ID}]}

SRCNAME="~/campus_simulator/staticInst/data/campus_outputs"

#Folder (PRTNAME) name should be changed for every intervention - should be the same as intervention name
PRTNAME="~/campus_simulator/staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME" 

num_affected="~/campus_simulator/staticInst/data/campus_outputs/num_affected.csv"
num_cases="~/campus_simulator/staticInst/data/campus_outputs/num_cases.csv"
num_fatalities="~/campus_simulator/staticInst/data/campus_outputs/num_fatalities.csv"
num_recovered="~/campus_simulator/staticInst/data/campus_outputs/num_recovered.csv"
num_tested_positive="~/campus_simulator/staticInst/data/campus_outputs/num_tested_positive.csv"
num_tests_requested="~/campus_simulator/staticInst/data/campus_outputs/num_tests_requested.csv"
disease_label_stats="~/campus_simulator/staticInst/data/campus_outputs/disease_label_stats.csv"
#make clean
make -f makefile_np all

mkdir $PRTNAME

for sim in {1..10}
do 
./drive_simulator --input_directory ~/campus_simulator/staticInst/data/campus_data --output_directory ~/campus_simulator/staticInst/data/campus_outputs --SEED_FIXED_NUMBER  --INIT_FIXED_NUMBER_INFECTED $SEED_VALUE --ENABLE_TESTING --testing_protocol_filename testing_protocol_001.json
echo 
echo "----------------------------------------$sim RUN DONE------------------------------------------"

#Folder names should be same as PRTNAME & should be for every intervention - should be the same as intervention name  
DST_NUM_AFFECTED="~/campus_simulator/staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME/num_affected_$sim.csv"
DST_NUM_CASES="~/campus_simulator/staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME/num_cases_$sim.csv"
DST_NUM_FATALITIES="~/campus_simulator/staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME/num_fatalities_$sim.csv"
DST_NUM_RECOVERED="~/campus_simulator/staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME/num_recovered_$sim.csv"
DST_NUM_TESTED_POSITIVE="~/campus_simulator/staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME/num_tested_positive_$sim.csv"
DST_NUM_TESTS_REQUESTED="~/campus_simulator/staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME/num_tests_requested_$sim.csv"
DST_DISEASE_LABEL_STATS="~/campus_simulator/staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME/disease_label_stats_$sim.csv"


cp $num_affected $DST_NUM_AFFECTED 
cp $num_cases $DST_NUM_CASES
cp $num_fatalities $DST_NUM_FATALITIES
cp $num_recovered $DST_NUM_RECOVERED
cp $num_tested_positive $DST_NUM_TESTED_POSITIVE
cp $num_tests_requested $DST_NUM_TESTS_REQUESTED
cp $disease_label_stats $DST_DISEASE_LABEL_STATS
done
