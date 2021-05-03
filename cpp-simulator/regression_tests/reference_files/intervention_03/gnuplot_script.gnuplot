set datafile separator ','
set key autotitle columnhead
set term png
set termoption noenhanced
set output "num_affected.png"
set title "num_affected"
plot "num_affected.csv" using 1:2 with lines
set output "num_cases.png"
set title "num_cases"
plot "num_cases.csv" using 1:2 with lines
set output "num_critical.png"
set title "num_critical"
plot "num_critical.csv" using 1:2 with lines
set output "num_cumulative_hospitalizations.png"
set title "num_cumulative_hospitalizations"
plot "num_cumulative_hospitalizations.csv" using 1:2 with lines
set output "num_cumulative_infective.png"
set title "num_cumulative_infective"
plot "num_cumulative_infective.csv" using 1:2 with lines
set output "num_exposed.png"
set title "num_exposed"
plot "num_exposed.csv" using 1:2 with lines
set output "num_fatalities.png"
set title "num_fatalities"
plot "num_fatalities.csv" using 1:2 with lines
set output "num_hospitalised.png"
set title "num_hospitalised"
plot "num_hospitalised.csv" using 1:2 with lines
set output "num_infected.png"
set title "num_infected"
plot "num_infected.csv" using 1:2 with lines
set output "num_recovered.png"
set title "num_recovered"
plot "num_recovered.csv" using 1:2 with lines
set output "num_symptomatic.png"
set title "num_symptomatic"
plot "num_symptomatic.csv" using 1:2 with lines
