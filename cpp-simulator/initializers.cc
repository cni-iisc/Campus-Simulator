#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>

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


int main(){
  auto homes = init_homes();
  auto workplaces = init_workplaces();
  auto communities = init_community();
  return 0;
}
