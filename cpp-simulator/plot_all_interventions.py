import pandas, matplotlib
from collections import defaultdict
from pathlib import Path
import sys

output_base = sys.argv[1]
def intervention(i):
    return str(i)

var_names = ["num_infected",
	 "num_exposed",
	 "num_hospitalised",
	 "num_critical",
	 "num_fatalities",
	 "num_recovered",
	 "num_affected"
]

dfs = defaultdict(list)

for var_name in var_names:
    for INTERVENTION in range(0, 8):
        dfs[var_name].append(pandas.read_csv(Path(output_base, f"intervention_{INTERVENTION}", f"{var_name}.csv")))

combined_dfs = {}
for var_name in var_names:
    df = dfs[var_name][0];
    df.rename(columns={var_name: intervention(0)}, inplace = True)
    for i in range(1, 8):
        df[intervention(i)] = dfs[var_name][i][var_name]

    ax = df.plot(x = "Time", title = var_name)
    output_path = Path(output_base, f"{var_name}.png")
    print(f"Saving {var_name} plot in {output_path}", flush=True)
    ax.figure.savefig(output_path)



