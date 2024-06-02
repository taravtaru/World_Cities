import matplotlib.pyplot as plt
import re

all_x = []
all_y = []
f = open("city_clusters.txt", "r")
while True:
    s = f.readline()
    if not s:
        break
    n = int(s)
    cur_x = []
    cur_y = []
    for i in range(n):
        coords = f.readline()
        w = coords.split()
        cur_x.append(float(w[0]))
        cur_y.append(float(w[1]))
    all_x.append(cur_x)
    all_y.append(cur_y)
f.close()
f = open("silhouette.txt", "r")
s_clusters = []
while True:
    s = f.readline()
    if not s:
        break
    n = int(s)
    cur_n = []
    cur_c = []
    for i in range(n):
        data = f.readline()
        data = data[:-1]
        w = re.split(r"\t+", data)
        print(w)
        cur_n.append(w[0])
        cur_c.append(float(w[1]))
    s_clusters.append((cur_n, cur_c))
f.close()
for i in range(len(s_clusters)):
    plt.barh(s_clusters[i][0], s_clusters[i][1])
plt.show()
print(s_clusters[0])
for i in range(len(all_x)):
    plt.scatter(all_x[i], all_y[i], s=1)
plt.show()
