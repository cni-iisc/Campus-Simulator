To run the simulator, you need:

1. a recent version of the `g++` compiler (6.1 or later) or `clang` (I
think 4 or later works, but have not tested), and optionally `openmp`
(this is to enable parallelization, and can be disabled by removing
the `-fopenmp` option from the `Makefile`, in case you do not have it
installed),

2. The `make` program.

3. The `bash` shell.

On windows, the easiest ways to get these things is to install `msys2`
shell from [here](https://www.msys2.org/).  Let me know if you need
help.  On Mac and Linux, these are typically already there, or else
are easy to install with a package manager.

You can now compile the code in the cpp_simulator directory by running:

```
make all
```

This will generate an executable file called `drive_simulator` in the
same directory.


Now  to actually run the code, you will have to edit the file
`drive_simulator_temp.sh` in the same directory.  Here, you can specify

1) All the input parameters

2) The directory where the output will be generated (The
`output_directory` parameter in the file).

Once you have fixed the prameters, you can run the script as

```
bash drive_simulator_temp.sh
```

in the `cpp-simulator` directory.  If everything works, it will end
with a message indicating success after a little while (~25s on my
computer with 1,00,000 agents).

The output can now be seen by using your favorite browser to open the
`plots.html` file in the output directory you specified above.  There
will be one ouput directory per intervention, containing the plots
just for that intervention.  In addition, there will also be a top
level directory with plots which plot the observable variables for all
the interventions on the same plot.

