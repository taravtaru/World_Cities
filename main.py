import matplotlib.pyplot as plt

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
for i in range(len(all_x)):
    plt.scatter(all_x[i], all_y[i], s=1)
plt.show()
