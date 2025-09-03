#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <numeric> 

using namespace std;

struct DistanceInfo {
    string refPos;
    double distance;
};

bool isImage = true;
string getPosition(const string& line) {
    size_t space_pos = line.find(' ');
    string pos = line.substr(1, space_pos - 1);
    isImage = (pos.find("'") == string::npos);
    return pos;
}

vector<double> getData(const string& line) {
    size_t space_pos = line.find(' ');
    string content = line.substr(space_pos + 1);
    vector<double> result;
    stringstream ss(content);
    double value;
    while (ss >> value) {
        result.push_back(value);
    }
    return result;
}

int main() {
    ifstream file("data_map1_full3circles_23082025_input_3.2 & 4.3.2 .txt");
    /*  ifstream file("data_map1(r1)_24082025_input_4.3.2.txt");
        ifstream file("data_map1(r2)_24082025_input_4.3.2.txt");
        ifstream file("data_map1(r3)_24082025_input_4.3.2.txt");
    */
    if (!file.is_open()) {
        cerr << "Khong the mo file!" << endl;
        return 1;
    }

    vector<string> imPos;
    vector<string> refPos;
    vector<vector<double>> imData;
    vector<vector<double>> refData;

    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            string pos = getPosition(line);
            if (isImage) {
                imPos.push_back(pos);
                imData.push_back(getData(line));
            } else {
                refPos.push_back(pos);
                refData.push_back(getData(line));
            }
        }
    }
    file.close();

    double pi = 3.1416;
    double r3 = 90;
    double area = pi * r3 * r3;

    vector<double> all_confidence_scores;

    for (int i = 0; i < imData.size(); ++i) {
        vector<DistanceInfo> distances;

        for (int j = 0; j < refData.size(); ++j) {
            double sum = 0.0;
            for (int k = 0; k < imData[i].size(); ++k) {
                sum += fabs(imData[i][k] - refData[j][k]);
            }
            double dis = sum / area;
            dis = round(dis * 10000.0) / 10000.0;
            distances.push_back({refPos[j], dis});
        }

        sort(distances.begin(), distances.end(), [](const DistanceInfo& a, const DistanceInfo& b) {
            return a.distance < b.distance;
        });

        double confidence_score = 0.0;
        if (distances.size() >= 2) {
            double d1 = distances[0].distance;
            double d2 = distances[1].distance;
            if (d2 > 1e-9) {
                confidence_score = (d2 - d1) / d2;
            }
        }
        
        all_confidence_scores.push_back(confidence_score); 
        
        
        cout << "=========================================================" << endl;
        cout << "Analysis for image point: " << imPos[i] << endl;
        cout << "-> Best Match: " << (distances.empty() ? "N/A" : distances[0].refPos) 
             << " (distance: " << fixed << setprecision(4) << (distances.empty() ? 0.0 : distances[0].distance) << ")" << endl;
        cout << "-> Confidence Score: " << fixed << setprecision(2) << confidence_score 
             << " (" << (confidence_score * 100.0) << "%)" << endl;
        cout << "---------------------------------------------------------" << endl;
        cout << left << setw(10) << "Rank" << setw(15) << "Ref Point" << "Distance" << endl;
        cout << "---------------------------------------------------------" << endl;

        for (int k = 0; k < distances.size(); ++k) {
            cout << left << setw(10) << k + 1 
                 << setw(15) << distances[k].refPos 
                 << fixed << setprecision(4) << distances[k].distance << endl;
        }
        cout << endl << endl;
    }

    double confidence_mean = 0.0;
    double confidence_median = 0.0;

    if (!all_confidence_scores.empty()) {
        double sum_of_scores = std::accumulate(all_confidence_scores.begin(), all_confidence_scores.end(), 0.0);
        confidence_mean = sum_of_scores / all_confidence_scores.size();

        sort(all_confidence_scores.begin(), all_confidence_scores.end());
        size_t n = all_confidence_scores.size();
        if (n % 2 == 1) {
            confidence_median = all_confidence_scores[n / 2];
        } else {
            confidence_median = (all_confidence_scores[n / 2 - 1] + all_confidence_scores[n / 2]) / 2.0;
        }
    }

    cout << "#########################################################" << endl;
    cout << "#                   OVERALL STATISTICS                  #" << endl;
    cout << "#########################################################" << endl;
    cout << left << setw(25) << "Confidence Mean:" << fixed << setprecision(2) << confidence_mean << endl;
    cout << left << setw(25) << "Confidence Median:" << fixed << setprecision(2) << confidence_median << endl;
    cout << "#########################################################" << endl;

    return 0;
}