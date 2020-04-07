#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>

#include "models.h"
#include "initializers.h"

#ifdef DEBUG
#include <cassert>
#endif

using namespace std;

const string input_base = "../simulator/input_files/";

auto readJSONFile(string filename){
  ifstream ifs(filename, ifstream::in);
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


vector<house> init_homes(){
  auto houseJSON = readJSONFile(input_base + "houses.json");
  auto size = houseJSON.GetArray().Size();
  vector<house> homes(size);
  GLOBAL.num_homes = size;

  int index = 0;
  for (auto &elem: houseJSON.GetArray()){
	homes[index].set(elem["lat"].GetDouble(),
					 elem["lon"].GetDouble(),
					 compliance());
	++index;
  }
  return homes;
}

vector<workplace> init_workplaces() {
  auto schoolJSON = readJSONFile(input_base + "schools.json");
  auto wpJSON = readJSONFile(input_base + "workplaces.json");

  auto school_size = schoolJSON.GetArray().Size();
  GLOBAL.num_schools = school_size;

  auto wp_size = wpJSON.GetArray().Size();
  GLOBAL.num_workplaces = wp_size;

  auto size = wp_size +  school_size;
  vector<workplace> wps(size);

  int index = 0;
  // schools come first followed by workspaces, as in the JSON version
  for (auto &elem: schoolJSON.GetArray()){
	wps[index].set(elem["lat"].GetDouble(),
				   elem["lon"].GetDouble(),
				   WorkplaceType::school);
	++index;
  }

  for (auto &elem: wpJSON.GetArray()){
	wps[index].set(elem["lat"].GetDouble(),
				   elem["lon"].GetDouble(),
				   WorkplaceType::office);
	++index;
  }

  return wps;
}

vector<community> init_community() {
  auto comJSON = readJSONFile(input_base + "commonArea.json");

  auto size = comJSON.GetArray().Size();
  GLOBAL.num_communities = size;

  vector<community> communities(size);

  int index = 0;
  // schools come first followed by workspaces, as in the JSON version
  for (auto &elem: comJSON.GetArray()){
	communities[index].set(elem["lat"].GetDouble(),
						   elem["lon"].GetDouble(),
						   elem["wardNo"].GetInt());
	++index;
  }
  sort(communities.begin(), communities.end(),
	   [](const auto& a, const auto& b){
		 return a.wardNo < b.wardNo;
	   });
  return communities;
}


vector<double> compute_prob_infection_given_community(double infection_probability, bool set_uniform){
  auto fracPopJSON = readJSONFile(input_base + "fractionPopulation.json");
  auto num_communities = fracPopJSON.GetArray().Size();
  if(set_uniform){
	return vector<double>(num_communities, infection_probability);
  }
  else {
	auto fracQuarantinesJSON = readJSONFile(input_base + "quarantinedPopulation.json");
	const rapidjson::Value& quar_array = fracQuarantinesJSON.GetArray();
	vector<double> prob_infec_given_community(num_communities);
	int index = 0;
	for(auto &elem: fracPopJSON.GetArray()){
	  prob_infec_given_community[index] =
		infection_probability *
		quar_array[index]["fracQuarantined"].GetDouble() /
		elem["fracPopulation"].GetDouble();
	  ++index;
	}
	return prob_infec_given_community;
  }
}


vector<agent> init_nodes(){
  auto indivJSON = readJSONFile(input_base + "individuals.json");
  auto size = indivJSON.GetArray().Size();
  GLOBAL.num_people = size;
  vector<agent> nodes(size);
  auto community_infection_prob = compute_prob_infection_given_community(GLOBAL.INIT_FRAC_INFECTED, GLOBAL.USE_SAME_INFECTION_PROB_FOR_ALL_WARDS);

  int i = 0;
  for (auto &elem: indivJSON.GetArray()){
 	nodes[i].loc = location{elem["lat"].GetDouble(),
							elem["lon"].GetDouble()};

#ifdef DEBUG
	assert(elem["age"].IsInt());
#endif
	int age = elem["age"].GetInt();
	nodes[i].age = age;
	nodes[i].age_group = get_age_group(age);
	nodes[i].age_index = get_age_index(age);
	nodes[i].zeta_a = zeta(age);

#ifdef DEBUG
	assert(elem["household"].IsInt());
#endif
	nodes[i].home = elem["household"].GetInt();

	nodes[i].workplace = WORKPLACE_HOME; //null workplace, by default
	nodes[i].workplace_type = WorkplaceType::home; //home, by default

	if(elem["workplaceType"].IsInt()){
	  switch(elem["workplaceType"].GetInt()){
	  case 1:
		if(elem["workplace"].IsNumber()){
		  nodes[i].workplace_type = WorkplaceType::office;
		  nodes[i].workplace = int(elem["workplace"].GetDouble());
		}
		break;
	  case 2:
		if(elem["school"].IsNumber()){
		  nodes[i].workplace_type = WorkplaceType::school;
		  nodes[i].workplace = int(elem["school"].GetDouble());
		}
	  default:
		break;
	  }
	}
#ifdef DEBUG
	assert(elem["wardNo"].IsInt());
#endif
	int community = elem["wardNo"].GetInt() - 1;
	//minus 1 for 0-based indexing.  POSSIBLE BUG: Might need to use
	//"wardIndex" instead, because that is the one actually sent by
	//the generator scripts.
	nodes[i].community = community;
	if(bernoulli(community_infection_prob[community])){
	  nodes[i].infection_status = Progression::exposed;
	} else {
	  nodes[i].infection_status = Progression::susceptible;
	}

	nodes[i].funct_d_ck = f_kernel(elem["CommunityCentreDistance"].GetDouble());


	if(SEED_INFECTION_FROM_FILE){
#ifdef DEBUG
	  assert(elem["infection_status"].GetInt());
#endif
	  nodes[i].infection_status = static_cast<Progression>(elem["infection_status"].GetInt());
	  nodes[i].time_of_infection = (nodes[i].infection_status == Progression::exposed)?(-elem["time_since_infected"].GetDouble()):0;
	} else {
	  nodes[i].time_of_infection = (nodes[i].infection_status == Progression::exposed)?(-uniform_real(0, nodes[i].incubation_period)):0;
	  //Why do we take time of infection to be 0 by default?
	}
	//Set infective status, set the time of infection, and other age-related factors
	nodes[i].infective = (nodes[i].infection_status == Progression::infective);
	//POSSIBLE BUG: In the JS code, and here, infection_status is
	//never set to infective, so the above line would be superfluous!

	++i;
  }
#ifdef DEBUG
  cout << i << "\n";
#endif
  return nodes;
}

matrix<double> compute_community_distances(const vector<community>& communities){
  auto wardDistJSON = readJSONFile(input_base + "wardCentreDistance.json");
  const rapidjson::Value& mat = wardDistJSON.GetArray();
  auto size = mat.Size();
  matrix<double> dist_matrix(size, vector<double>(size));
  for(int i = 0; i < size; ++i){
	for(int j = i + 1; j < size; ++j){
	  dist_matrix[i][j] = mat[i][to_string(j + 1).c_str()].GetDouble();
	  dist_matrix[j][i] = dist_matrix[i][j];
	}
  }
  return dist_matrix;
}

void assign_individual_home_community(vector<agent>& nodes, vector<house>& homes, vector<workplace>& workplaces, vector<community>& communities){
  //Assign individuals to homes, workplace, community
  for(int i = 0; i < nodes.size(); ++i){
	int home = nodes[i].home;
	homes[home].individuals.push_back(i);
	 //No checking for null as all individuals have a home
	nodes[i].compliant = homes[home].compliant;
	//All members of the household are set the same compliance value
	
	int workplace = nodes[i].workplace;
	if(workplace != WORKPLACE_HOME){
	  workplaces[workplace].individuals.push_back(i);
	}
	//No checking for null as all individuals have a community/ward
	communities[nodes[i].community].individuals.push_back(i);
  }
}

// Compute scale factors for each home, workplace and community. Done once at the beginning.
void compute_scale_homes(vector<house>& homes){
  for (int w = 0; w < homes.size(); w++){
	if(homes[w].individuals.size()==0){
	  homes[w].scale = 0;
	} else {
	  homes[w].scale = GLOBAL.BETA_H*homes[w].Q_h/(pow(homes[w].individuals.size(), GLOBAL.ALPHA));
	}
  }
}

void compute_scale_workplaces(vector<workplace>& workplaces){
  double beta_workplace;
  for (int w=0; w < workplaces.size(); w++) {
	if(workplaces[w].individuals.size()==0){
	  workplaces[w].scale = 0;
	} else {
	  if(workplaces[w].workplace_type == WorkplaceType::office){
		beta_workplace = GLOBAL.BETA_W; //workplace
	  } else if (workplaces[w].workplace_type == WorkplaceType::school){
		beta_workplace = GLOBAL.BETA_S; //school
	  }
	  workplaces[w].scale = beta_workplace*workplaces[w].Q_w/workplaces[w].individuals.size();
	}
  }
}

void compute_scale_communities(const vector<agent>& nodes, vector<community>& communities){
  for (int w=0; w < communities.size(); w++) {
	double sum_value = 0;
	for (auto indiv: communities[w].individuals){
	  sum_value += nodes[indiv].funct_d_ck;
	}
	if(sum_value==0){
	  communities[w].scale = 0;
	}
	else communities[w].scale = GLOBAL.BETA_C*communities[w].Q_c/sum_value;
  }
}


