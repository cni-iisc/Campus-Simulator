NUM_DAYS=120
INIT_FRAC_INFECTED=0.0001
INCUBATION_PERIOD=2.25
MEAN_ASYMPTOMATIC_PERIOD=0.5
MEAN_SYMPTOMATIC_PERIOD=5
SYMPTOMATIC_FRACTION=0.67
MEAN_HOSPITAL_REGULAR_PERIOD=8
MEAN_HOSPITAL_CRITICAL_PERIOD=8
COMPLIANCE_PROBABILITY=0.9
BETA_H=0.47
BETA_W=0.94
BETA_C=0.47045
BETA_S=1.88
BETA_TRAVEL=10.0
INTERVENTION=0
output_directory_base="outputs/test_output"

for INTERVENTION in `seq 0 7`;
do
	echo "Running with INTERVENTION=${INTERVENTION}..."
	output_directory="${output_directory_base}/intervention_${INTERVENTION}"
	if [ ! -d "${output_directory}" ];
	then
		mkdir -p "${output_directory}";
	fi;
	echo "Output will be genrerated for this intervention in ${output_directory}."
	command="time ./drive_simulator \
	 $NUM_DAYS \ $INIT_FRAC_INFECTED \ $INCUBATION_PERIOD \ $MEAN_ASYMPTOMATIC_PERIOD \
	 $MEAN_SYMPTOMATIC_PERIOD \
	 $SYMPTOMATIC_FRACTION \
	 $MEAN_HOSPITAL_REGULAR_PERIOD \
	 $MEAN_HOSPITAL_CRITICAL_PERIOD \
	 $COMPLIANCE_PROBABILITY \
	 $BETA_H \
	 $BETA_W \
	 $BETA_C \
	 $BETA_S \
     $BETA_TRAVEL \
	 $INTERVENTION \
	 $output_directory"

	echo $command;
	if eval $command; then
		echo "Simulation over for INTERVENTION=${INTERVENTION}."
		cur=`pwd`;
		cd $output_directory

		command="${GPLOT_PATH}gnuplot gnuplot_script.gnuplot"
		echo $command;
		if eval $command; then
			echo "Plots generated successfully for INTERVENTION=${INTERVENTION}."
		fi;
		cd "$cur";
	else
		echo "error with INTERVENTION=${INTERVENTION}: see above for output"
	fi;
	echo "Done with INTERVENTION=${INTERVENTION}"
	echo "*****************"
	echo "*****************"
done;

echo "Now plotting consolidated plots for all interventions"
python plot_all_interventions.py "${output_directory_base}"

