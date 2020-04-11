#!/usr/bin/env python
# coding: utf-8

# In[ ]:


import json
import geopandas as gpd
import math
from shapely.geometry import Point, MultiPolygon

import csv
import numpy as np
import random

import argparse
import os
import sys

interactive = 0
default_miniPop = 10000
default_ibasepath = 'data/base/mumbai/'
default_obasepath = 'data/mumbai/'

if interactive:
    miniPop = 10000
    ibasepath = default_ibasepath
    obasepath = default_obasepath
else:
    my_parser = argparse.ArgumentParser(description='Create mini-city for COVID-19 simulation')
    my_parser.add_argument('-n', help='target population', default=default_miniPop)
    my_parser.add_argument('-i', help='input folder', default=default_ibasepath)
    my_parser.add_argument('-o', help='output folder', default=default_obasepath)
    args = my_parser.parse_args()
    miniPop = int(args.n)
    ibasepath = args.i
    obasepath = args.o
if ibasepath[-1]!='/':
    ibasepath = ibasepath+'/'
if obasepath[-1]!='/':
    obasepath = obasepath+'/'


citygeojsonfile  = ibasepath+"city.geojson"
demographicsfile = ibasepath+"demographics.csv"
employmentfile   = ibasepath+"employment.csv"
householdfile    = ibasepath+"households.csv"
cityprofilefile  = ibasepath+"cityProfile.json"
slumfracfile     = ibasepath+"slumFraction.csv"
slumclusterfile  = ibasepath+"slumClusters.geojson"
ODMatrixfile     = ibasepath+"ODMatrix.csv"

individualsjson        = obasepath+"individuals.json"
housesjson             = obasepath+"houses.json"
workplacesjson         = obasepath+"workplaces.json"
schoolsjson            = obasepath+"schools.json"
wardCentreDistancejson = obasepath+"wardCentreDistance.json"
commonAreajson         = obasepath+"commonArea.json"
fractionPopulationjson = obasepath+"fractionPopulation.json"

#fixing for now
slum_schoolsize_factor = 1
slum_householdsize_scalefactor = 1

print("Creating city with a population of approximately ",miniPop,flush=True)
print("")

print("Reading city.geojson to get ward polygons...",end='',flush=True)
geoDF = gpd.read_file(citygeojsonfile)
geoDF['wardNo'] = geoDF['wardNo'].astype(int)
geoDF['wardIndex'] = geoDF['wardNo'] - 1
geoDF = geoDF[['wardIndex','wardNo', 'wardName', 'geometry']]
geoDF['wardBounds'] = geoDF.apply(lambda row: MultiPolygon(row['geometry']).bounds, axis=1)

##!! Note that the geojson file has coordinates in (longitude, latitude) order!
geoDF['wardCentre'] = geoDF.apply(lambda row: (MultiPolygon(row['geometry']).centroid.x, MultiPolygon(row['geometry']).centroid.y), axis=1)

geoDF["neighbors"] = geoDF.apply(lambda row: ", ".join([str(ward) for ward in geoDF[~geoDF.geometry.disjoint(row['geometry'])]['wardNo'].tolist()]) , axis=1)
print("done.",flush=True)


# In[ ]:

slum_flag = 0
slumcluster_flag = 0
slumpoints_precomputed = 0
if os.path.exists(slumfracfile):
    print(slumfracfile,"exists... processing slum data",flush=True)
    slum_flag = 1
    slum_fractions = []
    with open(slumfracfile, newline='') as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
        for row in reader:
            if row[0]=='wardIndex':
                continue
            slum_fractions.append(float(row[2]))

    if os.path.exists(ibasepath + 'slumpoints/0.csv'):
        slumpoints_precomputed = 1

    if os.path.exists(slumclusterfile):
        slumcluster_flag=1
        print("Slum clustter file found. Parsing slum clusters...",end='',flush=True)
        geoDFslums = gpd.read_file(slumclusterfile)
        wardslums = [[] for _ in range(len(geoDF))]

        for i in range(len(geoDFslums)):
            for j in range(len(geoDF)):
                if geoDFslums["geometry"][i].intersects(geoDF["geometry"][j]):
                    wardslums[j].append(i)
        print("done.",flush=True)
    else:
        slumcluster_flag=0
        print("Slum clustter file not found.",end='',flush=True)
else:
    slum_flag=0
    slumcluster_flag=0
    print(slumfracfile,"does not exist... not processing slum data",flush=True)



# In[ ]:


print("Reading demographics, employment and household data (csv)...",end='',flush=True)


wardname = []
wardpop = []
wardarea = []
wardemployed = []
wardunemployed = []
wardworkforce = []
wardhouseholds = []

with open(demographicsfile, newline='') as csvfile:
    reader = csv.reader(csvfile, delimiter=',')
    for row in reader:
        if row[0]=='wardIndex':
            continue
        wardname.append(row[2])
        wardarea.append(float(row[3]))
        wardpop.append(int(row[4]))
with open(employmentfile, newline='') as csvfile:
    reader = csv.reader(csvfile, delimiter=',')
    for row in reader:
        if row[0]=='wardIndex':
            continue
        wardemployed.append(int(row[3]))
        wardunemployed.append(int(row[4]))
        wardworkforce.append(int(row[5]))
with open(householdfile, newline='') as csvfile:
    reader = csv.reader(csvfile, delimiter=',')
    for row in reader:
        if row[0]=='wardIndex':
            continue
        wardhouseholds.append(int(row[3]))
print("done.",flush=True)


# In[ ]:


with open(cityprofilefile, newline='') as file:
    cityprofiledata = json.load(file)

hbins = cityprofiledata['householdSize']['bins']
hweights = cityprofiledata['householdSize']['weights']
hweights[0]=hweights[0] + 1- sum(hweights)
def sampleHouseholdSize():
    s = np.random.choice(hbins,1,p=hweights)[0]
    if '+' in s:
        n = int(s[:-1])
    elif '-' in s:
        (a,b) = s.split('-')
        n = random.randint(int(a),int(b))
    else:
        n = int(s)
    return n


# In[ ]:


agebins = cityprofiledata['age']['bins']
ageweights = cityprofiledata['age']['weights']
ageweights[0] = ageweights[0] + 1 - sum(ageweights)

def sampleAge():
    s = np.random.choice(agebins,1,p=ageweights)[0]
    if '+' in s:
        n = int(s[:-1])
    else:
        (a,b) = s.split('-')
        n = random.randint(int(a),int(b))
    return n


# In[ ]:


totalPop = sum(wardpop)
scale = miniPop/totalPop
nwards = len(wardname)



mwardpop = [int(a * scale) for a in wardpop]
mwardemployed = [int(a * scale) for a in wardunemployed]
mwardunemployed = [int(a * scale) for a in wardemployed]
mwardworkforce = [int(a * scale) for a in wardworkforce]
mwardhouseholds = [int(a * scale) for a in wardhouseholds]

if slum_flag:
    mslumwardpop = [int(mwardpop[i] * slum_fractions[i]) for i in range(nwards)]
    mnonslumwardpop = [mwardpop[i] - mslumwardpop[i] for i in range(len(wardpop))]
else:
    mslumwardpop = [0]*nwards
    mnonslumwardpop = mwardpop.copy()

# In[ ]:
slumpoints=[[] for _ in range(nwards)]
if slumpoints_precomputed:
    print("Loading precomputed slum points...",end='',flush=True)
    for i in range(nwards):
        with open(ibasepath+'slumpoints/'+str(i)+'.csv', newline='') as csvfile:
            reader = csv.reader(csvfile, delimiter=',')
            for row in reader:
                lat = float(row[0])
                lon = float(row[1])
                slumpoints[i].append((lat,lon))
    print("done",flush=True)

def sampleRandomLatLong(wardIndex):
    #I'm not sure why the order is longitude followed by latitude
    (lon1,lat1,lon2,lat2) = geoDF['wardBounds'][wardIndex]
    while True:
        lat = random.uniform(lat1,lat2)
        lon = random.uniform(lon1,lon2)
        point = Point(lon,lat)
        if MultiPolygon(geoDF['geometry'][wardIndex]).contains(point):
            return (lat,lon)

def sampleRandomLatLong_s(wardIndex,slumbit):
    #slumbit = 0 => get point in nonslum
    #slumbit = 1 => get point in slum

    if slumcluster_flag==0:
        return sampleRandomLatLong(wardIndex)

    #I'm not sure why the order is longitude followed by latitude
    (lon1,lat1,lon2,lat2) = geoDF['wardBounds'][wardIndex]

    if slumpoints_precomputed:
        if slumbit==1:
            if len(slumpoints[wardIndex])==0:
                return sampleRandomLatLong(wardIndex)
            i = random.randint(0,len(slumpoints[wardIndex])-1)
            return slumpoints[wardIndex][i]
        else:
            #Just going to return a random point in the ward
            return sampleRandomLatLong(wardIndex)

    #If not precomputed, do rejection sampling
    attempts = 0
    while attempts<30:
        attempts+=1
        lat = random.uniform(lat1,lat2)
        lon = random.uniform(lon1,lon2)
        point = Point(lon,lat)
        if MultiPolygon(geoDF['geometry'][wardIndex]).contains(point):
            for i in wardslums[wardIndex]:
                if geoDFslums["geometry"][i].contains(point):
                    if slumbit==1:
                        return (lat,lon)
                else:
                    if slumbit==0:
                        return(lat,lon)
    #Just sample a random point in the ward if unsuccessful
    #print("Gave up on sampleRandomLatLong_s with ",wardIndex,slumflag)
    return sampleRandomLatLong(wardIndex)


def distance(lat1, lon1, lat2, lon2):
    radius = 6371 # km

    dlat = math.radians(lat2-lat1)
    dlon = math.radians(lon2-lon1)
    a = math.sin(dlat/2) * math.sin(dlat/2) + math.cos(math.radians(lat1))         * math.cos(math.radians(lat2)) * math.sin(dlon/2) * math.sin(dlon/2)
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1-a))
    d = radius * c

    return d

def getCommunityCenterDistance(lat,lon,wardIndex):
    #I'm not sure why the order is longitude followed by latitude
    (lonc,latc) = geoDF['wardCentre'][wardIndex]
    return distance(lat,lon,latc,lonc)





# In[ ]:


print("Creating households for each ward...",end='',flush=True)


houses = []
hid = 0
for wardIndex in range(nwards):
    wnonslumpop = mnonslumwardpop[wardIndex]
    wslumpop = mslumwardpop[wardIndex]
    currnonslumwpop = 0
    currslumwpop = 0
    while(currnonslumwpop < wnonslumpop):
        h = {}
        h["id"]=hid
        h["wardIndex"]=wardIndex
        if slum_flag:
            h["slum"]=0

        s = sampleHouseholdSize()
        h["size"]=s
        currnonslumwpop+=s
        (lat,lon) = sampleRandomLatLong_s(wardIndex,0)
        h["lat"] = lat
        h["lon"] = lon
        houses.append(h)
        hid+=1

    #if slum_flag=0, then wslumpop = 0
    while(currslumwpop < wslumpop):
        h = {}
        h["id"]=hid
        h["wardIndex"]=wardIndex
        if slum_flag:
            h["slum"]=1
        s = int(sampleHouseholdSize() * slum_householdsize_scalefactor)
        h["size"]=s
        currslumwpop+=s
        (lat,lon) = sampleRandomLatLong_s(wardIndex,1)
        h["lat"] = lat
        h["lon"] = lon
        houses.append(h)
        hid+=1
print("done.",flush=True)


# In[ ]:


homeworkmatrix = []
if os.path.exists(ODMatrixfile):
    with open(ODMatrixfile, newline='') as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
        for row in reader:
            if row[0]=='wardNo':
                continue
            homeworkmatrix.append(list(map(lambda x: float(x),row[1:])))
else:
    print(ODMatrixfile, "not found. Using uniform ODmatrix.",flush=True)
    homeworkmatrix = [[(1/nwards) for _ in range(nwards)] for _ in range(nwards)]
for i in range(nwards):
    homeworkmatrix[i][0] = homeworkmatrix[i][0] + 1 - sum(homeworkmatrix[i])

# In[ ]:


print("Creating individuals to populate the households...",end='',flush=True)

pid = 0
individuals = []
schoolers = [[] for _ in range(nwards)]
workers = [[] for _ in range(nwards)]
slum_schoolers = [[] for _ in range(nwards)]
nonslum_schoolers = [[] for _ in range(nwards)]

for h in houses:
    s = h["size"]
    for i in range(s):
        p = {"id":pid}

        p["household"]=h["id"]
        wardIndex = h["wardIndex"]
        p["wardIndex"]=wardIndex
        p["wardNo"] = wardIndex+1
        if slum_flag:
            p["slum"] = h["slum"]

        p["lat"] = h["lat"]
        p["lon"] = h["lon"]

        p["CommunityCentreDistance"] = getCommunityCenterDistance(h["lat"],h["lon"],wardIndex)

        age = sampleAge()
        p["age"] = age

        #initialising most stuff to None
        p["employed"]=None
        p["workplace"]=None
        p["workplaceType"]=0
        p["school"]=None

        if age<=15:
            #decide about his/her school
            p["employed"]=0
            p["workplaceType"]=2 #this is school

            #assuming they all go to school
            #schoolers[wardIndex].append(pid)
            if slum_flag ==1 and p["slum"]==1:
                slum_schoolers[wardIndex].append(pid)
            else:
                nonslum_schoolers[wardIndex].append(pid)

        elif age>=15 and age<65:
            #decide about employment
            eprob = wardemployed[wardIndex]/wardpop[wardIndex]
            eprobadjusted = eprob/sum([ageweights[a] for a in range(3,13)])
            if(random.uniform(0,1)<eprobadjusted):
                #person is employed
                p["employed"]=1

                p["workplace"]="TODO"
                workplaceward = int(np.random.choice(list(range(nwards)),1,p=homeworkmatrix[wardIndex])[0])
                p["workplaceward"]=workplaceward
                p["workplaceType"]=1
                workers[workplaceward].append(pid)
            else:
                p["employed"]=0
                p["workplaceType"]=0
        else:
            #decide about seniors
            test = 0
            p["employed"]=0
        individuals.append(p)
        pid+=1

print("done.",flush=True)


# In[ ]:


# Just taking the code from assignWorkplaces.py
# Don't know what exactly the distribution is

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

def sampleWorkplaceSize():
    wsdist = workplaces_size_distribution()
    m_max = len(wsdist)
    return int(np.random.choice(np.arange(m_max),1,p=wsdist)[0])


# In[ ]:


print("Assigning workplaces to people...",end='',flush=True)

#assigning workplaces to people who want work
workplaces = []
wid = 0
for wardIndex in range(nwards):
    wworkers = len(workers[wardIndex])
    while len(workers[wardIndex])>0:
        w = {"id":wid}
        (lat,lon) = sampleRandomLatLong(wardIndex)
        w["lat"] = lat
        w["lon"] = lon

        w["wardIndex"]=wardIndex
        s = sampleWorkplaceSize()
        i = 0
        while(i < s and len(workers[wardIndex])>0):
            pid = workers[wardIndex].pop(random.randrange(len(workers[wardIndex])))
            individuals[pid]["workplace"]=wid
            del individuals[pid]["workplaceward"]
            i+=1
        workplaces.append(w)
        wid+=1

print('done.',flush=True)


# In[ ]:


schoolsizebins = ["0-100", "100-200", "200-300", "300-400", "400-500", "500-600", "600-700", "700-800", "800-900"]
schoolsizebinweights = [0.0185, 0.1204, 0.2315, 0.2315, 0.1574, 0.0889, 0.063, 0.0481, 0.0408]
schoolsizebinweights[0] = schoolsizebinweights[0] + 1 - sum(schoolsizebinweights)
def sampleSchoolSize():
    s = int(np.random.choice(list(range(len(schoolsizebinweights))),1,p=schoolsizebinweights)[0])
    return (100*s + random.randint(0,99))



print("Assigning schools to people...",end='',flush=True)

#assigning school to people who want go to school
schools = []
sid = 0
if slum_flag:
    for wardIndex in range(nwards):
        wslum_schoolers = len(slum_schoolers[wardIndex])
        while len(slum_schoolers[wardIndex])>0:
            s = {"ID":sid} #capitalised in the previous code so keeping it so
            s["wardIndex"]=wardIndex
            (lat,lon) = sampleRandomLatLong_s(wardIndex,1)
            s["lat"] = lat
            s["lon"] = lon
            s["slum"]=1

            size = int(sampleSchoolSize()*slum_schoolsize_factor)

            i = 0
            while(i < size and len(slum_schoolers[wardIndex])>0):
                pid = slum_schoolers[wardIndex].pop(random.randrange(len(slum_schoolers[wardIndex])))
                individuals[pid]["school"]=sid
                i+=1
            schools.append(s)
            sid+=1

for wardIndex in range(nwards):
    wnonslum_schoolers = len(nonslum_schoolers[wardIndex])
    while len(nonslum_schoolers[wardIndex])>0:
        s = {"ID":sid} #capitalised in the previous code so keeping it so
        s["wardIndex"]=wardIndex
        (lat,lon) = sampleRandomLatLong_s(wardIndex,0)
        s["lat"] = lat
        s["lon"] = lon
        if slum_flag:
            s["slum"]=0

        size = sampleSchoolSize()
        i = 0
        while(i < size and len(nonslum_schoolers[wardIndex])>0):
            pid = nonslum_schoolers[wardIndex].pop(random.randrange(len(nonslum_schoolers[wardIndex])))
            individuals[pid]["school"]=sid
            i+=1
        schools.append(s)
        sid+=1


print("done.",flush=True)

print("")
print("Created (mini)city")
print("Population:",len(individuals))
print("Households:",len(houses))
print("Schools:",len(schools))
print("Workplaces:",len(workplaces))
print("")


# In[ ]:


commonAreas = []
for i in range(nwards):
    c = {"ID":i}
    c["wardNo"] = i+1
    #I'm not sure why the order is longitude followed by latitude
    (lon,lat)= geoDF['wardCentre'][i]
    c["lat"] = lat
    c["lon"] = lon
    commonAreas.append(c)

fractionPopulations = []
for i in range(nwards):
    w = {"wardNo":i+1}
    w["totalPopulation"] = wardpop[i]
    w["fracPopulation"] = wardpop[i]/totalPop
    fractionPopulations.append(w)

wardCentreDistances = [ {"ID":i+1} for i in range(nwards)]
for i in range(nwards):
    for j in range(nwards):
        d = distance(commonAreas[i]["lat"],commonAreas[i]["lon"],commonAreas[j]["lat"],commonAreas[j]["lon"])
        wardCentreDistances[i][str(j+1)]=d


# In[ ]:


print("Dumping to json files...",end='',flush=True)

if not os.path.exists(obasepath):
    os.makedirs(obasepath)

f = open(individualsjson, "w+")
f.write(json.dumps(individuals))
f.close
print("(individuals.json, ",end='',flush=True)

f = open(housesjson, "w+")
f.write(json.dumps(houses))
f.close
print("houses.json, ",end='',flush=True)

f = open(workplacesjson, "w+")
f.write(json.dumps(workplaces))
f.close
print("workplaces.json, ",end='',flush=True)

f = open(schoolsjson, "w+")
f.write(json.dumps(schools))
f.close
print("schools.json, ",end='',flush=True)


# In[ ]:


f = open(commonAreajson, "w+")
f.write(json.dumps(commonAreas))
f.close
print("commonArea.json, ",end='',flush=True)

f = open(fractionPopulationjson, "w+")
f.write(json.dumps(fractionPopulations))
f.close
print("fractionPopulation.json, ",end='',flush=True)

f = open(wardCentreDistancejson, "w+")
f.write(json.dumps(wardCentreDistances))
f.close
print("wardCentreDistance.json) ... done.",flush=True)




# In[ ]:





# In[ ]:
