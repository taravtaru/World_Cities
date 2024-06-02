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

struct city_sc {
    string name;
    double coef;

    city_sc(string n, double c) :
            name(n), coef(c) {}
};

struct city_sc_sorter_desc {
    bool operator()(const city_sc &a, const city_sc &b) const {
        return a.coef > b.coef;
    }
};

vector<vector<city_sc>>
silhouette(const vector<city *> &data, const vector<double> &cx, const vector<double> &cy, const vector<int> &c_idx,
           const vector<int> &total_per_cluster, const int n_cluster) {
    vector<vector<city_sc>> res;
    // для каждой точки найдем ближайший соседний кластер
    vector<int> nearest_other(data.size(), -1);
    for (int j = 0; j < data.size(); ++j) {
        int best_other_idx = -1;
        double min_other_dist = 1e12;
        for (int i = 0; i < n_cluster; ++i) {
            if (i == c_idx[j]) {
                continue;
            }
            double other_dist = sqrt(pow(data[j]->x - cx[i], 2) + pow(data[j]->y - cy[i], 2));
            if (best_other_idx == -1 || other_dist < min_other_dist) {
                best_other_idx = i;
                min_other_dist = other_dist;
            }
        }
        nearest_other[j] = best_other_idx;
    }
    vector<vector<int>> clusters;
    for (int i = 0; i < n_cluster; ++i) {
        clusters.push_back({});
        res.push_back({});
    }
    for (int j = 0; j < data.size(); ++j) {
        clusters[c_idx[j]].push_back(j);
    }
    for (int k = 0; k < n_cluster; ++k) {
        if (clusters[k].size() < 2) {
            continue;
        }
        for (int i = 0; i < clusters[k].size(); ++i) {
            // для i-той точки копим а и b
            double sa = 0;
            for (int j = 0; j < clusters[k].size(); ++j) {
                if (j == i) {
                    continue;
                }
                sa += sqrt(pow(data[clusters[k][i]]->x - data[clusters[k][j]]->x, 2) +
                           pow(data[clusters[k][i]]->y - data[clusters[k][j]]->y, 2));
            }
            sa /= clusters[k].size() - 1;
            double sb = 0;
            int k2 = nearest_other[clusters[k][i]];
            for (int j = 0; j < clusters[k2].size(); ++j) {
                sb += sqrt(pow(data[clusters[k][i]]->x - data[clusters[k2][j]]->x, 2) +
                           pow(data[clusters[k][i]]->y - data[clusters[k2][j]]->y, 2));
            }
            sb /= clusters[k2].size();
            double coef = (sb - sa) / std::max(sa, sb);
            string name = data[clusters[k][i]]->name;
            res[k].push_back(city_sc(name, coef));
        }
        //закончили очередной кластер, сортируем
        std::sort(res[k].begin(), res[k].end(), city_sc_sorter_desc());
    }
    return res;
}

void demo_city_cluster(int n_cluster) {
    srand(time(nullptr));
    const int n = 47868;
    vector<city *> data;
    vector<int> cluster_idx(n, -1);
    FILE *city_db = fopen("../worldcities.csv", "r");
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
        /// Зануляем байт который идёт после названия города
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
    vector<double> cx(n_cluster, 0);
    vector<double> cy(n_cluster, 0);
    for (int i = 0; i < n_cluster; ++i) {
        while (true) {
            int j = rand() % data.size();
            double x_0 = data[j]->x;
            double y_0 = data[j]->y;
            auto k = std::find(cx.begin(), cx.end(), x_0);
            if (k != cx.end()) {
                continue;
            }
            cx[i] = x_0;
            cy[i] = y_0;
            break;
        }
    }
    vector<double> ncx(n_cluster, 0);
    vector<double> ncy(n_cluster, 0);
    vector<int> total_per_cluster(n_cluster, 0);
    int n_step = 0;
    while (true) {
        printf("%d\n", ++n_step);
        /// Для каждой точки считаем расстояние до центров и выбираем ближайший
        for (int i = 0; i < n; ++i) {
            int best_idx = 0;
            double min_dist_sq =
                    (data[i]->x - cx[0]) * (data[i]->x - cx[0]) + (data[i]->y - cy[0]) * (data[i]->y - cy[0]);
            for (int j = 1; j < n_cluster; ++j) {
                double new_dist_sq =
                        (data[i]->x - cx[j]) * (data[i]->x - cx[j]) + (data[i]->y - cy[j]) * (data[i]->y - cy[j]);
                if (new_dist_sq < min_dist_sq) {
                    best_idx = j;
                    min_dist_sq = new_dist_sq;
                }
            }
            cluster_idx[i] = best_idx;
        }
        for (int j = 0; j < n_cluster; ++j) {
            ncx[j] = 0;
            ncy[j] = 0;
            total_per_cluster[j] = 0;
        }
        for (int i = 0; i < n; ++i) {
            total_per_cluster[cluster_idx[i]]++;
            ncx[cluster_idx[i]] += data[i]->x;
            ncy[cluster_idx[i]] += data[i]->y;
        }
        for (int j = 0; j < n_cluster; ++j) {
            printf("cluster#%d -> %d\n", j, total_per_cluster[j]);
            ncx[j] /= total_per_cluster[j];
            ncy[j] /= total_per_cluster[j];
        }
        /// Проверка на схождение(новые центры достаточно близки к старым центрам)
        bool converged_yes = true;
        double sum_dist_sq = 0;
        for (int i = 0; i < n_cluster; ++i) {
            double dist_sq = (ncx[i] - cx[i]) * (ncx[i] - cx[i]) + (ncy[i] - cy[i]) * (ncy[i] - cy[i]);
            sum_dist_sq += dist_sq;
            if (dist_sq > 1e-10) {
                converged_yes = false;

            }

        }
        if (converged_yes || n_step == 100) {
            break;
        } else {
            for (int i = 0; i < n_cluster; ++i) {
                cx[i] = ncx[i];
                cy[i] = ncy[i];
            }
        }
    }
    FILE *pf2 = fopen("city_clusters.txt", "w");
    for (int i = 0; i < n_cluster; ++i) {
        fprintf(pf2, "%d\n", total_per_cluster[i]);
        for (int j = 0; j < n; ++j) {
            if (cluster_idx[j] == i) {
                fprintf(pf2, "%lf\t%lf\n", data[j]->x, data[j]->y);
            }
        }
        printf("\n");
    }
    fclose(pf2);
    auto res = silhouette(data, cx, cy, cluster_idx, total_per_cluster, n_cluster);
    FILE *pf3 = fopen("silhouette.txt", "w");
    for (int k = 0; k < res.size(); ++k) {
        fprintf(pf3, "%ld\n", res[k].size());
        for (int i = 0; i < res[k].size(); ++i) {
            fprintf(pf3, "%s\t%lf\n", res[k][i].name.c_str(), res[k][i].coef);
        }
    }
    fclose(pf3);
}

void demo1() {
    demo_city_cluster(10);
}

int main() {
    demo1();
    return 0;
}