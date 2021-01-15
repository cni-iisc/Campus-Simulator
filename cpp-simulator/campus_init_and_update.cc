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
#include <cstdio>

#include "models.h"
//#include "interventions.h"
//#include "testing.h"
//#include "initializers.h"

auto readJSONFile(std::string filename)
{
  std::ifstream ifs(filename, std::ifstream::in);
  rapidjson::IStreamWrapper isw(ifs);
  rapidjson::Document d;
  d.ParseStream(isw);
  return d;
}

auto prettyJSON(const rapidjson::Document &d)
{
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  d.Accept(writer);
  return buffer.GetString();
}

void seed_initial_infection_at_node(agent &node, double time_of_infection)
{
  node.infection_status = Progression::exposed;
  node.time_of_infection = time_of_infection;
  ++GLOBAL.INIT_ACTUALLY_INFECTED;
}

template <class T>
void set_node_initial_infection(agent &node,
                                double community_infection_probability,
                                int node_index,
                                const T &elem,
                                std::vector<count_type> &seed_candidates)
{
  if (SEED_INFECTION_FROM_FILE)
  {
#ifdef DEBUG
    assert(elem["infection_status"].IsInt());
#endif
    if (elem["infection_status"].GetInt())
    {
      seed_initial_infection_at_node(node, -elem["time_since_infected"].GetDouble());
    }
  }
  else
  {
    //Infections not being seeded from file
    bool seed_candidate =
        (GLOBAL.SEED_HD_AREA_POPULATION || !node.hd_area_resident) && !(GLOBAL.SEED_ONLY_NON_COMMUTER && node.has_to_travel);

    if (GLOBAL.SEED_FIXED_NUMBER)
    {
      if (seed_candidate)
      {
        seed_candidates.push_back(node_index);
      }
    }
    else
    {
      if (seed_candidate && bernoulli(community_infection_probability))
      {
        // Always seed non-high-density-ares residents
        // High-density-area residents seeded based on global flag.
        seed_initial_infection_at_node(node, -uniform_real(0, node.incubation_period));
      }
    }
  }
  // node.infective = (node.infection_status == Progression::infective);
}

std::vector<agent> init_nodes_campus()
{
  auto indivJSON = readJSONFile(GLOBAL.input_base + "individuals.json");
  auto size = indivJSON.GetArray().Size();
  GLOBAL.num_people = size;
  std::vector<agent> nodes(size);
  //auto community_infection_prob = compute_prob_infection_given_community(GLOBAL.INIT_FRAC_INFECTED, GLOBAL.USE_SAME_INFECTION_PROB_FOR_ALL_WARDS);

  count_type i = 0;

  std::vector<count_type> seed_candidates;
  seed_candidates.reserve(size);

  for (auto &elem : indivJSON.GetArray())
  {
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
    int periodicity = elem["Periodicity"].GetInt();

    nodes[i].infectiousness = gamma(GLOBAL.INFECTIOUSNESS_SHAPE,
                                    GLOBAL.INFECTIOUSNESS_SCALE);
    nodes[i].severity = bernoulli(GLOBAL.SEVERITY_RATE) ? 1 : 0;

    //auto indivJSON_001 = readJSONFile("./individual.json");
    //count_type var1 = 0;
    int day = 0;
    nodes[i].interaction_strength.resize(periodicity); //Resize acc to days
    for (auto &x : elem["interaction_strength"].GetArray())
    {
      //nodes.push_back();
      for (auto &j : x.GetObject())
      {
        //std::cout<<j.name.GetString()<<" "<<j.value.GetDouble()<<"\n";
        nodes[i].interaction_strength[day][std::stoi(j.name.GetString())] = j.value.GetDouble();
        //std::cout<<nodes[i].interaction_strength[day][std::stoi(j.name.GetString())];
        nodes[i].kappa[std::stoi(j.name.GetString())] = 1;

        // if(i == 9){
        //   std::cout<<j.name.GetString()<<"\t"<<j.value.GetDouble()<<"\t";
        // }
        //nodes[i].interaction_strength[day].insert({std::stoi(j.name.GetString()),j.value.GetDouble()});
        //nodes[var1].interaction_strength[day][j.value.GetDouble()];
        //nodes[j].interaction_strength = elem["interaction_strength"].GetArray();
      }
      day++;
      //std::cout<<day<<"\n";
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

    nodes[i].test_status.tested_epoch = -1 * GLOBAL.MINIMUM_TEST_INTERVAL * GLOBAL.SIM_STEPS_PER_DAY;

    ++i;
  }
  assert(i == GLOBAL.num_people);

  // If seeding a fixed number, go through the list of seed candidates
  // and seed a randomly chosen fixed number of them
  if (GLOBAL.SEED_FIXED_NUMBER)
  {
    count_type candidate_list_size = seed_candidates.size();
    if (candidate_list_size > GLOBAL.INIT_FIXED_NUMBER_INFECTED)
    {

      //Randomly permute the list of candidates
      std::shuffle(seed_candidates.begin(), seed_candidates.end(), GENERATOR);
    }
    count_type num = std::min(candidate_list_size, GLOBAL.INIT_FIXED_NUMBER_INFECTED);
    for (count_type j = 0; j < num; ++j)
    {
      seed_initial_infection_at_node(nodes[seed_candidates[j]],
                                     -uniform_real(0, nodes[seed_candidates[j]].incubation_period));
    }
  }
  return nodes;
}

std::vector<Interaction_Space> init_interaction_spaces()
{
  auto interactionJSON = readJSONFile(GLOBAL.input_base + "interaction_spaces.json");

  auto interactionSize = interactionJSON.GetArray().Size();
  //GLOBAL.num_workplaces = interactionSize;
  //GLOBAL.num_people = interactionSize;

  auto size = interactionSize;
  std::vector<Interaction_Space> interaction_spaces(size);
  //Interaction space is a new data structure, we can change models.h later
  //Need to store in nodes
  count_type index = 0;

  for (auto &elem : interactionJSON.GetArray())
  {
    interaction_spaces[index].set(elem["lat"].GetDouble(),
                                  elem["lon"].GetDouble());

    interaction_spaces[index].interaction_type = static_cast<InteractionType>(elem["type"].GetInt());
    interaction_spaces[index].set_active_duration(elem["active_duration"].GetDouble());
    interaction_spaces[index].set_id(elem["id"].GetInt());
    //interaction_spaces[index].set_alpha(elem["alpha"].GetDouble());
    //interaction_spaces[index].set_beta(elem["beta"].GetDouble());
    ++index;
  }
  //xassert(index == GLOBAL.num_workplaces);
  //assert(index == GLOBAL.num_people);
  return interaction_spaces;
}

void init_transmission_coefficients(std::vector<Interaction_Space> &interaction_spaces){
  auto transmission_coefficients_json = readJSONFile(GLOBAL.input_base + "transmission_coefficients.json");
  auto transmission_coefficients_json_size = transmission_coefficients_json.GetArray().Size();

  auto size = transmission_coefficients_json_size;
  std::vector<Interaction_Space> transmission_coefficients(size);
  count_type index = 0;
  for (auto &elem: transmission_coefficients_json.GetArray()){
    transmission_coefficients[index].interaction_type = static_cast<InteractionType>(elem["type"].GetInt());
    transmission_coefficients[index].set_alpha(elem["alpha"].GetDouble());
    transmission_coefficients[index].set_beta(elem["beta"].GetDouble());
    ++index;
  }
  for (auto &i_space : interaction_spaces){
    switch (i_space.interaction_type)
    {
    case InteractionType ::day_scholar :
      i_space.set_alpha(transmission_coefficients[0].alpha);
      i_space.set_beta(transmission_coefficients[0].beta);
      break;

    case InteractionType ::classroom :
      i_space.set_alpha(transmission_coefficients[1].alpha);
      i_space.set_beta(transmission_coefficients[1].beta);
      break;

    case InteractionType :: hostel : 
      i_space.set_alpha(transmission_coefficients[2].alpha);
      i_space.set_beta(transmission_coefficients[2].beta);
      break;
    
    case InteractionType :: mess : 
      i_space.set_alpha(transmission_coefficients[3].alpha);
      i_space.set_beta(transmission_coefficients[3].beta);
      break;

    default:
      break;
    }
    // i_space.set_alpha(transmission_coefficients[i_space.interaction_type].alpha);
  }
}

void print_intervention_params(const int index, const intervention_params intv_params)
{
  std::cout << std::endl<< "Index : " << index << ". num_days = " << intv_params.num_days;
  std::cout << ". Case Isolation : " << intv_params.case_isolation;
  //std::cout<<". Home Quarantine : " << intv_params.home_quarantine;
  std::cout << ". Lockdown : " << intv_params.lockdown;
  //std::cout<<". SDO : " << intv_params.social_dist_elderly;
  //std::cout<<". School Closed : " << intv_params.school_closed;
  //std::cout<<". workplace_odd_even : " << intv_params.workplace_odd_even;
  //std::cout<<". SC_factor : " << intv_params.SC_factor;
  //std::cout<<". community_factor : " << intv_params.community_factor;
  //std::cout<<". neighbourhood_containment : " << intv_params.neighbourhood_containment;
  //std::cout<<". ward_containment : " << intv_params.ward_containment;
  //std::cout<<". compliance : " << intv_params.compliance;
  //std::cout<<". compliance_hd : " << intv_params.compliance_hd;
  //std::cout<<". trains_active : " << intv_params.trains_active;
  //std::cout<<". fraction_forced_to_take_train : " << intv_params.fraction_forced_to_take_train;
  std::cout << ". Class Isolation : " << intv_params.class_isolation;
}

std::vector<intervention_params> init_intervention_params()
{
  std::vector<intervention_params> intv_params;
  //std::cout<<int(GLOBAL.INTERVENTION);
  if (GLOBAL.INTERVENTION == Intervention::intv_file_read)
  {
    std::cout << std::endl
              << "Inside init_intervention_params";
    auto intvJSON = readJSONFile(GLOBAL.input_base + "campus_interventions_00.json");

    intv_params.reserve(intvJSON.GetArray().Size());

    int index = 0;
    for (auto &elem : intvJSON.GetArray())
    {
      intervention_params temp;
      if ((elem.HasMember("num_days")) && (elem["num_days"].GetInt() > 0))
      {
        temp.num_days = elem["num_days"].GetInt();
        if (elem.HasMember("compliance"))
        {
          temp.compliance = elem["compliance"].GetDouble();
          temp.compliance_hd = elem["compliance"].GetDouble();
          //By default, compliance = compliance_hd. Can be reset below
        }
        else
        {
          temp.compliance = GLOBAL.COMPLIANCE_PROBABILITY;
          temp.compliance_hd = GLOBAL.COMPLIANCE_PROBABILITY;
          //By default, compliance = compliance_hd. Can be reset below
        }
        if (elem.HasMember("compliance_hd"))
        {
          temp.compliance_hd = elem["compliance_hd"].GetDouble();
        }
        else
        {
          temp.compliance_hd = GLOBAL.COMPLIANCE_PROBABILITY;
        }
        if (elem.HasMember("case_isolation"))
        {
          temp.case_isolation = elem["case_isolation"]["active"].GetBool();
        }
        if (elem.HasMember("class_isolation"))
        {
          temp.class_isolation = elem["class_isolation"]["active"].GetBool();
        }
        if (elem.HasMember("lockdown"))
        {
          //TODO: collect all these statements in a function.
          temp.lockdown = elem["lockdown"]["active"].GetBool();
        }

        /*if(elem.HasMember("workplace_odd_even")){
        temp.workplace_odd_even = elem["workplace_odd_even"]["active"].GetBool();
      }*/
        /*if(elem.HasMember("community_factor")){
        temp.community_factor = elem["community_factor"].GetDouble();
      }*/
        /*    if(elem.HasMember("trains")){
            temp.trains_active = elem["trains"]["active"].GetBool();
            if(elem["trains"].HasMember("fraction_forced_to_take_train")){
              temp.fraction_forced_to_take_train = elem["trains"]["fraction_forced_to_take_train"].GetDouble();
            }
          }*/
        /*if(elem.HasMember("neighbourhood_containment")){
        temp.neighbourhood_containment = elem["neighbourhood_containment"]["active"].GetBool() && GLOBAL.ENABLE_CONTAINMENT;
        if(!GLOBAL.ENABLE_CONTAINMENT){
        std::cout<<std::endl<<"To enable containment strategies, add  --ENABLE_CONTAINMENT to argument list. Ignoring neighbourhood containment.";
        }
      }*/
        /*if(elem.HasMember("ward_containment")){
        temp.ward_containment = elem["ward_containment"]["active"].GetBool() && GLOBAL.ENABLE_CONTAINMENT;
        if(!GLOBAL.ENABLE_CONTAINMENT){
        std::cout<<std::endl<<"To enable containment strategies, add  --ENABLE_CONTAINMENT to argument list. Ignoring ward containment.";
        }
      }*/
        print_intervention_params(index, temp);
        intv_params.push_back(temp);
        ++index;
      }
      else
      {
        std::cout << std::endl
                  << "num_days not specified or less than 1. Skipping current index.";
        assert(false);
      }
    }
  }
  std::cout << std::endl
            << "Intervention params size = " << intv_params.size();
  //std::cout<<std::endl<<"GLOBAL INTERVENTION: "<<GLOBAL.INTERVENTION;
  return intv_params;
}

double update_interaction_spaces(agent &node, int cur_time, Interaction_Space &i_space)
{
  //std::cout<<(node.kappa[i_space.id])<<"\t"<<node.infective<<"\t";
  //std::cout<<node.infective<<"\n";
  //std::cout<<"Inside update interaction spaces function"<<"\n";
  return ((node.infective ? 1.0 : 0.0) * node.kappa[i_space.id] * node.infectiousness * node.interaction_strength[cur_time][i_space.id]);
}

double update_n_k(agent &node, int cur_time, Interaction_Space &i_space)
{
  //std::cout<<"Inside update nk function"<<"\n";

  // if(i_space.id == 61 &&x == 10){
  //   std::cout<<x<<":"<<"\t";
  //   for(auto& isp: node.interaction_strength[0]){
  //     std::cout<<isp.first<<"\t"<<isp.second<<"\t";
  //   }
  // }

  return node.interaction_strength[cur_time][i_space.id];
}

void update_interaction_space_lambda(std::vector<agent> &nodes, std::vector<Interaction_Space> &i_spaces, int day)
{
  //std::cout<<"Inside update interaction space lambda function"<<"\n";

  // for (auto& node: nodes){
  //   if(day <= 3){
  //     for(auto& ispace: node.interaction_strength[day]){
  //       ispace.second = 0;
  //     }
  //   }
  // }
  for (auto &i_space : i_spaces)
  {
    double sum_value = 0;
    double sum_n_k = 0;
    double N_k, Lam_k_t;
    for (auto &individual : i_space.individuals[day])
    {
      sum_value += update_interaction_spaces(nodes[individual], day, i_space);
      sum_n_k += update_n_k(nodes[individual], day, i_space);
      //update_interaction_spaces(nodes[individual], 1, i_space);
      // if (i_space.id == 85){
      //   std::cout<<individual<<"\t"<<sum_value<<"\t"<<sum_n_k<<"\n";
      // }
    }
    N_k = (1 / i_space.active_duration) * sum_n_k;
    Lam_k_t = i_space.beta * (pow(N_k, -i_space.alpha)) * (1 / i_space.active_duration) * sum_value;
    //std::cout<<"Beta: "<<i_space.beta<<"\t";
    i_space.lambda = (Lam_k_t);
    //std::cout<<"Day: "<<cur_day<<"\n";
    // for(auto& lam: i_space.lambda){
    //   if(lam != 0){
    //     std::cout<<"Lambda values: "<<lam<<"\t"<<"ID: "<<i_space.id<<"\n";
    //   }
    // }
  }
  //std::cout<<i_space.id<<"\t"<<Lam_k_t<<"\t"<<i_space.active_duration<<"\t"<<sum_n_k<<"\t"<<sum_value<<"\n";
}
//Take care of cur_time
//Write .h file - Done
//Make changes to makefile t compile this - go through makefile
void update_individual_lambda(std::vector<agent> &nodes, std::vector<Interaction_Space> &i_spaces, int day)
{
  //std::cout<<"Inside update individual lambda function"<<"\n";
  // for (auto& ispace: i_spaces){
  // 		//int x = 0;
  // 		for (int cur_day = 0; cur_day < day; cur_day++){
  // 			std::cout<<"ID: "<<ispace.id<<"\t"<<"Day: "<<cur_day<<"\t"<<"Lambda: "<<ispace.lambda[cur_day]<<"\n";
  // 			//x++;
  // 		}
  // 	}
  //int i = 0;
  // for (auto& node: nodes){
  //   if(day <= 3){
  //     for(auto& ispace: node.interaction_strength[day]){
  //       ispace.second = 0;
  //     }
  //   }
  // }
  for (auto &node : nodes)
  {
    //std::cout<<"Inside first for loop\n";
    //std::cout<<"Inside second for loop\n";
    double sum = 0;
    for (auto &ispace : node.interaction_strength[day])
    {
      //std::cout<<"Inside third for loop\n";
      sum += ispace.second * node.kappa[ispace.first] * i_spaces[ispace.first].lambda;
      //std::cout<<"ID: "<<i<<"\t"<<"Day: "<<cur_day<<"\t"<<"Kappa value: "<<node.kappa[ispace.first]<<"\t"<<"Lambda values: "<<i_spaces[ispace.first].lambda[cur_day]<<"\n";
    }
    node.lambda = sum;
    //std::cout<<"ID: "<<i<<"\t"<<"Day: "<<cur_day<<"\t"<<"Individual lambda: "<<sum<<"\n";
    //i++;
  }
}

//  void print_interaction_strength(agent& node, std::vector<Interaction_Space>& i_spaces, int day){
//    for(auto& ispace: node.interaction_strength[day]){
//       std::cout<<"Day: "<<day<<"\t"<<"Interaction space ID: "<<ispace.first<<"\t"<<"Interaction strength: "<<ispace.second<<"\n";
//    }
//  }

//Update assign individual to take in more days
void assign_individual_campus(std::vector<agent> &nodes, std::vector<Interaction_Space> &interaction_space, int day)
{

  std::vector<int> person;
  for (auto &ispace : interaction_space)
  {
    for (int cur_day = 0; cur_day < day; cur_day++)
    {
      ispace.individuals.push_back(person);
    }
  }
  //std::cout<<"Inside assign individual function"<<"\t"<<"Day: "<<day<<"\t"<<"Nodes.size: "<<nodes.size()<<"\n";
  for (count_type i = 0; i < nodes.size(); ++i)
  {
    //std::cout<<"Inside 1st loop"<<"\n";
    for (int cur_time = 0; cur_time < day; cur_time++)
    {
      //std::cout<<"Inside 2nd loop"<<"\n";
      for (auto &ispace : nodes[i].interaction_strength[cur_time])
      {
        //std::cout<<"Inside 3rd loop"<<"\n";
        interaction_space[ispace.first].individuals[cur_time].push_back(i);
        //std::cout<<interaction_space[ispace];
      }
    }
  }

  // int count = 0;
  // std::cout<<day<<"\n";
  // for(auto& ispace: interaction_space){
  //   std::cout<<"ID: "<<ispace.id<<"\n";
  //   for (int cur_day = 0; cur_day < day; cur_day++){
  //     std::cout<<"Day: "<<cur_day<<"\n";
  //     for (auto& individual: ispace.individuals[cur_day]){
  //      std::cout<<"Individuals associated: "<<individual<<"\t";
  //     }
  //   }
  // }
}

node_update_status update_infection(agent &node, int cur_time, int day)
{
  int age_index = node.age_index;
  bool transition = false;
  node_update_status update_status;
  //console.log(1-Math.exp(-node['lambda']/SIM_STEPS_PER_DAY))
  ///TODO: Parametrise transition times
  //printf(node.infection_status)
  if (node.infection_status == Progression::susceptible)
  {
    //#pragma omp critical
    {
      transition = bernoulli(1 - exp(-node.lambda / GLOBAL.SIM_STEPS_PER_DAY));
    }

    if (transition)
    {
      node.infection_status = Progression::exposed; //move to exposed state
      node.time_of_infection = cur_time;
      node.infective = false;
      update_status.new_infection = true;
    }
  }
  else if (node.infection_status == Progression::exposed && (double(cur_time) - node.time_of_infection > node.incubation_period))
  {
    node.infection_status = Progression::infective; //move to infective state
    node.infective = true;
    node.time_became_infective = cur_time;
    update_status.new_infective = true;
  }
  else if (node.infection_status == Progression::infective && (double(cur_time) - node.time_of_infection > (node.incubation_period + node.asymptomatic_period)))
  {
    //#pragma omp critical
    {
      transition = bernoulli(GLOBAL.SYMPTOMATIC_FRACTION);
    }
    if (transition)
    {
      node.infection_status = Progression::symptomatic; //move to symptomatic
      node.infective = true;
      update_status.new_symptomatic = true;
      node.entered_symptomatic_state = true;
    }
    else
    {
      node.state_before_recovery = node.infection_status;
      node.infection_status = Progression::recovered; //move to recovered
      node.infective = false;
    }
  }
  else if (node.infection_status == Progression::symptomatic && (double(cur_time) - node.time_of_infection > (node.incubation_period + node.asymptomatic_period + node.symptomatic_period)))
  {
    //#pragma omp critical
    {
      transition = bernoulli(STATE_TRAN[age_index][0]);
    }
    if (transition)
    {
      node.infection_status = Progression::hospitalised; //move to hospitalisation
      node.infective = false;
      update_status.new_hospitalization = true;
      node.entered_hospitalised_state = true;
    }
    else
    {
      node.state_before_recovery = node.infection_status;
      node.infection_status = Progression::recovered; //move to recovered
      node.infective = false;
    }
  }
  else if (node.infection_status == Progression::hospitalised && (double(cur_time) - node.time_of_infection > (node.incubation_period + node.asymptomatic_period + node.symptomatic_period + node.hospital_regular_period)))
  {
    //#pragma omp critical
    {
      transition = bernoulli(STATE_TRAN[age_index][1]);
    }
    if (transition)
    {
      node.infection_status = Progression::critical; //move to critical care
      node.infective = false;
    }
    else
    {
      node.state_before_recovery = node.infection_status;
      node.infection_status = Progression::recovered; //move to recovered
      node.infective = false;
    }
  }
  else if (node.infection_status == Progression::critical && (double(cur_time) - node.time_of_infection > (node.incubation_period + node.asymptomatic_period + node.symptomatic_period + node.hospital_regular_period + node.hospital_critical_period)))
  {
    //#pragma omp critical
    {
      transition = bernoulli(STATE_TRAN[age_index][2]);
    }
    if (transition)
    {
      node.infection_status = Progression::dead; //move to dead
      node.infective = false;
    }
    else
    {
      node.state_before_recovery = node.infection_status;
      node.infection_status = Progression::recovered; //move to recovered
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