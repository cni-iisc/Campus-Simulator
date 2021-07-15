To run the simulator, you need:

1. a recent version of the `g++` compiler (version 7 or later) or `clang`
(version 11 or later should work, though we have have not tested), and
optionally `openmp` (this is to enable parallelization, and can be disabled
easily, see below in the compiling section if your installation does not provide
it),

2. The `make` program.

3. The `bash` shell.

4. The `gnuplot` program for generating some of the plots.

5. `python` pointing to a `python3` executable with `matplotlib` and `pandas`
   installed, in your execution `PATH`. (Otherwise, you will have to edit the
   `python` invocation in the shell script to point to such a `python3`
   installation.)  See the `README.md` [file](../README.md) in the root of the
   source distribution directory for one way to obtain such a `python`
   environment.

On windows, the easiest way to get these is to install `msys2` shell
from [here](https://www.msys2.org/).  However, it may be tricky to get
an MSYS installation with a `python3` capable of installing `pandas`.
You might then have to point `MSYS` to actually use your Windows
`python3` installation. Let me know if you need help.

On Mac and Linux, most of the above programs are typically already there, or
else are easy to install with a package manager.


## Compiling the code

You can now compile the code in the `cpp_simulator` directory by running:

```
make all
```

However, if you do not want to use, or do not have, the `openmp`
parallel libraries, you should instead run

```
make -f Makefile_np all
```


This will generate an executable file called `drive_simulator` in the
same directory.  This is the simulator program.


## Running the code

Please proceed with this step only after compiling the code, as described in the
previous step.

There are two ways to run the simulator.  However, before running it, you will
need to generate the input files for the simulator (corresponding to the city
you want to simulate).  Instructions for doing this are in the README.md
[file](../README.md) in the root installation directory.

### Method 1: Running `drive_simulator` directly

For this, first run the following command

```
./drive_simulator --input_directory ../staticInst/data/campus_data --output_directory ../staticInst/data/campus_outputs --SEED_FIXED_NUMBER  --INIT_FIXED_NUMBER_INFECTED 100 --NUM_DAYS 120 --ENABLE_TESTING --testing_protocol_filename testing_protocol_001.json --intervention_filename intervention_params.json
```

and examine the various options available.  In particular, you need to specify
the `input_directory` as the directory were you generated the input files for
the city you want to simulate.  For more details on the other parameters
(especially the BETA parameters) see the description in the write-up.

You can now run the program after providing the parameters at the command line
in the usual way.  Options that you do not specify will be set to their default
values as described in the output of the command `./drive_simulator -h` you ran
above.  Note that the `intervention_filename` which is a `JSON file` is the required file in which the interventions can be enabled/disabled for a period of time.

| Interventions supported|
| :-- |
| no_intervention|
| case_isolation |
| class_isolation |
| lockdown (or) campus shutdown |
| selective shutdown of interaction spaces |
| evactuation of the campus |

Check ../staticInst/data/campus_data to see the different intervention files and modify them.



The output of the program will be generated in the `output_directory` that you
provide.  It consists of various CSV files (with human readable names)
containing time series data of various observables in the model.