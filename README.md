# Campus Rakshak Simulator: An agent-based epidemic simulator for campuses

CampusSim is an agent-based simulator that models the disease's spread via various interaction spaces on the campus. It generates an interaction graph between agents and interaction spaces. Agents are assigned different interactions spaces such as classrooms, hostels, messes, and cafeterias. CampusSim is a parsimonious model that enables a fast and concise simulation of the epidemic spread. The simulator models intervention strategies (case isolation, hostel quarantine, etc.) and users can also define their own, custom interventions to simulate. The simulator is a derivative based out of the [IISc-TIFR City-Scale simulator](https://github.com/cni-iisc/epidemic-simulator). 

This project is carried out by the Centre for Networked Intelligence at the Indian Institute of Science, Bangalore and is funded as part of the Campus Rakshak project by the Department of Science and Technology, Govt. of India.

# Running the Simulator with Default Inputs

To run the simulator with default input files, then simply run the following shell script:

```shell
bash initial_simulation.sh
```

Then check the visualization section to obtain your plots.
It is recommended that you do this immediately after you clone the repository.

If you want to make custom simulations, follow the instructions given below sequentially.


# Instantiation of the campus

1. Go to the directory staticInst/
2. In order to create your own campus, you need the following csv files:
	1. classes.csv
	2. common_areas.csv
	3. mess.csv
	8. staff.csv
	7. student.csv
	6. timetable.csv
A sample of these files is present in /staticInst/data/campus_sample_data. Make sure to follow the format of the files perfectly.
3. Run the following command:
```python
python3 campus_parse_and_instantiate.py -i /data/campus_sample_data -o /data/campus_data
```
-i option allows you to specify input file path, -o option allows you to follow output file path
4. Then go to the directory /staticInst/data/campus_data. There you can change the parameters of the following input files:
	1. transmission_coefficients.json to change the crowding factor and contact rates of each type of interaction space.
	2. config.json to change various configuration parameters.
	3. intervention_params.json to change the default intervention policy.
	4. testing_protocol_001.json to change the testing protocol parameters.
5. The simulator offers the following interventions:
	1. Case Isolation
	2. Class Isolation
	3. Lockdown
	4. Selective Shutdown of Buildings
	5. Evacuation
	6. A Combination of two or more of the above

# Running the simulation

1. Go back to the root directory and then go to the directory cpp-simulator/
2. Run the following commands to build the simulator:
```cpp
make clean
make -f Makefile_np all
```
3. For one run of the simulation, run the following command:
```cpp
./drive_simulator --input_directory ../staticInst/data/campus_data --output_directory ../staticInst/data/campus_outputs --SEED_FIXED_NUMBER  --INIT_FIXED_NUMBER_INFECTED 100 --NUM_DAYS 120 --ENABLE_TESTING --testing_protocol_filename testing_protocol_001.json --intervention_filename intervention_params.json
```
4. To average multiple runs of the simulator (recommended), run the following command:
```shell
bash run_sims.sh
```

# Visualization

1. Go back to the root directory and go the the directory visualization/
2. Run the following command:
```python
python3 campus_plotting.py
```
3. You will find the plots in /staticInst/data/campus_outputs/campus_plots/. Look in the directory that specifies the date that you ran the simulations and rename the folder to your convenience.

## License
The source code provided in this repository is available for public-use under the [Apache2 License terms](License.md).
