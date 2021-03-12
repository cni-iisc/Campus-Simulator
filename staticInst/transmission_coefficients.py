import json
import os
import pandas as pd
import numpy as np

markov_simuls = True

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

def transmission_coefficients(interaction_type_list, BETA, ALPHA, NAMES):
    transmission_coeff = []
    transmission_json = {}
    print(len(interaction_type_list))
    for type in range(len(interaction_type_list)):
    # print(f"Generating transmission coefficents for {interaction_type[type]}...")
        transmission_json = {
            "type" : interaction_type_list[type], 
            "beta" : BETA[type],
            "alpha" : ALPHA,
            "name" : NAMES[type]
        }
        print(type)
        # //print(f"{interaction_type[type]} done.")
        transmission_coeff.append(transmission_json)
    return transmission_coeff

if __name__ == "__main__":
    output_file = "transmission_coefficients.json"
    input = "/Users/Minhaas/CODING/iisc/rough/campus_input_csv/json_files/"
    interactionJson = pd.read_json(input + "interaction_spaces.json")

    interaction_type_list = interactionJson.type.unique()
    print(interaction_type_list)

    if markov_simuls: 
        output_file_dir = "/Users/Minhaas/CODING/iisc/campus-simulator/markov_simuls/staticInst/data/campus_data/"
    else :
        output_file_dir = "/Users/Minhaas/CODING/iisc/rough/campus_input_csv/json_files/"

    BETA_CLASSROOM = np.random.uniform(0,0.5) + 1
    BETA_MESS = np.random.uniform(0,0.5) + 1
    BETA_HOSTEL = np.random.uniform(0,0.5) + 1 
    BETA_DAY_SCHOLAR = 0
    BETA = [BETA_DAY_SCHOLAR, BETA_CLASSROOM, BETA_HOSTEL, BETA_MESS]
    ALPHA = 1

    if not os.path.exists(output_file_dir):
        os.makedirs(output_file_dir)

    interaction_type = {
        0 : "Day Scholar", 
        1 : "Classroom", 
        2 : "Hostel", 
        3 : "Mess"
    }
    transmission_coefficients_json = transmission_coefficients(interaction_type_list, BETA, ALPHA)
    f = open(output_file_dir+output_file, "w+")
    f.write(json.dumps(transmission_coefficients_json, cls= NpEncoder))
    f.close




