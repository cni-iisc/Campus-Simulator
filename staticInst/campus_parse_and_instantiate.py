import pandas as pd
import numpy as np 
import argparse
import json 
from collections import Counter
import os
from .transmission_coefficients import transmission_coefficients

sim_test = False
cafe = True

def convert(o):
    if isinstance(o, np.int64): return int(o)
    raise TypeError

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

    residence_block_list = [int(x) for x in inputfiles["staff"]["residence_block"]  ]
    adult_family_members = [int(x) for x in inputfiles["staff"]["adult_family_members"]]
    num_children = [int(x) for x in inputfiles["staff"]["num_children"]]

    # for i in range(len(inputfiles["staff"])):
    #     residence_block_list[i] = residence_block_list[i].astype(int)
    #     adult_family_members[i] = adult_family_members[i].astype(int)
    #     num_children[i] = num_children[i].astype(int)

    inputfiles["class"]["active_duration"] = (inputfiles["class"]["active_duration"]/24).astype(float)
    inputfiles["mess"]["active_duration"] = (inputfiles["mess"]["active_duration"]/24).astype(float)
    inputfiles["common_areas"]["active_duration"] = (inputfiles["common_areas"]["active_duration"]/24).astype(float)
    inputfiles["common_areas"]["average_time_spent"] = (inputfiles["common_areas"]["average_time_spent"]/24).astype(float)
    # inputfiles["staff"]["residence_block"] = (inputfiles["staff"]["residence_block"]/1).astype(int)
    # inputfiles["staff"]["adult_family_members"] = (inputfiles["staff"]["adult_family_members"]/1).astype(int)
    # inputfiles["staff"]["num_children"] = (inputfiles["staff"]["num_children"]/1).astype(int)

    print(type(inputfiles["staff"]["residence_block"][0]))
    #faculty_pop = len(inputfiles["class"]["faculty_id"].unique())
    fac_df = inputfiles["staff"][inputfiles["staff"]["dept_associated"] == -1]
    faculty_pop = len(fac_df)
    #print(faculty_pop)
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
    mess_active_duration = {}
    for i in range(len(inputfiles["mess"])):
        mess_active_duration[inputfiles["mess"]["mess_id"].iloc[i]] = (inputfiles["mess"]["average_time_spent"].iloc[i]/24).astype(float)
    #print(mess_active_duration)
    weekends = range(2)

    time_table = inputfiles["timetable"].values.tolist()
    for i in range(len(time_table)):
        time_table[i] = [x for x in time_table[i] if str(x)!='nan']

    periodicity = 7

    spaces_map = {
        0 : "outside_campus",
        1 : "Classes",
        2 : "Hostels",
        3 : "Mess",
        4 : "Cafe",
        5 : "Library",
        6 : "Sports_facility",
        7 : "Recreational_facility",
        8 : "residence_block",
        9 : "house"
    }

    print("Instantiating students and faculty", end=" ... ", flush=True)
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
            d_o_t_f = str(tt_df["days"].iloc[0])
            days_of_the_week = d_o_t_f.split("-")
            for k in range(len(days_of_the_week)):
                if sim_test : 
                    if (int(days_of_the_week[k])-1)%2 == 0: 
                        int_st[int(days_of_the_week[k])-1][classID] = float(tt_df["active_duration"])
                    else: 
                        int_st[int(days_of_the_week[k])-1][classID] = 0
                else: 
                    int_st[int(days_of_the_week[k])-1][classID] = float(tt_df["active_duration"])

        person["interaction_strength"] = int_st
        person["house"] = 0
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
                daily_int_st[student_hostel[i]] = 1 - sum - mess_active_duration[student_mess[i]]
                daily_int_st[student_mess[i]] = mess_active_duration[student_mess[i]]
            if sim_test : 
                count += 1
    
    if cafe:
        for i in range(student_pop):
            for daily_int_st in individual[i]["interaction_strength"]:
                for j in range(inputfiles["common_areas"]["starting_id"][0], inputfiles["common_areas"]["starting_id"][3]+inputfiles["common_areas"]["number"][3]):
                    daily_int_st[j] = 0

    for i in range(faculty_pop):
        faculty_dict = {}
        faculty_dict["id"] = student_pop + i
        faculty_dict["Type"] = 1
        faculty_dict["age"] = np.random.randint(31,55) #Parameterise ALL quantities
        faculty_dict["Hostel"] = 0
        faculty_dict["Periodicity"] = periodicity
        class_fac = inputfiles["class"][inputfiles["class"]["faculty_id"] == faculty_dict["id"]]
        class_fac = class_fac.reset_index(drop = True)
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
                key = int(key)
                if key > len(inputfiles["class"]):
                    continue 
                fac = inputfiles["class"]["faculty_id"][key-1]
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


    #house = 116
    #house_df = inputfiles["common_areas"].iloc[-1]
    #house = int(house_df["starting_id"] + house_df["number"])
    #print(type(house))

    house = len(inputfiles["class"]) + len(inputfiles["students"]["hostel"].unique()) + len(inputfiles["mess"]) + len(inputfiles["staff"]["residence_block"].unique())
    for j in range(len(inputfiles["common_areas"])):   
        house += int(inputfiles["common_areas"]["number"].iloc[j])

    start_house = house
    print(house)

    fac_res = len(inputfiles["staff"]) - faculty_pop
    for i in range(faculty_pop):
        residence_block = residence_block_list[fac_res + i]
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
                daily_int_st[residence_block] = 1 - sum
                daily_int_st[house] = 1 - sum
            if sim_test : 
                count += 1
        individual[student_pop + i]["house"] = house
        house += 1
    
    if cafe:
        for i in range(student_pop, student_pop+faculty_pop):
            for daily_int_st in individual[i]["interaction_strength"]:
                for j in range(inputfiles["common_areas"]["starting_id"][0], inputfiles["common_areas"]["starting_id"][3]+inputfiles["common_areas"]["number"][3]):
                    daily_int_st[j] = 0
    print("done.", flush=True)


    print("Instantiating staff", end=" .... ", flush=True)
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
                    daily_int_st = {residence_block_list[i] : 0.33, staff_dept[i]: 0.67, house : 0.33}
                elif staff_type[i] == 2:
                    daily_int_st = {residence_block_list[i] : 1-inputfiles["mess"]["active_duration"].iloc[0], staff_dept[i]: inputfiles["mess"]["active_duration"].iloc[0], house : 1 - inputfiles["mess"]["active_duration"].iloc[0]}
                elif staff_type[i] == 3 :
                    daily_int_st = {residence_block_list[i] : 1 - inputfiles["common_areas"]["active_duration"].iloc[0], staff_dept[i] : inputfiles["common_areas"]["active_duration"].iloc[0], house : 1 - inputfiles["common_areas"]["active_duration"].iloc[0]}
                elif staff_type[i] == 4 :
                    daily_int_st = {residence_block_list[i] : 1 - inputfiles["common_areas"]["active_duration"].iloc[1], staff_dept[i] : inputfiles["common_areas"]["active_duration"].iloc[1], house : 1 - inputfiles["common_areas"]["active_duration"].iloc[1]}
                elif staff_type[i] == 5 :
                    daily_int_st = {residence_block_list[i] : 1 - inputfiles["common_areas"]["active_duration"].iloc[2], staff_dept[i] : inputfiles["common_areas"]["active_duration"].iloc[2], house : 1 - inputfiles["common_areas"]["active_duration"].iloc[2]}
                elif staff_type[i] == 6 :
                    daily_int_st = {residence_block_list[i] : 1 - inputfiles["common_areas"]["active_duration"].iloc[3], staff_dept[i] : inputfiles["common_areas"]["active_duration"].iloc[3], house : 1 - inputfiles["common_areas"]["active_duration"].iloc[3]}

            int_st.append(daily_int_st)
        staff_dict["interaction_strength"] = int_st 
        staff_dict["house"] = house
        individual.append(staff_dict)
        house += 1
    print("done.", flush=True)

   
    print("Instantiating family members", end=" .... ", flush=True)
    for i in range(len(inputfiles["staff"])):
        for j in range(adult_family_members[i]):
            family_dict = {}
            family_dict["id"] = len(individual)
            family_dict["Type"] = 3
            staff_dict["Hostel"] = 0
            family_dict["age"] = np.random.randint(31, 80)
            family_dict["Dept"] = 0
            family_dict["Periodicity"] = periodicity
            daily_int_st = {}
            int_st = []
            num_hours = np.random.choice([10/24, 1/24])
            for k in range(periodicity): 
                daily_int_st = {residence_block_list[i] : 1 -  num_hours, 0 : num_hours, individual[student_pop + len(Counter(faculty)) - 1 + i]["house"] : 1 - num_hours}
                int_st.append(daily_int_st)
            family_dict["interaction_strength"] = int_st
            family_dict["house"] = individual[student_pop + len(Counter(faculty)) - 1 + i]["house"]
            individual.append(family_dict)

        for j in range(num_children[i]):
            family_dict = {}
            family_dict["id"] = len(individual)
            family_dict["Type"] = 3
            staff_dict["Hostel"] = 0
            family_dict["age"] = np.random.randint(4, 18)
            family_dict["Dept"] = 0
            family_dict["Periodicity"] = periodicity
            daily_int_st = {}
            int_st = []
            num_hours = np.random.choice([7/24, 3/24])
            for k in range(periodicity): 
                daily_int_st = {residence_block_list[i] : 1 -  num_hours, 0 : num_hours, individual[student_pop + len(Counter(faculty)) - 1 + i]["house"] : 1 - num_hours}
                int_st.append(daily_int_st)
            family_dict["interaction_strength"] = int_st
            family_dict["house"] = individual[student_pop + len(Counter(faculty)) - 1 + i]["house"]
            individual.append(family_dict)
    print("done.", flush=True)

    print("Creating interaction_spaces.json...")

    num_dept = len(Counter(student_dept).keys())
    num_hostel = len(Counter(student_hostel).keys())
    num_mess = len(Counter(student_mess).keys())
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
        #print(i)
        i_space_mess["type"] = 3
        i_space_mess["beta"] = np.random.uniform(0,0.5) + 1
        i_space_mess["alpha"] = 1
        i_space_mess["active_duration"] = mess_active_duration[i + num_classes + num_hostel + 1]
        i_space_mess["avg_time"] = 0.125
        i_space_mess["lat"] = np.random.uniform(10.0,20.0)
        i_space_mess["lon"] = np.random.uniform(15.0,18.0)
        interaction_spaces.append(i_space_mess)
    print("Mess instantiated")

    if cafe :     
        for j in range(len(inputfiles["common_areas"])):   
            for i in range(inputfiles["common_areas"]["number"][j]):
                #i_space_cafe = {​​​​​}​​​​​
                i_space_cafe = {}
                i_space_cafe["id"] = i + inputfiles["common_areas"]["starting_id"][j]
                i_space_cafe["type"] = 4 + j
                i_space_cafe["beta"] = np.random.uniform(0,0.5) + 1
                i_space_cafe["alpha"] = 1
                i_space_cafe["active_duration"] = inputfiles["common_areas"]["active_duration"][j]
                i_space_cafe["avg_time"] = inputfiles["common_areas"]["average_time_spent"][j]
                i_space_cafe["lat"] = np.random.uniform(10.0,20.0)
                i_space_cafe["lon"] = np.random.uniform(15.0,18.0)
                interaction_spaces.append(i_space_cafe)

        print("Common areas instantiated")

    #TODO: When real data comes in, activate the next 3 lines 
    # res_list = inputfiles["staff"]["residence_block"].unique()
    # res_blocks = [i for i in res_list if i != 0]
    # num_res_blocks = len(res_blocks)

    num_res_blocks = len(inputfiles["staff"]["residence_block"].unique()) - 1
    

    print("Instantiating residential blocks")

    for i in range(num_res_blocks):
        # if residential_block[i] == 0:
        #     continue
        i_space_res_fac = {}
        # i_space_res_fac["id"] = inputfiles["staff"]["residence_block"].unique()[i]
        i_space_res_fac["id"] = len(interaction_spaces)
        i_space_res_fac["type"] = 8 
        i_space_res_fac["beta"] = np.random.uniform(0,0.5) + 1
        i_space_res_fac["alpha"] = 1
        i_space_res_fac["active_duration"] = 1
        i_space_res_fac["avg_time"] = 14/24
        i_space_res_fac["lat"] = np.random.uniform(10.0,20.0)
        i_space_res_fac["lon"] = np.random.uniform(15.0,18.0)
        interaction_spaces.append(i_space_res_fac)
    
    num_houses = house - start_house

    print("Instantiating houses")

    for i in range(num_houses):
        i_space_house = {}
        i_space_house["id"] = len(interaction_spaces)
        i_space_house["type"] = 9
        i_space_house["beta"] = np.random.uniform(0,0.5) + 1
        i_space_house["alpha"] = 1
        i_space_house["active_duration"] = 1
        i_space_house["avg_time"] = 14/24
        i_space_house["lat"] = np.random.uniform(10.0,20.0)
        i_space_house["lon"] = np.random.uniform(15.0,18.0)
        interaction_spaces.append(i_space_house)

    
        
    type_list = []
    for spaces in interaction_spaces:
        type_list.append(spaces["type"])
    
    #type_arr = np.unique(np.array(type_list))
    type_arr = list(range(10))
    names = []
    BETA = []
    
    for elem in type_arr:
        names.append(spaces_map[elem])
        BETA.append(1)
    
    ALPHA = 1

    trans_coeffs = transmission_coefficients(type_arr, BETA, ALPHA, names)

    return individual, interaction_spaces, trans_coeffs


if __name__ == "__main__":
    default_outPath = "./data/campus_data"
    default_inPath = "./data/campus_sample_data"

    my_parser = argparse.ArgumentParser(description='Instantiate a synthetic campus based on a set of input files descirbing the campus')
    my_parser.add_argument('-i', help='Set the path for the directory containing the input files (without / at the end)', default=default_inPath)
    my_parser.add_argument('-o', help='Set the path for the directory to store the output (without / at the end)', default=default_outPath)
    
    args = my_parser.parse_args()
    output_file_dir = args.o
    input_file_dir = args.i

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

 

    if not os.path.exists(output_file_dir):
        os.makedirs(output_file_dir)

    column_names = [i for i in range(24)]
    student_df = pd.read_csv(f'{input_file_dir}/{inputfiles["students"]}')
    class_df = pd.read_csv(f'{input_file_dir}/{inputfiles["class"]}')
    staff_df = pd.read_csv(f'{input_file_dir}/{inputfiles["staff"]}')
    mess_df = pd.read_csv(f'{input_file_dir}/{inputfiles["mess"]}')
    timetable_df = pd.read_csv(f'{input_file_dir}/{inputfiles["timetable"]}', header=None, names = column_names)
    common_areas_df = pd.read_csv(f'{input_file_dir}/{inputfiles["common_spaces"]}')

    input_dfs = {
        "students" : student_df,
        "class" : class_df,
        "staff" : staff_df,
        "mess" : mess_df,
        "timetable" : timetable_df,
        "common_areas" : common_areas_df
    }

    individuals, interaction_spaces, trans_coeffs = campus_parse(input_dfs)

    individual_json = open(f'{output_file_dir}/{outputfiles["individuals"]}', "w+")
    individual_json.write(json.dumps(individuals, cls = NpEncoder))
    individual_json.close
    print("individuals.json generated")

    interaction_spaces_json = open(f'{output_file_dir}/{outputfiles["interaction_spaces"]}', "w+")
    interaction_spaces_json.write(json.dumps(interaction_spaces, cls = NpEncoder))
    interaction_spaces_json.close
    print("interaction_spaces.json generated.")
