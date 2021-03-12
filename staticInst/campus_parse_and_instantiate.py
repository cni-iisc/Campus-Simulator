import pandas as pd
import numpy as np 
import json 
import warnings
from collections import Counter
import os 
from transmission_coefficients import transmission_coefficients
warnings.filterwarnings('ignore')

DEBUG = False
markov_simuls = True
sim_test = False
modularise = True
cafe = True

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

def campus_parse(inputfiles):
    student_id = [int(x) for x in inputfiles["students"]["id"]]
    student_age = [int(x) for x in inputfiles["students"]["age"]]
    student_hostel = [int(x) for x in inputfiles["students"]["hostel"]]
    student_dept = [int(x) for x in inputfiles["students"]["dept_id"]]
    student_mess = [int(x) for x in inputfiles["students"]["mess"]]
    class_list = [int(x) for x in inputfiles["class"]["class_id"]]

    class_dept = [int(x) for x in inputfiles["class"]["dept"]] 
    class_faculty = [int(x) for x in inputfiles["class"]["faculty_id"]]
    staff_type = [int(x) for x in inputfiles["staff"]["dept_associated"]]
    staff_dept = [int(x) for x in inputfiles["staff"]["interaction_space"]]
    faculty = [float(x) for x in inputfiles["class"]["faculty_id"]] 

    faculty_pop = 27
    student_pop = inputfiles["students"]["id"].count()
    hostel_pop  = np.bincount(inputfiles["students"]["hostel"])
    staff_pop = inputfiles["staff"]["staff_id"].count()

    print("Creating individuals.json...")
    individual = []

    def unique(hostel_mess_list):
        x = np.array(hostel_mess_list)
        unique_array = np.unique(x)
        return unique_array

    hostel_list = unique(student_hostel)
    mess_list = unique(student_mess)

    duration_sum = 0
    mess_active_duration = inputfiles["mess"]["active_duration"][0]
    weekends = range(2)

    time_table = inputfiles["timetable"].values.tolist()
    for i in range(len(time_table)):
        time_table[i] = [x for x in time_table[i] if str(x)!='nan']

    periodicity = 7

    spaces_map = {
        0 : "Day Scholar",
        1 : "Classes",
        2 : "Hostels",
        3 : "Mess",
        4 : "Cafe"
    }

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
            tt_df = inputfiles["class"][inputfiles["class"]["class_id"] == classID]
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
    
    if cafe:
        for i in range(student_pop):
            for daily_int_st in individual[i]["interaction_strength"]:
                for j in range(inputfiles["common_areas"]["starting_id"][0], inputfiles["common_areas"]["starting_id"][0]+inputfiles["common_areas"]["number"][0]):
                    daily_int_st[j] = 0

    print("Student done.")

    for i in range(faculty_pop):
        faculty_dict = {}
        faculty_dict["id"] = student_pop + i
        faculty_dict["Type"] = 1
        faculty_dict["age"] = np.random.randint(31,55) #Parameterise ALL quantities
        faculty_dict["Hostel"] = 0
        faculty_dict["Periodicity"] = periodicity
        class_fac = inputfiles["class"][inputfiles["class"]["faculty_id"] == faculty_dict["id"]]
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
                if key > len(inputfiles["class"]):
                    continue 
                fac = inputfiles["class"]["faculty_id"][key - 1]
                if sim_test : 
                    if count % 2 ==0 : 
                        individual[fac]["interaction_strength"][dotf][key] = inputfiles["class"]["active_duration"][key-1] 
                    else : 
                        individual[fac]["interaction_strength"][dotf][key] = 0 
                else: 
                    individual[fac]["interaction_strength"][dotf][key] = inputfiles["class"]["active_duration"][key-1]  #list of dictionaries of list of dictionaries
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
                    daily_int_st = {0: 0, staff_dept[i]: 1}
                else :
                    daily_int_st = {0: 1-mess_active_duration, staff_dept[i]: mess_active_duration}
            int_st.append(daily_int_st)
        staff_dict["interaction_strength"] = int_st 
        individual.append(staff_dict)
    print("Staff done.")

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
    i_space["active_duration"] = 1
    i_space["avg_time"] = 0.66
    i_space["lat"] = np.random.uniform(10.0,20.0)
    i_space["lon"] = np.random.uniform(15.0,18.0)
    interaction_spaces.append(i_space)

    for i in range(num_classes):
        i_space_class = {}
        i_space_class["id"] = i + 1
        i_space_class["type"] = 1
        i_space_class["beta"] = np.random.uniform(0,0.5) + 1
        i_space_class["alpha"] = 1
        i_space_class["active_duration"] = inputfiles["class"]["active_duration"][i]
        i_space_class["avg_time"] = inputfiles["class"]["active_duration"][i]
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
        i_space_hostel["active_duration"] = 1
        i_space_hostel["avg_time"] = 0.55
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
        i_space_mess["active_duration"] = mess_active_duration
        i_space_mess["avg_time"] = 0.125
        i_space_mess["lat"] = np.random.uniform(10.0,20.0)
        i_space_mess["lon"] = np.random.uniform(15.0,18.0)
        interaction_spaces.append(i_space_mess)

    print("Mess instantiated")

    if cafe :     
        for i in range(inputfiles["common_areas"]["number"][0]):
            #i_space_cafe = {​​​​​}​​​​​
            i_space_cafe = {}
            i_space_cafe["id"] = i + inputfiles["common_areas"]["starting_id"][0]
            i_space_cafe["type"] = 4
            i_space_cafe["beta"] = np.random.uniform(0,0.5) + 1
            i_space_cafe["alpha"] = 1
            i_space_cafe["active_duration"] = inputfiles["common_areas"]["active_duration"][0]
            i_space_cafe["avg_time"] = inputfiles["common_areas"]["average_time_spent"][0]
            i_space_cafe["lat"] = np.random.uniform(10.0,20.0)
            i_space_cafe["lon"] = np.random.uniform(15.0,18.0)
            interaction_spaces.append(i_space_cafe)

        print("Cafes instantiated")
    
    type_list = []
    for spaces in interaction_spaces:
        type_list.append(spaces["type"])
    
    type_arr = np.unique(np.array(type_list))

    names = []
    BETA = []
    
    for elem in type_arr:
        names.append(spaces_map[elem])
        BETA.append(1)
    
    ALPHA = 1

    trans_coeffs = transmission_coefficients(type_arr, BETA, ALPHA, names)

    return individual, interaction_spaces, trans_coeffs


if __name__ == "__main__":

    inputfiles = {
        "students" : "student.csv",
        "class" : "classes.csv",
        "staff" : "staff.csv",
        "mess" : "mess.csv",
        "timetable" : "timetable.csv",
        "common_spaces" : "common_areas.csv"
    }

    outputfiles = {
        "individuals" : "individuals.json",
        "interaction_spaces" : "interaction_spaces.json"
    }

    if markov_simuls: 
        output_file_dir = "/Users/Minhaas/CODING/iisc/campus_simulator/markov_simuls/staticInst/data/campus_data/"
    else: 
        output_file_dir = "/Users/Minhaas/CODING/iisc/rough/campus_input_csv/json_files/"

    if not os.path.exists(output_file_dir):
        os.makedirs(output_file_dir)

    input_file_dir = "/Users/Minhaas/CODING/iisc/rough/campus_input_csv/"
    column_names = [i for i in range(24)]
    student_df = pd.read_csv(input_file_dir + inputfiles["students"])
    class_df = pd.read_csv(input_file_dir + inputfiles["class"])
    staff_df = pd.read_csv(input_file_dir + inputfiles["staff"])
    mess_df = pd.read_csv(input_file_dir + inputfiles["mess"])
    timetable_df = pd.read_csv(input_file_dir + inputfiles["timetable"], header=None, names = column_names)
    common_areas_df = pd.read_csv(input_file_dir + inputfiles["common_spaces"])

    input_dfs = {
        "students" : student_df,
        "class" : class_df,
        "staff" : staff_df,
        "mess" : mess_df,
        "timetable" : timetable_df,
        "common_areas" : common_areas_df
    }

    individuals, interaction_spaces, trans_coeffs = campus_parse(input_dfs)

    individual_json = open(output_file_dir + outputfiles['individuals'], "w+")
    individual_json.write(json.dumps(individuals, cls = NpEncoder))
    individual_json.close
    print("individuals.json generated")

    interaction_spaces_json = open(output_file_dir + outputfiles['interaction_spaces'], "w+")
    interaction_spaces_json.write(json.dumps(interaction_spaces, cls = NpEncoder))
    interaction_spaces_json.close
    print("interaction_spaces.json generated.")
