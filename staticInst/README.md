# README #
This directory contains the mined, cleaned data for each city that is used to create the instantiations for the Markov chain simulator. 
The instantiated parameters are passed as JSON objects to the simulator.

### To instantiate a city, execute the following commad from the staticInst directory:
python parse_and_instantiate.py -c city_name -n target_population -i inputPath -o outputPath

The default parameters are:
city_name = 'bangalore'
target_population = 100000
inputPath = './data/base/bangalore/'
outputPath = './data/bangalore-100K/'

The parse_and_instantiate.py script outputs the JSON files necessary for the simulator to run, and validation 
plots that show the empirical distributions of age, household size, school size, workplace size and commuter distance 
in the synthetic city.

### Sub-Directory Structure
The sub-directory structure followed for storing and processing of static data source used for instantiations is outlined as follows. 


```
|- modules/                          #modules for data processing and adaptation of algorithms in legacy/
|- legacy/                           #original code for instantiations
|- data/
   |- base/                         # Raw data for each city
      |- bangalore/                  # Data for Banglore City
         |- demographics.csv       # demographic data about each ward 
         |- households_and_age.json # age and household-size distributions
         |- common_areas.csv       # location of places where people congrugate
         |- city.geojson       # geographic boundaries of wards
         |- employment.csv    # census data on employed people
	 |- cityProfile.json       # processed demographic data dump for all wards
      |- chennai/
   |      
   |- bangalore/      # Data used for instantiating banglore city
      |- workplaces.json    # instantiation of workplaces
      |- commonArea.json  # instantiation of commonplaces
      |- households.json    # instantiation of households
      |- schools.json       # instantiation of schools
      |- individuals.json       # instantiation of individuals
      |- map.geojson   # instantiation of individuals
```

**Suggestion**: For consistency, we can have naming convention in lowercase-only with underscore separators. 

### Data Needs
The following table lists the data needs for the instantiation script to run 

| Dataset Description| Required Fields in the Data | File Format |
|--------------------|-----------------------------|-------------|
|Ward boundaries of a city | ward no, ward name, geometry(Multipolygon Geometry) | GeoJSON|
|Age Distribution (bins of 5), household size distribution for the  city| -  |JSON|
|Demographic data for all wards of the city|ward no, total population in the ward, area of the ward, total number of households per ward | CSV |
|Employment data for all wards of the city|ward no, total population employed, total population unemployed| CSV |
|Common Areas (or) Points of Interest in the City| latitude, longitude of common areas like transport communities, markets, restaurants, places of worship) per ward (if possible) | CSV|

The above data for Bangalore and Mumabi have been processesd and stored in './data/base/bangalore' and './data/base/mumbai' respectively.