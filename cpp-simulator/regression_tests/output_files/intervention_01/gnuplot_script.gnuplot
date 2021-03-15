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
set output "num_fatalities.png"
set title "num_fatalities"
plot "num_fatalities.csv" using 1:2 with lines
set output "num_recovered.png"
set title "num_recovered"
plot "num_recovered.csv" using 1:2 with lines
