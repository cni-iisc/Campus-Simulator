import os
import filecmp

os.chdir(os.path.dirname(os.path.abspath(__file__)))

default_options = {}
default_flags={}
default_options['--NUM_DAYS'] = 20
default_options['--output_directory'] ="./output_files/"
default_options['--input_directory'] ="./input_files/"
#default_options['--input_directory'] ="../../staticInst/data/campus_data"
default_flags['--SEED_FIXED_NUMBER'] = True
default_options['--INIT_FIXED_NUMBER_INFECTED'] =100
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
	result_array=[]
	f = open("regression_results.txt", "w")
	for reference_directory in sorted(os.listdir('reference_files')):
			test_pass = True
			for reference_file in os.listdir(os.path.join('reference_files',reference_directory)):
				ref_file=os.path.join('reference_files',reference_directory,reference_file)
				test_file =os.path.join('output_files',reference_directory,reference_file)
				if(os.path.exists(test_file)):
					if(not filecmp.cmp(ref_file,test_file)):
						temp= ref_file +" "+ test_file + " differ."
						f.writelines(temp + "\n")
						print(temp)
						test_pass=False
				else:
					temp= test_file +" does not exist"
					f.writelines(temp + "\n")
					print(temp)
					test_pass=False
			if(test_pass):
				temp="Test : " + reference_directory + ": PASS"
				print (temp)
				f.writelines(temp + "\n")
			else:
				temp="Test : " + reference_directory + ": FAIL"
				print (temp)
				f.writelines(temp + "\n")
	
	f.close()
	


regression_tests= []

###############################################
#### Start adding regression tests

# # 2 Test all interventions
# for intervention in range(4):	
# 	current_test={}
# 	test_id = 'intervention_'+str(intervention).zfill(2)
	
# 	test_options = default_options.copy()
# 	test_options['--output_directory'] += test_id
# 	test_options['--INTERVENTION'] = intervention
	
# 	test_flags = default_flags.copy()

# 	current_test['test_id'] = test_id
# 	current_test['test_options'] = test_options
# 	current_test['test_flags'] = test_flags

# 	regression_tests.append(current_test)

 
###########
intervention_map = {
	0 : "No Intervention",
	1 : "Case Isolation",
	2 : "Class Isolation",
	3 : "Shutdown"
}
#5 file based intervention configuration testing
for intervention in range(4):	
	current_test={}
	test_id = 'intervention_'+str(intervention).zfill(2)
	test_options = default_options.copy()
	test_options['--output_directory'] += test_id
	test_options['--intervention_filename']='intervention_'+str(intervention).zfill(2)+'.json'
	
	test_flags = default_flags.copy()

	current_test['test_id'] = test_id
	current_test['test_options'] = test_options
	current_test['test_flags'] = test_flags

	regression_tests.append(current_test)

## configure a new regression text
#6
# current_test={}
# test_id = 'smaller_networks'

# test_options = default_options.copy()
# test_options['--output_directory'] += test_id
# test_options['--INTERVENTION'] = 15
# test_options['--WARD_CONTAINMENT_THRESHOLD'] = 0
# test_options['--BETA_CLASS']=0.1
# test_options['--BETA_PROJECT']=0.1
# test_options['--BETA_RANDOM_COMMUNITY']=0.1
# test_options['--BETA_NBR_CELLS']=0.1
# test_options['--PROVIDE_INITIAL_SEED_GRAPH']=4123

# test_flags = default_flags.copy()
# test_flags['--ENABLE_CONTAINMENT'] = True
# test_flags['--ENABLE_NBR_CELLS'] = True
# test_flags['--ENABLE_TESTING'] = False


# current_test['test_id'] = test_id
# current_test['test_options'] = test_options
# current_test['test_flags'] = test_flags

# regression_tests.append(current_test)
## end of regresstion test addition

## configure a new regression text
#7
# current_test={}
# test_id = 'smaller_networks_testing_001'

# test_options = default_options.copy()
# test_options['--output_directory'] += test_id
# test_options['--INTERVENTION'] = 15
# test_options['--WARD_CONTAINMENT_THRESHOLD'] = 0
# test_options['--BETA_CLASS']=0.1
# test_options['--BETA_PROJECT']=0.1
# test_options['--BETA_RANDOM_COMMUNITY']=0.1
# test_options['--BETA_NBR_CELLS']=0.1
# test_options['--PROVIDE_INITIAL_SEED_GRAPH']=4123
# test_options['--TESTING_PROTOCOL']=2
# test_options['--testing_protocol_filename']='../../../../cpp-simulator/regression_tests/input_files/testing_protocol_001.json'


# test_flags = default_flags.copy()
# test_flags['--ENABLE_CONTAINMENT'] = True
# test_flags['--ENABLE_NBR_CELLS'] = True
# test_flags['--ENABLE_TESTING'] = True


# current_test['test_id'] = test_id
# current_test['test_options'] = test_options
# current_test['test_flags'] = test_flags

# regression_tests.append(current_test)
# ## end of regresstion test addition

# ## configure a new regression text
# #7
# current_test={}
# test_id = 'smaller_networks_testing_002'

# test_options = default_options.copy()
# test_options['--output_directory'] += test_id
# test_options['--INTERVENTION'] = 15
# test_options['--WARD_CONTAINMENT_THRESHOLD'] = 0
# test_options['--BETA_CLASS']=0.1
# test_options['--BETA_PROJECT']=0.1
# test_options['--BETA_RANDOM_COMMUNITY']=0.1
# test_options['--BETA_NBR_CELLS']=0.1
# test_options['--PROVIDE_INITIAL_SEED_GRAPH']=4123
# test_options['--TESTING_PROTOCOL']=2
# test_options['--testing_protocol_filename']='../../../../cpp-simulator/regression_tests/input_files/testing_protocol_002.json'


# test_flags = default_flags.copy()
# test_flags['--ENABLE_CONTAINMENT'] = True
# test_flags['--ENABLE_NBR_CELLS'] = True
# test_flags['--ENABLE_TESTING'] = True


# current_test['test_id'] = test_id
# current_test['test_options'] = test_options
# current_test['test_flags'] = test_flags

# regression_tests.append(current_test)
# ## end of regresstion test addition



# ## configure a new regression test
# # attendance file based test
# current_test={}
# test_id = 'attendance_file_001'

# test_options = default_options.copy()
# test_options['--output_directory'] += test_id
# test_options['--INTERVENTION'] = 8
# test_options['--LOCKED_COMMUNITY_LEAKAGE'] = 0.25
# test_options['--attendance_filename']='../../../../cpp-simulator/regression_tests/input_files/attendance_file_001.json'

# test_flags = default_flags.copy()
# test_flags['--ENABLE_CONTAINMENT'] = True
# test_flags['--IGNORE_ATTENDANCE_FILE'] = False

# current_test['test_id'] = test_id
# current_test['test_options'] = test_options
# current_test['test_flags'] = test_flags

# regression_tests.append(current_test)

# ## configure a new regression test
# # attendance file based test
# current_test={}
# test_id = 'attendance_file_002'

# test_options = default_options.copy()
# test_options['--output_directory'] += test_id
# test_options['--INTERVENTION'] = 8
# test_options['--LOCKED_COMMUNITY_LEAKAGE'] = 0.25
# test_options['--attendance_filename']='../../../../cpp-simulator/regression_tests/input_files/attendance_file_002.json'

# test_flags = default_flags.copy()
# test_flags['--ENABLE_CONTAINMENT'] = True
# test_flags['--IGNORE_ATTENDANCE_FILE'] = False

# current_test['test_id'] = test_id
# current_test['test_options'] = test_options
# current_test['test_flags'] = test_flags

# regression_tests.append(current_test)

# ## end of regresstion test addition


#remove old output files
os.system('rm -rf ./output_files/')

# Launch all regresstion tests
launch_regression(regression_tests)

compare_regressions()
