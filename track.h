#include <iostream>
#include <vector>
#include <random>

using namespace std;


void makeTrack(vector<pair<float, pair<float, bool>>> &vecTrack, int nTrackSize) {
    float fCurvature, fDistance;
    float fTotaldistance = 0.0f;
    bool bCheckPoint;

    // The structure is: Curvature, Distance, CheckPoint
    vecTrack.push_back(make_pair(0.0f, make_pair(10.0f, false)));
    for (int x = 0; x < nTrackSize; x++) {
        fTotaldistance += vecTrack[x].second.first;
        fCurvature = (0.0f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(2.0f-0.0f))))-1.0f;
        fDistance = 0.0f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(200.0f-0.0f)));
        // Every 2000 "meters" insert a CheckPoint
        if (fTotaldistance > 2000.0f){
            bCheckPoint = true;
            fTotaldistance = 0.0f;
            vecTrack.push_back(make_pair(0.0f, make_pair(30.0f, bCheckPoint)));
        } else {
            bCheckPoint = false;
            vecTrack.push_back(make_pair(fCurvature, make_pair(fDistance, bCheckPoint)));
        }   
    }
    vecTrack.push_back(make_pair(0.0f, make_pair(10.0f, false)));

}

float getTrackDistance(vector<pair<float, pair<float, bool>>> & vecTrack) {
    float fTrackDistance = 0.0f;
    for (auto t : vecTrack) {
        fTrackDistance += t.second.first;
    }
    return fTrackDistance;
};

