import os
import numpy as np
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import time

DEBUG = False
CLI_GUI = True

#Change simdir directory to absolute path of markov_simuls/staticInst/data/campus_outputs/plots_data/ folder
sim_dir = "../staticInst/data/campus_outputs/plots_data/"
intv_array = ['no_intervention', 'case_isolation', 'class_isolation', 'lockdown', 'selective_shutdown', 'evacuation', 'custom_intervention']
intv_str = {
    0: 'No Intervention',
    1: 'Case Isolation',
    2: 'Class Isolation',
    3: 'Lockdown',
    4: 'Selective Shutdown',
    5: 'Evacuation',
    6: 'Custom Intervention',
    100: 'All of the above Interventions'
}

file_names = [
    'num_affected',
    'num_cases',
    'num_fatalities',
    'num_recovered',
    'disease_label_stats'
]

underscore = '_'
offset = -1
interventions_to_plot = []
num_intv = range(len(intv_array))
#num_intv = range(1)
num_runs = input("Enter number of runs: ")
sym_id_array = range(num_runs)
num_days = input("Enter number of days: ")
timesteps = np.arange(0,num_days,0.25)
color_str = ['darkcyan', 'navy', 'darkorchid', 'maroon', 'dodgerblue', 'crimson', 'goldenrod', 'brown', 'darkslateblue', 'darkgreen']
blues = ["#a4a4ac", "#B2EFAF", "#EFC5AF", "#EEAFEF",  "#AFB4EF", "#D2AFEF"]
timesteps = np.arange(0,num_days,0.25)
sns.palplot(color_str)
sns.palplot(blues)


if CLI_GUI:
    print("Intervention Index: \n")
    for key in intv_str:
        print(f"{key} ––> {intv_str[key]} \n")
    all_intv = [0,1,2,3,4,5,6]
    num_INTV = int(input("Enter number of interventions you want to plot (Enter 100 for all): "))
    NUM_INTV = range(num_INTV)
    if num_INTV == 100:
        print("All intervention scenarios are being plotted..")
        interventions_to_plot.extend(all_intv)
    else: 
        for plot_intv in NUM_INTV:
            INTV_NUM = int(input(f"Enter intervention index {plot_intv+1}: "))
            interventions_to_plot.append(INTV_NUM)
        print(f"Intervention scenarios being plotted are..\n")
        for intv_plot in interventions_to_plot:
            print(f"{intv_str[intv_plot]} \n")
else: 
    interventions_to_plot = [0,1,2,3,4,5,6]


if DEBUG : print(f"Interventions to plot: {interventions_to_plot} ")

df_cumulative_cases_matrix = []
df_fatalities_matrix = []
df_daily_cases_matrix = [] 
df_recovered_matrix = []
#df_num_tested_positive_matrix = []
#df_tests_requested_matrix = []
df_disease_label_stats_matrix = []

for intv in num_intv:
    df_cumulative_cases_intv=[] 
    df_fatalities_intv=[]
    df_daily_cases_intv = []
    df_recovered_intv = []
    #df_num_tested_positive_intv = []
    #df_tests_requested_intv = []
    df_disease_label_stats_intv = []

    
    for sym_id in sym_id_array:        
        dir_name=sim_dir+intv_array[intv] + '/'
        file_num = sym_id + 1
        df_cumulative_cases_intv.append(pd.read_csv(dir_name+file_names[0]+ underscore + str(file_num)+'.csv').set_index('Time'))
        df_daily_cases_intv.append(pd.read_csv(dir_name+file_names[1]+ underscore +str(file_num)+'.csv').set_index('Time'))
        df_fatalities_intv.append(pd.read_csv(dir_name+file_names[2]+ underscore +str(file_num)+'.csv').set_index('Time'))
        df_recovered_intv.append(pd.read_csv(dir_name+file_names[3]+ underscore +str(file_num)+'.csv').set_index('Time'))
        #df_num_tested_positive_intv.append(pd.read_csv(dir_name+file_names[4]+ underscore +str(file_num)+'.csv').set_index('Time'))
        #df_tests_requested_intv.append(pd.read_csv(dir_name+file_names[5]+ underscore +str(file_num)+'.csv').set_index('Time'))
        df_disease_label_stats_intv.append(pd.read_csv(dir_name+file_names[4]+ underscore +str(file_num)+'.csv').set_index('Time'))

     
    df_daily_cases_matrix.append(df_daily_cases_intv)
    df_cumulative_cases_matrix.append(df_cumulative_cases_intv)
    df_fatalities_matrix.append(df_fatalities_intv)
    df_recovered_matrix.append(df_recovered_intv)
    #df_num_tested_positive_matrix.append(df_num_tested_positive_intv)
    #df_tests_requested_matrix.append(df_tests_requested_intv)
    df_disease_label_stats_matrix.append(df_disease_label_stats_intv)

if DEBUG:
    print("Df_matrix printing")
    for i in range(10):
        print(f"––––––––––––––––––––––––{i}–––––––––––––––––––––––––")
        print(df_cumulative_cases_matrix[0][i]) #Print for each intervention and each data dump if needed

column_std = []
def get_column_names(column_names):
    column_names_std = [x+"_std" for x in column_names]
    column_names.extend(column_names_std)
    column_std.extend(column_names_std)
    return column_names

if DEBUG: print(f"Column names: {column_std}")

def mean_array(df_matrix):
    df_mean_array = []
    for df_cases in df_matrix:
        df_cases_concat = pd.concat(df_cases)
        column_names_combined = get_column_names(list(df_cases_concat.columns.values))
        df_cases_mean = df_cases_concat.groupby(df_cases_concat.index).mean()
        df_cases_std = df_cases_concat.groupby(df_cases_concat.index).std()
        df_cases_mean = pd.concat([df_cases_mean,df_cases_std],axis=1)
        df_cases_mean.columns = column_names_combined
        df_mean_array.append(df_cases_mean)
    return df_mean_array

df_daily_cases_mean_array = mean_array(df_daily_cases_matrix)
df_fatalities_mean_array = mean_array(df_fatalities_matrix)
df_recovered_mean_array = mean_array(df_recovered_matrix)
df_cumulative_cases_mean_array= mean_array(df_cumulative_cases_matrix)
#df_tested_positive_mean_array= mean_array(df_num_tested_positive_matrix)
#df_tests_requested_mean_array= mean_array(df_tests_requested_matrix)
df_disease_label_mean_array= mean_array(df_disease_label_stats_matrix)

if DEBUG:
    print("DF Mean array printing")
    df_array = [df_daily_cases_mean_array, df_cumulative_cases_mean_array, df_fatalities_mean_array, df_recovered_mean_array]
    print(f"{df_array}")
    
plots_dir = '../staticInst/data/campus_outputs/campus_plots/'
folder_name = time.strftime("%Y%m%d")
timestr = time.strftime("%Y%m%d-%H%M%S")
if not os.path.exists(plots_dir + folder_name):
    os.mkdir(plots_dir + folder_name)

#Cumulative cases
plt.figure(figsize=(16,8))
plt.grid()
plt.title('Cumulative cases')

for intv_index in interventions_to_plot:
    time0 = df_cumulative_cases_mean_array[0].index - offset
    plot_df = df_cumulative_cases_mean_array[intv_index]
    if DEBUG: print(f"For Intervention Scenario: {intv_str[intv_index]} \n The dataframe is: \n {plot_df}")
    plt.plot(time0, plot_df['num_affected'], color = color_str[intv_index], label = intv_str[intv_index])
    plt.fill_between(time0, plot_df['num_affected'] + plot_df['num_affected_std'], plot_df['num_affected'] - plot_df['num_affected_std'], color = color_str[intv_index], alpha = 0.15)

plt.tight_layout()
plt.legend()
plt.xlabel('Timesteps', fontsize=14)
plt.ylabel('Cases', fontsize = 14)
plt.savefig(plots_dir + folder_name +  f'/cumulative_cases_{timestr}.png')

# #Tested Positive cases
# plt.figure(figsize=(16,8))
# plt.grid()
# plt.title('Cumulative cases')

# for intv_index in interventions_to_plot:
#     time0 = df_tested_positive_mean_array[0].index - offset
#     plot_df = df_tested_positive_mean_array[intv_index]
#     if DEBUG: print(f"For Intervention Scenario: {intv_str[intv_index]} \n The dataframe is: \n {plot_df}")
#     plt.plot(time0, plot_df['num_tested_positive'], color = color_str[intv_index], label = intv_str[intv_index])
#     plt.fill_between(time0, plot_df['num_tested_positive'] + plot_df['num_tested_positive_std'], plot_df['num_tested_positive'] - plot_df['num_tested_positive_std'], color = color_str[intv_index], alpha = 0.15)

# plt.tight_layout()
# plt.legend()
# plt.xlabel('Timesteps', fontsize=14)
# plt.ylabel('Tested Positive', fontsize = 14)
# plt.savefig(plots_dir + '/tested_positive__all_23Apr_testing_1.png')

#Tests requested
plt.figure(figsize=(16,8))
plt.grid()
plt.title('People tested')

for intv_index in interventions_to_plot:
    time0 = df_disease_label_mean_array[0].index - offset
    plot_df = df_disease_label_mean_array[intv_index]
    # print(f"For Intervention Scenario: {intv_str[intv_index]} \n The dataframe is: \n {plot_df}")
    plt.plot(time0, plot_df["people_tested"], color = color_str[intv_index], label = intv_str[intv_index])
    plt.fill_between(time0, plot_df['people_tested'] + plot_df['people_tested_std'], plot_df['people_tested'] - plot_df['people_tested_std'], color = color_str[intv_index], alpha = 0.15)

plt.tight_layout()
plt.legend()
plt.xlabel('Timesteps', fontsize=14)
plt.ylabel('Tests', fontsize = 14)
plt.savefig(plots_dir + folder_name + f'/people_tested_{timestr}.png')

#Positive cases
plt.figure(figsize=(16,8))
plt.grid()
plt.title('Cumulative positive cases')

for intv_index in interventions_to_plot:
    time0 = df_disease_label_mean_array[0].index - offset
    plot_df = df_disease_label_mean_array[intv_index]
    if DEBUG: print(f"For Intervention Scenario: {intv_str[intv_index]} \n The dataframe is: \n {plot_df}")
    plt.plot(time0, plot_df['cumulative_positive_cases'], color = color_str[intv_index], label = intv_str[intv_index])
    plt.fill_between(time0, plot_df['cumulative_positive_cases'] + plot_df['cumulative_positive_cases_std'], plot_df['cumulative_positive_cases'] - plot_df['cumulative_positive_cases_std'], color = color_str[intv_index], alpha = 0.15)

plt.tight_layout()
plt.legend()
plt.xlabel('Timesteps', fontsize=14)
plt.ylabel('Cases', fontsize = 14)
plt.savefig(plots_dir + folder_name + f'/tested_positive_{timestr}.png')

#Daily cases
plt.figure(figsize=(16,8))
plt.grid()
plt.title("Daily cases")
for intv_index in interventions_to_plot:
    time0 = df_cumulative_cases_mean_array[0].index - offset
    plot_df = df_cumulative_cases_mean_array[intv_index]
    plt.plot(time0, plot_df['num_affected'].diff(periods=4), color = color_str[intv_index], label = intv_str[intv_index])

plt.tight_layout()
plt.legend()
plt.xlabel('Timesteps', fontsize=14)
plt.ylabel('Cases', fontsize = 14)
plt.savefig(plots_dir + folder_name + f'/daily_cases_{timestr}.png')
