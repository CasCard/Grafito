import pandas as pd
import numpy as np
import csv as csv

# read data.csv into a dataframe
def read_data(filename):
    data = pd.read_csv(filename, header=0)
    return data

d = read_data('data.csv')
print(d['depth'][5])

