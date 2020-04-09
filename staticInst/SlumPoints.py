#!/usr/bin/env python
# coding: utf-8

# In[7]:


import json
import random
import geopandas as gpd
from shapely.geometry import Point, MultiPolygon
import argparse

my_parser = argparse.ArgumentParser()
my_parser.add_argument('-n')
args = my_parser.parse_args()

geoDFslums = gpd.read_file('data/base/mumbai/slumClusters.geojson')
geoDF = gpd.read_file('data/base/mumbai/city.geojson')

wardslums = [[] for _ in range(len(geoDF))]

for i in range(len(geoDFslums)):
    for j in range(len(geoDF)):
        if geoDFslums["geometry"][i].intersects(geoDF["geometry"][j]):
            wardslums[j].append(i)

geoDF['wardBounds'] = geoDF.apply(lambda row: MultiPolygon(row['geometry']).bounds, axis=1)
def sampleRandomLatLong_s(wardIndex):

    #I'm not sure why the order is longitude followed by latitude
    (lon1,lat1,lon2,lat2) = geoDF['wardBounds'][wardIndex]
    while True:
        lat = random.uniform(lat1,lat2)
        lon = random.uniform(lon1,lon2)
        point = Point(lon,lat)
        if MultiPolygon(geoDF['geometry'][wardIndex]).contains(point):
            for i in wardslums[wardIndex]:
                if geoDFslums["geometry"][i].contains(point):
                    return (lat,lon)
def samplePoints(wardIndex):
    if wardIndex==2:
        return 0
    for i in range(500):
        (lat,lon) = sampleRandomLatLong_s(wardIndex)
        print(lat,",",lon,flush=True)
samplePoints(int(args.n))
