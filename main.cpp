#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <algorithm>

using std::vector;
using std::cout;
using std::string;

struct city {
    string name;
    double x;
    double y;
    int population;

    city(string n, double _x, double _y, int _pop) {
        name = n;
        x = _x;
        y = _y;
        population = _pop;
    }
};


void demo_city_cluster(int n_cluster) {
    srand(time(nullptr));
    const int n = 47868;
    vector<city *> data;
    vector<int> cluster_idx(n, -1);
    FILE *city_db = fopen("worldcities.csv", "r");
    if (city_db == nullptr) {
        cout << "file not found" << std::endl;
        return;
    }
    char buf[256];
    int buf_com[10];
    for (int i = 0; i < n; ++i) {
        int k = 0;
        fgets(buf, 256, city_db);
        int if_cov = -1;
        for (int j = 0; buf[j] != 0; ++j) {
            if (buf[j] == '"') {
                if_cov *= -1;
            }
            if (buf[j] == ',' and if_cov == -1) {
                buf_com[k] = j;
                k++;
            }
        }
        buf[buf_com[1] - 1] = 0;
        string name = buf + buf_com[0] + 2;
        double lat = strtod(buf + buf_com[1] + 2, nullptr);
        double lng = strtod(buf + buf_com[2] + 2, nullptr);
        int pop = strtol(buf + buf_com[8] + 2, nullptr, 10);
        data.push_back(new city(name, lng, lat, pop));
    }
    for (int i = 0; i < data.size(); ++i) {
        printf("%s\t%lf\t%lf\t%d\n", data[i]->name.c_str(), data[i]->x, data[i]->y, data[i]->population);
    }
    fclose(city_db);
    printf("\n");


void demo1() {
    demo_city_cluster(10);
}

int main() {
    demo1();
    return 0;
}