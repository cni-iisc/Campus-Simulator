#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <cmath>
#include <set>

#include "models.h"
//#include "initializers.h"

auto readJSONFile(std::string filename){
  std::ifstream ifs(filename, std::ifstream::in);
  rapidjson::IStreamWrapper isw(ifs);
  rapidjson::Document d;
  d.ParseStream(isw);
  return d;
}

auto prettyJSON(const rapidjson::Document& d){
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  d.Accept(writer);
  return buffer.GetString();
}

void seed_initial_infection_at_node(agent& node, double time_of_infection){
  node.infection_status = Progression::exposed;
  node.time_of_infection = time_of_infection;
  ++GLOBAL.INIT_ACTUALLY_INFECTED;
}

template <class T>
void set_node_initial_infection(agent& node,
								double community_infection_probability,
								int node_index,
								const T& elem,
								std::vector<count_type>& seed_candidates
								){
  if(SEED_INFECTION_FROM_FILE){
#ifdef DEBUG
	assert(elem["infection_status"].IsInt());
#endif
	if(elem["infection_status"].GetInt()){
	  seed_initial_infection_at_node(node, -elem["time_since_infected"].GetDouble());
	}
  }
  else {
	//Infections not being seeded from file
	bool seed_candidate =
	  (GLOBAL.SEED_HD_AREA_POPULATION || !node.hd_area_resident)
	  && !(GLOBAL.SEED_ONLY_NON_COMMUTER && node.has_to_travel);

	if(GLOBAL.SEED_FIXED_NUMBER){
	  if(seed_candidate){
		seed_candidates.push_back(node_index);
	  }
	}
	else {
	  if(seed_candidate
		 && bernoulli(community_infection_probability)){
		// Always seed non-high-density-ares residents
		// High-density-area residents seeded based on global flag.
		seed_initial_infection_at_node(node, -uniform_real(0, node.incubation_period));
	  }
	}
  }
  // node.infective = (node.infection_status == Progression::infective);
  
}


std::vector<agent> init_nodes_campus(){
  auto indivJSON = readJSONFile(GLOBAL.input_base + "individuals.json");
  auto size = indivJSON.GetArray().Size();
  GLOBAL.num_people = size;
  std::vector<agent> nodes(size);
  //auto community_infection_prob = compute_prob_infection_given_community(GLOBAL.INIT_FRAC_INFECTED, GLOBAL.USE_SAME_INFECTION_PROB_FOR_ALL_WARDS);

  count_type i = 0;

  std::vector<count_type> seed_candidates;
  seed_candidates.reserve(size);
 
  for (auto &elem: indivJSON.GetArray()){
    /*nodes[i].loc = location{elem["lat"].GetDouble(),
                            elem["lon"].GetDouble()};*/

#ifdef DEBUG
    assert(elem["age"].IsInt());
#endif
    int age = elem["age"].GetInt();
    nodes[i].age = age;
    nodes[i].age_group = get_age_group(age);
    nodes[i].age_index = get_age_index(age);
    nodes[i].zeta_a = zeta(age);

    nodes[i].infectiousness = gamma(GLOBAL.INFECTIOUSNESS_SHAPE,
                                    GLOBAL.INFECTIOUSNESS_SCALE);
    nodes[i].severity = bernoulli(GLOBAL.SEVERITY_RATE)?1:0;

  //auto indivJSON_001 = readJSONFile("./individual.json");
	  //count_type var1 = 0;
 		int day = 0;
    nodes[i].interaction_strength.resize(1);
 		for (auto &x: elem["interaction_strength"].GetArray()){
       //nodes.push_back();
 			for(auto &j: x.GetObject()){
 				//std::cout<<j.name.GetString()<<" "<<j.value.GetDouble()<<"\n";
 				nodes[i].interaction_strength[day][std::stoi(j.name.GetString())] = j.value.GetDouble();
         //std::cout<<nodes[i].interaction_strength[day][std::stoi(j.name.GetString())];
        nodes[i].kappa[std::stoi(j.name.GetString())] = 1;
        //nodes[i].interaction_strength[day].insert({std::stoi(j.name.GetString()),j.value.GetDouble()});
 				//nodes[var1].interaction_strength[day][j.value.GetDouble()];
 				//nodes[j].interaction_strength = elem["interaction_strength"].GetArray();
 			}
 		day++;
		}
    
//#ifdef DEBUG
//    assert(elem["household"].IsInt());
//#endif
    //change the nodes data structure
    //include type, hostel, department, interaction strength
    //nodes[i].hostel = elem["hostel"].GetInt();
    //nodes[i].dept = elem["dept"].GetInt();
    //nodes[i].type = elem["type"].GetInt();
    /*nodes[i].home = elem["household"].GetInt();

    nodes[i].workplace = WORKPLACE_HOME; //null workplace, by default
    nodes[i].workplace_type = WorkplaceType::home; //home, by default
    nodes[i].workplace_subnetwork = 0;*/

    /*if(elem["workplaceType"].IsInt()){
      switch(elem["workplaceType"].GetInt()){
      case 1:
        if(elem["workplace"].IsNumber()){
          nodes[i].workplace_type = WorkplaceType::office;
          nodes[i].workplace = int(elem["workplace"].GetDouble());
          //Travel
          nodes[i].has_to_travel = bernoulli(GLOBAL.P_TRAIN);
        }
        break;
      case 2:
        if(elem["school"].IsNumber()){
          nodes[i].workplace_type = WorkplaceType::school;
          nodes[i].workplace = int(elem["school"].GetDouble());
          //Travel
          nodes[i].has_to_travel = bernoulli(GLOBAL.P_TRAIN);
          nodes[i].workplace_subnetwork = age;
        }
        break;
      default:
        break;
      }
    }*/
//#ifdef DEBUG
//    assert(elem["wardNo"].IsInt());
//#endif
    //count_type community = elem["wardNo"].GetInt() - 1;
    //minus 1 for 0-based indexing. PB: Might need to use
    //"wardIndex" instead, because that is the one actually sent by
    //the generator scripts.


    // Does the individual live in a slum?  In that case we need to
    // scale the contribution to their infection rates by a factor.
    //
    // Only use this feature if the field is present in the
    // "individuals" input files.
    /*if(elem.HasMember("slum")){

#ifdef DEBUG
      assert(elem["slum"].IsInt());
#endif

      if(elem["slum"].GetInt()){
        nodes[i].hd_area_resident = true;
        nodes[i].hd_area_factor = GLOBAL.HD_AREA_FACTOR;
        nodes[i].hd_area_exponent = GLOBAL.HD_AREA_EXPONENT;
      }
    }*/
    
    //nodes[i].community = community;
    //nodes[i].funct_d_ck = f_kernel(elem["CommunityCentreDistance"].GetDouble());

    nodes[i].incubation_period = gamma(GLOBAL.INCUBATION_PERIOD_SHAPE,
                                       GLOBAL.INCUBATION_PERIOD_SCALE);
    nodes[i].asymptomatic_period = gamma(1.0,
                                         GLOBAL.ASYMPTOMATIC_PERIOD);
    nodes[i].symptomatic_period = gamma(1.0,
                                        GLOBAL.SYMPTOMATIC_PERIOD);
    
    nodes[i].hospital_regular_period = GLOBAL.HOSPITAL_REGULAR_PERIOD;
    nodes[i].hospital_critical_period = GLOBAL.HOSPITAL_CRITICAL_PERIOD;

    double community_infection_prob = 0;
    //Now procees node to check if it could be an initial seed given
    //all its other data
    set_node_initial_infection(nodes[i],
                               community_infection_prob,
                               i, elem,
                               seed_candidates);

    nodes[i].test_status.tested_epoch = -1*GLOBAL.MINIMUM_TEST_INTERVAL*GLOBAL.SIM_STEPS_PER_DAY;
    
    ++i;
  }
  assert(i == GLOBAL.num_people);

  // If seeding a fixed number, go through the list of seed candidates
  // and seed a randomly chosen fixed number of them
  if(GLOBAL.SEED_FIXED_NUMBER){
    count_type candidate_list_size = seed_candidates.size();
    if (candidate_list_size > GLOBAL.INIT_FIXED_NUMBER_INFECTED){

      //Randomly permute the list of candidates
      std::shuffle(seed_candidates.begin(), seed_candidates.end(), GENERATOR);
      
    }
    count_type num = std::min(candidate_list_size, GLOBAL.INIT_FIXED_NUMBER_INFECTED);
    for(count_type j = 0; j < num; ++j){
      seed_initial_infection_at_node(nodes[seed_candidates[j]],
                                     -uniform_real(0, nodes[seed_candidates[j]].incubation_period));
    }
  }
  return nodes;
}


std::vector<Interaction_Space> init_interaction_spaces(){
    auto interactionJSON = readJSONFile(GLOBAL.input_base+"interaction_spaces.json");

    auto interactionSize = interactionJSON.GetArray().Size();
    //GLOBAL.num_workplaces = interactionSize;
    //GLOBAL.num_people = interactionSize;

    auto size = interactionSize;
    std::vector<Interaction_Space> interaction_spaces(size);
    //Interaction space is a new data structure, we can change models.h later
    //Need to store in nodes
    count_type index =0;

    for (auto &elem: interactionJSON.GetArray()){
        interaction_spaces[index].set(elem["lat"].GetDouble(),
                        elem["lon"].GetDouble());
        
        interaction_spaces[index].interaction_type = static_cast<InteractionType>(elem["type"].GetInt());
        interaction_spaces[index].set_active_duration(elem["active_duration"].GetDouble());
        interaction_spaces[index].set_id(elem["id"].GetInt());
        interaction_spaces[index].set_alpha(elem["alpha"].GetDouble());
        interaction_spaces[index].set_beta(elem["beta"].GetDouble());
        ++index;
    }
    //xassert(index == GLOBAL.num_workplaces);
    //assert(index == GLOBAL.num_people);
    return interaction_spaces;
}

double update_interaction_spaces(agent& node,int cur_time, Interaction_Space& i_space){
  //std::cout<<(node.kappa[i_space.id])<<"\n";
  return ((node.infective?1.0:0.0)
	* node.kappa[i_space.id]
	* node.infectiousness
	* node.interaction_strength[cur_time][i_space.id]); 
}

double update_n_k(agent& node, int cur_time, Interaction_Space& i_space){
  return node.interaction_strength[cur_time][i_space.id];
}

void update_interaction_space_lambda(std::vector<agent> nodes,  std::vector<Interaction_Space>& i_spaces, int cur_day){
  for(auto& i_space: i_spaces){
      double sum_value = 0;
      double sum_n_k = 0;
      double N_k, Lam_k_t;
    for (auto& individual: i_space.individuals){
      sum_value += update_interaction_spaces(nodes[individual], cur_day, i_space);
      sum_n_k += update_n_k(nodes[individual], cur_day, i_space); 
    }
    N_k = (1/i_space.active_duration)*sum_n_k;
    Lam_k_t = i_space.beta*(pow(N_k, -i_space.alpha))*(1/i_space.active_duration)*sum_value;
    i_space.lambda = Lam_k_t;
    //std::cout<<N_k<<" "<<Lam_k_t<<"\n";
  } 
}
//Take care of cur_time
//Write .h file - Done
//Make changes to makefile t compile this - go through makefile
void update_individual_lambda(std::vector<agent>& nodes, std::vector<Interaction_Space>& i_spaces, int cur_day){
  for(auto& node: nodes){
    double sum = 0;
    for(auto& ispace: node.interaction_strength[cur_day]){
      sum+=ispace.second*node.kappa[ispace.first]*i_spaces[ispace.first].lambda;
    }
    node.lambda = sum;  
  }
}


void assign_individual_campus(std::vector<agent>& nodes, std::vector<Interaction_Space>& interaction_space, int cur_day){
  for(count_type i = 0; i < nodes.size(); ++i){
    for(auto& ispace: nodes[i].interaction_strength[cur_day]){
      interaction_space[ispace.first].individuals.push_back(i);
    }
  }
}

node_update_status update_infection(agent& node, int cur_time){
  int age_index = node.age_index;
  bool transition = false;
  node_update_status update_status;
  //console.log(1-Math.exp(-node['lambda']/SIM_STEPS_PER_DAY))
  ///TODO: Parametrise transition times
  if (node.infection_status==Progression::susceptible){
	//#pragma omp critical
	{
	  transition = bernoulli(1-exp(-node.lambda/GLOBAL.SIM_STEPS_PER_DAY));
	}
	if(transition){
	  node.infection_status = Progression::exposed; //move to exposed state
	  node.time_of_infection = cur_time;
	  node.infective = false;
	  update_status.new_infection = true;
	}
  }
  else if(node.infection_status==Progression::exposed
		  && (double(cur_time) - node.time_of_infection
			  > node.incubation_period)){
	node.infection_status = Progression::infective; //move to infective state
	node.infective = true;
	node.time_became_infective = cur_time;
	update_status.new_infective = true;
  }
  else if(node.infection_status==Progression::infective
		  && (double(cur_time) - node.time_of_infection
			  > (node.incubation_period
				 + node.asymptomatic_period)) ){
	//#pragma omp critical
	{
	  transition = bernoulli(GLOBAL.SYMPTOMATIC_FRACTION);
	}
	if(transition){
	  node.infection_status = Progression::symptomatic; //move to symptomatic
	  node.infective = true;
	  update_status.new_symptomatic = true;
	  node.entered_symptomatic_state = true;
	}
	else {
	  node.state_before_recovery = node.infection_status;
	  node.infection_status = Progression::recovered; //move to recovered
	  node.infective = false;
	}
  }
  else if(node.infection_status==Progression::symptomatic
		  && (double(cur_time) - node.time_of_infection
			  > (node.incubation_period
				 + node.asymptomatic_period
				 + node.symptomatic_period))){
	//#pragma omp critical
	{
	  transition = bernoulli(STATE_TRAN[age_index][0]);
	}
	if(transition){
	  node.infection_status = Progression::hospitalised; //move to hospitalisation
	  node.infective = false;
	  update_status.new_hospitalization = true;
	  node.entered_hospitalised_state = true;
	}
	else {
	  node.state_before_recovery = node.infection_status;
	  node.infection_status = Progression::recovered; //move to recovered
	  node.infective = false;
	}
  }
  else if(node.infection_status==Progression::hospitalised
		  && (double(cur_time) - node.time_of_infection
			  > (node.incubation_period
				 + node.asymptomatic_period
				 + node.symptomatic_period
				 + node.hospital_regular_period))){
	//#pragma omp critical
	{
	  transition = bernoulli(STATE_TRAN[age_index][1]);
	}
	if(transition){
	  node.infection_status = Progression::critical; //move to critical care
	  node.infective = false;
	}
	else {
	  node.state_before_recovery = node.infection_status;
	  node.infection_status = Progression::recovered; //move to recovered
	  node.infective = false;
	}
  }
  else if(node.infection_status==Progression::critical
		  && (double(cur_time) - node.time_of_infection
			  > (node.incubation_period
				 + node.asymptomatic_period
				 + node.symptomatic_period
				 + node.hospital_regular_period
				 + node.hospital_critical_period))){
	//#pragma omp critical
	{
	  transition = bernoulli(STATE_TRAN[age_index][2]);
	}
	if(transition){
	  node.infection_status = Progression::dead;//move to dead
	  node.infective = false;
	}
	else {
	  node.state_before_recovery = node.infection_status;
	  node.infection_status = Progression::recovered;//move to recovered
	  node.infective = false;
	}
  }
  //node.lambda_h = update_individual_lambda_h(node,cur_time);
  //node.lambda_w = update_individual_lambda_w(node,cur_time);
  //node.lambda_c = update_individual_lambda_c(node,cur_time);
  //node.lambda_nbr_cell = update_individual_lambda_nbr_cell(node,cur_time);
  //std::cout<<update_status.new_infective<<"\n";
  return update_status;
}



