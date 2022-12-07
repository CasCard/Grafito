import numpy as np
import random
import time
header = "batterLevel,cpuTemp,camera,depth,velocity,acceleration,drag,distance,flow1,flow2,roll,pitch,yaw,prop_1,prop_2,prop_3,prop_4,prop_5,prop_6,time_now"
with open("data.csv", "w") as f:
    f.write(header + "\n")

# random integer generator between 0 and 12
for _ in range(1000):
    batterLevel = random.randint(0, 12)
    # cpu temperature between 0 and 50
    cpuTemp = random.randint(0, 50)
    # random boolean generator
    camera = random.randint(0, 1)
    # depth float between 0 and 100
    depth = random.uniform(0, 100).__round__(2)
    velocity = random.uniform(0, 100).__round__(2)
    acceleration = random.uniform(-50, 50).__round__(2)
    drag = random.uniform(-50, 50).__round__(2)
    distance = random.uniform(0, 100).__round__(2)
    flow1= random.uniform(0, 100).__round__(2)
    flow2= random.uniform(0, 100).__round__(2)
    roll, pitch, yaw = random.randint(-180, 180), random.randint(-180, 180), random.randint(-180, 180)
    prop_1 = random.randint(0, 1023)
    prop_2 = random.randint(0, 1023)
    prop_3 = random.randint(0, 1023)
    prop_4 = random.randint(0, 1023)
    prop_5 = random.randint(0, 1023)
    prop_6 = random.randint(0, 1023)
#     random time generator in 24 hour format with seconds
    time_now = time.strftime("%H:%M:%S", time.gmtime(random.uniform(0, 86400)))
    data = str(batterLevel) + "," + str(cpuTemp) + "," + str(camera) + "," + str(depth) + "," + str(velocity) + "," + str(acceleration) + "," + str(drag) + "," + str(distance) + "," + str(flow1) + "," + str(flow2) + "," + str(roll) + "," + str(pitch) + "," + str(yaw) + "," + str(prop_1) + "," + str(prop_2) + "," + str(prop_3) + "," + str(prop_4) + "," + str(prop_5) + "," + str(prop_6) + "," + str(time_now) + "\n"
    with open('data.csv', 'a') as f:
        f.write(data)




# save all the data to a file in string format








