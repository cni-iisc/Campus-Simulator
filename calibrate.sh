chrome_dir="Chrome_standalone/ChromePortableGCPM"
if [ ! -d $chrome_dir ]; then
    echo "Creating standalone chrome"
    cd Chrome_standalone/
    chmod +x GCPM
    ./GCPM google-chrome-stable_current_amd64.deb
    cd ..
fi	

PORT=9000
python3 -m http.server $PORT &> /dev/null &
pid=$!

echo "Serving on port $PORT"

# Give server time to start up
sleep 1

echo "Running the calibration scripts"
python3 simulator/python_scripts/tune_model.py | tee run_log.txt

# Stop server
kill "${pid}"

archive_dir='../../Archive'
if [ ! -d $archive_dir ]; then
    echo "Creating archive directory"
    mkdir $archive_dir
fi

sim_dir='../../sim_data'
if [ ! -d ${sim_dir} ]; then
    echo "Creating archive directory"
    mkdir ${sim_dir}
fi

sim_file="./data/combined_plot_log_scale.png"
if [ -f $sim_file ]; then
    echo "Moving files to archive"
     
    backup_dir=$(date +'%d_%m_%Y_%H_%M')
    mkdir ./Archive/${backup_dir}
    mkdir ./Archive/${backup_dir}'/Results'
    
    echo "Moving run log, csv files and mean data to archive"
    mv simulator/python_scripts/run_log.txt ./Archive/${backup_dir}/Results/
    cp -r simulator/python_scripts/data/* ./Archive/${backup_dir}/Results/
    
    cp -r ./sim_data/* ./Archive/${backup_dir}/
fi
