import pandas as pd

input_file_path = "./data/campus_outputs/"

fatality_df = pd.read_csv(input_file_path + "num_fatalities.csv")
fatality_df["cumulative_fatalities"] = fatality_df["num_fatalities"].cumsum()
fatality_df.to_csv(input_file_path + "num_fatalities.csv", index = false)

recovered_df = pd.read_csv(input_file_path + "num_recovered.csv")
recovered_df["cumulative_recovered"] = fatality_df["num_recovered"].cumsum()
recovered_df.to_csv(input_file_path + "num_recovered.csv", index = false)

testing_df = pd.read_csv(input_file_path + "disease_label_stats.csv")
fatality_df["cumulative_tests"] = fatality_df["requested_tests"].cumsum()
fatality_df["daily_positive"] = fatality_df["cumulative_positive_tests"].diff()
fatality_df.to_csv(input_file_path + "disease_label_stats.csv", index = false)