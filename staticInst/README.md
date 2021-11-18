Generates static files to instantiate a synthetic campus

To instantiate a campus, the following files are needed:
1. student.csv
2. classes.csv
3. mess.csv
4. staff.csv
5. timetable.csv
6. common areas.csv
A sample of these files can be found in "./data/campus_sample_data/"

To instantiate a synthetic version of the desired college/university campus, run the script "campus_parse_and_instantiate.py" on the command line:

`python3 campus_parse_and_instantiate.py -i <Input file path> -o <Output file path>`

This will output two json files: individuals.json and interaction_spaces.json which can be found by default in ./data/campus_data

NOTE: If you're running this on a terminal, please make the following edit to campus_parse_and_instantiate.py:
The line: from .transmission_coefficients import transmission_coefficients
Changes to: from transmission_coefficients import transmission_coefficients