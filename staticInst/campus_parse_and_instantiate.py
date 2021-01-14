from numpy.core.numeric import indices
import pandas as pd
import numpy as np 
import json 
import warnings
from collections import Counter
warnings.filterwarnings('ignore')

DEBUG = False

inputfiles = {
    "students" : "student.csv",
    "class" : "classes.csv",
    "staff" : "staff.csv",
    "mess" : "mess.csv",
    "timetable" : "timetable.csv"
}

outputfiles = {
    "individuals" : "individuals.json",
    "interaction_spaces" : "interaction_spaces.json"
}

input_file_dir = "/Users/Minhaas/CODING/iisc/rough/campus_input_csv/"
output_file_dir = "/Users/Minhaas/CODING/iisc/campus-simulator/markov_simuls/staticInst/data/campus_data/"

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

student_df = pd.read_csv(input_file_dir + inputfiles["students"])
class_df = pd.read_csv(input_file_dir + inputfiles["class"])
staff_df = pd.read_csv(input_file_dir + inputfiles["staff"])
mess_df = pd.read_csv(input_file_dir + inputfiles["mess"])
timetable_df = pd.read_csv(input_file_dir + inputfiles["timetable"])

student_id = [int(x) for x in student_df["id"]]
student_age = [int(x) for x in student_df["age"]]
student_hostel = [int(x) for x in student_df["hostel"]]
student_dept = [int(x) for x in student_df["dept_id"]]
student_mess = [int(x) for x in student_df["mess"]]
class_list = [int(x) for x in class_df["class_id"]]

class_dept = [int(x) for x in class_df["dept"]] 
class_faculty = [int(x) for x in class_df["faculty_id"]]
staff_type = [int(x) for x in staff_df["dept_associated"]]
staff_dept = [int(x) for x in staff_df["interaction_space"]]
faculty = [float(x) for x in class_df["faculty_id"]] 

student_pop = student_df["id"].count()
hostel_pop  = np.bincount(student_df["hostel"])
staff_pop = staff_df["staff_id"].count()

print("Creating individuals.json...")
individual = []

def unique(hostel_mess_list):
    x = np.array(hostel_mess_list)
    unique_array = np.unique(x)
    return unique_array

hostel_list = unique(student_hostel)
mess_list = unique(student_mess)

duration_sum = 0
mess_active_duration = 0.33
class_active_duration = 0.04

time_table = timetable_df.values.tolist()
num_days = time_table[1].count(-1)
periodicity = num_days

def active_duration(timetable, hostel_active_duration):
    num_days = [i for i, x in enumerate(timetable) if x == -1]
    active_duration_hostel = []
    total_days = range(len(num_days))
    for day in total_days:
        hostel_duration = hostel_active_duration
        if day == 0:
            for ispace in range(1,num_days[day]):
                if DEBUG : print(timetable[ispace])
                if timetable[ispace] != -1:
                    if DEBUG: print("Not -1")
                    if timetable[ispace] in class_list:
                        if DEBUG : print("In class list")
                        hostel_duration -= class_active_duration
                else: 
                    if DEBUG : print("-1 encountered")
                    continue
            hostel_duration -= mess_active_duration
            active_duration_hostel.append(hostel_duration)
        else : 
            for ispace in range(num_days[day-1], num_days[day]):
                if DEBUG : print(timetable[ispace])
                if timetable[ispace] != -1:
                    if DEBUG: print("Not -1")
                    if timetable[ispace] in class_list:
                        if DEBUG : print("In class list")
                        hostel_duration -= class_active_duration
                else: 
                    if DEBUG : print("-1 encountered")
                    continue
            hostel_duration -= mess_active_duration
            active_duration_hostel.append(hostel_duration)
    return active_duration_hostel


for i in range(student_pop):
    person = {}
    person["id"] = student_id[i]
    person["Type"] = 0
    days =0
    person["age"] = student_age[i]
    person["Hostel"] = student_hostel[i]
    person["Dept"] = student_dept[i]
    person["Periodicity"] = periodicity
    daily_int_st = {}
    int_st = []
    hostel_active_duration = 1
    hostel_duration = active_duration(time_table[i], hostel_active_duration)
    #if DEBUG : print(hostel_duration)
    daily_int_st = {student_hostel[i] : hostel_duration[0], student_mess[i] : mess_active_duration}
    for j in range(1,len(time_table[i])):
        if time_table[i][j] == -1:
            days = days + 1
            int_st.append(daily_int_st)
            #hostel_act_dur = active_duration(time_table[i], hostel_active_duration)
            daily_int_st = {student_hostel[i]: hostel_duration[1], student_mess[i] : mess_active_duration}
            continue
        if days != num_days:
            daily_int_st[time_table[i][j]] = class_active_duration
        else: 
            break

    while days < num_days:
        int_st.append(daily_int_st)

    person["interaction_strength"] = int_st
    individual.append(person)


for i in range(len(Counter(faculty))):
    faculty_dict = {}
    faculty_dict["id"] = student_pop + i
    faculty_dict["Type"] = 1
    faculty_dict["age"] = np.random.randint(31,55) #Parameterise ALL quantities 
    faculty_dict["Hostel"] = 0
    faculty_dict["Periodicity"] = periodicity
    class_fac = []
    #num_days_fac = 1
    for x in range(len(class_faculty)):
        if class_faculty[x] == faculty_dict["id"]:
            faculty_dict["dept"] = class_dept[x]
            class_fac.append(x)
    daily_int_st = {}
    int_str = []
    for j in range(num_days):
        daily_int_st[0] = 1 - (class_active_duration*len(class_fac))
        #daily_int_st = {0: 0.88}
        for k in range(len(class_fac)):
            daily_int_st[class_fac[k]] = class_active_duration
            #daily_int_st = {class_fac[k]: 0.04}
        int_str.append(daily_int_st)
    faculty_dict["interaction_strength"] = int_str 
    individual.append(faculty_dict)


for i in range(staff_pop):
    staff_dict = {}
    staff_dict["id"] = student_pop + len(Counter(faculty)) - 1 + i #Take care - verify the -1
    staff_dict["Type"] = 2
    staff_dict["Hostel"] = 0
    staff_dict["age"] = np.random.randint(31, 55)
    staff_dict["Dept"] = staff_dept[i]
    staff_dict["Periodicity"] = periodicity
    #num_days_staff = 2
    daily_int_st = {}
    int_st = []
    for j in range(num_days):
        if staff_type[i] == 1:
            daily_int_st = {0: 0.45, staff_dept[i]: 0.55}
        else :
            daily_int_st = {0: 0.67, staff_dept[i]: 0.33}
        int_st.append(daily_int_st)
    staff_dict["interaction_strength"] = int_st 
    individual.append(staff_dict)

f = open(output_file_dir+outputfiles['individuals'], "w+")
f.write(json.dumps(individual, cls = NpEncoder))
f.close
print("Done.")

print("Creating interaction_spaces.json...")

num_dept = len(Counter(student_dept).keys())
num_hostel = len(Counter(student_hostel).keys())
num_mess = len(Counter(student_hostel).keys())
num_classes = len(class_dept)
num_interaction_spaces = len(Counter(student_dept).keys()) + (len(Counter(student_hostel).keys())*2) + num_classes

interaction_spaces = []
i_space = {}
i_space["id"] = 0
i_space["type"] = 0
i_space["beta"] = 0
i_space["alpha"] = 1
i_space["active_duration"] = 0.66
i_space["lat"] = np.random.uniform(10.0,20.0)
i_space["lon"] = np.random.uniform(15.0,18.0)
interaction_spaces.append(i_space)

for i in range(num_classes):
    i_space_class = {}
    i_space_class["id"] = i + 1
    i_space_class["type"] = 1
    i_space_class["beta"] = np.random.uniform(0,0.5) + 1
    i_space_class["alpha"] = 1
    i_space_class["active_duration"] = 0.04
    i_space_class["lat"] = np.random.uniform(10.0,20.0)
    i_space_class["lon"] = np.random.uniform(15.0,18.0)
    interaction_spaces.append(i_space_class)

for i in range(num_hostel):
    i_space_hostel = {}
    i_space_hostel["id"] = i + num_classes + 1
    i_space_hostel["type"] = 2
    i_space_hostel["beta"] = np.random.uniform(0,0.5) + 1
    i_space_hostel["alpha"] = 1
    i_space_hostel["active_duration"] = 0.55
    i_space_hostel["lat"] = np.random.uniform(10.0,20.0)
    i_space_hostel["lon"] = np.random.uniform(15.0,18.0)
    interaction_spaces.append(i_space_hostel)

for i in range(num_mess):
    i_space_mess = {}
    i_space_mess["id"] = i + num_classes + num_hostel + 1
    i_space_mess["type"] = 3
    i_space_mess["beta"] = np.random.uniform(0,0.5) + 1
    i_space_mess["alpha"] = 1
    i_space_mess["active_duration"] = 0.33
    i_space_mess["lat"] = np.random.uniform(10.0,20.0)
    i_space_mess["lon"] = np.random.uniform(15.0,18.0)
    interaction_spaces.append(i_space_mess)

#Need to have a validaton script for csv and json files

f = open(output_file_dir+outputfiles['interaction_spaces'], "w+")
f.write(json.dumps(interaction_spaces, cls = NpEncoder))
f.close
print("Done.")
#print(num_interaction_spaces)
#print(interaction_spaces)
#"""