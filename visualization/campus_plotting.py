import os
import numpy as np
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

DEBUG = False
CLI_GUI = True

#Change simdir directory to absolute path of markov_simuls/staticInst/data/campus_outputs/plots_data/ folder
sim_dir = "/Users/Minhaas/CODING/iisc/campus_simulator/markov_simuls/staticInst/data/campus_outputs/plots_data/"
intv_array = ['no_intervention', 'case_isolation', 'class_isolation', 'lockdown', 'custom_intervention']
intv_str = {
    0: 'No Intervention',
    1: 'Case Isolation',
    2: 'Class Isolation',
    3: 'Lockdown',
    4: 'Custom Intervention',
    100: 'All of the above Interventions'
}

file_names = [
    'num_affected',
    'num_cases',
    'num_fatalities',
    'num_recovered'
]

underscore = '_'
offset = -1
interventions_to_plot = []
num_intv = range(len(intv_array))
sym_id_array = range(10)
timesteps = np.arange(0,120,0.25)
color_str = ['darkcyan', 'navy', 'darkorchid', 'maroon', 'dodgerblue', 'crimson', 'goldenrod', 'brown', 'darkslateblue', 'darkgreen']
blues = ["#a4a4ac", "#B2EFAF", "#EFC5AF", "#EEAFEF",  "#AFB4EF", "#D2AFEF"]
timesteps = np.arange(0,120,0.25)
sns.palplot(color_str)
sns.palplot(blues)


if CLI_GUI:
    print("Intervention Index: \n")
    for key in intv_str:
        print(f"{key} ––> {intv_str[key]} \n")
    all_intv = [0,1,2,3,4]
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
    interventions_to_plot = [0,1,2,3,4]


if DEBUG : print(f"Interventions to plot: {interventions_to_plot} ")

df_cumulative_cases_matrix = []
df_fatalities_matrix = []
df_daily_cases_matrix = [] 
df_recovered_matrix = []

for intv in num_intv:
    df_cumulative_cases_intv=[] 
    df_fatalities_intv=[]
    df_daily_cases_intv = []
    df_recovered_intv = []
    
    for sym_id in sym_id_array:        
        dir_name=sim_dir+intv_array[intv] + '/'
        file_num = sym_id + 1
        df_cumulative_cases_intv.append(pd.read_csv(dir_name+file_names[0]+ underscore + str(file_num)+'.csv').set_index('Time'))
        df_daily_cases_intv.append(pd.read_csv(dir_name+file_names[1]+ underscore +str(file_num)+'.csv').set_index('Time'))
        df_fatalities_intv.append(pd.read_csv(dir_name+file_names[2]+ underscore +str(file_num)+'.csv').set_index('Time'))
        df_recovered_intv.append(pd.read_csv(dir_name+file_names[3]+ underscore +str(file_num)+'.csv').set_index('Time'))

     
    df_daily_cases_matrix.append(df_daily_cases_intv)
    df_cumulative_cases_matrix.append(df_cumulative_cases_intv)
    df_fatalities_matrix.append(df_fatalities_intv)
    df_recovered_matrix.append(df_recovered_intv)

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

if DEBUG:
    print("DF Mean array printing")
    df_array = [df_daily_cases_mean_array, df_cumulative_cases_mean_array, df_fatalities_mean_array, df_recovered_mean_array]
    print(f"{df_array}")
    
plots_dir = '/Users/Minhaas/CODING/iisc/rough/plots'
if not os.path.exists(plots_dir):
    os.mkdir(plots_dir)

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
plt.savefig(plots_dir + '/cumulative_cases_all_22032021.png')

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
plt.savefig(plots_dir + '/daily_cases_22032021.png')