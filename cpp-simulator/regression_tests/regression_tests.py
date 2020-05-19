import os
import filecmp

os.chdir(os.path.dirname(__file__))

default_options = {}
default_flags={}
default_options['--NUM_DAYS'] = 20
default_options['--INIT_FRAC_INFECTED'] = 0.001
default_options['--INCUBATION_PERIOD'] = 2.3
default_options['--MEAN_ASYMPTOMATIC_PERIOD'] = 0.5
default_options['--MEAN_SYMPTOMATIC_PERIOD'] =5
default_options['--SYMPTOMATIC_FRACTION'] =0.67
default_options['--MEAN_HOSPITAL_REGULAR_PERIOD'] =8
default_options['--MEAN_HOSPITAL_CRITICAL_PERIOD'] =8
default_options['--COMPLIANCE_PROBABILITY'] =0.9
default_options['--F_KERNEL_A'] =10.751
default_options['--F_KERNEL_B'] =5.384
default_options['--BETA_H'] =1.0925
default_options['--BETA_W'] =0.524166
default_options['--BETA_C'] =0.206177
default_options['--BETA_S'] =1.04833
default_options['--BETA_TRAVEL'] =0
default_options['--HD_AREA_FACTOR'] =1.0
default_options['--HD_AREA_EXPONENT'] =1.0
default_options['--INTERVENTION'] =0
default_options['--output_directory'] ="./output_files/"
default_options['--input_directory'] ="../../../markov_simuls/staticInst/data/web_input_files/bengaluru/"
default_options['--CALIBRATION_DELAY'] =1
default_options['--DAYS_BEFORE_LOCKDOWN'] =2
default_options['--FIRST_PERIOD'] =3
default_options['--SECOND_PERIOD'] =4
default_options['--THIRD_PERIOD'] =5
default_options['--OE_SECOND_PERIOD'] =6
# Set this to "--SEED_HD_AREA_POPULATION" to seed hd area population
# as well.
# SEED_HD_AREA_POPULATION="--SEED_HD_AREA_POPULATION"
default_flags['--SEED_HD_AREA_POPULATION'] = False
# Set this to "--SEED_ONLY_NON_COMMUTER" to seed only those who do not
# take public transit
# SEED_ONLY_NON_COMMUTER="--SEED_ONLY_NON_COMMUTER"
default_flags['--SEED_ONLY_NON_COMMUTER'] = False
# Set this to "--SEED_FIXED_NUMBER" to seed only a fixed number of
# people. In this case, the value of INIT_FRAC_INFECTED will be
# ignored in favour of the value of INIT_FIXED_NUMBER_INFECTED
default_flags['--SEED_FIXED_NUMBER'] = True
default_flags['--IGNORE_ATTENDANCE_FILE'] = True
#SEED_FIXED_NUMBER=
default_options['--INIT_FIXED_NUMBER_INFECTED'] =100
default_options['--LOCKED_COMMUNITY_LEAKAGE'] =1 #0.25
default_options['--PROVIDE_INITIAL_SEED'] = 1234

#print(default_options)

###############
def launch_test(params,flags):
	os.system("mkdir -p "+params['--output_directory'])
	command="time"+ " "
	command+="../drive_simulator "
	for key, value in params.items():
		command+=(" "+key+" "+ str(value))
	
	for key, value in flags.items():
		if(value):
			command+=(" "+key)

	print(command)

	os.system(command)

###################
def launch_regression(regression_tests):
	for test in regression_tests:
		print("Launching test for " + test['test_id'])
		launch_test(test['test_options'],test['test_flags'])
###################

def compare_regressions():
	for reference_directory in os.scandir('reference_files'):
		if(reference_directory.is_dir()):
			test_pass = True
			for reference_file in os.listdir(reference_directory):
				ref_file=os.path.join('reference_files',reference_directory.name,reference_file)
				test_file =os.path.join('output_files',reference_directory.name,reference_file)
				if(os.path.exists(test_file)):
					if(not filecmp.cmp(ref_file,test_file)):
						print(ref_file, test_file, "differ.")
						test_pass=False
				else:
					print(test_file, " does not exist")
					test_pass=False
			if(test_pass):
				print ("Test : ", reference_directory.name, ": PASS")
			else:
				print ("Test : ", reference_directory.name, ": FAIL")
	


regression_tests= []

###############################################
#### Start adding regression tests
# 1
current_test={}
test_id = 'test_001'
test_options = default_options.copy() #IMPORTANT to use the copy method.
test_options['--output_directory'] += test_id
test_flags = default_flags.copy()

current_test['test_id'] = test_id
current_test['test_options'] = test_options
current_test['test_flags'] = test_flags

regression_tests.append(current_test)

# 2 Test all interventions
for intervention in range(16):	
	current_test={}
	test_id = 'intervention_'+str(intervention)
	test_options = default_options.copy()
	test_options['--output_directory'] += test_id
	test_options['--INTERVENTION'] = intervention
	test_flags = default_flags.copy()

	current_test['test_id'] = test_id
	current_test['test_options'] = test_options
	current_test['test_flags'] = test_flags

	regression_tests.append(current_test)

current_test={}
test_id = 'intervention_14_enabled'
test_options = default_options.copy()
test_options['--output_directory'] += test_id
test_options['--INTERVENTION'] = 14
test_options['--WARD_CONTAINMENT_THRESHOLD'] = 0
test_flags = default_flags.copy()
test_flags['--ENABLE_CONTAINMENT'] = True

current_test['test_id'] = test_id
current_test['test_options'] = test_options
current_test['test_flags'] = test_flags

regression_tests.append(current_test)


current_test={}
test_id = 'intervention_15_enabled'
test_options = default_options.copy()
test_options['--output_directory'] += test_id
test_options['--INTERVENTION'] = 15
test_options['--WARD_CONTAINMENT_THRESHOLD'] = 0
test_flags = default_flags.copy()
test_flags['--ENABLE_CONTAINMENT'] = True

current_test['test_id'] = test_id
current_test['test_options'] = test_options
current_test['test_flags'] = test_flags

regression_tests.append(current_test)


# Launch all regresstion tests
launch_regression(regression_tests)

compare_regressions()
