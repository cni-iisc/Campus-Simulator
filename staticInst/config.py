import json 

markov_simuls = True

config_json = []
def configCreate(min_group_size, max_group_size, beta_scaling_factor, avg_num_assns, periodicity):
    config = {}
    config["MIN_GROUP_SIZE"] = min_group_size
    config["MAX_GROUP_SIZE"] = max_group_size
    config["BETA_SCALING_FACTOR"] = beta_scaling_factor
    config["PERIODICITY"] = periodicity
    config["AVERAGE_NUMBER_ASSOCIATIONS"] = avg_num_assns
    config_json.append(config)

    return config_json

if __name__ == "__main__":

    output_file = "config.json"

    if markov_simuls: 
        output_file_dir = "/Users/Minhaas/CODING/iisc/campus_simulator/markov_simuls/staticInst/data/campus_data/"
    else :
        output_file_dir = "/Users/Minhaas/CODING/iisc/rough/campus_input_csv/json_files/"

    min_group_size = 10
    max_group_size = 15
    beta_scaling_factor = 10
    periodicity = 7
    avg_num_assns = 5

    config = configCreate(min_group_size, max_group_size, beta_scaling_factor, avg_num_assns, periodicity)

    f = open(output_file_dir + output_file, "w+")
    f.write(json.dumps(config))
    f.close()


    

