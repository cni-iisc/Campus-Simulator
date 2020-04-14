#Copyright [2020] [Indian Institute of Science, Bangalore]
#SPDX-License-Identifier: Apache-2.0

#from calculate_means_CPP import calculate_means
from calculate_r0 import calculate_r0
from calibrate import calibrate
from joblib import Parallel, delayed
import os
import numpy as np 
import pandas as pd

NUM_DAYS=120
INIT_FRAC_INFECTED=0.001
INCUBATION_PERIOD=2.25
MEAN_ASYMPTOMATIC_PERIOD=0.5
MEAN_SYMPTOMATIC_PERIOD=5
SYMPTOMATIC_FRACTION=0.67
MEAN_HOSPITAL_REGULAR_PERIOD=8
MEAN_HOSPITAL_CRITICAL_PERIOD=8
COMPLIANCE_PROBABILITY=0.9
F_KERNEL_A= 10.751
F_KERNEL_B= 5.384
BETA_H=1.2410293733942703
BETA_W=0.9289438506612563
BETA_C=0.23195981755789088
BETA_S=1.8387669724845188
BETA_TRAVEL=0
HD_AREA_FACTOR=2.0
HD_AREA_EXPONENT=0
INTERVENTION=0
output_directory_base="/home/nidhin/temp/CovidSim_Temp/CPP_Calib"
input_directory="/home/nidhin/temp/CovidSim_Temp/all-3-cities-instantiation/bangalore-1M"
CALIBRATION_DELAY=0
DAYS_BEFORE_LOCKDOWN=0
# Set this to "--SEED_HD_AREA_POPULATION" to seed hd area population
# as well.
# SEED_HD_AREA_POPULATION="--SEED_HD_AREA_POPULATION"
SEED_HD_AREA_POPULATION=" "
# Set this to "--SEED_ONLY_NON_COMMUTER" to seed only those who do not
# take public transit
# SEED_ONLY_NON_COMMUTER="--SEED_ONLY_NON_COMMUTER"
SEED_ONLY_NON_COMMUTER=" "
# Set this to "--SEED_FIXED_NUMBER" to seed only a fixed number of
# people. In this case, the value of INIT_FRAC_INFECTED will be
# ignored in favour of the value of INIT_FIXED_NUMBER_INFECTED
#SEED_FIXED_NUMBER="--SEED_FIXED_NUMBER"
SEED_FIXED_NUMBER=" "
INIT_FIXED_NUMBER_INFECTED=100
INTERVENTION=0
EXEC_DIR = "/home/nidhin/CPP_Simulator/markov_simuls/cpp-simulator"
######################
def calculate_means_fatalities_CPP(output_directory_base, num_sims,results_dir):
    column_names = ['timestep','dead']
    master_df = pd.DataFrame(columns=column_names)
    for sim_count in range(num_sims):
        output_directory=output_directory_base+"/intervention_"+ str(INTERVENTION)+"_"+str(sim_count)
        print(output_directory)
        df_temp = pd.read_csv(output_directory+"/num_fatalities.csv")
        df_temp.columns = column_names
        master_df = master_df.append(df_temp)
        print(master_df.size)
    master_df[column_names] = master_df[column_names].apply(pd.to_numeric)
    df_mean = master_df.groupby(['timestep']).mean()
    df_mean.to_csv(results_dir+'dead_mean.csv')
    print(df_mean)

def calculate_means_lambda_CPP(output_directory_base, num_sims,results_dir):
    column_names_lambda_H = ['timestep','lambda H']
    column_names_lambda_W = ['timestep','lambda W']
    column_names_lambda_C = ['timestep','lambda C']
    column_names = [column_names_lambda_H, column_names_lambda_W, column_names_lambda_C]
    lambda_array=['lambda_H','lambda_W','lambda_C']
    for lambda_count,lambda_ in enumerate(lambda_array):
        master_df = pd.DataFrame(columns=column_names[lambda_count])
        val = column_names[lambda_count][1]
        for sim_count in range(num_sims):
            output_directory=output_directory_base+"/""intervention_"+ str(INTERVENTION)+"_"+str(sim_count)
            df_temp = pd.read_csv(output_directory+"/cumulative_mean_fraction_"+lambda_+".csv")
            df_temp.columns = column_names[lambda_count]
            master_df = master_df.append(df_temp)
        master_df[column_names[lambda_count]] = master_df[column_names[lambda_count]].apply(pd.to_numeric)
        df_mean = master_df.groupby(['timestep']).mean()
        df_mean.to_csv(results_dir+val+'_mean.csv')

def run_sim(num_sims_count, params):
    print("Internal loop. Loop count = ", num_sims_count)
    output_directory=params['outputDirectoryBase']+"/""intervention_"+ str(params['intervention'])+"_"+str(num_sims_count)

    os.system("mkdir -p "+output_directory)

    command="time"+ " "
    command+=params['execDir']+"/drive_simulator"+ " "
    command+=params['seedHDAreaPopulation'] + " " 
    command+=params['seedOnlyNonCommuter'] + " "
    command+=params['seedFixedNumber']
    command+=" --NUM_DAYS "+ str(params['numDays'])
    command+=" --INIT_FRAC_INFECTED " + str(params['initFracInfected'])
    command+=" --INIT_FIXED_NUMBER_INFECTED "+ str(params['initFixedNumberInfected'])
    command+=" --INCUBATION_PERIOD " +  str(params['incubationPeriod'])
    command+=" --MEAN_ASYMPTOMATIC_PERIOD " +  str(params['MeanAsymptomaticPeriod']) 
    command+=" --MEAN_SYMPTOMATIC_PERIOD " + str(params['MeanSymptomaticPeriod']) 
    command+=" --SYMPTOMATIC_FRACTION " +  str(params['symptomaticFraction']) 
    command+=" --MEAN_HOSPITAL_REGULAR_PERIOD " + str(params['meanHospitalRegularPeriod'])
    command+=" --MEAN_HOSPITAL_CRITICAL_PERIOD " + str(params['meanHospitalCriticalPeriod'])
    command+=" --COMPLIANCE_PROBABILITY " + str(params['complianceProbability'])
    command+=" --F_KERNEL_A " + str(params['FKernelA'])
    command+=" --F_KERNEL_B " + str(params['FKernelB'])
    command+=" --BETA_H " + str(params['betaH'])
    command+=" --BETA_W " + str(params['betaW'])
    command+=" --BETA_C " + str(params['betaC'])
    command+=" --BETA_S " + str(params['betaS'])
    command+=" --BETA_TRAVEL " + str(params['betaTravel'])
    command+=" --HD_AREA_FACTOR " + str(params['hdAreaFactor'])
    command+=" --HD_AREA_EXPONENT " + str(params['hdAreaExponent'])
    command+=" --INTERVENTION " + str(params['intervention'])
    command+=" --output_directory " + str(output_directory)
    command+=" --input_directory " + str(params['inputDirectory'])
    command+=" --CALIBRATION_DELAY " + str(params['calibrationDelay'])
    command+=" --DAYS_BEFORE_LOCKDOWN " + str(params['daysBeforeLockdown'])

    print(command)

    os.system(command)

    return (True)

###########################
continue_run = True
resolution = 4
num_sims = 10
count = 0
num_cores = 2

while (continue_run):
   
    params = { 'execDir': EXEC_DIR,'seedHDAreaPopulation': SEED_HD_AREA_POPULATION, 'seedOnlyNonCommuter': SEED_ONLY_NON_COMMUTER,
               'seedFixedNumber':SEED_FIXED_NUMBER, 'seedFixedNumber':SEED_FIXED_NUMBER, 'numDays': NUM_DAYS, 
               'initFracInfected': INIT_FRAC_INFECTED, 'initFixedNumberInfected': INIT_FIXED_NUMBER_INFECTED, 'incubationPeriod': INCUBATION_PERIOD, 
               'MeanAsymptomaticPeriod': MEAN_ASYMPTOMATIC_PERIOD, 'MeanSymptomaticPeriod': MEAN_SYMPTOMATIC_PERIOD, 'symptomaticFraction': SYMPTOMATIC_FRACTION, 
               'meanHospitalRegularPeriod': MEAN_HOSPITAL_REGULAR_PERIOD, 'meanHospitalCriticalPeriod': MEAN_HOSPITAL_CRITICAL_PERIOD, 
               'complianceProbability': COMPLIANCE_PROBABILITY, 'FKernelA': F_KERNEL_A, 'FKernelB': F_KERNEL_B, 
               'betaH': BETA_H, 'betaW': BETA_W, 'betaC': BETA_C, 'betaS': BETA_S, 'betaTravel': BETA_TRAVEL,
               'hdAreaFactor':HD_AREA_FACTOR, 'hdAreaExponent':HD_AREA_EXPONENT, 'intervention': INTERVENTION, 
               'outputDirectoryBase': output_directory_base, 'inputDirectory': input_directory,
               'calibrationDelay': CALIBRATION_DELAY, 'daysBeforeLockdown': DAYS_BEFORE_LOCKDOWN }
    
    print ('Parameters: ', params)    

    processed_list = Parallel(n_jobs=num_cores)(delayed(run_sim)(simNum, params) for simNum in range(num_sims))     
    
     
    '''
    for num_sims_count in range(num_sims):
        print("Internal loop. Loop count = ", num_sims_count)
        output_directory=output_directory_base+"/""intervention_"+ str(INTERVENTION)+"_"+str(num_sims_count)

        os.system("mkdir -p "+output_directory)

        command="time"+ " "
        command+=EXEC_DIR+"/drive_simulator"+ " "
        command+=SEED_HD_AREA_POPULATION + " " 
        command+=SEED_ONLY_NON_COMMUTER + " "
        command+=SEED_FIXED_NUMBER
        command+=" --NUM_DAYS "+ str(NUM_DAYS)
        command+=" --INIT_FRAC_INFECTED " + str(INIT_FRAC_INFECTED)
        command+=" --INIT_FIXED_NUMBER_INFECTED "+ str(INIT_FIXED_NUMBER_INFECTED)
        command+=" --INCUBATION_PERIOD " +  str(INCUBATION_PERIOD)
        command+=" --MEAN_ASYMPTOMATIC_PERIOD " +  str(MEAN_ASYMPTOMATIC_PERIOD) 
        command+=" --MEAN_SYMPTOMATIC_PERIOD " + str(MEAN_SYMPTOMATIC_PERIOD) 
        command+=" --SYMPTOMATIC_FRACTION " +  str(SYMPTOMATIC_FRACTION) 
        command+=" --MEAN_HOSPITAL_REGULAR_PERIOD " + str(MEAN_HOSPITAL_REGULAR_PERIOD)
        command+=" --MEAN_HOSPITAL_CRITICAL_PERIOD " + str(MEAN_HOSPITAL_CRITICAL_PERIOD)
        command+=" --COMPLIANCE_PROBABILITY " + str(COMPLIANCE_PROBABILITY)
        command+=" --F_KERNEL_A " + str(F_KERNEL_A)
        command+=" --F_KERNEL_B " + str(F_KERNEL_B)
        command+=" --BETA_H " + str(BETA_H)
        command+=" --BETA_W " + str(BETA_W)
        command+=" --BETA_C " + str(BETA_C)
        command+=" --BETA_S " + str(BETA_S)
        command+=" --BETA_TRAVEL " + str(BETA_TRAVEL)
        command+=" --HD_AREA_FACTOR " + str(HD_AREA_FACTOR)
        command+=" --HD_AREA_EXPONENT " + str(HD_AREA_EXPONENT)
        command+=" --INTERVENTION " + str(INTERVENTION)
        command+=" --output_directory " + str(output_directory)
        command+=" --input_directory " + str(input_directory)
        command+=" --CALIBRATION_DELAY " + str(CALIBRATION_DELAY)
        command+=" --DAYS_BEFORE_LOCKDOWN " + str(DAYS_BEFORE_LOCKDOWN)


        print(command)

        os.system(command)
    '''

    ##############################################################
    calculate_means_fatalities_CPP(output_directory_base, num_sims,"./data/")
    calculate_means_lambda_CPP(output_directory_base, num_sims,"./data/") 
    
    [flag, BETA_SCALE_FACTOR, step_beta_h, step_beta_w, step_beta_c, delay] = calibrate(resolution,count)
    count+=1    
    if flag == True:
        continue_run = False
    else:
        BETA_H = max(BETA_H + step_beta_h,0)*BETA_SCALE_FACTOR
        BETA_W = max(BETA_W + step_beta_w,0)*BETA_SCALE_FACTOR
        BETA_S = BETA_W * 2
        BETA_C = max(BETA_C + step_beta_c,0)*BETA_SCALE_FACTOR
        #INIT_FRAC_SCALE_FACTOR = INIT_FRAC_SCALE_FACTOR*init_frac_mult_factor
        print ("count:", count, '. BETA_H: ', BETA_H, '. BETA_W: ',BETA_W, '. BETA_S: ', BETA_S, '. BETA_C: ', BETA_C,'. Delay: ',delay )
    #continue_run = False
