#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>

#include "models.h"

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
						   elem["lon"].GetDouble());
	++index;
  }
  return communities;
}


vector<double> compute_prob_infection_given_community(double infection_probability,bool set_uniform){
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
  auto community_infection_prob = compute_prob_infection_given_community(GLOBAL.INIT_FRAC_INFECTED, false);

  int i = 0;
  for (auto &elem: indivJSON.GetArray()){
	nodes[i].loc = location{elem["lat"].GetDouble(),
							elem["lon"].GetDouble()};
	int age = elem["age"].GetInt();
	nodes[i].age = age;
	nodes[i].age_group = get_age_group(age);
	nodes[i].zeta_a = zeta(age);


	nodes[i].home = elem["household"].GetInt();

	nodes[i].workplace = WORKPLACE_HOME; //null workplace, by default
	nodes[i].workplace_type = WorkplaceType::home; //home, by default
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

void run_simulation(){
  auto homes = init_homes();
  auto workplaces = init_workplaces();
  auto communities = init_community();
  auto nodes = init_nodes();

  auto community_dist_matrix = compute_community_distances(communities);

  assign_individual_home_community(nodes, homes, workplaces, communities);
  
}

int main(){
  run_simulation();
  return 0;
}
