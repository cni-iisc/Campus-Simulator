#!/bin/sh

echo -e "Welcome to campus simulator\n Enter the Intervention option that has been set in the simulator : \n -- 0 : No intervention \n -- 1 : Case Isolation \n -- 2 : Class Isolation \n -- 3 : Shutdown \n"
read INTERVENTION_ID

echo -e "Enter the seed infection value: \n"
read SEED_VALUE

#PARSE_PATH="/Users/Minhaas/CODING/iisc/campus-simulator/markov_simuls/staticInst"
#RUN_PATH="/Users/Minhaas/CODING/iisc/campus-simulator/markov_simuls/cpp-simulator"
#cd PARSE_PATH
#python3 campus_parse_and_instantiate.py 
#cd RUN_PATH


INTERVENTION_ARRAY=(no_intervention case_isolation class_isolation lockdown)
#Change the INTERVENTION_ARRAY index based on the name of the intervention scenario
#no_intervention - index 0 
#case_isolation - index 1
#class_isolation - index 2
#lockdown - index 3
INTERVENTION_NAME=${INTERVENTION_ARRAY[${INTERVENTION_ID}]}

SRCNAME="/Users/Minhaas/CODING/iisc/campus_simulator/markov_simuls/staticInst/data/campus_outputs"

#Folder (PRTNAME) name should be changed for every intervention - should be the same as intervention name
PRTNAME="/Users/Minhaas/CODING/iisc/campus_simulator/markov_simuls/staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME" 

num_affected="/Users/Minhaas/CODING/iisc/campus_simulator/markov_simuls/staticInst/data/campus_outputs/num_affected.csv"
num_cases="/Users/Minhaas/CODING/iisc/campus_simulator/markov_simuls/staticInst/data/campus_outputs/num_cases.csv"
num_fatalities="/Users/Minhaas/CODING/iisc/campus_simulator/markov_simuls/staticInst/data/campus_outputs/num_fatalities.csv"
num_recovered="/Users/Minhaas/CODING/iisc/campus_simulator/markov_simuls/staticInst/data/campus_outputs/num_recovered.csv"

#make clean
make -f makefile_np all

mkdir $PRTNAME

for sim in {1..10}
do 
./drive_simulator --input_directory /Users/Minhaas/CODING/iisc/campus_simulator/markov_simuls/staticInst/data/campus_data --output_directory /Users/Minhaas/CODING/iisc/campus_simulator/markov_simuls/staticInst/data/campus_outputs --SEED_FIXED_NUMBER  --INIT_FIXED_NUMBER_INFECTED $SEED_VALUE
echo 
echo "----------------------------------------$sim RUN DONE------------------------------------------"

#Folder names should be same as PRTNAME & should be for every intervention - should be the same as intervention name  
DST_NUM_AFFECTED="/Users/Minhaas/CODING/iisc/campus_simulator/markov_simuls/staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME/num_affected_$sim.csv"
DST_NUM_CASES="/Users/Minhaas/CODING/iisc/campus_simulator/markov_simuls/staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME/num_cases_$sim.csv"
DST_NUM_FATALITIES="/Users/Minhaas/CODING/iisc/campus_simulator/markov_simuls/staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME/num_fatalities_$sim.csv"
DST_NUM_RECOVERED="/Users/Minhaas/CODING/iisc/campus_simulator/markov_simuls/staticInst/data/campus_outputs/plots_data/$INTERVENTION_NAME/num_recovered_$sim.csv"

cp $num_affected $DST_NUM_AFFECTED 
cp $num_cases $DST_NUM_CASES
cp $num_fatalities $DST_NUM_FATALITIES
cp $num_recovered $DST_NUM_RECOVERED

done
