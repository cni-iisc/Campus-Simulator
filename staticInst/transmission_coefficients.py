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
    for type in range(len(interaction_type_list)):
        print(f"Generating transmission coefficents for {interaction_type[type]}...")
        transmission_json = {
            "type" : interaction_type_list[type], 
            "beta" : BETA[type],
            "alpha" : ALPHA,
            "name" : NAMES[type]
        }
        print(f"{interaction_type[type]} done.")
        transmission_coeff.append(transmission_json)
    return transmission_coeff

if __name__ == "__main__":
    output_file = "transmission_coefficients.json"
    input = "/Users/minhaas/CODING/iisc/campus_simulator/staticInst/data/campus_data/"
    interactionJson = pd.read_json(input + "interaction_spaces.json")

    interaction_type_list = interactionJson.type.unique()

    if markov_simuls: 
        output_file_dir = "/Users/Minhaas/CODING/iisc/campus_simulator/staticInst/data/campus_data/"
    else :
        output_file_dir = "/Users/Minhaas/CODING/iisc/rough/campus_input_csv/json_files/"

    BETA_CLASSROOM = np.random.uniform(0,0.5)
    BETA_MESS = np.random.uniform(0,0.5)
    BETA_HOSTEL = np.random.uniform(0,0.5)
    BETA_CAFE = np.random.uniform(0,0.5)
    BETA_LIBRARY = np.random.uniform(0,0.5)
    BETA_REC_FAC = np.random.uniform(0,0.5)
    BETA_SPORTS_FAC = np.random.uniform(0,0.5)
    BETA_RES_BLOCK = np.random.uniform(0,0.5)
    BETA_HOUSE = np.random.uniform(0,0.5)
    BETA_DAY_SCHOLAR = 0
    BETA = [BETA_DAY_SCHOLAR, BETA_CLASSROOM, BETA_HOSTEL, BETA_MESS, BETA_CAFE, BETA_LIBRARY, BETA_SPORTS_FAC, BETA_REC_FAC, BETA_RES_BLOCK, BETA_HOUSE]
    ALPHA = 1

    if not os.path.exists(output_file_dir):
        os.makedirs(output_file_dir)

    interaction_type = {
        0 : "Day Scholar", 
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
    
    NAMES = []
    for key in interaction_type.keys() : 
        NAMES.append(interaction_type[key])

    transmission_coefficients_json = transmission_coefficients(interaction_type_list, BETA, ALPHA, NAMES)
    f = open(output_file_dir+output_file, "w+")
    f.write(json.dumps(transmission_coefficients_json, cls= NpEncoder))
    f.close




