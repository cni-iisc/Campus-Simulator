#!/usr/bin/env python
# coding: utf-8

# In[ ]:


import numpy as np
import random
import pickle

import json
import pandas as pd
import geopandas as gpd
import csv
import argparse

import math
from shapely.geometry import Point, MultiPolygon

import os
from pathlib import Path

import time

import warnings
warnings.filterwarnings('ignore')

import matplotlib.pyplot as plt
from computeDistributions import *


# In[ ]:



inputfiles = {
    "citygeojson":"city.geojson",
    "demographics":"demographics.csv",
    "employment":"employment.csv",
    "household":"households.csv",
    "cityprofile":"cityProfile.json",
    "ODMatrix":"ODMatrix.csv"
    }

outputfiles = {
    "individuals":"individuals.json",
    "houses":"houses.json",
    "workplaces":"workplaces.json",
    "schools":"schools.json",
    "wardCentreDistance":"wardCentreDistance.json",
    "commonArea":"commonArea.json",
    "fractionPopulation":"fractionPopulation.json",
    "PRG_random_state":"PRG_random_state.bin",
    "PRG_np_random_state":"PRG_np_random_state.bin",
    }

workplacesTypes = {
    None: 0,
    "office": 1,
    "school": 2
    }

officeType = {
    "Other":0,
    "SEZ":1,
    "Government":2,
    "IT":3,
    "Construction":4,
    "Medical":5
    }

def workplaces_size_distribution(a=3.26, c=0.97, m_max=2870):
    count=1
    a=3.26
    c=0.97
    m_max=2870
    p_nplus = np.arange(float(m_max))
    for m in range(m_max):
        p_nplus[m] =  ((( (1+m_max/a)/(1+m/a))**c) -1) / (((1+m_max/a)**c) -1)

    p_nminus = 1.0 - p_nplus
    p_n = np.arange(float(m_max))
    prev=0.0
    for m in range(1, m_max):
        p_n[m] = p_nminus[m] - prev
        prev = p_nminus[m]

    return p_n/sum(p_n)


# In[ ]:


def fileExists(path):
    return os.path.isfile(path)

def folderExists(path):
    return os.path.exists(path)

def normalise(raw): 
    # Scale everything so that the array sums to 1
    # It doesn't quite, due to floating point errors, but 
    # np.random.choice does not complian anymore.
    s = sum([float(i) for i in raw]); return [float(i)/s for i in raw]
    
def sampleBinsWeights(bins,weights):
    assert len(bins) == len(weights)
    
    s = str(np.random.choice(bins,1,p=weights)[0])
    if '+' in s:
        return int(s[:-1])+1
        #This is the last bucket, something like x+. Choosing (x+1) by default.
    elif '-' in s:
        (a,b) = s.split('-')
        return random.randint(int(a),int(b)) #random.randint has both inclusive
        # TODO: Consider moving to np.random.random_integer so that we use just
        # one source of randomness. 
    else:
        return int(s)

def distance(lat1, lon1, lat2, lon2):
    radius = 6371 # km

    dlat = math.radians(lat2-lat1)
    dlon = math.radians(lon2-lon1)
    a = math.sin(dlat/2) * math.sin(dlat/2) + math.cos(math.radians(lat1)) * math.cos(math.radians(lat2)) * math.sin(dlon/2) * math.sin(dlon/2)
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1-a))
    d = radius * c

    return d

def workplaces_size_distribution(a=3.26, c=0.97, m_max=2870):
    count=1
    a=3.26
    c=0.97
    m_max=2870
    p_nplus = np.arange(float(m_max))
    for m in range(m_max):
        p_nplus[m] =  ((( (1+m_max/a)/(1+m/a))**c) -1) / (((1+m_max/a)**c) -1)

    p_nminus = 1.0 - p_nplus
    p_n = np.arange(float(m_max))
    prev=0.0
    for m in range(1, m_max):
        p_n[m] = p_nminus[m] - prev
        prev = p_nminus[m]

    return p_n/sum(p_n)


# In[ ]:


class City:
    
    state_numpy = None
    state_random = None
    #Default values:
    
    #ppl working at sez and gov (Bangalore data)
    max_sez=230000 /10
    max_gov= (2295000*(12.327/66.84)*0.5) /10
    max_ites = 1500000 /10
    max_ites_not_sez=max_ites-max_sez

    a_commuter_distance = 4 #parameter in distribution for commuter distance - Thailand paper
    b_commuter_distance = 3.8  #parameter in distribution for commuter distance - Thailand paper
    m_max_commuter_distance = None
    
    nwards = None
    totalPop = None
    
    demographics = None
    employments = None
    geoDF = None
    ODMatrix = None
    
    householdsize_bins = None
    householdsize_weights = None
    age_bins = None
    age_weights = None
    schoolsize_bins = None
    schoolsize_weights = None
    
    has_slums = False
    
    presampled_points = None
    community_centres = None
    
    workers = None
    schoolers = None
        
    #This is what we will eventually generate
    houses = None
    num_houses = None
    individuals = None
    num_individuals = None 
    workplaces = None
    num_workplaces = None
    schools = None
    num_schools = None    
    num_workers = None
    
    def get_random_seeds(self):
        self.state_random = random.getstate()
        self.state_numpy = np.random.get_state()

    def set_random_seeds(self, folder):
        print(f"Restoring random seeds from {folder}.")
        with open(os.path.join(folder, outputfiles['PRG_random_state']), 'rb') as f:
            random.setstate(pickle.load(f))
        with open(os.path.join(folder, outputfiles['PRG_np_random_state']), 'rb') as f:
            np.random.set_state(pickle.load(f))

    def checkName(self, df, name="df"):
        assert self.demographics is not None
        if "wardName" not in df.columns:
            return
        for i in range(df.shape[0]):
            if df["wardName"].iloc[i] != self.demographics['wardName'].iloc[i]:
                print(f"WARNING: Check if this is a mismatch!")
                print(f"{i}\t {name}: {df['wardName'].iloc[i]}\t demographics: {self.demographics['wardName'].iloc[i]}")

    def checkRows(self, df, name="df"):
        if self.nwards is None:
            for i in range(df.shape[0]):
                assert df["wardNo"].iloc[i] == i+1,f"Mismatch in {name}: row {i} has wardNo {df['wardNo'].iloc[i]}"
        else:
            assert df.shape[0] == self.nwards, f"Mismatch in {name}: num_rows is not {nwards}"
            for i in range(self.nwards):
                assert df["wardNo"].iloc[i] == i+1,f"Mismatch in {name}: row {i} has wardNo {df['wardNo'].iloc[i]}"
            if self.demographics is not None:
                self.checkName(df, name=name)
    
    def set_geoDF(self, input_dir):
        assert fileExists(Path(input_dir, inputfiles["citygeojson"])), f"{inputfiles['citygeojson']} missing"
        
        geoDF = gpd.read_file(Path(input_dir,inputfiles["citygeojson"]))
        geoDF['wardNo'] = geoDF['wardNo'].astype(int)
        geoDF = geoDF.sort_values('wardNo')
        geoDF['wardIndex'] = geoDF['wardNo'] - 1
        
        geoDF = geoDF[['wardIndex','wardNo', 'wardName', 'geometry']]
        geoDF['wardBounds'] = geoDF.apply(lambda row: MultiPolygon(row['geometry']).bounds, axis=1)
        self.checkRows(geoDF, "geoDF")
        ##!! Note that the geojson file has coordinates in (longitude, latitude) order!
        geoDF['wardCentre'] = geoDF.apply(
            lambda row: (
                MultiPolygon(row['geometry']).centroid.x, 
                MultiPolygon(row['geometry']).centroid.y
                ), 
            axis=1
            )
        self.geoDF = geoDF
        
    def set_demographics(self, input_dir):
        assert fileExists(Path(input_dir, inputfiles["demographics"])), f"{inputfiles['demographics']} missing"
        
        demographics = pd.read_csv(Path(input_dir,inputfiles["demographics"]))
        demographics["wardNo"] = demographics["wardNo"].astype(int)
        demographics = demographics.sort_values("wardNo")
        
        necessary_cols = ["wardNo", "wardName", "totalPopulation"]
        for col in necessary_cols:
            assert col in demographics.columns
        demographics["wardIndex"] = demographics["wardNo"] - 1
        self.checkRows(demographics,"demographics")
        
        if "hd_flag" in demographics.columns:
            self.has_slums = True
            demographics["hd_flag"] = demographics["hd_flag"].astype(int)
            necessary_cols += ["hd_flag"]
        else:
            self.has_slums = False

        demographics['totalPopulation'] = demographics['totalPopulation'].astype(int)

        self.nwards = demographics['wardIndex'].count()
        self.totalPop = demographics['totalPopulation'].sum()

        self.demographics = demographics[necessary_cols]
        
        
    def set_employments(self, input_dir):
        assert fileExists(Path(input_dir, inputfiles["employment"])), f"{inputfiles['employment']} missing"
        
        employments = pd.read_csv(Path(input_dir,inputfiles["employment"]))
        employments["wardNo"] = employments["wardNo"].astype(int)
        employments = employments.sort_values('wardNo')
        
        necessary_cols = ["wardNo", "Employed"]
        for col in necessary_cols:
            assert col in employments.columns
        employments["wardIndex"] = employments["wardNo"] - 1
        self.checkRows(employments, "employments")

        employments['Employed'] = employments['Employed'].astype(int)
        
        self.employments = employments[necessary_cols]

    
    def set_ODMatrix(self, input_dir):
        assert self.nwards is not None
        
        if fileExists(Path(input_dir, inputfiles["ODMatrix"])):
            #do something
            ODMatrix = pd.read_csv(Path(input_dir,inputfiles['ODMatrix'])).sort_values('wardNo')
            self.checkRows(ODMatrix,"ODMatrix")
            
            cols = [a for a in ODMatrix.columns if a != "wardNo"]
            for i in range(self.nwards):
                assert int(cols[i]) == i+1, f"Mismatch in ODMatrix.csv: col {i+1} has {cols[i]}"
            
            _ = ODMatrix.pop("wardNo").astype(float)

            self.ODMatrix = ODMatrix.values
        else:
            self.ODMatrix = [[(1/self.nwards) for i in range(self.nwards)] for j in range(self.nwards)]
            
        for i in range(self.nwards):
            self.ODMatrix[i] = normalise(self.ODMatrix[i])
       
    def set_presampled_points(self, input_dir):
        assert folderExists(Path(input_dir,'presampled-points')), "'presampled-points' missing"
        assert self.nwards is not None
        
        self.presampled_points = []
        for i in range(self.nwards):
            assert fileExists(Path(input_dir,"presampled-points",f"{i}.csv")), f"presampled-points/{i}.csv missing"
            df = pd.read_csv(Path(input_dir,"presampled-points",f"{i}.csv"),names=["lat","lon"]).astype(float)
            self.presampled_points.append(df)
        
    def set_city_profile(self, input_dir):
        assert fileExists(Path(input_dir, inputfiles["cityprofile"])), f"{inputfiles['cityprofile']} missing"
        with open(Path(input_dir, inputfiles["cityprofile"]),"r") as file:
            cityprofiledata = json.load(file)
        
        self.householdsize_bins = cityprofiledata['householdSize']['bins']
        self.householdsize_weights = normalise(cityprofiledata['householdSize']['weights'])
        assert len(self.householdsize_bins) == len(self.householdsize_weights), "household bins and weights differ in lengths"
        
        self.age_bins = cityprofiledata['age']['bins']
        self.age_weights = normalise(cityprofiledata['age']['weights'])
        assert len(self.age_bins) == len(self.age_weights), "age bins and weights differ in lengths"
    
        self.schoolsize_bins = cityprofiledata['schoolsSize']['bins']
        self.schoolsize_weights = normalise(cityprofiledata['schoolsSize']['weights'])
        assert len(self.schoolsize_bins) == len(self.schoolsize_weights), "schoolsSize bins and weights differ in lengths"        
        
        self.m_max_commuter_distance = cityprofiledata['maxWorkplaceDistance']
        
    def sampleRandomLatLon(self, wardIndex):
        if self.presampled_points is not None:
            i = random.randint(0,len(self.presampled_points[wardIndex])-1)
            (lat,lon) = self.presampled_points[wardIndex].loc[i]
            return (lat,lon)
        else:
            assert self.geoDF is not None
            (lon1,lat1,lon2,lat2) = self.geoDF['wardBounds'][wardIndex]
            while True:
                lat = random.uniform(lat1,lat2)
                lon = random.uniform(lon1,lon2)
                point = Point(lon,lat) #IMPORTANT: Point takes in order of longitude, latitude
                if MultiPolygon(self.geoDF['geometry'][wardIndex]).contains(point):
                    return (lat,lon)

    def rescale(self, n):
        assert self.demographics is not None 
        assert self.employments is not None
        
        scale = n / self.totalPop
        
        self.demographics["totalPopulation"] = (self.demographics["totalPopulation"] * scale).astype(int)
        self.employments["Employed"] = (self.employments["Employed"] * scale).astype(int)

        self.totalPop = self.demographics['totalPopulation'].sum()
        
    def sampleAge(self):
        assert self.age_bins is not None and self.age_weights is not None
        return sampleBinsWeights(self.age_bins, self.age_weights)

    def sampleHouseholdSize(self):
        assert self.householdsize_bins is not None and self.householdsize_weights is not None
        return sampleBinsWeights(self.householdsize_bins, self.householdsize_weights)

    def sampleSchoolSize(self):
        assert self.schoolsize_bins is not None and self.schoolsize_weights is not None
        return sampleBinsWeights(self.schoolsize_bins, self.schoolsize_weights)

    def sampleWorkplaceSize(self):
        wsdist = workplaces_size_distribution()
        m_max = len(wsdist)
        return int(np.random.choice(np.arange(m_max),1,p=wsdist)[0])

    def set_community_centres(self):
        assert self.nwards is not None
        community_centres = []
        
        for wardIndex in range(self.nwards):
            if self.presampled_points is not None:
                (lat,lon) = self.sampleRandomLatLon(wardIndex)
            else:
                assert self.geoDF is not None
                (lon,lat) = self.geoDF['wardCentre'].iloc[wardIndex]
                #IMPORTANT: shapely works with (lon, lat) order
            community_centres.append((lat,lon))
        self.community_centres = community_centres

    def getCommunityCenterDistance(self, lat, lon, wardIndex):
        assert self.community_centres is not None
        (latc,lonc) = self.community_centres[wardIndex]
        return distance(lat,lon,latc,lonc)
        
    def createHouses(self):        
        self.houses = []
        hid = 0
        for wardIndex in range(self.nwards):
            pop = self.demographics["totalPopulation"][wardIndex]
            currpop = 0

            #creating houses
            while(currpop < pop):
                h = {}
                h["id"]=hid
                h["wardIndex"]=wardIndex

                if self.has_slums:
                    h["slum"] = int(self.demographics["hd_flag"][wardIndex])

                s = self.sampleHouseholdSize()
                h["size"]=s
                currpop+=s

                (lat,lon) = self.sampleRandomLatLon(wardIndex)
                h["lat"] = lat
                h["lon"] = lon

                self.houses.append(h)
                hid+=1
        self.num_houses = hid
        
    def populateHouses(self):
        assert self.houses is not None
        
        pid = 0
        self.individuals = []
        self.workers = [[] for _ in range(self.nwards)]
        self.schoolers = [[] for _ in range(self.nwards)]
        
        employed_frac = self.employments["Employed"] / self.demographics["totalPopulation"]
        self.demographics["generatedPopulation"] = 0
        self.employments["generatedEmployed"] = 0
        generatedPop = 0
        num_workers = 0
        
        for h in self.houses:
            size = h["size"]
            wardIndex = h["wardIndex"]
            for i in range(size):
                p = {
                    "id":pid,
                    "household":h["id"],
                    "wardIndex":wardIndex,
                    "wardNo":wardIndex + 1,
                    "lat": h["lat"],
                    "lon": h["lon"],
                    "CommunityCentreDistance": self.getCommunityCenterDistance(h["lat"],h["lon"],wardIndex),
                    #Setting some default values
                    "employed": 0,
                    "workplaceType": workplacesTypes[None]
                    }

                if self.has_slums:
                    p["slum"] = h["slum"]

                age = self.sampleAge()
                p["age"] = age

                if age < 3:                         # toddlers stay at home
                    
                    p["employed"]=0
                    p["workplaceType"] = workplacesTypes[None]

                elif age >= 3 and age < 15 :        # decide about their school
                    
                    p["employed"]=0
                    p["workplaceType"] = workplacesTypes["school"]
                    #assuming they all go to school
                    self.schoolers[wardIndex].append(pid)

                elif age >= 15 and age < 65:        # decide about employment/school
                    
                    eprob = employed_frac.iloc[wardIndex]
                    eprob_adjusted = eprob / sum(
                        [self.age_weights[a] for a in range(
                            self.age_bins.index("15-19"),
                            self.age_bins.index("65-69"))
                        ])  #Probability that you are employed given 15 <= age < 65 

                    if(random.uniform(0,1) < eprob_adjusted):
                        
                        #person is employed
                        p["employed"] = 1

                        # p["workplace"] = "TODO"
                        workplaceward = int(np.random.choice(
                            list(range(self.nwards)),1,p=self.ODMatrix[wardIndex])[0]
                                           )
                        p["workplaceward"] = workplaceward
                        p["workplaceType"] = workplacesTypes["office"]
                        self.workers[workplaceward].append(pid)
                        num_workers+=1
                        self.employments["generatedEmployed"].iloc[wardIndex]+=1
                    else:
                        p["employed"] = 0
                        if age < 20:
                            p["workplaceType"] = workplacesTypes["school"]
                            # All the unemployed in this age bracket go to school
                            self.schoolers[wardIndex].append(pid)
                        else:
                            p["workplaceType"] = workplacesTypes[None]

                        p["workplaceType"] = workplacesTypes[None]
                else:
                    #decide about seniors
                    p["employed"] = 0
                    p["workplaceType"] = workplacesTypes[None]
                    
                self.individuals.append(p)
                self.demographics["generatedPopulation"].iloc[wardIndex]+=1
                generatedPop +=1
                pid+=1
        self.num_individuals = generatedPop
        self.num_workers = num_workers

        
    def sampleOfficeType(self, size):
        num_gov = 0
        num_ites = 0 
        num_sez = 0
        # Previous version had these as global variables. Not sure what they were used for
        #Large workplace at SEZs, officeType=1
        if num_sez < self.max_sez and size >= 200 and size < 300 and np.random.binomial(1,0.7):
            num_sez += size
            return  officeType['SEZ']
        elif num_sez < self.max_sez and size >= 300 and size < 400 and np.random.binomial(1,0.8):
            num_sez += size
            return  officeType['SEZ']
        elif num_sez < self.max_sez and size >= 400 and size < 500 and np.random.binomial(1,0.9):
            num_sez += size
            return  officeType['SEZ']
        elif num_sez < self.max_sez and size >= 500:
            num_sez += size
            return  officeType['SEZ']

        #Government offices
        elif size >= 20 and num_gov < self.max_gov:
            num_gov += size
            return  officeType['Government']

        #IT/ITES
        elif size >= 10 and num_ites <= self.max_ites_not_sez:
            num_ites += size
            return  officeType['IT']
        else:
            return  officeType['Other']

    def assignSchools(self):
        assert self.houses is not None
        assert self.individuals is not None
        
        self.schools = []
        sid = 0
        for wardIndex in range(self.nwards):
            while len(self.schoolers[wardIndex]) > 0: #some unassigned kids left in the ward
                #Set up basic facts about school to be created
                s = {"ID":sid} #capitalised in the previous code so keeping it so
                s["wardIndex"]=wardIndex
                (lat,lon) = self.sampleRandomLatLon(wardIndex)
                s["lat"] = lat
                s["lon"] = lon
                
                if self.has_slums:
                    s["slum"] = int(self.demographics["hd_flag"].iloc[wardIndex])

                size = self.sampleSchoolSize()

                #Fill up school with kids
                i = 0
                while(i < size and len(self.schoolers[wardIndex])>0):
                    pid = self.schoolers[wardIndex].pop(
                            random.randrange(len(self.schoolers[wardIndex]))
                            )
                    self.individuals[pid]["school"] = sid
                    i+=1
                self.schools.append(s)
                sid+=1
                #Note: This sort of creates a very skewed first-bracket for school size.
                #If the city size is small, then many schools will be "under-capacity".
                #Need to think about how to fix this corner case.

        self.num_schools = sid
        
    def assignWorkplaces(self):
        assert self.houses is not None
        assert self.individuals is not None
        assert self.schools is not None
        
        self.workplaces = []
        count = 0
        for wardIndex in range(self.nwards):
            num_ward_workers = len(self.workers[wardIndex])
            while len(self.workers[wardIndex])>0:
                wid = count + self.num_schools
                w = {
                    "id":wid,
                    "wardIndex":wardIndex
                }

                (lat,lon) = self.sampleRandomLatLon(wardIndex)
                w["lat"] = lat
                w["lon"] = lon
                
                s = self.sampleWorkplaceSize()
                oType = self.sampleOfficeType(s)
                w["officeType"]=oType

                i = 0
                while(i < s and len(self.workers[wardIndex])>0):
                    pid = self.workers[wardIndex].pop(random.randrange(len(self.workers[wardIndex])))
                    self.individuals[pid]["workplace"] = wid
                    del self.individuals[pid]["workplaceward"]
                    i+=1
                self.workplaces.append(w)
                count+=1
        self.num_workplaces = count
    
    def describe(self):
        
        print(f"Population: {self.num_individuals}")
        print(f"Number of wards: {self.nwards}")
        print(f"Has slums: {self.has_slums}")
        print("")
        print(f"Number of houses: {self.num_houses}")
        print(f"Number of schools: {self.num_schools}")
        print(f"Number of workplaces: {self.num_workplaces}")
        print(f"Number of workers: {self.num_workers}")
        print("")

    
    def generate(self, n):
        assert self.demographics is not None
        assert self.employments is not None
        assert self.ODMatrix is not None
        
        last = time.time()
        start = last
        self.rescale(n)
        print("Creating houses...",end='',flush=True)
        self.createHouses()
        t = time.time()
        print(f"done (in {t - last:.02f} seconds).",flush=True)

        last = t
        print("Populating houses...",end='',flush=True)
        self.populateHouses()
        t = time.time()
        print(f"done (in {t - last:.02f} seconds).",flush=True)
        
        last = t
        print("Assigning schools...",end='',flush=True)
        self.assignSchools()
        t = time.time()
        print(f"done (in {t - last:.02f} seconds).",flush=True)
        
        last = t
        print("Assigning workplaces...",end='',flush=True)
        self.assignWorkplaces()
        t = time.time()
        print(f"done (in {t - last:.02f} seconds).",flush=True)
        print(f"Total time: {time.time() - start:.02f} seconds.",flush=True)
        print("")
        self.describe()
        

    def dump_files(self, output_dir):
        assert self.houses is not None
        assert self.individuals is not None
        assert self.schools is not None
        assert self.workplaces is not None
        
        assert output_dir is not None
               
        start = time.time()
        print("Dumping json files...",end='',flush=True)
        
        Path(output_dir).mkdir(parents = True, exist_ok = True)
        
        commonAreas = []
        for i in range(self.nwards):
            c = {"ID":i}
            c["wardNo"] = i+1
            (lon,lat)= self.community_centres[i]
            c["lat"] = lat
            c["lon"] = lon
            commonAreas.append(c)
        
        fractionPopulations = []
        for i in range(self.nwards):
            w = {"wardNo":i+1}
            w["totalPopulation"] = int(self.demographics["generatedPopulation"].iloc[i])
            w["fracPopulation"] = float(self.demographics["generatedPopulation"].iloc[i] / self.num_individuals)
            fractionPopulations.append(w)
        
        wardCentreDistances = [ {"ID":i+1} for i in range(self.nwards)]
        for i in range(self.nwards):
            for j in range(self.nwards):
                wardCentreDistances[i][str(j+1)] = distance(commonAreas[i]["lat"],
                                                            commonAreas[i]["lon"],
                                                            commonAreas[j]["lat"],
                                                            commonAreas[j]["lon"])

        with open(os.path.join(output_dir,outputfiles['houses']), "w+") as f:
            f.write(json.dumps(self.houses))
        with open(os.path.join(output_dir,outputfiles['individuals']), "w+") as f:
            f.write(json.dumps(self.individuals))
        with open(os.path.join(output_dir,outputfiles['schools']), "w+") as f:
            f.write(json.dumps(self.schools))
        with open(os.path.join(output_dir,outputfiles['workplaces']), "w+") as f:
            f.write(json.dumps(self.workplaces))
        with open(os.path.join(output_dir,outputfiles['commonArea']), "w+") as f:
            f.write(json.dumps(commonAreas))
        with open(os.path.join(output_dir,outputfiles['fractionPopulation']), "w+") as f:
            f.write(json.dumps(fractionPopulations))
        with open(os.path.join(output_dir,outputfiles['wardCentreDistance']), "w+") as f:
            f.write(json.dumps(wardCentreDistances))     
        with open(os.path.join(output_dir,outputfiles['PRG_np_random_state']), "wb+") as f:
            pickle.dump(self.state_numpy,f)
        with open(os.path.join(output_dir,outputfiles['PRG_random_state']), "wb+") as f:
            pickle.dump(self.state_random,f)

        print(f"done (in {time.time() - start:.02f} seconds)")
        
    def __init__(self, city, input_dir, restore_randomness = None):
        
        if city=="bangalore":
            self.a_commuter_distance = 10.751
            self.b_commuter_distance = 5.384
        elif city=="mumbai":
            self.a_commuter_distance = 2.709
            self.b_commuter_distance = 1.278
        else:
            self.a_commuter_distance = 4
            self.b_commuter_distance = 3.8
        
        if restore_randomness is not None:
            assert fileExists(
                os.path.join(restore_randomness, outputfiles['PRG_np_random_state'])
                ), f"{os.path.join(restore_randomness, outputfiles['PRG_np_random_state'])} not found"
            assert fileExists(
                os.path.join(restore_randomness, outputfiles['PRG_random_state'])
                ), f"{os.path.join(restore_randomness, outputfiles['PRG_random_state'])} not found"
            self.set_random_seeds(restore_randomness)

        self.get_random_seeds()
        self.set_demographics(input_dir)
        self.set_employments(input_dir)
        self.set_city_profile(input_dir)
        self.set_ODMatrix(input_dir)
        if folderExists(Path(input_dir,'presampled-points')):
            self.set_presampled_points(input_dir)
        else:
            self.set_geoDF(input_dir) 
        self.set_community_centres()


# In[ ]:


def validate(city, plots_folder=None):
    ### I am just copying the validation scripts for now. 
    ### Not going through them carefully
    
    a_workplacesize = 3.26
    c_workplacesize = 0.97
    m_max_workplacesize = 2870
    avgSchoolsize = 300

    age_values, age_distribution = compute_age_distribution(city.age_weights)
    household_sizes, household_distribution = compute_household_size_distribution(
        city.householdsize_bins, 
        city.householdsize_weights
        )
    schoolsize_values, schoolsize_distribution = extrapolate_school_size_distribution(
        city.schoolsize_weights,
        avgSchoolsize
        )
    workplacesize_distribution = workplaces_size_distribution()
    
    df1 = pd.DataFrame(city.individuals)
    
    print("Validating age distribution in instantiation...",end='',flush=True)
    plt.plot(df1['age'].value_counts(normalize=True).sort_index(ascending=True), 'r-o',label='Instantiation')
    plt.plot(age_distribution, 'b-',label='Data')
    plt.xlabel('Age')
    plt.ylabel('Density')
    plt.title('Distribution of age')
    plt.grid(True)
    plt.legend()
    plt.xticks(np.arange(0,81,10), np.concatenate((age_values[np.arange(0,71,10)], ['80+'])) )
    if plots_folder is not None: 
        plt.savefig(os.path.join(plots_folder, 'age.png'))
    else:
        plt.show()
    plt.close()
    print("done.",flush=True)
    
    print("Validating household-size in instantiation...",end='',flush=True)
    house = df1['household'].value_counts().values
    unique_elements, counts_elements = np.unique(house, return_counts=True)
    counts_elements = counts_elements / np.sum(counts_elements)
    plt.plot(counts_elements, 'r-o', label='Instantiation')
    plt.plot(household_distribution, 'b-', label='data')
    plt.xlabel('Household-size')
    plt.ylabel('Density')
    plt.title('Distribution of household-size')
    plt.grid(True)
    plt.legend()
    plt.xticks(np.arange(0,len(household_sizes),1), np.concatenate((age_values[np.arange(1,household_sizes[-1],1)], [str(household_sizes[-1])+'+'])) )
    if plots_folder is not None:
        plt.savefig(os.path.join(plots_folder,'household_size.png'))
    else:
        plt.show()
    plt.close()
    print("done.",flush=True)

    print("Validating school-size in instantiation...",end='',flush=True)
    schoolsizeDistribution = city.schoolsize_weights
    full_frame = np.floor(
        np.array([
                len(np.where(df1['school'] == i)[0]) for i in np.unique(
                    df1['school'].values
                    )[~np.isnan(np.unique(df1['school'].values))]
                ])/100
        ).astype(int)

    schoolsize_output = [
        len(np.where(full_frame == j)[0]) for j in np.arange(
            0,
            len(schoolsizeDistribution)
            )
        ] / np.sum([
                len(np.where(full_frame == j)[0]) for j in np.arange(
                    0,
                    len(schoolsizeDistribution)
                    )
                ])
    
    plt.plot(schoolsize_output,'r-o', label='Instantiation')
    plt.plot(schoolsizeDistribution,'b-', label='Data')
    xlabel = np.arange(0,len(schoolsizeDistribution))
    plt.xticks(xlabel, np.concatenate((np.arange(1,10)*100, [str('901+')])))
    plt.xlabel('School size')
    plt.ylabel('Density')
    plt.legend()
    plt.title('Distribution of school size')
    plt.grid(True)
    if plots_folder is not None:
        plt.savefig(os.path.join(plots_folder, 'school_size.png'))
    else:
        plt.show()
    plt.close()
    print("done.",flush=True)

    # generate workplace size distribution
    a=a_workplacesize
    c=c_workplacesize
    m_max=m_max_workplacesize
    workplace_sizes = np.arange(m_max)
    p_nplus = np.arange(float(m_max))
    for m in range(m_max):
        p_nplus[m] =  ((( (1+m_max/a)/(1+m/a))**c) -1) / (((1+m_max/a)**c) -1)

    p_nminus = 1.0 - p_nplus
    p_n = np.arange(float(m_max))
    prev=0.0
    for m in range(1, m_max):
        p_n[m] = p_nminus[m] - prev
        prev = p_nminus[m]


    # workplace size
    print("Validating workplace-size in instantiation...",end='',flush=True)

    full_frame = np.array([
        len(np.where(df1['workplace'] == i)[0]) for i in np.unique(
            df1['workplace'].values
            )[~np.isnan(np.unique(df1['workplace'].values))]
        ])
    workplacesize_output = [len(np.where(full_frame == j)[0]) for j in workplace_sizes] / np.sum([
        len(np.where(full_frame == j)[0]) for j in workplace_sizes
        ])
    workplace_distribution = p_n
    plt.plot(np.log10(workplace_sizes),np.log10(workplacesize_output),'r',label='Instantiation')
    plt.plot(np.log10(workplace_sizes), np.log10(workplace_distribution),label='Model')
    plt.xlabel('Workplace size (log-scale)')
    plt.ylabel('log_10 Density')
    plt.title('Distribution of workplace size (in log-scale)')
    plt.grid(True)
    plt.legend()
    plot_xlabel =  [1, 10, 100, 1000, 2400]
    plot_xlabel1 = np.log10(workplace_sizes)[plot_xlabel]
    plt.xticks(plot_xlabel1, (workplace_sizes)[plot_xlabel])
    if plots_folder is not None:
        plt.savefig(os.path.join(plots_folder,'workplace_size.png'))
    else:
        plt.show()
    plt.close()
    print("done.",flush=True)
    
    
    wp = pd.DataFrame(city.workplaces)

    print("Validating workplace commute distance in instantiation...",end='',flush=True)
    full_frame = np.array([
        distance(
            df1.loc[i,'lat'],
            df1.loc[i,'lon'],
            wp.loc[(wp.index+city.num_schools)==int(df1.loc[i,'workplace']),'lat'].values[0],
            wp.loc[(wp.index+city.num_schools)==int(df1.loc[i,'workplace']),'lon'].values[0]
            ) for i in np.where(df1['workplaceType']==1)[0]
        ])
    commuter_distance_output = [
        len(np.where(np.array(np.floor(full_frame),dtype=int) ==i)[0]) for i in np.arange(
            0,
            city.m_max_commuter_distance
            )
        ] / np.sum([
                len(np.where(np.array(np.floor(full_frame),dtype=int) ==i)[0]) for i in np.arange(
                    0,
                    city.m_max_commuter_distance
                    )
                ])
    actual_dist=[]
    actual_dist = travel_distance_distribution(
        0,
        city.m_max_commuter_distance,
        city.a_commuter_distance,
        city.b_commuter_distance
        )
    d = np.arange(0,city.m_max_commuter_distance,1)
    plt.plot(np.log10(d),np.log10(actual_dist),'b-',label='Model')
    plt.plot(np.log10(d),np.log10((commuter_distance_output)),'r-o',label='Instantiation')
    plt.xlabel('Workplace distance (km) (log-scale)')
    plt.ylabel('log_10 Density')
    plt.title('Distribution of workplace distances')
    plot_xlabel=[1,5,25,31]
    plot_xlabel1 = np.log10(d)[plot_xlabel]
    plt.xticks(plot_xlabel1,d[plot_xlabel])
    plt.grid(True)
    plt.legend()
    if plots_folder is not None:
        plt.savefig(os.path.join(plots_folder,'workplace_distance.png'))
    else:
        plt.show()
    plt.close()
    print("done.",flush=True)


# In[ ]:


def main():
    
    default_pop = 100000
    default_city = "bangalore"
    default_ibasepath = 'data/base/bangalore/'
    default_obasepath = 'data/bangalore-100K/'

    my_parser = argparse.ArgumentParser(description='Create mini-city for COVID-19 simulation')
    my_parser.add_argument('-c', help='target city', default=default_city)
    my_parser.add_argument('-n', help='target population', default=default_pop)
    my_parser.add_argument('-i', help='input folder', default=default_ibasepath)
    my_parser.add_argument('-o', help='output folder', default=default_obasepath)
    my_parser.add_argument('--validate', help='validation on', action="store_true")
    my_parser.add_argument('-s', help='[for debug] restore random seed from folder', default=None)

    args = my_parser.parse_args()
    city = (args.c).lower()
    population = int(args.n)
    input_dir = args.i
    output_dir = args.o
    
    city = City(city, input_dir)
    city.generate(population)
    city.dump_files(output_dir)
    if args.validate:
        validate(city,output_dir)

if __name__ == "__main__":
    main()


# In[ ]:




