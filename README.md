# Markov Simulator for modeliing the spread of CoVID-19 in Indian demographics

This repository houses the source code used to develop the model for modelling the spread of CovID-19 in Indian demographics. This README document will help you familiarize yourself with the directory structure of the project and also provides the steps to run the simulator on your local machines. 

The source code for the simulator is organized into three major directories each of which represent one stage of the simulator's workflow.
The source code is still under development, and we shall be updating this README with more information as and when we have new modules developed.

```
|- .
  |- simulator/
  |- staticInst/
  |- visualizations/
  |- README.md
```


## Getting the source file
The source code is maintained on BitBucket, but the process for working with the source code is just like using Github

1. You clone this repository: `git clone https://<your bitbucket username>@bitbucket.org/iiscdsCov/markov_simuls.git`
2. Switch to the directory containing the repository: `cd markov_simuls`


## `StaticInst/` - Generates static files to instantiate a city based on Demographics data
The first stage of the simulator workflow is to generate static information required to instantiate a city.  To instantiate a Bangalore city with the mentioned configurations run the command

```python parse_and_instantiate.py -c city_name -n target_population -i inputPath -o outputPath```

The above script instantiates a synthetic city with the specified population where each individual being assigned to a house, school, workplace and community centre based on their age, and commute distance. The instantiated outputs are in the form of JSON files and will be available in the specified output directory.

A detailed description of the input files, the script and instructions to run are available at [staticInst/README.md](https://bitbucket.org/iiscdsCov/markov_simuls/src/master/staticInst/README.md)






## `simulator/` - running the simulation of CoVID-19 spread
The instantiated static files for Bangalore are now used to simulate the spread of the CoVID-19 infection spread based on a Markovian model. The design document contains detailed specifications about the mode,=l.

To set-up a running instance of the simulator, you would first need to switch to the directory `markov_simuls/simulator`
The simulator looks for the instantiated files for Bangalore in the simulator directory and thus copy the data files generated at `staticInst/data/bangalore` into `/input_files (root of the repository)`.

Now, we also need to setup a web-server to serve the simulator on the web-browser. This is done using the SimpleHTTPServer script avaialble by default in Python.

On Windows: `python -m http.server 8000`
On Linux:   `python -m SimpleHTTPServer 8000`

This sets up a web-server running locally on port 8000. This port number can changed to an available port number.
Once, the web-server is running, you can open your web browser and access `localhost:8000` or `127.0.0.1:8000` to access the simulator.

The simulator would first start by running the simulation on the background and outputs a `.csv` file which contains the number of people infected per ward.


## `visualizations/` - visualizing the infection spread over a period of time
We shall update this section of the README very soon..


