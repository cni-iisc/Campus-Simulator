import pandas as pd
import numpy as np 
import json 
import warnings
from collections import Counter
import os 
warnings.filterwarnings('ignore')

DEBUG = False
markov_simuls = True
sim_test = True

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

#file_dir = "/Users/Minhaas/CODING/iisc/campus-simulator/markov_simuls/staticInst/data/campus_data/"
#file_dir = "/Users/Minhaas/CODING/iisc/rough/"
input_file_dir = "/Users/Minhaas/CODING/iisc/rough/campus_input_csv/"

if markov_simuls: 
    output_file_dir = "/Users/Minhaas/CODING/iisc/campus-simulator/markov_simuls/staticInst/data/campus_data/"
else: 
    output_file_dir = "/Users/Minhaas/CODING/iisc/rough/campus_input_csv/json_files/"

if not os.path.exists(output_file_dir):
    os.makedirs(output_file_dir)

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

column_names = [i for i in range(24)]
student_df = pd.read_csv(input_file_dir + inputfiles["students"])
class_df = pd.read_csv(input_file_dir + inputfiles["class"])

staff_df = pd.read_csv(input_file_dir + inputfiles["staff"])
mess_df = pd.read_csv(input_file_dir + inputfiles["mess"])
timetable_df = pd.read_csv(input_file_dir + inputfiles["timetable"], header=None, names = column_names)

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

faculty_pop = 27
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
weekends = range(2)

time_table = timetable_df.values.tolist()
for i in range(len(time_table)):
    time_table[i] = [x for x in time_table[i] if str(x)!='nan']

num_days = time_table[1].count(-1) + 2 #To bring 7 day periodicity
periodicity = 7

print("Instantiating students...")
for i in range(student_pop):
    person = {}
    person["id"] = student_id[i]
    person["Type"] = 0
    days =0
    person["age"] = student_age[i]
    person["Hostel"] = student_hostel[i]
    person["Dept"] = student_dept[i]
    person["Periodicity"] = periodicity
    int_st = []
    for z in range(periodicity):
        daily_int_st = {}
        int_st.append(daily_int_st)

    for j in range(1,len(time_table[i])):
        classID = time_table[i][j]
        tt_df = class_df[class_df["class_id"] == classID]
        days_of_the_week = int(tt_df["days"])
        days_of_the_week = str(days_of_the_week)
        for k in range(len(days_of_the_week)):
            if sim_test : 
                if (int(days_of_the_week[k])-1)%2 == 0: 
                    int_st[int(days_of_the_week[k])-1][classID] = float(tt_df["active_duration"])
                else: 
                    int_st[int(days_of_the_week[k])-1][classID] = 0
            else: 
                int_st[int(days_of_the_week[k])-1][classID] = float(tt_df["active_duration"])

    person["interaction_strength"] = int_st
    individual.append(person)
 
for i in range(student_pop):
    count = 0
    for daily_int_st in individual[i]["interaction_strength"]:
        sum = 0
        val = daily_int_st.values()
        for v in val:
            sum += v
        if sim_test : 
            if count % 2 == 0: 
                daily_int_st[student_hostel[i]] = 1 - sum - mess_active_duration
                daily_int_st[student_mess[i]] = mess_active_duration
            else :
                daily_int_st[student_hostel[i]] = 0 
                daily_int_st[student_mess[i]] = 0
        else :
            daily_int_st[student_hostel[i]] = 1 - sum - mess_active_duration
            daily_int_st[student_mess[i]] = mess_active_duration
        if sim_test : 
            count += 1
print("Student done.")

for i in range(faculty_pop):
    faculty_dict = {}
    faculty_dict["id"] = student_pop + i
    faculty_dict["Type"] = 1
    faculty_dict["age"] = np.random.randint(31,55) #Parameterise ALL quantities
    faculty_dict["Hostel"] = 0
    faculty_dict["Periodicity"] = periodicity
    class_fac = class_df[class_df["faculty_id"] == faculty_dict["id"]]
    class_fac = class_fac.reset_index(drop = True)
    if DEBUG : print(class_fac)
    if class_fac.empty: 
        faculty_dict["dept"] = -1
    else: 
        faculty_dict["dept"] = class_fac["dept"][0]
    
    int_st = []
    for j in range(periodicity):
        daily_int_st = {}
        int_st.append(daily_int_st)
    faculty_dict["interaction_strength"] = int_st
    individual.append(faculty_dict)

for i in range(student_pop):
    dotf = 0
    count = 0
    for daily_int_st in individual[i]["interaction_strength"]:
        if sim_test : count +=1 
        int_st_keys = list(daily_int_st.keys())
        for key in int_st_keys:
            if key > len(class_df):
                continue 
            fac = class_df["faculty_id"][key - 1]
            if sim_test : 
                if count % 2 ==0 : 
                    individual[fac]["interaction_strength"][dotf][key] = class_df["active_duration"][key-1] 
                else : 
                    individual[fac]["interaction_strength"][dotf][key] = 0 
            else: 
                individual[fac]["interaction_strength"][dotf][key] = class_df["active_duration"][key-1]  #list of dictionaries of list of dictionaries
        dotf = dotf + 1
        if sim_test : 
            count += 1
        #print(dotf)


for i in range(faculty_pop):
    count = 0
    for daily_int_st in individual[student_pop + i]["interaction_strength"]:
        sum = 0
        val = daily_int_st.values()
        for v in val:
            sum += v
        if sim_test : 
            if count % 2 == 0: 
                daily_int_st[0] = 1 - sum
            else :
                daily_int_st[0] = 0
        else : 
            daily_int_st[0] = 1 - sum
        if sim_test : 
            count += 1

print("Faculty done")

print("Instantiating staff...")
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
    for j in range(periodicity):
        if sim_test :
            if j%2 == 0: 
                 daily_int_st = {0: 0, staff_dept[i]: 0}
            else: 
                if staff_type[i] == 1:
                    daily_int_st = {0: 0.45, staff_dept[i]: 0.55}
                else :
                    daily_int_st = {0: 0.67, staff_dept[i]: 0.33}
        else:   
            if staff_type[i] == 1:
                daily_int_st = {0: 0.45, staff_dept[i]: 0.55}
            else :
                daily_int_st = {0: 0.67, staff_dept[i]: 0.33}
        int_st.append(daily_int_st)
    staff_dict["interaction_strength"] = int_st 
    individual.append(staff_dict)
print("Staff done.")

f = open(output_file_dir + outputfiles['individuals'], "w+")
f.write(json.dumps(individual, cls = NpEncoder))
f.close
print("individuals.json generated")

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

print("Classes instantiated")

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

print("Hostels instantiated")

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

print("Mess instantiated")

f = open(output_file_dir + outputfiles['interaction_spaces'], "w+")
f.write(json.dumps(interaction_spaces, cls = NpEncoder))
f.close
print("interaction_spaces.json generated.")

