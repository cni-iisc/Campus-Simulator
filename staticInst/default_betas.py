import pandas as pd
from .transmission_coefficients import transmission_coefficients
import json

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

def default_betas(campus_df):
	interaction_type = {
		0 : "outside_campus", 
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

	interaction_type_list = list(interaction_type.keys())

	alpha = 1

	betas = []

	if campus_df['name'][0] == "iitjodhpur":
		betas.append(0)
		betas.append(campus_df['beta_classroom'][0])
		betas.append(campus_df['beta_hostel'][0])
		betas.append(campus_df['beta_hostel'][0]*0.16)
		betas.append(campus_df['beta_classroom'][0]*0.2)
		betas.append(campus_df['beta_classroom'][0]*0.2)
		betas.append(campus_df['beta_classroom'][0]*0.2)
		betas.append(campus_df['beta_classroom'][0]*0.2)
		betas.append(campus_df['beta_residential_block'][0])
		betas.append(campus_df['beta_residential_block'][0]*9)
		
	elif campus_df['name'][0] == "iiithyderabad":
		betas.append(0)
		betas.append(campus_df['beta_classroom'][0])
		betas.append(campus_df['beta_hostel'][0])
		betas.append(campus_df['beta_hostel'][0]*0.33)
		betas.append(campus_df['beta_classroom'][0]*0.2)
		betas.append(campus_df['beta_classroom'][0]*0.2)
		betas.append(campus_df['beta_classroom'][0]*0.29)
		betas.append(campus_df['beta_classroom'][0]*0.2)
		betas.append(campus_df['beta_residential_block'][0])
		betas.append(campus_df['beta_residential_block'][0]*9)

	else:
		print("wrong campus")

	t_c = transmission_coefficients(interaction_type_list, betas, alpha, NAMES)
	return t_c

if __name__ == "__main__":
	output_file = "./data/campus_data/transmission_coefficients.json"
	campus_df = pd.read_csv("./data/campus_sample_data/campus_setup.csv")
	calibrated_betas = default_betas(campus_df)
	f = open(output_file, "w")
	f.write(json.dumps(calibrated_betas, cls= NpEncoder))
	f.close
