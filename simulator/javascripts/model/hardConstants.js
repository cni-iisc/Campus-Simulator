
const WEBPAGE_VERSION = true;
//simulation inputs


NUM_DAYS = 120; //Number of days. Simulation duration
SIM_STEPS_PER_DAY = 4; //Number of simulation steps per day.
NUM_TIMESTEPS = NUM_DAYS * SIM_STEPS_PER_DAY; //
INIT_FRAC_INFECTED = 0.001; // Initial number of people infected
SEED_SCALING_FACTOR = 1.5;
SEED_WARD_FRACTION_UNIFORM = true
const RANDOM_SEEDING_WARDWISE = 0;
const SEED_FROM_INDIVIDUALS_JSON = 1;
const SEED_INFECTION_RATES = 2;
const SEED_EXP_RATE = 3;
SEEDING_MODE = RANDOM_SEEDING_WARDWISE;

// Seeding parameters for exp rate seeding
SEEDING_START_DATE = 0; // When to start seeding (with respect to simulator time)
SEEDING_DURATION = 22;  // For how long to seed (days) (March 1 - March 23)
SEEDING_DOUBLING_TIME = 4.18;	// Days after which the number of seeds double.
SEEDING_RATE_SCALE = 1;
CALIB_NO_INTERVENTION_DURATION = 24; // Lockdown starts from March 25
CALIB_LOCKDOWN_DURATION = 21;

//global variables.
NUM_PEOPLE = 100000; // Number of people. Will change once file is read.
NUM_HOMES = 25000; //Will change once file is read.
NUM_WORKPLACES = 5000; //Will change once file is read.
NUM_COMMUNITIES = 198; //Will change once file is read.
NUM_PUBLIC_TRANSPORT = 1;
NUM_SCHOOLS = 0;
NUM_DISEASE_STATES = 7; //0-S, 1-E, 2-I, 3-Symp,4-R, 5-H, 6-C, 7-D

//Various interventions. These will need to be generalised soon.
CALIBRATION_DELAY = 0; //// Assuming the simulator starts on March 1.
NUM_DAYS_BEFORE_INTERVENTIONS = 24 + CALIBRATION_DELAY;
const CALIBRATION = -1
const NO_INTERVENTION = 0
const CASE_ISOLATION = 1
const HOME_QUARANTINE = 2
const LOCKDOWN = 3
const CASE_ISOLATION_AND_HOME_QUARANTINE = 4
const CASE_ISOLATION_AND_HOME_QUARANTINE_SD_70_PLUS = 5
const LOCKDOWN_21_CI_HQ_SD_70_PLUS_21_CI = 6
const LOCKDOWN_21 = 7
const LD_21_CI_HQ_SD70_SC_21_SC_42 = 8
const LD_21_CI_HQ_SD70_SC_21 = 9
const LD_21_CI_HQ_SD70_SC_OE_30 = 10

const HOME_QUARANTINE_DAYS = 14
const SELF_ISOLATION_DAYS = 7

INTERVENTION = NO_INTERVENTION; //run_and_plot() changes this

//Disease progression in an individual
const SUSCEPTIBLE = 0
const EXPOSED = 1
const PRE_SYMPTOMATIC = 2
const SYMPTOMATIC = 3
const RECOVERED = 4
const HOSPITALISED = 5
const CRITICAL = 6
const DEAD = 7

let csvContent = "data:text/csv;charset=utf-8,"; //for file dump

INCUBATION_PERIOD = 2.3
MEAN_ASYMPTOMATIC_PERIOD = 0.5
MEAN_SYMPTOMATIC_PERIOD = 5
MEAN_HOSPITAL_REGULAR_PERIOD = 8
MEAN_HOSPITAL_CRITICAL_PERIOD = 8
//These are parameters associated with the disease progression
const NUM_DAYS_TO_RECOG_SYMPTOMS = 1;
const INCUBATION_PERIOD_SHAPE = 2;


INCUBATION_PERIOD_SCALE = INCUBATION_PERIOD * SIM_STEPS_PER_DAY; // 2.29 days
ASYMPTOMATIC_PERIOD = MEAN_ASYMPTOMATIC_PERIOD * SIM_STEPS_PER_DAY; // half a day
SYMPTOMATIC_PERIOD = MEAN_SYMPTOMATIC_PERIOD * SIM_STEPS_PER_DAY; // 5 days
HOSPITAL_REGULAR_PERIOD = MEAN_HOSPITAL_REGULAR_PERIOD * SIM_STEPS_PER_DAY;
HOSPITAL_CRITICAL_PERIOD = MEAN_HOSPITAL_CRITICAL_PERIOD * SIM_STEPS_PER_DAY;
SYMPTOMATIC_FRACTION = 0.67;

COMMUNITY_INFECTION_PROB = [];

const WTYPE_HOME = 0;
const WTYPE_OFFICE = 1;
const WTYPE_SCHOOL = 2;


NUM_AGE_GROUPS = 16;
USE_AGE_DEPENDENT_MIXING = false;

SIGMA_OFFICE = math.zeros([NUM_AGE_GROUPS]);
U_MATRIX_OFFICE = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);
V_MATRIX_OFFICE = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);
SIGNIFICANT_EIGEN_VALUES_OFFICE = 16; //NUM_AGE_GROUPS;

SIGMA_SCHOOL = math.zeros([NUM_AGE_GROUPS]);
U_MATRIX_SCHOOL = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);
V_MATRIX_SCHOOL = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);
SIGNIFICANT_EIGEN_VALUES_SCHOOL = 16; //NUM_AGE_GROUPS;

SIGMA_H = math.zeros([NUM_AGE_GROUPS]);
U_MATRIX_H = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);
V_MATRIX_H = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);
SIGNIFICANT_EIGEN_VALUES_H = 16; //NUM_AGE_GROUPS;

const PUBLIC_TRANSPORT_FRACTION = 0.5;

//age related transition probabilities, symptomatic to hospitalised to critical to fatality.
STATE_TRAN = [
    [0.0010000, 0.0500000, 0.4000000],
    [0.0030000, 0.0500000, 0.4000000],
    [0.0120000, 0.0500000, 0.5000000],
    [0.0320000, 0.0500000, 0.5000000],
    [0.0490000, 0.0630000, 0.5000000],
    [0.1020000, 0.1220000, 0.5000000],
    [0.1660000, 0.2740000, 0.5000000],
    [0.2430000, 0.4320000, 0.5000000],
    [0.2730000, 0.7090000, 0.5000000]
]


NUM_AFFECTED_COUNT = 0;

// Beta values
BETA_H = 0.67; //0.47 *1.0 //Thailand data
BETA_W = 0.50; //0.47 *1.0//Thailand data
BETA_S = 1.00; //0.94 *1.0//Thailand data
BETA_C = 0.15; //0.097*1// Thailand data. Product  = 0.47
BETA_PT = 0;

ALPHA = 0.8 //exponent of number of people in a household while normalising infection rate in a household.

var SIMULATION_STOP = false;
const RADIUS_EARTH = 6371; ///km

const plot_minRanges = {
    'num_affected_plot_2' : 100,
    'num_infected_plot_2': 50,
    'num_hospitalised_plot_2': 20,
    'num_critical_plot_2': 10,
    'num_fatalities_plot_2': 5
};
