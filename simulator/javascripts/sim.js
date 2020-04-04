//Copyright [2020] [Indian Institute of Science, Bangalore]
//SPDX-License-Identifier: Apache-2.0

const WEBPAGE_VERSION = true;
//simulation inputs

NUM_DAYS = 120; //Number of days. Simulation duration
SIM_STEPS_PER_DAY = 4; //Number of simulation steps per day.
NUM_TIMESTEPS = NUM_DAYS*SIM_STEPS_PER_DAY; //
INIT_FRAC_INFECTED = 0.0001; // Initial number of people infected
const SEED_INFECTION_FROM_FILE = false;

//global variables. 
NUM_PEOPLE = 100000; // Number of people. Will change once file is read.
NUM_HOMES = 25000; //Will change once file is read.
NUM_WORKPLACES = 5000; //Will change once file is read.
NUM_COMMUNITIES = 198; //Will change once file is read.
NUM_SCHOOLS = 0;
NUM_DISEASE_STATES = 7; //0-S, 1-E, 2-I, 3-Symp,4-R, 5-H, 6-C, 7-D

//Various interventions. These will need to be generalised soon.
const NO_INTERVENTION = 0
const CASE_ISOLATION = 1
const HOME_QUARANTINE = 2
const LOCKDOWN = 3
const CASE_ISOLATION_AND_HOME_QUARANTINE = 4
const CASE_ISOLATION_AND_HOME_QUARANTINE_SD_70_PLUS = 5
const LOCKDOWN_21_CI_HQ_SD_70_PLUS_21_CI = 6
const LOCKDOWN_21 = 7

const HOME_QUARANTINE_DAYS = 14
const SELF_ISOLATION_DAYS = 7

INTERVENTION = NO_INTERVENTION; //run_and_plot() changes this

//Disease progression in an individual
const SUSCEPTIBLE = 0
const EXPOSED = 1
const INFECTIVE = 2
const SYMPTOMATIC = 3
const RECOVERED = 4
const HOSPITALISED = 5
const CRITICAL = 6
const DEAD = 7

let csvContent = "data:text/csv;charset=utf-8,"; //for file dump

INCUBATION_PERIOD = 2.25
MEAN_ASYMPTOMATIC_PERIOD = 0.5
MEAN_SYMPTOMATIC_PERIOD = 5
MEAN_HOSPITAL_REGULAR_PERIOD = 8
MEAN_HOSPITAL_CRITICAL_PERIOD = 8
//These are parameters associated with the disease progression
const NUM_DAYS_TO_RECOG_SYMPTOMS = 1;
const INCUBATION_PERIOD_SHAPE = 2;


INCUBATION_PERIOD_SCALE = INCUBATION_PERIOD*SIM_STEPS_PER_DAY; // 2.29 days
ASYMPTOMATIC_PERIOD = MEAN_ASYMPTOMATIC_PERIOD*SIM_STEPS_PER_DAY; // half a day
SYMPTOMATIC_PERIOD = MEAN_SYMPTOMATIC_PERIOD*SIM_STEPS_PER_DAY; // 5 days
HOSPITAL_REGULAR_PERIOD = MEAN_HOSPITAL_REGULAR_PERIOD*SIM_STEPS_PER_DAY;
HOSPITAL_CRITICAL_PERIOD = MEAN_HOSPITAL_CRITICAL_PERIOD*SIM_STEPS_PER_DAY;
SYMPTOMATIC_FRACTION = 0.67;

COMMUNITY_INFECTION_PROB=[];

const WTYPE_HOME = 0;
const WTYPE_OFFICE = 1;
const WTYPE_SCHOOL = 2;



NUM_AGE_GROUPS = 16;
USE_AGE_DEPENDENT_MIXING = false;

SIGMA_OFFICE = math.zeros([NUM_AGE_GROUPS]);
U_MATRIX_OFFICE = math.zeros([NUM_AGE_GROUPS,NUM_AGE_GROUPS]);
V_MATRIX_OFFICE = math.zeros([NUM_AGE_GROUPS,NUM_AGE_GROUPS]);
SIGNIFICANT_EIGEN_VALUES_OFFICE = 16 ; //NUM_AGE_GROUPS;

SIGMA_SCHOOL = math.zeros([NUM_AGE_GROUPS]);
U_MATRIX_SCHOOL = math.zeros([NUM_AGE_GROUPS,NUM_AGE_GROUPS]);
V_MATRIX_SCHOOL = math.zeros([NUM_AGE_GROUPS,NUM_AGE_GROUPS]);
SIGNIFICANT_EIGEN_VALUES_SCHOOL = 16; //NUM_AGE_GROUPS;

SIGMA_H = math.zeros([NUM_AGE_GROUPS]);
U_MATRIX_H = math.zeros([NUM_AGE_GROUPS,NUM_AGE_GROUPS]);
V_MATRIX_H = math.zeros([NUM_AGE_GROUPS,NUM_AGE_GROUPS]);
SIGNIFICANT_EIGEN_VALUES_H = 16; //NUM_AGE_GROUPS;

//age related transition probabilities, symptomatic to hospitalised to critical to fatality.
STATE_TRAN=[
   [0.0010000,   0.0500000,   0.4000000],
   [0.0030000,   0.0500000,   0.4000000],
   [0.0120000,   0.0500000,   0.5000000],
   [0.0320000,   0.0500000,   0.5000000],
   [0.0490000,   0.0630000,   0.5000000],
   [0.1020000,   0.1220000,   0.5000000],
   [0.1660000,   0.2740000,   0.5000000],
   [0.2430000,   0.4320000,   0.5000000],
   [0.2730000,   0.7090000,   0.5000000]
]


// Beta values
BETA_H = 0.47 *1.0 //Thailand data
BETA_W = 0.47 *2//Thailand data
BETA_S = 0.94 *2//Thailand data
BETA_C = 0.097*4.85// Thailand data. Product  = 0.47


ALPHA = 0.8 //exponent of number of people in a household while normalising infection rate in a household.

//some required functions
COMPLIANCE_PROBABILITY = set_compliance();
//To what extent does a family comply with an intervention? 1 = full compliance, 0 = no compliance.
function set_compliance(){
	var val = 1;
	switch(INTERVENTION) {
		case NO_INTERVENTION:
			   val = 0.9; //No effect.
		  break;
		case CASE_ISOLATION:
			val = 0.7;
		  break;
		case HOME_QUARANTINE:
			val = 0.5;
			break;
		case LOCKDOWN:
			val = 0.9;
			break;
		case CASE_ISOLATION_AND_HOME_QUARANTINE:
			val = 0.7;
			break;
		case CASE_ISOLATION_AND_HOME_QUARANTINE_SD_70_PLUS:
			val = 0.7;
			break;
		case LOCKDOWN_21_CI_HQ_SD_70_PLUS_21_CI:
			val = 0.9;
			break;
		case LOCKDOWN_21:
			val = 0.9;
			break;			
		default:
			val = 1;
	}
	return val;
}

function compliance(){
	var val = (Math.random()<COMPLIANCE_PROBABILITY)?1:0;
	return val;
}

//This function seeds the infection based on ward-level probabilities.
//Data can be taken from a json file.
function compute_prob_infection_given_community(infection_probability,set_uniform){

	var prob_infec_given_community = [];
	var communities_population_json = JSON.parse(loadJSON_001('input_files/fractionPopulation.json'));
	var communities_frac_quarantined_json = JSON.parse(loadJSON_001('input_files/quarantinedPopulation.json'));
	var num_communities = communities_population_json.length;
	for (var w = 0; w < num_communities; w++){
		if(set_uniform){
			//set uniformly across wards. Ignore ward wise data.
			prob_infec_given_community.push(infection_probability);
		}
		else{
			//Use ward wise quarantine data
			prob_infec_given_community.push(infection_probability*communities_frac_quarantined_json[w]['fracQuarantined']/communities_population_json[w]['fracPopulation']);

		} 		
		
	}
	return prob_infec_given_community;
}


function get_age_group(age){
	var age_group = 0;
	age_group =  Math.floor(age/5);
	if(age > 79){
		age_group = 15;
	}
	return age_group;
}

// Initialise the nodes with various features.
function init_nodes() {

	var individuals_json = JSON.parse(loadJSON_001('input_files/individuals.json'));
	var workplace_json = JSON.parse(loadJSON_001('input_files/workplaces.json'));
	//console.log(individuals_json.length,individuals_json[0]);
	NUM_PEOPLE =individuals_json.length;
	NUM_WORKPLACES = workplace_json.length;
	//console.log("Num People", NUM_PEOPLE, "Num Workspaces",NUM_WORKPLACES)
	COMMUNITY_INFECTION_PROB = compute_prob_infection_given_community(INIT_FRAC_INFECTED,false);
	//console.log(COMMUNITY_INFECTION_PROB)
	var nodes = [];
	var stream1 = new Random(1234);

	for(var i = 0; i < NUM_PEOPLE; i++) {
		
		//console.log(stream1.gamma(1,1))
		
	    var node = {
			'loc': [individuals_json[i]['lat'],individuals_json[i]['lon']], // [lat, long]
			'age': individuals_json[i]['age'],
			'age_group' : get_age_group(individuals_json[i]['age']),
			'zeta_a': 1,
			'infectiousness': stream1.gamma(1,1), // a.k.a. rho
			'severity': (Math.random() <0.5)?1:0, // a.k.a. S_k
			'home': individuals_json[i]['household'], 
			'workplace': individuals_json[i]['workplaceType']==1? individuals_json[i]['workplace']:individuals_json[i]['school'],
			'community': individuals_json[i]['wardNo']-1, //minus one is temporary as the ward number indexing starts from 1,
			'time_of_infection': 0,
			'infection_status': (Math.random() <COMMUNITY_INFECTION_PROB[individuals_json[i]['wardNo']-1])?1:0, //random seeding
			'infective': 0,
			'lambda_h': 0, //individuals contribution to his home cluster
			'lambda_w': 0, //individuals contribution to his workplace cluster
			'lambda_c': 0, //individuals contribution to his community
			'lambda': 0,
			'kappa_T': 1,
			'psi_T': 0,
			'funct_d_ck': f_kernel(individuals_json[i]['CommunityCentreDistance']), // TODO: need to use the kernel function. function of distance from community...
			'workplace_type':  individuals_json[i]['workplaceType'], //either school or office
			'lambda_incoming': [], //infectiousness from home, workplace, community as seen by individual
			'compliant' : 1,
			'kappa_H' : 1,
			'kappa_W' : 1,
			'kappa_C' : 1,
			'incubation_period': stream1.gamma(INCUBATION_PERIOD_SHAPE,INCUBATION_PERIOD_SCALE),
			'asymptomatic_period': stream1.gamma(1,ASYMPTOMATIC_PERIOD),
			'symptomatic_period': stream1.gamma(1,SYMPTOMATIC_PERIOD),
			'hospital_regular_period': HOSPITAL_REGULAR_PERIOD,
			'hospital_critical_period': HOSPITAL_CRITICAL_PERIOD,
			'kappa_H_incoming': 1,
			'kappa_W_incoming': 1,
			'kappa_C_incoming': 1,
			'quarantined' : false
		};
		
		//Correct initialisation for individuals not associated to workplace or school
		if(node['workplace_type']==WTYPE_HOME) {
			node['workplace'] = null;
		}
		
	    //Set infective status, set the time of infection, and other age-related factors
	    node['infective'] = node['infection_status']==INFECTIVE?1:0; //initialise all infected individuals as infective 
		node['time_of_infection'] = node['infection_status']==EXPOSED?(-node['incubation_period']*Math.random()):0;
		node['zeta_a']=zeta(node['age']);
		nodes.push(node)
		if(SEED_INFECTION_FROM_FILE){
			node['infection_status'] = individuals_json[i]['infection_status'];
			node['time_of_infection'] = node['infection_status']==EXPOSED?(-individuals_json[i]['time_since_infected']):0;
		}
	}
	return nodes;
}


// This is a multiplication factor that quantifies an individual's infective status given the infection state.
function kappa_T(node, cur_time){
	var val = 0;
	if(node["infective"]!=1){
		val = 0;
	}
	else {
		var time_since_infection = cur_time - node["time_of_infection"];

		if(time_since_infection < node['incubation_period'] || time_since_infection > (node['incubation_period']+node['asymptomatic_period']+node['symptomatic_period'])) { 
			// Individual is not yet symptomatic or has been recovered, or has moved to the hospital
			val = 0;
		} else if(time_since_infection < node['incubation_period']+node['asymptomatic_period']) {
			val = 1;
		} else {
			val = 1.5;
		}
	}
	return val;
}

/*
// When interventions are applied, the multiplification factors may change based on the mixing space.
// Adjustment function for intervention in community mixing space.
function kappa_C(node, cur_time){

	var val = 1;
	var time_since_infection = cur_time - node["time_of_infection"];
	switch(INTERVENTION){
		case NO_INTERVENTION:
			val = 1;
			break;
		case CASE_ISOLATION:
			val = 1;
			if(node['compliant']){

				if(time_since_infection > NUM_DAYS_TO_RECOG_SYMPTOMS*SIM_STEPS_PER_DAY){ // The magin number 1 = time to recognise symptoms

					val = 0.25;
				}
			}
			break;
		case HOME_QUARANTINE:
			val = 1;
			if(node['compliant']){
				if(time_since_infection > NUM_DAYS_TO_RECOG_SYMPTOMS*SIM_STEPS_PER_DAY){
					val = 0.25;
				}
			}
			break;
		case LOCKDOWN:
			val = 1;
			if(node['compliant']){
					val = 0.1;			
			}
			break;
		default:
			val = 1;
	}
	return val;
}

// Adjustment function to handle interventions in workspaces
function kappa_W(node, cur_time){

	var val = 1;
	var time_since_infection = cur_time - node["time_of_infection"];
	switch(INTERVENTION){
		case NO_INTERVENTION:
			val = 1;
			break;
		case CASE_ISOLATION:
			val = 1;
			if(node['compliant']){
				if(time_since_infection > NUM_DAYS_TO_RECOG_SYMPTOMS*SIM_STEPS_PER_DAY){ //The magic number 1 = time to recognise symptoms. 
					val = 0.25;
				}
			}
			break;
		case HOME_QUARANTINE:
			val = 1;
			if(node['compliant']){
				if(time_since_infection > NUM_DAYS_TO_RECOG_SYMPTOMS*SIM_STEPS_PER_DAY){
					val = 0.25;
				}
			}
			break;
		case LOCKDOWN:
			val = 0; 
			if(node['workplace_type']==WTYPE_OFFICE){
				////workplace
				val = 0.1;
			}
			break;
		default:
			val = 1;
	}
	return val;
}

// Adjustment function to handle interventions at home
function kappa_H(node, cur_time){

	var val = 1;
	///var time_since_infection = cur_time - node["time_of_infection"];
	switch(INTERVENTION){
		case NO_INTERVENTION:
			val = 1;
			break;
		case CASE_ISOLATION:
			val = 1;
			break;
		case HOME_QUARANTINE:
			val = 1;
			if(node['compliant']){
				val=2;
			}
			break;
		case LOCKDOWN:
			val = 1.25; //Social distancing increases household interactions by 25%
			if(node['compliant']){
				val=2;
			}
			break;
		default:
			val = 1;
	}
	return val; 
}
*/
// Absenteeism parameter. This may depend on the workplace type.
function psi_T(node, cur_time){
	if(node["infective"]!=1){ //check if not infectious
		return 0;
	}
	var PSI_THRESHOLD = SIM_STEPS_PER_DAY;	
	var time_since_infection = cur_time - node["time_of_infection"];
	var scale_factor = 0.5; 
	if(node['workplace_type']==WTYPE_SCHOOL) {scale_factor = 0.1} //school
	else if(node['workplace_type']==WTYPE_OFFICE) {scale_factor = 0.5} //office
	if(time_since_infection < PSI_THRESHOLD){ return 0;}
	else {return scale_factor;}	
}

function f_kernel(d){
    var a = 4 //in kms
    var b = 3.8 //both values are for Thailand, until we get a fit for India
	return 1/(1+Math.pow(d/a,b))

}


function zeta(age){
	// This might change based on better age-related interaction data.
    if(age < 5) {
    return 0.1;
    } else if(age < 10) {
    return 0.25;
    } else if(age < 15) {
    return 0.5;
    } else if(age < 20) {
    return 0.75;
    } else if(age < 65) {
    return 1;
    } else if(age < 70) {
    return 0.75;
    } else if(age < 75) {
    return 0.5;
    } else if(age < 85) {
    return 0.25;
    } else {
    return 0.1;
    }
}
/*
function get_individuals_at_home(nodes, h){
	var individuals = []
	for (var i=0; i<NUM_PEOPLE; i++){
		if (nodes[i]['home']==h){
			individuals.push(i)
		}
	}
	return individuals;
}

function get_individuals_at_workplace(nodes, w){
	var individuals = []
	for (var i=0; i<NUM_PEOPLE; i++){
		if (nodes[i]['workplace']==w){
			individuals.push(i)
		}
	}
	return individuals;
}

function get_individuals_at_community(nodes, c){
	var individuals = []
	for (var i=0; i<NUM_PEOPLE; i++){
		if (nodes[i]['community']==c){
			individuals.push(i)
		}
	}
	return individuals;
}
*/

// Compute scale factors for each home, workplace and community. Done once at the beginning.
function compute_scale_homes(homes){
	
	for (var w=0; w < homes.length; w++) {
		if(homes[w]['individuals'].length==0){
			homes[w]['scale'] = 0;			
		} else {
			homes[w]['scale'] = BETA_H*homes[w]['Q_h']/(Math.pow(homes[w]['individuals'].length, ALPHA));
		}
	}
}

function compute_scale_workplaces(workplaces){
	var beta_workplace
	for (var w=0; w < workplaces.length; w++) {
		if(workplaces[w]['individuals'].length==0){
			workplaces[w]['scale'] = 0			
		} else {
			if(workplaces[w]['workplace_type']==WTYPE_OFFICE){
				beta_workplace = BETA_W; //workplace
			} else if (workplaces[w]['workplace_type']==WTYPE_SCHOOL){
				beta_workplace = BETA_S; //school
			}
			workplaces[w]['scale'] = beta_workplace*workplaces[w]['Q_w']/workplaces[w]['individuals'].length;
		}
	}
}


function compute_scale_communities(nodes, communities){
	
	for (var w=0; w < communities.length; w++) {
	
    	var sum_value = 0;
		for (var i=0; i<communities[w]['individuals'].length; i++){
			sum_value += nodes[communities[w]['individuals'][i]]['funct_d_ck'];
		}
		if(sum_value==0){
			communities[w]['scale'] = 0;
		}
		else communities[w]['scale'] = BETA_C*communities[w]['Q_c']/sum_value;
		}
	
}

//Functions to init homes, workplaces and communities

function init_homes(){
	
	var houses_json = JSON.parse(loadJSON_001('input_files/houses.json'));
	// console.log("In init homes:",houses_json.length,houses_json[0]);
	NUM_HOMES = houses_json.length;

	var homes = [];
	for (var h=0; h < NUM_HOMES; h++) {
		var home = {
			'loc': [houses_json[h]['lat'],houses_json[h]['lon']], // [lat, long],
			'lambda_home': 0,
			'individuals': [], // We will populate this later
			'Q_h': 1,
			'scale': 0,
			'compliant' : compliance(),
			'quarantined' : false,
			'age_dependent_mixing': math.zeros([NUM_AGE_GROUPS])
		};
		//home['scale'] = BETA_H*home['Q_h']/(Math.pow(home['individuals'].length, ALPHA));
		
		homes.push(home)
		
	}
	if(USE_AGE_DEPENDENT_MIXING){
		get_age_dependent_mixing_matrix_household(); //get age dependent mixing matrix for households.
	}
	return homes;
}

// Both schools and offices are treated as workspaces with some differences
function init_workplaces(){
	var workplaces_json = JSON.parse(loadJSON_001('input_files/workplaces.json'));
	var schools_json = JSON.parse(loadJSON_001('input_files/schools.json'));
	// console.log("In init workplaces:",workplaces_json.length,workplaces_json[0]);
	NUM_WORKPLACES = workplaces_json.length;
	NUM_SCHOOLS = schools_json.length;

	// console.log(NUM_WORKPLACES,NUM_SCHOOLS)

	var workplaces = [];
	// schools come first followed by workspaces

	for (var w=0; w < NUM_SCHOOLS; w++) {
		var workplace = {			
			'loc':  [schools_json[w]['lat'],schools_json[w]['lon']], // [lat, long],
			'lambda_workplace': 0, 
			'individuals': [], //get_individuals_at_workplace(nodes, w), // Populate with individuals in same workplace
			'Q_w': 1,
			'scale': 0,
			'workplace_type': WTYPE_SCHOOL, //tells this workplace is a school
			'quarantined' : false,
			'age_dependent_mixing': math.zeros([NUM_AGE_GROUPS])
		};
		//workplace['scale'] = BETA_W*workplace['Q_w']/workplace['individuals'].length;
		workplaces.push(workplace)
	}

	for (var w=0; w < NUM_WORKPLACES; w++) {
		var workplace = {			
			'loc':  [workplaces_json[w]['lat'],workplaces_json[w]['lon']], // [lat, long],
			'lambda_workplace': 0, 
			'individuals': [], //get_individuals_at_workplace(nodes, w), // Populate with individuals in same workplace
			'Q_w': 1,
			'scale': 0,
			'workplace_type': WTYPE_OFFICE,//tells this workplace is an office
			'quarantined' : false,
			'age_dependent_mixing': math.zeros([NUM_AGE_GROUPS])
		};
		//workplace['scale'] = BETA_W*workplace['Q_w']/workplace['individuals'].length;
		workplaces.push(workplace)
	}
	if(USE_AGE_DEPENDENT_MIXING){
		get_age_dependent_mixing_matrix_workplace();
	}
	
	return workplaces;
}

function compare_wards(a, b) {
	// Function to sort wards
	const wardA = a["wardNo"];
	const wardB = b["wardNo"];
  
	let comparison = 0;
	if (wardA > wardB) {
	  comparison = 1;
	} else if (wardA < wardB) {
	  comparison = -1;
	}
	return comparison;
}



function init_community(){

	var communities_json = JSON.parse(loadJSON_001('input_files/commonArea.json'));
	// console.log("In init community",communities_json.length,communities_json);
	communities_json.sort(compare_wards);
	// console.log("In init community",communities_json.length,communities_json[0]['location']);
	NUM_COMMUNITIES = communities_json.length;

	var communities = [];
	for (var c=0; c < NUM_COMMUNITIES; c++) {
		var community = {
			'loc':  [communities_json[c]['lat'],communities_json[c]['lon']], // [lat, long]
			'lambda_community': 0,
			'lambda_community_global': 0,  
			'individuals': [], // We will populate this later
			'Q_c': 1,
			'scale': 0,
			'quarantined' : false
		};
		
		communities.push(community)
	}
	return communities;
}
/*
const RADIUS_EARTH = 6371; ///km
function euclidean(loc1,loc2) {
	var lat1 = loc1[0];
	var lon1 = loc1[1];
	var lat2 = loc2[0];
	var lon2 = loc2[1];

	if ((lat1 == lat2) && (lon1 == lon2)) {
		return 0;
	}
	else {
		
		var radlat1 = Math.PI * lat1/180;
		var radlat2 = Math.PI * lat2/180;
		var theta = lon1-lon2;
		var radtheta = Math.PI * theta/180;
		var dist = 1- (Math.sin(radlat1) * Math.sin(radlat2) + Math.cos(radlat1) * Math.cos(radlat2) * Math.cos(radtheta));
		if (dist < 0) {
			dist = 0;
		}
		dist = Math.sqrt(2*dist);
		dist = dist*RADIUS_EARTH;
		//dist = Math.acos(dist);
		///dist = dist * 180/Math.PI;
		//dist = dist * 60 * 1.1515;
		//if (unit=="K") { dist = dist * 1.609344 }
		//if (unit=="N") { dist = dist * 0.8684 }
		return dist;
	}
}
*/

function compute_community_distances(communities){
	var inter_ward_distances_json = JSON.parse(loadJSON_001('input_files/wardCentreDistance.json'));

	 var community_dist_matrix = math.zeros([communities.length,communities.length]);
	/// console.log(community_dist_matrix)
	/*
	 for (var c1 =0; c1< communities.length;c1++){
		 for (var c2=c1+1; c2<communities.length;c2++){
			/// console.log(communities[c1]['loc'],communities[c2]['loc'])
			 community_dist_matrix[c1][c2] = euclidean(communities[c1]['loc'],communities[c2]['loc']); 
			 community_dist_matrix[c2][c1] = community_dist_matrix[c1][c2];
			 
		 }
	 }
	*/
	 
	 for (var c1 =0; c1< inter_ward_distances_json.length;c1++){
		for (var c2=c1+1; c2<inter_ward_distances_json.length;c2++){
		   /// console.log(communities[c1]['loc'],communities[c2]['loc'])
			community_dist_matrix[c1][c2] = inter_ward_distances_json[c1][(c2+1).toString()]; 
			community_dist_matrix[c2][c1] = community_dist_matrix[c1][c2];
			
		}
	}
	

	 return community_dist_matrix;
}




function assign_individual_home_community(nodes,homes,workplaces,communities){
	//Assign individuals to homes, workplace, community
	for (var i=0; i < nodes.length; i++) {
		if(nodes[i]['home']!= null) 	{
		
			homes[nodes[i]['home']]['individuals'].push(i); //No checking for null as all individuals have a home
			nodes[i]['compliant'] = homes[nodes[i]['home']]['compliant']; //All members of the household are set the same compliance value
		}
		if(nodes[i]['workplace']!= null)	{workplaces[nodes[i]['workplace']]['individuals'].push(i);}
		if(nodes[i]['community']!= null) 	{communities[nodes[i]['community']]['individuals'].push(i);}
	}

}

function update_individual_lambda_h(node){
	return node['infective'] * node['kappa_T'] * node['infectiousness'] * (1 + node['severity'])*node['kappa_H'];
}

function update_individual_lambda_w(node){
	return node['infective'] * node['kappa_T'] * node['infectiousness'] * (1 + node['severity']*(2*node['psi_T']-1))*node['kappa_W'];
}

function update_individual_lambda_c(node){
	return node['infective'] * node['kappa_T'] * node['infectiousness'] * node['funct_d_ck'] * (1 + node['severity'])*node['kappa_C'];
	// optimised version: return node['lambda_h] * node['funct_d_ck']; 
}
function get_init_stats(nodes,homes,workplaces,communities){
	for (var h = 0; h< homes.length;h++){
		let working_count  = 0
		for (var ind_count = 0; ind_count <  homes[h]['individuals'].length; ind_count++){
			if(nodes[homes[h]['individuals'][ind_count]]['workplace_type']==1) working_count++;
		}
		console.log("Home: ",h, " - ", homes[h]['individuals'].length, ". Working individuals  = ", working_count);
	}
	for (var h = 0; h< workplaces.length;h++){
		console.log("workplace: ",h, " - ", workplaces[h]['individuals'].length)
	}
	for (var h = 0; h< communities.length;h++){
		console.log("Community: ",h, " - ", communities[h]['individuals'].length)
	}
}

function update_infection(node,cur_time){
    
    
    var age_index = 0;
    // Determine age category of individual. TODO: Could be part of individual datastructure as this is static
    if(node['age'] < 10) {
    age_index = 0;
    } else if(node['age'] < 20) {
    age_index = 1;
    } else if(node['age'] < 30) {
    age_index = 2;
    } else if(node['age'] < 40) {
    age_index = 3;
    } else if(node['age'] < 50) {
    age_index = 4;
    } else if(node['age'] < 60) {
    age_index = 5;
    } else if(node['age'] < 70) {
    age_index = 6;
    } else if(node['age'] < 80) {
    age_index = 7;
    } else {
    age_index = 8;
    }
    
    //console.log(1-Math.exp(-node['lambda']/SIM_STEPS_PER_DAY))
    ///TODO: Parametrise transition times
	if (node['infection_status']==SUSCEPTIBLE && Math.random()<(1-Math.exp(-node['lambda']/SIM_STEPS_PER_DAY))){
    	node['infection_status'] = EXPOSED; //move to exposed state
		node['time_of_infection'] = cur_time;
		node['infective'] = 0;
		update_lambda_stats(node)		
	}
	else if(node['infection_status']==EXPOSED && (cur_time - node['time_of_infection'] > node['incubation_period'])){
    	node['infection_status'] = INFECTIVE;//move to infective state
    	node['infective'] = 1;
	}
	else if(node['infection_status']==INFECTIVE && (cur_time - node['time_of_infection'] > (node['incubation_period']+node['asymptomatic_period']) )){
    	if(Math.random() < SYMPTOMATIC_FRACTION){
            	node['infection_status'] = SYMPTOMATIC;//move to symptomatic
            	node['infective'] = 1;
    	}
    	else {
        	node['infection_status'] = RECOVERED;//move to recovered
            node['infective'] = 0;
    	}
	}
	else if(node['infection_status']==SYMPTOMATIC && (cur_time - node['time_of_infection'] > (node['incubation_period']+node['asymptomatic_period']+node['symptomatic_period']))){
    	if(Math.random() < STATE_TRAN[age_index][0]){
            	node['infection_status'] = HOSPITALISED;//move to hospitalisation
            	node['infective'] = 0;
    	}
    	else {
        	node['infection_status'] = RECOVERED;//move to recovered
            node['infective'] = 0;
    	}
	}
	else if(node['infection_status']==HOSPITALISED && (cur_time - node['time_of_infection'] > (node['incubation_period']+node['asymptomatic_period']+node['symptomatic_period']+node['hospital_regular_period']))){
    	if(Math.random() < STATE_TRAN[age_index][1]){
            	node['infection_status'] = CRITICAL;//move to critical care
            	node['infective'] = 0;
    	}
    	else {
        	node['infection_status'] = RECOVERED;//move to recovered
            node['infective'] = 0;
    	}
	}
	else if(node['infection_status']==CRITICAL && (cur_time - node['time_of_infection'] > (node['incubation_period']+node['asymptomatic_period']+node['symptomatic_period']+node['hospital_regular_period']+node['hospital_critical_period']))){
    	if(Math.random() < STATE_TRAN[age_index][2]){
            	node['infection_status'] = DEAD;//move to dead
            	node['infective'] = 0;
    	}
    	else {
        	node['infection_status'] = RECOVERED;//move to recovered
            node['infective'] = 0;
    	}
	}
	
	
	
	node['lambda_h'] = update_individual_lambda_h(node);
	node['lambda_w'] = update_individual_lambda_w(node);
	node['lambda_c'] = update_individual_lambda_c(node);
}
/*
function update_kappa(node, cur_time){
	node['kappa_T'] = kappa_T(node, cur_time);
	node['kappa_H'] = kappa_H(node, cur_time);
	node['kappa_W'] = kappa_W(node, cur_time);
	node['kappa_C'] = kappa_C(node, cur_time);
	//console.log(node['kappa_T'])
}
*/
function update_psi(node, cur_time){
	node['psi_T'] = psi_T(node, cur_time);
	//console.log(node['psi_T'])
}




function update_lambda_h_old(nodes, home){
	var sum_value = 0
	
	for (var i=0; i<home['individuals'].length; i++){
	//	var temp = nodes.filter( function(node) {
	//		return node['index']==home['individuals'][i];
	//	});
		sum_value += nodes[home['individuals'][i]]['lambda_h'];
	}
	return home['scale']*sum_value;
	// Populate it afterwards...
}


function get_age_dependent_mixing_matrix_household(){
	
	var sigma_json = JSON.parse(loadJSON_001('Sigma_household.json'));
	var U_matrix_json = JSON.parse(loadJSON_001('U_household.json'));
	var V_matrix_json = JSON.parse(loadJSON_001('Vtranspose_household.json'));

	SIGMA_H = math.zeros([NUM_AGE_GROUPS]);
	U_MATRIX_H = math.zeros([NUM_AGE_GROUPS,NUM_AGE_GROUPS]);
	V_MATRIX_H = math.zeros([NUM_AGE_GROUPS,NUM_AGE_GROUPS]);


	for (var count = 0; count < NUM_AGE_GROUPS; count++){ 
		//sigma_json is read as a diagonal matrix.
		SIGMA_H[count] = sigma_json[count][count];
	}
	for (var count = 0; count < NUM_AGE_GROUPS; count++){
		for (var count2 = 0;  count2 < NUM_AGE_GROUPS; count2++){
			U_MATRIX_H[count][count2] = U_matrix_json[count2][count]; //After JSON parsing, what you get is the transposed version.
			V_MATRIX_H[count][count2] = V_matrix_json[count2][count]; //V_MATRIX is the transpose of V in C = SUV'.
		}	
	}
	
	
}

function update_lambda_h(nodes,home){
	//Compute age_group related mixing
	var lambda_age_group = math.zeros([NUM_AGE_GROUPS]);

	if(!USE_AGE_DEPENDENT_MIXING){
		//////////////////////////
		//Sanity test --- use old lambdas
		var lambda_old = update_lambda_h_old(nodes,home)

		
		for (var count = 0; count < NUM_AGE_GROUPS;count++){
			lambda_age_group[count] = lambda_old;
		}
	////////////////////////////

	} else {

	
		var SIGMA = SIGMA_H;
		var V_MATRIX = V_MATRIX_H;
		var U_MATRIX = U_MATRIX_H;
		var SIGNIFICANT_EIGEN_VALUES = SIGNIFICANT_EIGEN_VALUES_H

		//add contributions to each age group
		var age_components = math.zeros([NUM_AGE_GROUPS]);
		for (var indv_count = 0; indv_count < home['individuals'].length;indv_count++){
			var indv_age_group = nodes[home['individuals'][indv_count]]['age_group'];
			age_components[indv_age_group]+=nodes[home['individuals'][indv_count]]['lambda_h'];		
		}
		//weighted sum of age contributions for each eigen component
		var V_T_x = math.zeros([SIGNIFICANT_EIGEN_VALUES]);
		for (var eigen_count = 0; eigen_count <SIGNIFICANT_EIGEN_VALUES_SCHOOL;eigen_count++){
			for (var count = 0; count < NUM_AGE_GROUPS;count++){
				V_T_x[eigen_count]+=V_MATRIX[eigen_count][count]*age_components[count];//Assumption is V_matrix is V' where C = USV'
			}
		}	

		for (var count = 0; count < NUM_AGE_GROUPS;count++){
			for (var eigen_count = 0; eigen_count <SIGNIFICANT_EIGEN_VALUES_SCHOOL;eigen_count++){		
				lambda_age_group[count]+=home['scale']*SIGMA[eigen_count]*U_MATRIX[count][eigen_count]*V_T_x[eigen_count];
			}
		}
	}
	return lambda_age_group;
}



function update_lambda_w_old(nodes, workplace){
	var sum_value = 0
	
	for (var i=0; i<workplace['individuals'].length; i++){
		//	var temp = nodes.filter( function(node) {
		///		return node['index']==workplace['individuals'][i];
		//	});
			sum_value += nodes[workplace['individuals'][i]]['lambda_w'];
		}
	
	
	return workplace['scale']*sum_value;
	// Populate it afterwards...
}

function get_age_dependent_mixing_matrix_workplace(){
	var sigma_json = JSON.parse(loadJSON_001('input_files/Sigma_workplace.json'));
	var U_matrix_json = JSON.parse(loadJSON_001('input_files/U_workplace.json'));
	var V_matrix_json = JSON.parse(loadJSON_001('input_files/Vtranspose_workplace.json'));

	SIGMA_OFFICE = math.zeros([NUM_AGE_GROUPS]);
	U_MATRIX_OFFICE = math.zeros([NUM_AGE_GROUPS,NUM_AGE_GROUPS]);
	V_MATRIX_OFFICE = math.zeros([NUM_AGE_GROUPS,NUM_AGE_GROUPS]);


	for (var count = 0; count < NUM_AGE_GROUPS; count++){ 
		//sigma_json is read as a diagonal matrix.
		SIGMA_OFFICE[count] = sigma_json[count][count];
	}
	for (var count = 0; count < NUM_AGE_GROUPS; count++){
		for (var count2 = 0;  count2 < NUM_AGE_GROUPS; count2++){
			U_MATRIX_OFFICE[count][count2] = U_matrix_json[count2][count]; //After JSON parsing, what you get is the transposed version.
			V_MATRIX_OFFICE[count][count2] = V_matrix_json[count2][count]; //V_MATRIX is the transpose of V in C = SUV'.
		}
		
	}

	var sigma_json = JSON.parse(loadJSON_001('input_files/Sigma_school.json'));
	var U_matrix_json = JSON.parse(loadJSON_001('input_files/U_school.json'));
	var V_matrix_json = JSON.parse(loadJSON_001('input_files/Vtranspose_school.json'));

	SIGMA_SCHOOL = math.zeros([NUM_AGE_GROUPS]);
	U_MATRIX_SCHOOL = math.zeros([NUM_AGE_GROUPS,NUM_AGE_GROUPS]);
	V_MATRIX_SCHOOL = math.zeros([NUM_AGE_GROUPS,NUM_AGE_GROUPS]);


	for (var count = 0; count < NUM_AGE_GROUPS; count++){ //TODO:Change loop max to NUM_AGE_GROUPS
		SIGMA_SCHOOL[count] = sigma_json[count][count];
	}
	for (var count = 0; count < NUM_AGE_GROUPS; count++){
		for (var count2 = 0;  count2 < NUM_AGE_GROUPS; count2++){
			U_MATRIX_SCHOOL[count][count2] = U_matrix_json[count2][count];
			V_MATRIX_SCHOOL[count][count2] = V_matrix_json[count2][count];
		}
		
	}
}



function update_lambda_w(nodes,workplace){
	//Compute age_group related mixing
	var lambda_age_group = math.zeros([NUM_AGE_GROUPS]);

	if(!USE_AGE_DEPENDENT_MIXING){
		//////////////////////////
		//Sanity test --- use old lambdas
		var lambda_old = update_lambda_w_old(nodes,workplace)

		
		for (var count = 0; count < NUM_AGE_GROUPS;count++){
			lambda_age_group[count] = lambda_old;
		}
	////////////////////////////

	} else {

	
		var SIGMA = SIGMA_OFFICE;
		var V_MATRIX = V_MATRIX_OFFICE;
		var U_MATRIX = U_MATRIX_OFFICE;
		var SIGNIFICANT_EIGEN_VALUES = SIGNIFICANT_EIGEN_VALUES_OFFICE

		if(workplace['workplace_type'] == WTYPE_SCHOOL){
			SIGMA = SIGMA_SCHOOL;
			V_MATRIX = V_MATRIX_SCHOOL;
			U_MATRIX = U_MATRIX_SCHOOL;
			SIGNIFICANT_EIGEN_VALUES = SIGNIFICANT_EIGEN_VALUES_SCHOOL
		}
		
		//add contributions to each age group
		var age_components = math.zeros([NUM_AGE_GROUPS]);
		for (var student_count = 0; student_count < workplace['individuals'].length;student_count++){
			var student_age_group = nodes[workplace['individuals'][student_count]]['age_group'];
			age_components[student_age_group]+=nodes[workplace['individuals'][student_count]]['lambda_w'];		
		}
		//weighted sum of age contributions for each eigen component
		var V_T_x = math.zeros([SIGNIFICANT_EIGEN_VALUES]);
		for (var eigen_count = 0; eigen_count <SIGNIFICANT_EIGEN_VALUES_SCHOOL;eigen_count++){
			for (var count = 0; count < NUM_AGE_GROUPS;count++){
				V_T_x[eigen_count]+=V_MATRIX[eigen_count][count]*age_components[count];//Assumption is V_matrix is V' where C = USV'
			}
		}	

		for (var count = 0; count < NUM_AGE_GROUPS;count++){
			for (var eigen_count = 0; eigen_count <SIGNIFICANT_EIGEN_VALUES_SCHOOL;eigen_count++){		
				lambda_age_group[count]+=workplace['scale']*SIGMA[eigen_count]*U_MATRIX[count][eigen_count]*V_T_x[eigen_count];
			}
		}
	}
	return lambda_age_group;
}

function update_lambda_c_local(nodes, community){
	var sum_value = 0
	for (var i=0; i<community['individuals'].length; i++){
	//	var temp = nodes.filter( function(node) {
	//		return node['index']==community['individuals'][i];
	//	});
		sum_value += nodes[community['individuals'][i]]['lambda_c'];
	}
	return community['scale']*sum_value;
	// Populate it afterwards...
}

function update_lambda_c_global(communities,community_distance_matrix){
	for (var c1=0; c1<communities.length; c1++){
		var temp = 0;
		var temp2 = 0;
		for (var c2 = 0;c2<communities.length;c2++){			 
			temp+=f_kernel(community_distance_matrix[c1][c2])*communities[c2]['lambda_community'];
			temp2+=f_kernel(community_distance_matrix[c1][c2]);			
			//console.log(c1,c2,f_kernel(community_distance_matrix[c1][c2])*communities[c2]['lambda_community'])
		}
		communities[c1]['lambda_community_global'] = temp/temp2;
		
	}
}


function get_infected_community(nodes, community){
	var affected_stat = 0
	var infected_stat = 0
	var exposed_stat = 0
	var hospitalised_stat = 0
	var critical_stat = 0
	var dead_stat = 0
	var recovered_stat = 0
	var exposed_stat = 0

	for (var i=0; i<community['individuals'].length; i++){
	//	var temp = nodes.filter( function(node) {
	//		return node['index']==community['individuals'][i];
	//	});

		if (nodes[community['individuals'][i]]['infection_status']==EXPOSED) {exposed_stat+=1}
		if (nodes[community['individuals'][i]]['infection_status']==RECOVERED) {recovered_stat+=1}
		if (nodes[community['individuals'][i]]['infection_status']==HOSPITALISED) {hospitalised_stat+=1}
		if (nodes[community['individuals'][i]]['infection_status']==CRITICAL) {critical_stat+=1}
		if (nodes[community['individuals'][i]]['infection_status']==DEAD) {dead_stat+=1}


		if (nodes[community['individuals'][i]]['infection_status']==INFECTIVE ||
    		nodes[community['individuals'][i]]['infection_status']==SYMPTOMATIC || 
    		nodes[community['individuals'][i]]['infection_status']==HOSPITALISED ||
    		nodes[community['individuals'][i]]['infection_status']==CRITICAL) {infected_stat+=1}
	}
	affected_stat = exposed_stat+infected_stat+recovered_stat+dead_stat;

	return [infected_stat,affected_stat,hospitalised_stat,critical_stat,dead_stat];
	// Populate it afterwards...
}

function update_lambdas(node,homes,workplaces,communities,nodes,cur_time){
	
	/*
	///////TODO: See if this can be made as a function
	var node_home_quarantined = false;
	if(INTERVENTION == HOME_QUARANTINE && node['compliant']){
		var house_members = homes[node['home']]['individuals'];
		for (var l = 0; l < house_members.length; l++){
			var time_since_symptoms = cur_time - nodes[house_members[l]]['time_since_infection'] - nodes[house_members[l]]['incubation_period'] - nodes[house_members[l]]['asymptomatic_period'];
			node_home_quarantined = node_home_quarantined || 
			(	(nodes[house_members[l]]['infection_status']!=SUSCEPTIBLE) && 
				(nodes[house_members[l]]['infection_status']!=EXPOSED) && 
				(time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*SIM_STEPS_PER_DAY) && 
				(time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*SIM_STEPS_PER_DAY)	);
		}
	}
	*/
	node['lambda_incoming']=[0,0,0];
	if(node['home']!=null &&  node['home']!=undefined) {
		var age_group = node['age_group'];
		node['lambda_incoming'][0] = node['kappa_H_incoming']*homes[node['home']]['age_dependent_mixing'][age_group];
	}
	if(node['workplace']!=null &&  node['workplace']!=undefined) {
		var age_group = node['age_group'];
		node['lambda_incoming'][1] = node['kappa_W_incoming']* workplaces[node['workplace']]['age_dependent_mixing'][age_group];
	}
	if(node['community']!=null &&  node['community']!=undefined) {
		//for all communities
		//add the community lambda with a distance related scaling factor
		node['lambda_incoming'][2] = node['kappa_C_incoming']*node['zeta_a']*node['funct_d_ck']*communities[node['community']]['lambda_community_global'];
	}

	node['lambda'] = node['lambda_incoming'][0]+node['lambda_incoming'][1]+node['lambda_incoming'][2];
	
}
/*
function get_lambda_stats(time,node,lambda_stats_variable){
	var temp = [time,node,0,0,0,0];
	if(node['home']!=null) {temp[2]=homes[node['home']]['lambda_home'];}
	if(node['workplace']!=null ) {temp[3] = workplaces[node['workplace']]['lambda_workplace'];}
	if(node['community']!=null) {temp[4] = node['zeta_a']*node['funct_d_ck']*communities[node['community']]['lambda_community'];}
	temp[5] = temp[2]+temp[3]+temp[4]
	lambda_stats_variable.push(temp)
}
*/

function update_lambda_stats(node){
	var sum_inv = 1/math.sum(node['lambda_incoming']);
	var lambda_vector = [];
	for (var i= 0; i<node['lambda_incoming'].length; i++){
		lambda_vector.push(node['lambda_incoming'][i]*sum_inv);
		LAMBDA_INFECTION_MEAN[i] = (LAMBDA_INFECTION_MEAN[i]*LAMBDA_INFECTION_STATS.length+lambda_vector[i])/(LAMBDA_INFECTION_STATS.length+1);
	}
	LAMBDA_INFECTION_STATS.push(lambda_vector)

}


let csvContent_alltogether = "data:text/csv;charset=utf-8,";

function update_all_kappa(nodes,homes,workplaces,communities,cur_time){
	switch(INTERVENTION){
		case NO_INTERVENTION:
			get_kappa_no_intervention(nodes, homes, workplaces, communities,cur_time);
			break;
		case CASE_ISOLATION:
			get_kappa_case_isolation(nodes, homes, workplaces, communities,cur_time);
			break;
		case HOME_QUARANTINE:
			get_kappa_home_quarantine(nodes, homes, workplaces, communities,cur_time);
			break;
		case LOCKDOWN:
			get_kappa_lockdown(nodes, homes, workplaces, communities,cur_time);
			break;
		case CASE_ISOLATION_AND_HOME_QUARANTINE:
			get_kappa_CI_HQ(nodes, homes, workplaces, communities,cur_time);
			break;
		case CASE_ISOLATION_AND_HOME_QUARANTINE_SD_70_PLUS:
			get_kappa_CI_HQ_70P(nodes, homes, workplaces, communities,cur_time);
			break;
		case LOCKDOWN_21_CI_HQ_SD_70_PLUS_21_CI:
			get_kappa_LOCKDOWN_21_CI_HQ_SD_70_PLUS_21_CI(nodes, homes, workplaces, communities,cur_time);
			break;
		case LOCKDOWN_21:
			get_kappa_LOCKDOWN_21(nodes, homes, workplaces, communities,cur_time);
			break;
		default:
			break;
	}
}


function run_simulation() {
	
	var homes = init_homes();
	var workplaces = init_workplaces();
	var communities = init_community();
	var nodes = init_nodes();
	var community_distance_matrix = compute_community_distances(communities);

	//console.log(community_distance_matrix)
	console.log(NUM_PEOPLE,NUM_HOMES, NUM_WORKPLACES,NUM_SCHOOLS,NUM_COMMUNITIES)

	assign_individual_home_community(nodes,homes,workplaces,communities);
	
	compute_scale_homes(homes)
	compute_scale_workplaces(workplaces)
	compute_scale_communities(nodes, communities)
	
	//get_init_stats(nodes,homes,workplaces,communities);

	var days_num_infected = [];
	var days_num_exposed = [];
	var days_num_hospitalised = [];
	var days_num_critical = [];
	var days_num_fatalities = [];
	var days_num_recovered = [];
	var days_num_affected = [];
	var plotly_test_plot = []
	var infected_count= 0;
	var lambda_evolution = []
	LAMBDA_INFECTION_STATS=[] //global variable to track lambda evolution when a person gets infected
	LAMBDA_INFECTION_MEAN = [0, 0, 0];
	

	for(var time_step = 0; time_step < NUM_TIMESTEPS; time_step++) {
		console.log(time_step/SIM_STEPS_PER_DAY);
		
		for (var j=0; j<NUM_PEOPLE; j++){
			update_infection(nodes[j],time_step);
			//update_kappa(nodes[j], time_step);
			update_psi(nodes[j], time_step);
		}
		update_all_kappa(nodes,homes,workplaces,communities,time_step)
		for (var h=0; h<NUM_HOMES; h++){
			homes[h]['age_dependent_mixing'] = update_lambda_h(nodes, homes[h]);
		}
		for (var w=0; w<NUM_SCHOOLS+NUM_WORKPLACES; w++){
			workplaces[w]['age_dependent_mixing'] = update_lambda_w(nodes, workplaces[w]);
		}
		
		for (var c=0; c<NUM_COMMUNITIES; c++){
			communities[c]['lambda_community'] = update_lambda_c_local(nodes, communities[c]);
			///console.log("lambda_community:",c,communities[c]['lambda_community'])
			var temp_stats = get_infected_community(nodes, communities[c]);
			
			//infection_status_community.push([]);
			let row = [time_step/SIM_STEPS_PER_DAY,c,temp_stats[0],temp_stats[1],temp_stats[2],temp_stats[3],temp_stats[4]].join(",");
            csvContent += row + "\r\n";
		}

			
		update_lambda_c_global(communities,community_distance_matrix);
			


		
		for (var j=0; j<NUM_PEOPLE; j++){
			var lambda_current_stats = [];
			update_lambdas(nodes[j],homes,workplaces,communities,nodes,time_step);
			//get_lambda_stats(i,j,lambda_current_stats);			
		}
		
		//lambda_current_stats_avg = math.mean(math.mean(lambda_current_stats, 0));
		//lambda_evolution.push([i/SIM_STEPS_PER_DAY,lambda_current_stats_avg[2],lambda_current_stats_avg[3],lambda_current_stats_avg[4],lambda_current_stats_avg[5]]);
		
		var n_infected_wardwise = nodes.reduce(function(partial_sum, node) {return partial_sum + ((node['infection_status']==INFECTIVE||node['infection_status']==SYMPTOMATIC||node['infection_status']==HOSPITALISED||node['infection_status']==CRITICAL) ? 1 : 0);}, 0);
		days_num_infected.push([time_step/SIM_STEPS_PER_DAY, n_infected]);
		csvContent_ninfected = [time_step/SIM_STEPS_PER_DAY, n_infected].join(',')+"\r\n"
		
		var n_infected = nodes.reduce(function(partial_sum, node) {return partial_sum + ((node['infection_status']==INFECTIVE||node['infection_status']==SYMPTOMATIC||node['infection_status']==HOSPITALISED||node['infection_status']==CRITICAL) ? 1 : 0);}, 0);
		days_num_infected.push([time_step/SIM_STEPS_PER_DAY, n_infected]);
        
        var n_exposed = nodes.reduce(function(partial_sum, node) {return partial_sum + ((node['infection_status']==EXPOSED) ? 1 : 0);}, 0);
		days_num_exposed.push([time_step/SIM_STEPS_PER_DAY, n_exposed]);
		
		var n_hospitalised = nodes.reduce(function(partial_sum, node) {return partial_sum + ((node['infection_status']==HOSPITALISED) ? 1 : 0);}, 0);
		days_num_hospitalised.push([time_step/SIM_STEPS_PER_DAY, n_hospitalised]);
		
		var n_critical = nodes.reduce(function(partial_sum, node) {return partial_sum + ((node['infection_status']==CRITICAL) ? 1 : 0);}, 0);
		days_num_critical.push([time_step/SIM_STEPS_PER_DAY, n_critical]);
		
		var n_fatalities = nodes.reduce(function(partial_sum, node) {return partial_sum + ((node['infection_status']==DEAD) ? 1 : 0);}, 0);
		days_num_fatalities.push([time_step/SIM_STEPS_PER_DAY, n_fatalities]);
		
		var n_recovered = nodes.reduce(function(partial_sum, node) {return partial_sum + ((node['infection_status']==RECOVERED) ? 1 : 0);}, 0);
		days_num_recovered.push([time_step/SIM_STEPS_PER_DAY, n_recovered]);
		
		var n_affected = nodes.reduce(function(partial_sum, node) {return partial_sum + ((node['infection_status']) ? 1 : 0);}, 0);
		days_num_affected.push([time_step/SIM_STEPS_PER_DAY, n_affected]);

		let row = [time_step/SIM_STEPS_PER_DAY,n_affected,n_recovered,n_infected,n_exposed,n_hospitalised,n_critical,n_fatalities].join(",");
			csvContent_alltogether += row + "\r\n";
		if(LAMBDA_INFECTION_STATS.length > 0){
			lambda_evolution.push([time_step/SIM_STEPS_PER_DAY,[LAMBDA_INFECTION_MEAN[0],LAMBDA_INFECTION_MEAN[1],LAMBDA_INFECTION_MEAN[2]]])
		}
		
		///update_sim_progress_status(time_step,NUM_TIMESTEPS);
		
	}
	if(LAMBDA_INFECTION_STATS.length > 0){
		console.log(math.mean(LAMBDA_INFECTION_STATS,0));
	}
	
	return [days_num_infected,days_num_exposed,days_num_hospitalised,days_num_critical,days_num_fatalities,days_num_recovered,days_num_affected,lambda_evolution];
}
/*
function update_sim_progress_status(time_step,num_time_steps){
	var elem = document.getElementById("myBar");
	var width = time_step/num_time_steps;
	elem.style.width = width + "%";
	//document.getElementById("sim_progress").innerHTML="Simulation Progress: " + String(time_step/sim_steps_per_day)+ " days.";
}
*/

function plot_lambda_evolution(data,plot_position,title_text,legends) {
	var trace = [];

	for (var count = 0; count < data.length;count++){
		for (var lambda_length_count = 0; lambda_length_count < data[0][0][1].length;lambda_length_count++){
			//iterate over lambda_h, lambda_w,lambda_c
			var trace1 = {
				x: [],
				y: [],
				mode: 'bar',
				name: legends[lambda_length_count]
			  };
			  for (var count2 = 0; count2 < data[count].length;count2++){
				  trace1.x.push(data[count][count2][0]);
				  trace1.y.push(data[count][count2][1][lambda_length_count]);
	
			  }
			  trace.push(trace1)

		}
		
	}
	
	  
	  var data_plot = trace;
	  
	  var layout = {
		
		barmode: 'stack',
		title: {
		  text: title_text,
		  font: {
			family: 'Courier New, monospace',
			size: 24
		  },
		  xref: 'paper',
		  x: 0.05,
		},
		xaxis: {
		  title: {
			text: 'Days',
			font: {
			  family: 'Courier New, monospace',
			  size: 18,
			  color: '#7f7f7f'
			}
		  },
		},
		yaxis: {
		  title: {
			text: title_text,
			font: {
			  family: 'Courier New, monospace',
			  size: 18,
			  color: '#7f7f7f'
			}
		  },
		  range:[0,1]
		}
	  };
	  
	  
	  Plotly.newPlot(plot_position, data_plot, layout);
}

/*

function plot_simulation(days_num_infected,plot_element,title_1,title_2) {
	google.charts.load('current', {packages: ['corechart', 'line']});
	google.charts.setOnLoadCallback(drawBasic);

	function drawBasic() {
	    var data = new google.visualization.DataTable();
	    data.addColumn('number', 'X');
	    data.addColumn('number', title_1);

	    data.addRows(days_num_infected);

	    var options = {
	        hAxis: {
	            title: 'Days'
	        },
	        vAxis: {
	            title: title_1
	        },
			title: title_2,
			legend: {position:'none'}
	    };

	    var chart = new google.visualization.LineChart(document.getElementById(plot_element));

		chart.draw(data, options);
		
	}
}
*/
function run_and_plot(intervention) {
	var returned_values 
	INTERVENTION = intervention
	console.log(INTERVENTION, intervention)
	returned_values = run_simulation();
	
	plot_plotly([returned_values[6]],'num_affected_plot_2','Number Affected (cum.)','Evolution of Affected Population');
	plot_plotly([returned_values[0]],'num_infected_plot_2','Number Infected (daily)','Evolution of Infected Population');
	plot_plotly([returned_values[1]],'num_exposed_plot_2','Number Exposed (daily)','Evolution of Exposed Population');
	plot_plotly([returned_values[2]],'num_hospitalised_plot_2','Number Hospitalised (daily)','Evolution of Hospitalised Population');
	plot_plotly([returned_values[3]],'num_critical_plot_2','Number Critical (daily)','Evolution of Critical Population');
	plot_plotly([returned_values[4]],'num_fatalities_plot_2','Number Fatalities (cum.)','Evolution of Fatalities Population');
	plot_plotly([returned_values[5]],'num_recovered_plot_2','Number Recovered (cum.)','Evolution of Recovered Population');
	plot_lambda_evolution([returned_values[7]],'lambda_evolution','Source of infection',['Home','School/Workplace','Community'])

	var encodedUri = encodeURI(csvContent);
    var link = document.createElement("a");
    link.setAttribute("href", encodedUri);
    link.setAttribute("download", "my_data.csv");
    document.body.appendChild(link); // Required for FF
	if(!WEBPAGE_VERSION){
		link.click();	//TODO: Instead of click link, add link for download on page.
	}
	

	encodedUri = encodeURI(csvContent_alltogether);
    link = document.createElement("a");
    link.setAttribute("href", encodedUri);
    link.setAttribute("download", "my_data_all_together.csv");
    document.body.appendChild(link); // Required for FF
	document.getElementById("status").innerHTML="Numbers plotted are per " + String(NUM_PEOPLE)+".";
	if(!WEBPAGE_VERSION){
		link.click();	//TODO: Instead of click link, add link for download on page.
	}
	

}

function plot_plotly(data,plot_position,title_text,legends){

	var trace = [];

	for (var count = 0; count < data.length;count++){
		var trace1 = {
			x: [],
			y: [],
			mode: 'lines',
			name: legends[count]
		  };
		  for (var count2 = 0; count2 < data[count].length;count2++){
			  trace1.x.push(data[count][count2][0]);
			  trace1.y.push(data[count][count2][1]);

		  }
		  trace.push(trace1)
	}
	
	  
	  var data_plot = trace;
	  
	  var layout = {
		// title: {
		//   text: "Numbers plotted are per " + String(NUM_PEOPLE)+".",
		//   font: {
		// 	family: 'Courier New, monospace',
		// 	size: 24
		//   },
		//   xref: 'paper',
		//   x: 0.05,
		// },
		xaxis: {
		  title: {
			text: 'Days',
			font: {
			  family: 'Courier New, monospace',
			  size: 18,
			  color: '#7f7f7f'
			}
		  },
		},
		yaxis: {
		  title: {
			text: title_text,
			font: {
			  family: 'Courier New, monospace',
			  size: 18,
			  color: '#7f7f7f'
			}
		  }
		}
	  };
	  
	  
	  Plotly.newPlot(plot_position, data_plot, layout);
}

function run_and_plot_2() {
	var interventions = [NO_INTERVENTION,CASE_ISOLATION,HOME_QUARANTINE]
	var returned_values = []
	var legends = ['No Intervention','Case Isolation','Home Quarantine'];

	for (var count = 0; count < interventions.length;count++){
		INTERVENTION = interventions[count];
		returned_values.push(run_simulation());
	}
	
	

	
	plot_plotly([returned_values[0][6],returned_values[1][6],returned_values[2][6]],'num_affected_plot','Number Affected',legends);
	plot_plotly([returned_values[0][0],returned_values[1][0],returned_values[2][0]],'num_infected_plot','Number Infected',legends);
	plot_plotly([returned_values[0][1],returned_values[1][1],returned_values[2][1]],'num_exposed_plot','Number Exposed',legends);
	plot_plotly([returned_values[0][2],returned_values[1][2],returned_values[2][2]],'num_hospitalised_plot','Number Hospitalised',legends);
	plot_plotly([returned_values[0][3],returned_values[1][3],returned_values[2][3]],'num_critical_plot','Number Critical',legends);
	plot_plotly([returned_values[0][4],returned_values[1][4],returned_values[2][4]],'num_fatalities_plot','Number Fatalities',legends);
	plot_plotly([returned_values[0][5],returned_values[1][5],returned_values[2][5]],'num_recovered_plot','Number Recovered',legends);
	
	var encodedUri = encodeURI(csvContent);
    var link = document.createElement("a");
    link.setAttribute("href", encodedUri);
    link.setAttribute("download", "my_data.csv");
    document.body.appendChild(link); // Required for FF

	link.click();	//TODO: Instead of click link, add link for download on page.

	encodedUri = encodeURI(csvContent_alltogether);
    link = document.createElement("a");
    link.setAttribute("href", encodedUri);
    link.setAttribute("download", "my_data_all_together.csv");
    document.body.appendChild(link); // Required for FF

	link.click();	

}


//Main function
function runSimulations(){

	//clear_plots();

	//get the inputs from the HTML page
	NUM_DAYS = document.getElementById("numDays").value; // == 0 ? NUM_DAYS : document.getElementById("numDays").value;
	NUM_TIMESTEPS = NUM_DAYS*SIM_STEPS_PER_DAY;

	INIT_FRAC_INFECTED = document.getElementById("initFrac").value;
//	COMPLIANCE_PROBABILITY = document.getElementById("compliance").value;

	INCUBATION_PERIOD = parseFloat(document.getElementById("Incubation").value)/2;
	INCUBATION_PERIOD_SCALE = INCUBATION_PERIOD*SIM_STEPS_PER_DAY; // 2.29 days

	
	MEAN_ASYMPTOMATIC_PERIOD = document.getElementById("asymptomaticMean").value;
	MEAN_SYMPTOMATIC_PERIOD = document.getElementById("symptomaticMean").value;
	SYMPTOMATIC_FRACTION = document.getElementById("symtomaticFraction").value;
	MEAN_HOSPITAL_REGULAR_PERIOD = document.getElementById("meanHospitalPeriod").value;
	MEAN_HOSPITAL_CRITICAL_PERIOD = document.getElementById("meanICUPeriod").value;
	COMPLIANCE_PROBABILITY = document.getElementById("compliance").value;
	


	ASYMPTOMATIC_PERIOD = MEAN_ASYMPTOMATIC_PERIOD*SIM_STEPS_PER_DAY; 
	SYMPTOMATIC_PERIOD = MEAN_SYMPTOMATIC_PERIOD*SIM_STEPS_PER_DAY; 
	HOSPITAL_REGULAR_PERIOD = MEAN_HOSPITAL_REGULAR_PERIOD*SIM_STEPS_PER_DAY;
	HOSPITAL_CRITICAL_PERIOD = MEAN_HOSPITAL_CRITICAL_PERIOD*SIM_STEPS_PER_DAY;

	BETA_H = document.getElementById("betaHouse").value;
	BETA_W = document.getElementById("betaWork").value;
	BETA_C = document.getElementById("betaCommunity").value;
	BETA_S = document.getElementById("betaSchools").value;

	INTERVENTION = parseInt(document.getElementById("interventions").value);

	console.log(NUM_DAYS, INIT_FRAC_INFECTED, INTERVENTION)
	//where simulation starts
	run_and_plot(INTERVENTION); //run_and_plot(LOCKDOWN_21_CI_HQ_SD_70_PLUS_21_CI);
//run_and_plot_2();
}

function clear_plots(){
	//clear previous plots
	document.getElementById("status").innerHTML = "Simulation in Progress....";
	document.getElementById("num_affected_plot_2").innerHTML = "";
	document.getElementById("num_infected_plot_2").innerHTML = "";
	document.getElementById("num_exposed_plot_2").innerHTML = "";
	document.getElementById("num_hospitalised_plot_2").innerHTML = "";
	document.getElementById("num_critical_plot_2").innerHTML = "";
	document.getElementById("num_fatalities_plot_2").innerHTML = "";
	document.getElementById("num_recovered_plot_2").innerHTML = "";
	document.getElementById("lambda_evolution").innerHTML = "";

	runSimulations();
}

function set_default_values_html(){
	document.getElementById("numDays").value = NUM_DAYS;
	document.getElementById("initFrac").value = INIT_FRAC_INFECTED;
	document.getElementById("Incubation").value= 2*INCUBATION_PERIOD;
	document.getElementById("asymptomaticMean").value = MEAN_ASYMPTOMATIC_PERIOD;
	document.getElementById("symptomaticMean").value=MEAN_SYMPTOMATIC_PERIOD;
	document.getElementById("symtomaticFraction").value = SYMPTOMATIC_FRACTION;
	document.getElementById("meanHospitalPeriod").value = MEAN_HOSPITAL_REGULAR_PERIOD;
	document.getElementById("meanICUPeriod").value = MEAN_HOSPITAL_CRITICAL_PERIOD;
	document.getElementById("compliance").value = COMPLIANCE_PROBABILITY;
	document.getElementById("betaHouse").value = BETA_H;
	document.getElementById("betaWork").value = BETA_W;
	document.getElementById("betaCommunity").value =BETA_C;
	document.getElementById("betaSchools").value = BETA_S;
	document.getElementById("interventions").value = "0";
}

set_default_values_html()
