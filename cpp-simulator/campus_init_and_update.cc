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
#include "campus_testing.h"
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

void init_config_params()
{
  auto config_json = readJSONFile(GLOBAL.input_base + GLOBAL.config_file);
  for (auto &elem : config_json.GetArray())
  {
    GLOBAL.MINIMUM_SUBGROUP_SIZE = elem["MIN_GROUP_SIZE"].GetInt();
    GLOBAL.MAXIMUM_SUBGROUP_SIZE = elem["MAX_GROUP_SIZE"].GetInt();
    GLOBAL.BETA_SCALING_FACTOR = elem["BETA_SCALING_FACTOR"].GetDouble();
    GLOBAL.PERIODICITY = elem["PERIODICITY"].GetInt();
    GLOBAL.AVERAGE_NUMBER_ASSOCIATIONS = elem["AVERAGE_NUMBER_ASSOCIATIONS"].GetInt();
    GLOBAL.minimum_hostel_time = elem["minimum_hostel_time"].GetDouble();
    GLOBAL.kappa_class_case_isolation = elem["kappa_class_case_isolation"].GetDouble();
    GLOBAL.kappa_hostel_case_isolation = elem["kappa_hostel_case_isolation"].GetDouble();
    GLOBAL.kappa_mess_case_isolation = elem["kappa_mess_case_isolation"].GetDouble();
    GLOBAL.kappa_cafe_case_isolation = elem["kappa_cafe_case_isolation"].GetDouble();
    GLOBAL.kappa_smaller_networks_case_isolation = elem["kappa_smaller_networks_case_isolation"].GetDouble();
    GLOBAL.kappa_lib_case_isolation = elem["kappa_lib_case_isolation"].GetDouble();
    GLOBAL.kappa_class_lockdown = elem["kappa_class_lockdown"].GetDouble();
    GLOBAL.kappa_hostel_lockdown = elem["kappa_hostel_lockdown"].GetDouble();
    GLOBAL.kappa_mess_lockdown = elem["kappa_mess_lockdown"].GetDouble();
    GLOBAL.kappa_cafe_lockdown = elem["kappa_cafe_lockdown"].GetDouble();
    GLOBAL.kappa_smaller_networks_lockdown = elem["kappa_smaller_networks_lockdown"].GetDouble();
    GLOBAL.kappa_lib_lockdown = elem["kappa_lib_lockdown"].GetDouble();
  }
}

std::vector<agent> init_nodes_campus()
{
  auto indivJSON = readJSONFile(GLOBAL.input_base + GLOBAL.individuals);
  auto size = indivJSON.GetArray().Size();
  GLOBAL.num_people = size;
  std::vector<agent> nodes(size);

  count_type i = 0;

  std::vector<count_type> seed_candidates;
  seed_candidates.reserve(size);

  for (auto &elem : indivJSON.GetArray())
  {

#ifdef DEBUG
    assert(elem["age"].IsInt());
#endif
    int age = elem["age"].GetInt();
    nodes[i].age = age;
    nodes[i].age_group = get_age_group(age);
    nodes[i].age_index = get_age_index(age);
    nodes[i].zeta_a = zeta(age);
    nodes[i].personType = static_cast<person_type>(elem["Type"].GetInt());
    nodes[i].infectiousness = gamma(GLOBAL.INFECTIOUSNESS_SHAPE,
                                    GLOBAL.INFECTIOUSNESS_SCALE);
    nodes[i].severity = bernoulli(GLOBAL.SEVERITY_RATE) ? 1 : 0;
    nodes[i].compliance_factor = get_non_compliance_metric();
    int day = 0;
    nodes[i].interaction_strength.resize(GLOBAL.PERIODICITY); //Resize acc to days
    for (auto &x : elem["interaction_strength"].GetArray())
    {
      for (auto &j : x.GetObject())
      {
        nodes[i].interaction_strength[day][std::stoi(j.name.GetString())] = j.value.GetDouble();
        // std::cout<<nodes[i].interaction_strength[day]<<"\t";
        //std::cout<<j.name.GetString()<<"\t";
        nodes[i].kappa[std::stoi(j.name.GetString())] = 1;
      }
      day++;
    }

    nodes[i].incubation_period = gamma(GLOBAL.INCUBATION_PERIOD_SHAPE,
                                       GLOBAL.INCUBATION_PERIOD_SCALE);
    nodes[i].asymptomatic_period = gamma(1.0,
                                         GLOBAL.ASYMPTOMATIC_PERIOD);
    nodes[i].symptomatic_period = gamma(1.0,
                                        GLOBAL.SYMPTOMATIC_PERIOD);

    nodes[i].time_to_test_positive = nodes[i].incubation_period;
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
  auto interactionJSON = readJSONFile(GLOBAL.input_base + GLOBAL.interaction_spaces);

  auto interactionSize = interactionJSON.GetArray().Size();
  auto size = interactionSize;
  std::vector<Interaction_Space> interaction_spaces(size);

  count_type index = 0;

  for (auto &elem : interactionJSON.GetArray())
  {
    assert(index == elem["id"].GetInt());
    interaction_spaces[index].set(elem["lat"].GetDouble(),
                                  elem["lon"].GetDouble());

    interaction_spaces[index].interaction_type = static_cast<InteractionType>(elem["type"].GetInt());
    interaction_spaces[index].set_active_duration(elem["active_duration"].GetDouble());
    interaction_spaces[index].set_id(elem["id"].GetInt());
    interaction_spaces[index].set_avg_time(elem["avg_time"].GetDouble());
    ++index;
  }
  return interaction_spaces;
}

Interaction_Space create_interaction_space(double lat, double lon, InteractionType type, double active_duration, int id, double alpha, double beta, std::vector<std::vector<int> > individuals, double avg_time)
{
  Interaction_Space ispace;
  ispace.set(lat, lon);
  ispace.interaction_type = type;
  ispace.set_active_duration(active_duration);
  ispace.set_alpha(alpha);
  ispace.set_id(id);
  ispace.set_beta(beta);
  ispace.set_avg_time(avg_time);
  ispace.individuals = individuals;
  return ispace;
}

void init_transmission_coefficients(std::vector<Interaction_Space> &interaction_spaces)
{
  auto transmission_coefficients_json = readJSONFile(GLOBAL.input_base + GLOBAL.transmission_coefficients);
  auto transmission_coefficients_json_size = transmission_coefficients_json.GetArray().Size();

  auto size = transmission_coefficients_json_size;
  std::vector<Interaction_Space> transmission_coefficients(size);
  count_type index = 0;
  for (auto &elem : transmission_coefficients_json.GetArray())
  {
    assert(index == elem["type"].GetInt());
    transmission_coefficients[index].interaction_type = static_cast<InteractionType>(elem["type"].GetInt());
    transmission_coefficients[index].set_alpha(elem["alpha"].GetDouble());
    transmission_coefficients[index].set_beta(elem["beta"].GetDouble());
    ++index;
  }
  for (int i = 0; i < interaction_spaces.size(); ++i)
  {
    interaction_spaces[i].set_alpha(transmission_coefficients[int(interaction_spaces[i].interaction_type)].alpha);
    interaction_spaces[i].set_beta(transmission_coefficients[int(interaction_spaces[i].interaction_type)].beta);
  }
}

void print_intervention_params(const int index, const intervention_params intv_params)
{
  std::cout << std::endl
            << "Index : " << index << ". num_days = " << intv_params.num_days;
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
  std::cout << ". compliance : " << intv_params.compliance;
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
    auto intvJSON = readJSONFile(GLOBAL.input_base + GLOBAL.intervention_filename);

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
          //By default, compliance = compliance_hd. Can be reset below
        }
        else
        {
          temp.compliance = GLOBAL.COMPLIANCE_PROBABILITY;
          //By default, compliance = compliance_hd. Can be reset below
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

void print_testing_protocol(const int index, const testing_probability probabilities)
{
  std::cout << std::endl
            << "Index : " << index << ". num_days = " << probabilities.num_days;
  std::cout << ".  prob_test_index_symptomatic:  " << probabilities.prob_test_index_symptomatic;
  std::cout << ".  prob_test_index_hospitalised:  " << probabilities.prob_test_index_hospitalised;

  for (auto &prob : probabilities.prob_test_positive_symptomatic)
  {
    std::cout << ". prob_test_positive_symptomatic for interaction type" << prob.first << " = " << prob.second;
  }

  for (auto &prob : probabilities.prob_test_hospitalised_symptomatic)
  {
    std::cout << ". prob_test_hospitalised_symptomatic for interaction type" << prob.first << " = " << prob.second;
  }

  for (auto &prob : probabilities.prob_test_symptomatic_symptomatic)
  {
    std::cout << ". prob_test_symptomatic_symptomatic for interaction type" << prob.first << " = " << prob.second;
  }

  for (auto &prob : probabilities.prob_test_positive_asymptomatic)
  {
    std::cout << ". prob_test_positive_asymptomatic for interaction type" << prob.first << " = " << prob.second;
  }

  for (auto &prob : probabilities.prob_test_hospitalised_asymptomatic)
  {
    std::cout << ". prob_test_hospitalised_asymptomatic for interaction type" << prob.first << " = " << prob.second;
  }

  for (auto &prob : probabilities.prob_test_symptomatic_asymptomatic)
  {
    std::cout << ". prob_test_symptomatic_asymptomatic for interaction type" << prob.first << " = " << prob.second;
  }

  // for(int i = 0; i < probabilities.prob_test_positive_symptomatic.size(); i++){
  //   std::cout<<". prob_test_positive_symptomatic for interaction type" << i <<" = " << probabilities.prob_test_positive_symptomatic[static_cast<InteractionType>(i)];
  // }

  // for(int i = 0; i < probabilities.prob_test_hospitalised_symptomatic.size(); i++){
  //   std::cout<<". prob_test_hospitalised_symptomatic for interaction type" << i <<" = " << probabilities.prob_test_hospitalised_symptomatic[static_cast<InteractionType>(i)];
  // }

  // for(int i = 0; i < probabilities.prob_test_symptomatic_symptomatic.size(); i++){
  //   std::cout<<". prob_test_symptomatic_symptomatic for interaction type" << i <<" = " << probabilities.prob_test_symptomatic_symptomatic[static_cast<InteractionType>(i)];
  // }

  // for(int i = 0; i < probabilities.prob_test_positive_asymptomatic.size(); i++){
  //   std::cout<<". prob_test_positive_asymptomatic for interaction type" << i <<" = " << probabilities.prob_test_positive_asymptomatic[static_cast<InteractionType>(i)];
  // }

  // for(int i = 0; i < probabilities.prob_test_hospitalised_asymptomatic.size(); i++){
  //   std::cout<<". prob_test_hospitalised_asymptomatic for interaction type" << i <<" = " << probabilities.prob_test_hospitalised_asymptomatic[static_cast<InteractionType>(i)];
  // }

  // for(int i = 0; i < probabilities.prob_test_symptomatic_asymptomatic.size(); i++){
  //   std::cout<<". prob_test_symptomatic_asymptomatic for interaction type" << i <<" = " << probabilities.prob_test_symptomatic_asymptomatic[static_cast<InteractionType>(i)];
  // }

  std::cout << ".  prob_retest_recovered:  " << probabilities.prob_retest_recovered;
}

std::vector<testing_probability> init_testing_protocol()
{
  std::vector<testing_probability> testing_protocol;
  std::cout << "Function called"
            << "\n";
  if (GLOBAL.TESTING_PROTOCOL == Testing_Protocol::testing_protocol_file_read)
  {
    std::cout << std::endl
              << "Inside init_testing_protocol";
    auto testProtJSON = readJSONFile(GLOBAL.input_base + GLOBAL.testing_protocol_filename);

    testing_protocol.reserve(testProtJSON.GetArray().Size());
    count_type index = 0;
    count_type num_index = 0;
    for (auto &elem : testProtJSON.GetArray())
    {
      testing_probability temp;
      if ((elem.HasMember("num_days")) && (elem["num_days"].GetInt() > 0))
      {
        temp.num_days = elem["num_days"].GetInt();
        if (elem.HasMember("test_false_positive"))
        {
          GLOBAL.TEST_FALSE_POSITIVE = elem["test_false_positive"].GetDouble();
        }
        if (elem.HasMember("test_false_negative"))
        {
          GLOBAL.TEST_FALSE_NEGATIVE = elem["test_false_negative"].GetDouble();
        }
        if (elem.HasMember("prob_test_index_symptomatic"))
        {
          temp.prob_test_index_symptomatic = elem["prob_test_index_symptomatic"].GetDouble();
        }
        if (elem.HasMember("prob_test_index_hospitalised"))
        {
          temp.prob_test_index_hospitalised = elem["prob_test_index_hospitalised"].GetDouble();
        }

        //Testing probabilities for household networks

        if (elem.HasMember("prob_test_positive_symptomatic"))
        {
          for (auto &probmap : elem["prob_test_positive_symptomatic"].GetObject())
          {
            temp.prob_test_positive_symptomatic[num_index] = probmap.value.GetDouble();
            num_index++; //have to static cast? how?
          }
          num_index = 0;
        }

        if (elem.HasMember("prob_test_hospitalised_symptomatic"))
        {
          for (auto &probmap : elem["prob_test_hospitalised_symptomatic"].GetObject())
          {
            temp.prob_test_hospitalised_symptomatic[num_index] = probmap.value.GetDouble();
            num_index++;
          }
          num_index = 0;
        }

        if (elem.HasMember("prob_test_symptomatic_symptomatic"))
        {
          for (auto &probmap : elem["prob_test_symptomatic_symptomatic"].GetObject())
          {
            temp.prob_test_symptomatic_symptomatic[num_index] = probmap.value.GetDouble();
            num_index++;
          }
          num_index = 0;
        }

        if (elem.HasMember("prob_test_positive_asymptomatic"))
        {
          for (auto &probmap : elem["prob_test_positive_asymptomatic"].GetObject())
          {
            temp.prob_test_positive_asymptomatic[num_index] = probmap.value.GetDouble();
            num_index++;
          }
          num_index = 0;
        }

        if (elem.HasMember("prob_test_hospitalised_asymptomatic"))
        {
          for (auto &probmap : elem["prob_test_hospitalised_asymptomatic"].GetObject())
          {
            temp.prob_test_hospitalised_asymptomatic[num_index] = probmap.value.GetDouble();
            num_index++;
          }
          num_index = 0;
        }

        if (elem.HasMember("prob_test_symptomatic_asymptomatic"))
        {
          for (auto &probmap : elem["prob_test_symptomatic_asymptomatic"].GetObject())
          {
            temp.prob_test_symptomatic_asymptomatic[num_index] = probmap.value.GetDouble();
            num_index++;
          }
          num_index = 0;
        }

        if (elem.HasMember("prob_retest_recovered"))
        {
          temp.prob_retest_recovered = elem["prob_retest_recovered"].GetDouble();
        }

        if (elem.HasMember("prob_contact_trace_shospitalisedymptomatic"))
        {
          for (auto &probmap : elem["prob_contact_trace_symptomatic"].GetObject())
          {
            temp.prob_contact_trace_symptomatic[num_index] = probmap.value.GetDouble();
            num_index++;
          }
          num_index = 0;
        }

        if (elem.HasMember("prob_contact_trace_hospitalised"))
        {
          for (auto &probmap : elem["prob_contact_trace_hospitalised"].GetObject())
          {
            temp.prob_contact_trace_hospitalised[num_index] = probmap.value.GetDouble();
            num_index++;
          }
          num_index = 0;
        }

        if (elem.HasMember("prob_contact_trace_positive"))
        {
          for (auto &probmap : elem["prob_contact_trace_positive"].GetObject())
          {
            temp.prob_contact_trace_positive[num_index] = probmap.value.GetDouble();
            num_index++;
          }
          num_index = 0;
        }

        testing_protocol.push_back(temp);
        print_testing_protocol(index, temp);
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
            << "Testing Protocal size = " << testing_protocol.size();
  return testing_protocol;
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
    // std::cout<<"Beta: "<<i_space.beta<<"\t";
    i_space.lambda = (Lam_k_t);
    //std::cout<<"Day: "<<cur_day<<"\n";
    // for(auto& lam: i_space.lambda){
    //   if(lam != 0){
    //     std::cout<<"Lambda values: "<<lam<<"\t"<<"ID: "<<i_space.id<<"\n";
    //   }
    // }
    // if (i_space.interaction_type == InteractionType :: cafeteria ){
    //   std::cout<<"Lambda : "<<i_space.lambda<<"\t";
    //   std::cout<<"Beta: "<<i_space.beta<<"\t";
    //   std::cout<<"Sum value : "<<sum_value<<"\t";
    //   std::cout<<"NK: "<<N_k<<"\n";
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
void assign_individual_campus(std::vector<agent> &nodes, std::vector<Interaction_Space> &interaction_space)
{

  std::vector<int> person;
  for (auto &ispace : interaction_space)
  {
    for (int cur_day = 0; cur_day < GLOBAL.PERIODICITY; cur_day++)
    {
      ispace.individuals.push_back(person);
    }
  }
  //std::cout<<"Inside assign individual function"<<"\t"<<"Day: "<<day<<"\t"<<"Nodes.size: "<<nodes.size()<<"\n";
  for (count_type i = 0; i < nodes.size(); ++i)
  {
    //std::cout<<"Inside 1st loop"<<"\n";
    for (int cur_time = 0; cur_time < GLOBAL.PERIODICITY; cur_time++)
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

  for (auto &node : nodes)
  {
    for (auto &elem : node.interaction_strength)
    {
      for (auto &int_space : elem)
      {
        node.spaces.insert(int_space.first);
      }
    }
  }
}

void sample_groups(std::vector<agent> &nodes, std::vector<Interaction_Space> &interaction_spaces)
{
  //std::cout<<"\n BETA is: "<<GLOBAL.BETA_SCALING_FACTOR<<"\n";
  srand(time(0));
  std::vector<Interaction_Space> interac_space;
  int avg_group_size = (GLOBAL.MINIMUM_SUBGROUP_SIZE + GLOBAL.MAXIMUM_SUBGROUP_SIZE) / 2;
  //std::cout<<"Number of groups : "<<NUM_GROUPS<<"\n";
  int index = interaction_spaces.size();
  for (auto &ispace : interaction_spaces)
  {
    if (ispace.interaction_type == InteractionType ::hostel)
    {
      int NUM_GROUPS = GLOBAL.AVERAGE_NUMBER_ASSOCIATIONS * ispace.individuals[0].size() / avg_group_size;
      //std::cout<<"\n"<<"Number of sub groups: "<<NUM_GROUPS<<"\n";
      for (int j = 0; j < NUM_GROUPS; j++)
      {
        //std::cout<<"Create interaction space called"<<"\n";
        std::vector<int> temp;
        std::vector<std::vector<int> > GROUPS;
        int random_value = uniform_count_type(GLOBAL.MINIMUM_SUBGROUP_SIZE, GLOBAL.MAXIMUM_SUBGROUP_SIZE);
        //std::cout<<"random value: " << random_value << "\n";
        int GROUP_STRENGTH = random_value > int(ispace.individuals[0].size()) ? int(ispace.individuals[0].size()) / 2 : random_value;
        //std::cout<<int(individual.size())<<"\n";
        //std::cout<<"Group strength is: "<<GROUP_STRENGTH<<"\n";
        std::sample(ispace.individuals[0].begin(), ispace.individuals[0].end(), std::back_inserter(temp), GROUP_STRENGTH, GENERATOR);
        for (int i = 0; i < GLOBAL.PERIODICITY; i++)
        {
          GROUPS.push_back(temp);
        }
        //`std::cout<<"Create interaction space called"<<"\n";
        Interaction_Space int_space = create_interaction_space(ispace.lat, ispace.lon, InteractionType::smaller_networks, ispace.active_duration, index, ispace.alpha, ispace.beta * GLOBAL.BETA_SCALING_FACTOR, GROUPS, ispace.avg_time);
        // std::cout<<int_space.id<<"\n";
        // for (auto &individual : int_space.individuals){
        //   for (auto &indi : individual) {
        //     std::cout<<indi<<"\t";
        // }
        // std::cout<<"\n";
        // }
        interac_space.push_back(int_space);
        //interaction_spaces.push_back(int_space);
        for (auto temp1 : temp)
        {
          for (int k = 0; k < GLOBAL.PERIODICITY; k++)
          {
            nodes[temp1].interaction_strength[k].insert({index, int_space.avg_time});
          }
          nodes[temp1].kappa.insert({index, 1});
          nodes[temp1].spaces.insert(int_space.id);
        }

        index++;
        // for (auto &temp1 : GROUPS){
        //   std::cout<<"\n";
        //   for (auto &sometingelse : temp1){
        //     std::cout<<sometingelse<<"\t";
        //   }
        // }
        // std::cout<<"Groups are: "<<"\n";
        // for (auto &group : temp) {
        //   std::cout<<group<<"\t";
        //   }
      }
    }
  }
  interaction_spaces.insert(interaction_spaces.end(), interac_space.begin(), interac_space.end());
}

void cafeteria_active_duration(std::vector<agent> &nodes, std::vector<Interaction_Space> &interaction_spaces, int day)
{
  std::vector<int> cafeteria;
  for (auto &i_space : interaction_spaces)
  {
    if (i_space.interaction_type == InteractionType::cafeteria)
    {
      cafeteria.push_back(i_space.id);
    }
  }
  for (auto &node : nodes)
  {
    std::vector<int> cafe;
    std::sample(cafeteria.begin(), cafeteria.end(), std::back_inserter(cafe), 1, GENERATOR);
    double cafe_time = gamma(GLOBAL.ACTIVE_DURATION_SHAPE, interaction_spaces[cafe[0]].avg_time / GLOBAL.ACTIVE_DURATION_SHAPE);
    // std::cout<<"Cafe time: "<<cafe_time<<"\t";
    if (node.personType == person_type::student || node.personType == person_type::faculty)
    {
      node.interaction_strength[day][cafe[0]] = cafe_time;
      // std::cout<<node.interaction_strength[day][cafe[0]]<<"\n";
    }
    for (auto &elem : node.interaction_strength[day])
    {
      if (interaction_spaces[elem.first].interaction_type == InteractionType::hostel || interaction_spaces[elem.first].interaction_type == InteractionType::outside_campus)
      {
        if (elem.second <= cafe_time)
        {
          node.interaction_strength[day][cafe[0]] = elem.second - GLOBAL.minimum_hostel_time; //add to config.json
          elem.second = GLOBAL.minimum_hostel_time;
        }
        else
        {
          elem.second -= cafe_time;
        }
      }
    }
  }
}

void library_active_duration(std::vector<agent> &nodes, std::vector<Interaction_Space> &interaction_spaces, int day)
{
  std::vector<int> library;
  for (auto &i_space : interaction_spaces)
  {
    if (i_space.interaction_type == InteractionType::library)
    {
      library.push_back(i_space.id);
    }
  }
  for (auto &node : nodes)
  {
    std::vector<int> lib;
    std::sample(library.begin(), library.end(), std::back_inserter(lib), 1, GENERATOR);
    double lib_time = gamma(GLOBAL.ACTIVE_DURATION_SHAPE, interaction_spaces[lib[0]].avg_time / GLOBAL.ACTIVE_DURATION_SHAPE);
    // std::cout<<"Cafe time: "<<cafe_time<<"\t";
    if (node.personType == person_type::student || node.personType == person_type::faculty)
    {
      node.interaction_strength[day][lib[0]] = lib_time;
      // std::cout<<node.interaction_strength[day][cafe[0]]<<"\n";
    }
    for (auto &elem : node.interaction_strength[day])
    {
      if (interaction_spaces[elem.first].interaction_type == InteractionType::hostel || interaction_spaces[elem.first].interaction_type == InteractionType::outside_campus)
      {
        if (elem.second <= lib_time)
        {
          node.interaction_strength[day][lib[0]] = elem.second - GLOBAL.minimum_hostel_time; //add to config.json
          elem.second = GLOBAL.minimum_hostel_time;
        }
        else
        {
          elem.second -= lib_time;
        }
      }
    }
  }
}

void cafeteria_reset(std::vector<agent> &nodes, std::vector<Interaction_Space> &interaction_spaces, int day)
{
  double temp;
  for (auto &node : nodes)
  {
    for (auto &elem : node.interaction_strength[day])
    {
      if (interaction_spaces[elem.first].interaction_type == InteractionType::cafeteria && elem.second != 0)
      {
        temp = elem.second;
        elem.second = 0;
      }
    }
    for (auto &elem : node.interaction_strength[day])
    {
      if (interaction_spaces[elem.first].interaction_type == InteractionType::hostel || interaction_spaces[elem.first].interaction_type == InteractionType::outside_campus)
      {
        elem.second += temp;
      }
    }
  }
}

void library_reset(std::vector<agent> &nodes, std::vector<Interaction_Space> &interaction_spaces, int day)
{
  double temp;
  for (auto &node : nodes)
  {
    for (auto &elem : node.interaction_strength[day])
    {
      if (interaction_spaces[elem.first].interaction_type == InteractionType::library && elem.second != 0)
      {
        temp = elem.second;
        elem.second = 0;
      }
    }
    for (auto &elem : node.interaction_strength[day])
    {
      if (interaction_spaces[elem.first].interaction_type == InteractionType::hostel || interaction_spaces[elem.first].interaction_type == InteractionType::outside_campus)
      {
        elem.second += temp;
      }
    }
  }
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
      node.infection_status = Progression::hospitalised;
      GLOBAL.debug_hospitalised++; //move to hospitalisation
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
      node.infection_status = Progression::critical;
      GLOBAL.debug_critical++; //move to critical care
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

void update_test_request(std::vector<agent> &nodes, std::vector<Interaction_Space> &ispaces, const count_type current_time, std::vector<testing_probability> &testing_protocol, count_type day)
{
  testing_probability probabilities;
  if (current_time >= GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS)
  {
    // switch(GLOBAL.TESTING_PROTOCOL){
    // case Testing_Protocol::no_testing:
    //   break;
    // case Testing_Protocol::test_household:
    //   probabilities.prob_test_index_hospitalised = 1;
    //   probabilities.prob_test_household_symptomatic_symptomatic = 0;
    //   probabilities.prob_test_household_symptomatic_asymptomatic = 0;

    //   probabilities.prob_test_household_hospitalised_symptomatic = 1;
    //   probabilities.prob_test_household_hospitalised_asymptomatic = 0;
    //   probabilities.prob_test_household_positive_symptomatic = 1;
    //   probabilities.prob_test_household_positive_asymptomatic = 0;

    //   probabilities.prob_test_neighbourhood_hospitalised_symptomatic = 0;
    //   probabilities.prob_test_neighbourhood_hospitalised_asymptomatic = 0;
    //   probabilities.prob_test_neighbourhood_positive_symptomatic = 0;
    //   probabilities.prob_test_neighbourhood_positive_asymptomatic = 0;

    //   probabilities.prob_contact_trace_household_hospitalised = 1;
    //   probabilities.prob_contact_trace_household_positive = 1;

    //   probabilities.prob_retest_recovered = 1;
    //   set_test_request(nodes, homes, workplaces, nbr_cells, communities, probabilities, current_time);
    //   break;
    //case Testing_Protocol::testing_protocol_file_read:
    set_test_request_fileread(nodes, ispaces, testing_protocol, current_time, day);
    //   break;
    // default:
    //   break;
    // }
  }
}

void update_test_status(std::vector<agent> &nodes, count_type current_time)
{
  // std::cout<<"update_test_status"<<"\n";
  // int count = 0;
  for (auto &node : nodes)
  {
    // std::cout<<"Node ID: "<<count<<"\t"<<node.test_status.test_requested<<"\n";
    if (node.test_status.test_requested)
    {
      GLOBAL.debug_count_tests_requested ++;
      if (node.infection_status == Progression::infective || node.infection_status == Progression::symptomatic || node.infection_status == Progression::hospitalised || node.infection_status == Progression::critical)
      {
        node.test_status.state = bernoulli(GLOBAL.TEST_FALSE_NEGATIVE) ? test_result::negative : test_result::positive;
        node.test_status.tested_positive = node.test_status.tested_positive || (node.test_status.state == test_result::positive);
        node.test_status.tested_epoch = current_time;
        if(node.test_status.tested_positive){
          GLOBAL.debug_count_positive++;
          node.time_tested.push_back(1);
        }
      }
      else if (node.infection_status == Progression::exposed && current_time - node.time_of_infection > node.time_to_test_positive)
      {
        node.test_status.state = bernoulli(GLOBAL.TEST_FALSE_NEGATIVE) ? test_result::negative : test_result::positive;
        node.test_status.tested_positive = node.test_status.tested_positive || (node.test_status.state == test_result::positive);
        //We might want to have higher false negative rate here, depending upon updates in the data.
        node.test_status.tested_epoch = current_time;
        if(node.test_status.tested_positive){
          GLOBAL.debug_count_positive++;
          node.time_tested.push_back(1);
        }
      }
      else
      {
        // Test could come positive for a succeptible/recovered/dead person
        node.test_status.state = bernoulli(GLOBAL.TEST_FALSE_POSITIVE) ? test_result::positive : test_result::negative;
        node.test_status.tested_positive = node.test_status.tested_positive || (node.test_status.state == test_result::positive);
        node.test_status.tested_epoch = current_time;
        if(node.test_status.tested_positive){
          GLOBAL.debug_count_positive++;
          node.time_tested.push_back(1);
        }
      }
      node.test_status.test_requested = false;
    }
    // std::cout<<"Node ID: "<<count<<"\t"<<node.test_status.test_requested<<"\n";
    // count ++;
  }
  // std::cout<<"Tests requested: "<<GLOBAL.debug_count_tests_requested<<"\n";
}