#ifndef SAVING_ALGORITHM_SEQUENTIAL_VERSION
#define SAVING_ALGORITHM_SEQUENTIAL_VERSION

#include "ProblemInstance.cpp"
#include "VrpSolution.cpp"
#include "TspOptimizer.cpp"

namespace Saving_algorithm_sequential_verion {
    vector<set<pair<double, int> > > adj;
    int limitStartCandidateSize;
    double percentGetBest;

    void Assign_parameter() {
        limitStartCandidateSize = 10;
        percentGetBest = 0.9;
    }

    void Initialize(vector<vector<double> > &phe) {
        adj.clear();
        adj.resize(VRP_INSTANCE.nNode);

        for (int u = 1; u < VRP_INSTANCE.nNode; ++u) 
        for (int v = u+1; v < VRP_INSTANCE.nNode; ++v) {
            double saving_cost = VRP_INSTANCE.distances[0][u] + VRP_INSTANCE.distances[0][v] - VRP_INSTANCE.distances[u][v];
            double value = saving_cost / VRP_INSTANCE.distances[u][v] * phe[u][v] * saving_cost / VRP_INSTANCE.distances[u][v] * phe[u][v];
            if (saving_cost < 1e-9) continue;

            adj[u].insert( make_pair(-value, v) );
            adj[v].insert( make_pair(-value, u) );
        }
    }

    int findStartPoint(set<pair<double, int> > &nearestDepot, vector<bool> &isDeleted) {
        vector<pair<double, int> > startCandidate;
        double sumDistance = 0;
        for (auto it = nearestDepot.begin(); it != nearestDepot.end();) {
            if (isDeleted[it->second]) {
                auto tmpIT = it; ++tmpIT;
                it = nearestDepot.erase(it);
                if (tmpIT != nearestDepot.end()) assert( (*it) == (*tmpIT) );
                else assert( it == nearestDepot.end() );
                continue;
            }

            startCandidate.push_back(*it);
            sumDistance += -it->first;
            if ( (int) startCandidate.size() >= limitStartCandidateSize ) break;
            ++it;
        }
        Common::randomShuffle(startCandidate);

        double take_part = sumDistance * Common::getRandomInt(0, 100000) / (double) 100000;
        for (auto foo : startCandidate) {
            take_part += foo.first;
            if (take_part >= -1e-9) {
                int startPoint = foo.second;
                nearestDepot.erase(nearestDepot.find(foo));
                return startPoint;
            } 
        }
        return -1;
    }

    VrpSolution execute() {
        set<pair<double, int> > nearestDepot;
        for (int u = 1; u < VRP_INSTANCE.nNode; ++u) nearestDepot.insert( make_pair(VRP_INSTANCE.distances[0][u], u) );

        vector<bool> isDeleted;
        isDeleted.resize( VRP_INSTANCE.nNode, false );

        VrpSolution vrp_tour;
        while (true) {
            int startPoint = findStartPoint(nearestDepot, isDeleted);
            if (startPoint == -1) break;

            isDeleted[startPoint] = true;
            int total_demand = VRP_INSTANCE.demands[startPoint];
            
            Tour tour;
            tour.append(0);
            tour.append(startPoint);

            int currentPoint = startPoint;
            while (true) {
                double percent = Common::getRandomInt(0, 100000) / (double) 100000;
                int nextPoint = -1;

                if (percent <= percentGetBest) {
                    for (auto it = adj[currentPoint].begin(); it != adj[currentPoint].end();) {
                        int point = it->second;
                        if ( isDeleted[point] ) {
                            it = adj[currentPoint].erase(it);
                            continue;
                        }
                        if ( total_demand + VRP_INSTANCE.demands[point] > VRP_INSTANCE.vehicleCapacity ) { ++it; continue; }

                        nextPoint = point;                        
                        break;
                    }
                }
                else {
                    /// percent probability choice
                    vector<pair<double, int> > candidate;
                    double sumSaving = 0;
                    for (auto it = adj[currentPoint].begin(); it != adj[currentPoint].end();) {
                        int point = it->second;
                        if ( isDeleted[point] ) {
                            auto tmpIT = it; ++tmpIT;
                            it = adj[currentPoint].erase(it);
                            if (it != adj[currentPoint].end()) assert( (*it) == (*tmpIT) );
                            else assert( tmpIT == adj[currentPoint].end() );
                            continue;
                        }
                        if ( total_demand + VRP_INSTANCE.demands[point] > VRP_INSTANCE.vehicleCapacity ) { ++it; continue; }

                        candidate.push_back(*it);
                        sumSaving += abs( it->first );
                        ++it;
                    }

                    Common::randomShuffle(candidate);
                    double take_part = sumSaving * Common::getRandomInt(0, 100000) / (double) 100000;
                    for (auto foo : candidate) {
                        take_part -= abs( foo.first );
                        if (take_part <= 1e-9) { nextPoint = foo.second; break; }
                    }
                }

                if (nextPoint == -1) {
                    tour.append(0);
                    vrp_tour.tours.push_back(tour);
                    break;
                }
                else {
                    isDeleted[nextPoint] = true;
                    currentPoint = nextPoint;
                    total_demand += VRP_INSTANCE.demands[currentPoint];
                    tour.append(currentPoint);
                }
            }
        }

            vrp_tour.debug();
            vrp_tour.validate();
            exit(0);

        vrp_tour.balance();
        vrp_tour.validate();
            

        return vrp_tour;
    }
}

#endif // SAVING_ALGORITHM_SEQUENTIAL_VERSION
