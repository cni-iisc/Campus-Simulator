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
INTERVENTION=0
output_directory="outputs/test_output"

command="time ./drive_simulator.exe \
	 $NUM_DAYS \
	 $INIT_FRAC_INFECTED \
	 $INCUBATION_PERIOD \
	 $MEAN_ASYMPTOMATIC_PERIOD \
	 $MEAN_SYMPTOMATIC_PERIOD \
	 $SYMPTOMATIC_FRACTION \
	 $MEAN_HOSPITAL_REGULAR_PERIOD \
	 $MEAN_HOSPITAL_CRITICAL_PERIOD \
	 $COMPLIANCE_PROBABILITY \
	 $BETA_H \
	 $BETA_W \
	 $BETA_C \
	 $BETA_S \
	 $INTERVENTION \
	 $output_directory"

echo $command;
if eval $command; then
	echo "Simulation over."
	cd $output_directory

	command="${GPLOT_PATH}gnuplot gnuplot_script.gnuplot"
	echo $command;
	if eval $command; then
		echo "Plots generated successfully."
	fi;
else
	echo "error: see above for output"
	exit 1;
fi;
