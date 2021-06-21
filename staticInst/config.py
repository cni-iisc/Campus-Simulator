import json 
import argparse

default_outPath = './data/campus_data'

def configCreate(min_group_size, max_group_size, beta_scaling_factor, avg_num_assns, periodicity, minimum_hostel_time, kappa_class_case_isolation = 0.0, kappa_hostel_case_isolation = 0.2, kappa_mess_case_isolation = 0.1, kappa_cafe_case_isolation = 0.1, kappa_smaller_networks_case_isolation = 0.4, kappa_recreational_facility_case_isolation = 0.1, kappa_sports_facility_case_isolation = 0.1, kappa_residential_block_case_isolation = 0.6, kappa_house_case_isolation = 0.9,  kappa_lib_case_isolation = 0.1, kappa_class_lockdown = 0.0, kappa_hostel_lockdown = 0.2, kappa_mess_lockdown = 0.0, kappa_cafe_lockdown = 0.0, kappa_smaller_networks_lockdown = 0.0, kappa_lib_lockdown = 0.0, testing_capacity = 100):
    config = {}
    config["MIN_GROUP_SIZE"] = min_group_size
    config["MAX_GROUP_SIZE"] = max_group_size
    config["BETA_SCALING_FACTOR"] = beta_scaling_factor
    config["PERIODICITY"] = periodicity
    config["AVERAGE_NUMBER_ASSOCIATIONS"] = avg_num_assns
    config["minimum_hostel_time"] = minimum_hostel_time
    config["kappa_class_case_isolation"] = kappa_class_case_isolation
    config["kappa_hostel_case_isolation"] = kappa_hostel_case_isolation
    config["kappa_mess_case_isolation"] = kappa_mess_case_isolation
    config["kappa_cafe_case_isolation"] = kappa_cafe_case_isolation
    config["kappa_smaller_networks_case_isolation"] = kappa_smaller_networks_case_isolation
    config["kappa_recreational_facility_case_isolation"] = kappa_recreational_facility_case_isolation
    config["kappa_sports_facility_case_isolation"] = kappa_sports_facility_case_isolation
    config["kappa_house_case_isolation"] = kappa_house_case_isolation
    config["kappa_residential_block_case_isolation"] = kappa_residential_block_case_isolation
    config["kappa_lib_case_isolation"] = kappa_lib_case_isolation
    config["kappa_class_lockdown"] = kappa_class_lockdown
    config["kappa_hostel_lockdown"] = kappa_hostel_lockdown
    config["kappa_mess_lockdown"] = kappa_mess_lockdown
    config["kappa_cafe_lockdown"] = kappa_cafe_lockdown
    config["kappa_smaller_networks_lockdown"] = kappa_smaller_networks_lockdown
    config["kappa_lib_lockdown"] = kappa_lib_lockdown
    config["testing_capacity"] = testing_capacity

    return [config]

if __name__ == "__main__":
    my_parser = argparse.ArgumentParser(description='Create mini-city for COVID-19 simulation')
    my_parser.add_argument('-o', help='A', default=default_outPath)
    args = my_parser.parse_args()
    output_file_dir = args.o

    min_group_size = 10
    max_group_size = 15
    beta_scaling_factor = 5
    periodicity = 7
    avg_num_assns = 5
    min_hostel_time = 0.3
    kappa_class_case_isolation = 0.0
    kappa_hostel_case_isolation = 0.2
    kappa_mess_case_isolation = 0.1
    kappa_cafe_case_isolation = 0.1
    kappa_smaller_networks_case_isolation = 0.4 
    kappa_recreational_facility_case_isolation = 0.1 
    kappa_sports_facility_case_isolation = 0.1 
    kappa_residential_block_case_isolation = 0.6
    kappa_house_case_isolation = 0.9
    kappa_class_lockdown = 0.0
    kappa_hostel_lockdown = 0.2
    kappa_mess_lockdown = 0.0
    kappa_cafe_lockdown = 0.0
    kappa_smaller_networks_lockdown = 0.0
    testing_capacity = 100

    config = configCreate(min_group_size, max_group_size, beta_scaling_factor, avg_num_assns, periodicity, min_hostel_time)

    f = open(f"{ output_file_dir }/config.json", "w")
    f.write(json.dumps(config))
    f.close()
