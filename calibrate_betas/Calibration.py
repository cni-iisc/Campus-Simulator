#!/usr/bin/env python
# coding: utf-8

# In[ ]:


from pathlib import Path
import math
import pandas as pd
import numpy as np
import os
import subprocess
import joblib
from collections import defaultdict
import functools
import datetime
import logging
from functools import wraps
from time import time
import argparse
import sys
import json

sys.path.append('../staticInst')
from transmission_coefficients import transmission_coefficients

class NpEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, np.integer):
            return int(obj)
        elif isinstance(obj, np.floating):
            return float(obj)
        elif isinstance(obj, np.ndarray):
            return obj.tolist()
        else:
            return super(NpEncoder, self).default(obj)
DEBUG=False

def measure(func):
    @wraps(func)
    def _time_it(*args, **kwargs):
        start = time()
        try:
            return func(*args, **kwargs)
        finally:
            end = int((time() - start)*1000)
            print(f"Finished in  {end} ms")
    return _time_it


# In[ ]:

# Some global variables that will be used
smaller_networks_scale = 9.0
params = {}
betas = {}
logfile = None

def processParams(params_json):
    global params
    global betas
    global smaller_networks_scale
    betas['outside_campus'] = 0
    with open(params_json) as f:
        tmp_params = json.load(f)
        for k in ['classroom', 'hostel', 'residence_block']:
            betas[k] = tmp_params['betas'][k]
        del tmp_params['betas']
        params = tmp_params

        #Now to process the betas
        # betas['S'] = 2 * betas['W']
        # betas['PROJECT'] = betas['W'] * smaller_networks_scale
        # betas['CLASS'] = betas['S'] * smaller_networks_scale
        # betas['NBR_CELLS'] = betas['C'] * smaller_networks_scale
        # betas['RANDOM_COMMUNITY'] = betas['NBR_CELLS']
        betas['mess'] = betas['hostel']*0.33
        betas['cafeteria'] = betas['classroom']*0.2
        betas['library'] = betas['classroom']*0.2
        betas['sports_facility'] = betas['classroom']*0.29
        betas['recreational_facility'] = betas['classroom']*0.2
        betas['smaller_networks'] = betas['hostel']*smaller_networks_scale
        betas['house'] = betas['residence_block']*smaller_networks_scale


def get_mean_cases(outputdir, nruns):
    data_dir = Path(outputdir)
    glob_str = f"run_[0-{nruns-1}]/disease_label_stats.csv"
    files_list = data_dir.glob(glob_str)
    #print(files_list)
    df = (pd.concat([pd.read_csv(f) for f in files_list], ignore_index = True)
          .groupby('Time').mean())
    return df

def get_mean_lambdas(outputdir, nruns):
    data_dir = Path(outputdir)
    lambdas = {'classroom': ["lambda_classroom","lambda_cafeteria","lambda_library","lambda_sports_facility","lambda_recreational_facility"], 
               'hostel': ["lambda_hostel", "lambda_mess","lambda_smaller_networks"], 
               #'mess': ["lambda_mess"],
               #'cafeteria': ["lambda_cafeteria"],
               #'library': ["lambda_library"],
               #'sports_facility': ["lambda_sports_facility"],
               #'recreational_facility': ["lambda_recreational_facility"],
               'residence_block': ["lambda_residential_block", "lambda_house"]
              }
    values = {}
    for lam in lambdas.keys():
        lam_sum = 0
        for lam_inner in lambdas[lam]:
            glob_str = f"run_[0-{nruns-1}]/cumulative_mean_fraction_{lam_inner}.csv"
            files_list = data_dir.glob(glob_str)
            #print(files_list)
            df = (pd.concat([pd.read_csv(f) for f in files_list], ignore_index = True)
              .groupby('Time').mean())
            df.to_csv("a.csv")
            lam_sum += df[f"cumulative_mean_fraction_{lam_inner}"].iloc[-1]
        values[lam] = lam_sum
        #print(values)
    return values

def print_and_log(outstring, filename=logfile):
    print(outstring)
    with open(filename, "a+") as f:
        f.write(outstring)
        f.write("\n")

def print_betas():
    print("")
    print_and_log(f"BETA_classroom                     : {betas['classroom']:.5f}", logfile)
    print_and_log(f"BETA_hostel                        : {betas['hostel']:.5f}", logfile)
    #print_and_log(f"BETA_mess                          : {betas['mess']:.5f}", logfile)
    #print_and_log(f"BETA_cafeteria                     : {betas['cafeteria']:.5f}", logfile)
    #print_and_log(f"BETA_library                       : {betas['library']:.5f}", logfile)
    #print_and_log(f"BETA_sports_facility               : {betas['sports_facility']:.5f}", logfile)
    #print_and_log(f"BETA_recreational_facility         : {betas['recreational_facility']:.5f}", logfile)
    print_and_log(f"BETA_residence_block             : {betas['residence_block']:.5f}", logfile)
    print("")


# In[ ]:


def run_sim(run, params, betas):

    output_folder = Path(output_base, f"run_{run}")
    output_folder.mkdir(parents = True, exist_ok = True)
    cmd = [f"{cpp_exec}"]
    for param in params.keys():
        if params[param]!= False:
            if params[param]==True:
                cmd+= [f"--{param}"]
            else:
                cmd+= [f"--{param}", f"{params[param]}"]
    # for b in betas.keys():
    #     cmd+= [f"--BETA_{b}",f"{betas[b]}"]
    cmd += [f"--input_directory", f"{input_folder}"]
    cmd += [f"--output_directory", f"{output_folder}"]
    print(" ".join(cmd))
    logging.info(" ".join(cmd))
    if DEBUG: 
        subprocess.call(cmd)
    else:
        ## Suppress other output
        subprocess.call(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)        


# In[ ]:


#target_slope = 0.1803300052477795

def getTargetSlope():
    #print("in function")
    ## Don't need to run this every time.
    ## So putting it in a function that is never called
    global target_slope
    
    ts_df = pd.read_csv('data/iiithcases.csv')
    #print(ECDP)
    ts_df = ts_df[['Date','Positive']]
    ts_df['cumulative_cases'] = ts_df['Positive'].cumsum()
    #india_data = ECDP[ECDP['geoId']=='IN'][['dateRep', 'cases']][::-1]
    ts_df = ts_df[(ts_df['cumulative_cases'] > 0) 
                           & (ts_df['cumulative_cases'] < 30)].reset_index(drop=True)
    #print(india_data)
    target_slope = np.polyfit(ts_df.index, np.log(ts_df['cumulative_cases']), deg = 1)[0]
    #print(type(target_slope))

# In[ ]:


def get_slope(outputdir, nruns, low_thresh = 0, up_thresh = 30):
    df = get_mean_cases(outputdir, nruns)
    df = df[(df['cumulative_positive_cases'] > low_thresh)]
    #print(df.shape[0])
    #print(df.shape)
    if df.shape[0] < 5:
        #print("type error")
        raise TypeError("Too few cases")
    else:
        df = df[df['cumulative_positive_cases'] < up_thresh]
        #print(np.polyfit(df.index, np.log(df['num_affected']), deg = 1)[0])
        return np.polyfit(df.index, np.log(df['cumulative_positive_cases']), deg = 1)[0]


# In[ ]:


def update_betas(diffs, count=0):
    global smaller_networks_scale
    
    if diffs == -1: #Happens if the getslope fails due to too few fatalities
        print_betas()
        print("Too few cases. Doubling betas")
        betas['classroom'] *= 2
        betas['hostel'] *= 2
        #betas['mess'] *= 2
        #betas['cafeteria'] *= 2
        # betas['library'] *= 2
        # betas['sports_facility'] *= 2
        # betas['recreational_facility'] *= 2
        betas['residence_block'] *= 2    
    else:
        (lambda_classroom_diff, lambda_hostel_diff, lambda_residence_block_diff, slope_diff) = diffs
        step_beta_classroom = -1*lambda_classroom_diff/(10+count) 
        step_beta_hostel = -1*lambda_hostel_diff/(10+count) 
        # step_beta_mess = -1*lambda_mess_diff/(5+count)
        # step_beta_cafeteria = -1*lambda_cafeteria_diff/(20+count) 
        # step_beta_library = -1*lambda_library_diff/(20+count) 
        # step_beta_sports_facility = -1*lambda_sports_facility_diff/(20+count)
        # step_beta_recreational_facility = -1*lambda_recreational_facility_diff/(20+count) 
        step_beta_residence_block = -1*lambda_residence_block_diff/(10+count)  
        beta_scale_factor = max(min(np.exp(slope_diff),1.5), 0.66)

        if (count>=30):
            beta_scale_factor = max(min(np.exp(slope_diff/(count-25)),1.5), 0.66)
        elif (abs(lambda_classroom_diff) < 0.02 and abs(lambda_hostel_diff) < 0.02 and lambda_residence_block_diff < 0.02):
            beta_scale_factor = max(min(np.exp(slope_diff/(5)),1.5), 0.66)

        betas['classroom'] = max(betas['classroom'] + step_beta_classroom , 0) * beta_scale_factor
        betas['hostel'] = max(betas['hostel'] + step_beta_hostel , 0) * beta_scale_factor
        # betas['mess'] = max(betas['mess'] + step_beta_mess , 0) * beta_scale_factor
        # betas['cafeteria'] = max(betas['cafeteria'] + step_beta_cafeteria , 0) * beta_scale_factor
        # betas['library'] = max(betas['library'] + step_beta_library , 0) * beta_scale_factor
        # betas['sports_facility'] = max(betas['sports_facility'] + step_beta_sports_facility , 0) * beta_scale_factor
        # betas['recreational_facility'] = max(betas['recreational_facility'] + step_beta_recreational_facility , 0) * beta_scale_factor
        betas['residence_block'] = max(betas['residence_block'] + step_beta_residence_block , 0) * beta_scale_factor
        #betas['residence_block'] = 0

    betas['house'] = betas['residence_block'] * smaller_networks_scale
    betas['smaller_networks'] = betas['hostel'] * smaller_networks_scale
    betas['mess'] = betas['hostel']*0.33
    betas['cafeteria'] = betas['classroom']*0.2
    betas['library'] = betas['classroom']*0.2
    betas['sports_facility'] = betas['classroom']*0.29
    betas['recreational_facility'] = betas['classroom']*0.2
    # betas['PROJECT'] = betas['W'] * smaller_networks_scale
    # betas['RANDOM_COMMUNITY']  =  betas['C'] * smaller_networks_scale
    # betas['NBR_CELLS'] = betas['C'] * smaller_networks_scale

def satisfied(diffs, slope_tolerance = 0.001, lam_tolerance = 0.01):
    if diffs==-1:
        return False
    else:
        return (abs(diffs[0]) < lam_tolerance and
            abs(diffs[1]) < lam_tolerance and
            abs(diffs[2]) < lam_tolerance and
            # diffs[3] < lam_tolerance and
            # diffs[4] < lam_tolerance and
            # diffs[5] < lam_tolerance and
            # diffs[6] < lam_tolerance and
            # diffs[7] < lam_tolerance and
            abs(diffs[3]) < slope_tolerance)        


# In[ ]:


def run_parallel(nruns, ncores, params, betas):
    processed_list = joblib.Parallel(n_jobs=ncores)(
        joblib.delayed(run_sim)(run, params, betas) for run in range(nruns)
    )

def create_beta_file(betas):
    alpha = 1
    interaction_type_names = {
        0 : "outside_campus", 
        1 : "Classroom", 
        2 : "Hostel", 
        3 : "Mess",
        4 : "Cafe",
        5 : "Library",
        6 : "Sports_facility",
        7 : "Recreational_facility",
        8 : "Residence_block",
        9 : "House"
    }
    interaction_type_list = list(range(10))
   # print(interaction_type_list)
   # print(betas)
   # print(interaction_type_names)
    #BETAS = list(betas.values())
    #print(betas)
    #print(BETAS)
    BETAS = []
    BETAS.append(betas['outside_campus'])
    BETAS.append(betas['classroom'])
    BETAS.append(betas['hostel'])
    BETAS.append(betas['mess'])
    BETAS.append(betas['cafeteria'])
    BETAS.append(betas['library'])
    BETAS.append(betas['sports_facility'])
    BETAS.append(betas['recreational_facility'])
    BETAS.append(betas['residence_block'])
    BETAS.append(betas['house'])
    #print(BETAS)
    output_file = "../staticInst/data/campus_data/transmission_coefficients.json"
    trans_coeff = transmission_coefficients(interaction_type_list, BETAS, alpha, interaction_type_names)
    f = open(output_file, "w")
    f.write(json.dumps(trans_coeff, cls= NpEncoder))
    f.close

    
@measure
def calibrate(nruns, ncores, params, betas, resolution=4):

    
    #run_parallel(nruns, ncores, params, betas)
    #print("calibrating")
    create_beta_file(betas)
    for run in range(nruns):
        #os.mkdir("~/campus_simulator/calibrate_betas/calibration_output/run_{run}")
        output_folder = Path(output_base, f"run_{run}")
        output_folder.mkdir(parents = True, exist_ok = True)
        os.system(f"../cpp-simulator/drive_simulator --NUM_DAYS 19 --SEED_FIXED_NUMBER --INIT_FIXED_NUMBER_INFECTED 100 --ENABLE_TESTING --testing_protocol_filename testing_protocol_001.json --input_directory ../staticInst/data/campus_data/ --output_directory calibration_output/run_{run}")  
    try:
        slope = get_slope(output_base, nruns)
        #print(slope)
        #print(target_slope)
        #getTargetSlope()
    except TypeError:
        #print("type error")
        return -1

    lambdas = get_mean_lambdas(output_base, nruns)
    [lambda_classroom, lambda_hostel, lambda_residence_block] = [lambdas[key] for key in ['classroom', 'hostel', 'residence_block']]
    lambda_classroom_diff = float(lambda_classroom) - (1.0/3)
    lambda_hostel_diff = float(lambda_hostel) - (1.0/3)
    # lambda_mess_diff = float(lambda_mess) - (1.0/5)
    # lambda_cafeteria_diff = float(lambda_cafeteria) - (1.0/20)
    # lambda_library_diff = float(lambda_library) - (1.0/20)
    # lambda_sports_facility_diff = float(lambda_sports_facility) - (1.0/20)
    # lambda_recreational_facility_diff = float(lambda_recreational_facility) - (1.0/20)
    lambda_residence_block_diff = float(lambda_residence_block) - (1.0/3)
        
    slope_diff = target_slope - slope

    print_betas()
    print_and_log(f"lambda_classroom_diff: {lambda_classroom_diff:.5f}", logfile)
    print_and_log(f"lambda_hostel_diff: {lambda_hostel_diff:.5f}", logfile)
    # print_and_log(f"lambda_mess_diff: {lambda_mess_diff:.5f}", logfile)
    # print_and_log(f"lambda_cafeteria_diff: {lambda_cafeteria_diff:.5f}", logfile)
    # print_and_log(f"lambda_library_diff: {lambda_library_diff:.5f}", logfile)
    # print_and_log(f"lambda_sports_facility_diff: {lambda_sports_facility_diff:.5f}", logfile)
    # print_and_log(f"lambda_recreational_facility_diff: {lambda_recreational_facility_diff:.5f}", logfile)
    print_and_log(f"lambda_residence_block_diff: {lambda_residence_block_diff:.5f}", logfile)
    print_and_log(f"slope_diff   : {slope_diff:.5f}", logfile)
    print_and_log("", logfile)
    logging.info(f"Slope: slope")
    #print("lambda classroom: ", lambda_classroom)
    #print("lambda hostel: ", lambda_hostel)
    #print("lambda residence block: ", lambda_residence_block)
    #print("calibrating")
    #print(lambda_classroom_diff, lambda_hostel_diff, lambda_residential_block_diff, slope_diff)
    return (lambda_classroom_diff, lambda_hostel_diff, lambda_residence_block_diff, slope_diff)


# In[ ]:


def main():
    global betas
    global params
    global smaller_networks_scale
    global logfile
    global cpp_exec, output_base, input_folder

    
    resolution = 4

    default_binary_path = "../cpp-simulator/drive_simulator"
    default_input_folder = "../staticInst/data/campus_data/"
    default_output_folder = "./calibration_output/"
    default_nruns = 10

    class MyParser(argparse.ArgumentParser):
        def error(self, message):
            sys.stderr.write('error: %s\n' % message)
            sys.stderr.write('\n\n')
            self.print_help()
            sys.exit(2)
    
    my_parser = MyParser(description='Calibration directories',
                         formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    my_parser.add_argument(
        '-e', help='Location of drive_simulator binary',
        default=default_binary_path)
    my_parser.add_argument(
        '-i', help='Location of input folder (with city json files)',
        default=default_input_folder)
    my_parser.add_argument(
        '-o', help='Location of output folder',
        default=default_output_folder)
    my_parser.add_argument(
        '-s', help="scale factor for smaller network",
        default = smaller_networks_scale)
    my_parser.add_argument(
        '-r', help="number of runs per calibration step",
        default = default_nruns)
    my_parser.add_argument(
        '-c', help="Number of cpus to use",
        default = 4)
    my_parser.add_argument(
        '-p', help='Starting parameters json',
        required=True
    )

    args = my_parser.parse_args() or my_parser.print_help()
    cpp_exec = f"./{args.e}" or exit("Error: Couldn't process argument to -e.\n", my_parser.print_help())
    input_folder = args.i or exit("Error: Couldn't process argument to -i.\n" , my_parser.print_help())
    output_base = args.o or exit("Error: Couldn't process argument to -o.\n", my_parser.print_help())
    params_json = args.p or exit("Error: Couldn't process argument to -p.\n" , my_parser.print_help())
    nruns = int(args.r) or exit("Error: Couldn't process argument to -r.\n", my_parser.print_help())
    ncores = int(args.c) or exit("Error: couldn't process argument to -c.\n", my_parser.print_help())

    smaller_networks_scale = float(args.s)

    Path(output_base).mkdir(parents=True, exist_ok = True)
    logfile = Path(output_base, "calibration.log")

    processParams(params_json)
    getTargetSlope()
    count = 1
    while True:
        print_and_log("", logfile)
        print_and_log(f"Count: {count}", logfile)
        diffs = calibrate(nruns, ncores, params, betas)
        if satisfied(diffs):
            print("Satisfied!")
            break
        else:
            update_betas(diffs, count)
            count+=1


# In[ ]:


if __name__ == "__main__":
    main()


# In[ ]:



