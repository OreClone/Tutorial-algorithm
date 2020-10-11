#ifndef INSERTED_STRATEGY
#define INSERTED_STRATEGY

#include "ProblemInstance.cpp"
#include "VrpSolution.cpp"
#include "TspOptimizer.cpp"

namespace inserted_strategy {
    vector<set<pair<double, int> > > adj;
    int limitStartCandidateSize;
    double percentGetBest;

    void Assign_parameter() {
        limitStartCandidateSize = 1000;
        percentGetBest = 0.9;
    }

    void Initialize(vector<vector<double> > &phe) {
        adj.clear();
        adj.resize(VRP_INSTANCE.nNode);

        for (int u = 1; u < VRP_INSTANCE.nNode; ++u) 
        for (int v = u+1; v < VRP_INSTANCE.nNode; ++v) {
            double saving_cost = VRP_INSTANCE.distances[0][u] + VRP_INSTANCE.distances[0][v] - VRP_INSTANCE.distances[u][v];
            // double value = saving_cost / VRP_INSTANCE.distances[u][v] * phe[u][v] * saving_cost / VRP_INSTANCE.distances[u][v] * phe[u][v];
            // double value = phe[u][v] * phe[u][v] / VRP_INSTANCE.distances[u][v] / VRP_INSTANCE.distances[u][v];
            double value = 1 / VRP_INSTANCE.distances[u][v];
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

            /// debug 
            if (startCandidate.empty()) return -1;           
            auto foo = startCandidate[0];
            nearestDepot.erase(nearestDepot.find(foo));
            return foo.second;

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
            int tour_capacity = VRP_INSTANCE.demands[startPoint];

            deque<int> dq; 
            dq.push_back(startPoint);

            while (true) {
                int nextPoint = -1, side = -1;
                double percent = Common::getRandomInt(0, 10000) / (double) 10000; 
                vector<int> lsEndPoint = { dq.front(), dq.back() };

                if (percent <= percentGetBest) {
                    double bestCost = (double) 1e9+7;
                    for (int id = 0; id <= 1; ++id) {
                        int currentPoint = lsEndPoint[id];
                        for (auto it = adj[currentPoint].begin(); it != adj[currentPoint].end(); ) {
                            if ( isDeleted[it->second] ) {
                                it = adj[currentPoint].erase(it);
                                continue;
                            }
                            if ( tour_capacity + VRP_INSTANCE.demands[it->second] > VRP_INSTANCE.vehicleCapacity ) { ++it; continue; } 

                            if ( nextPoint == -1 || bestCost > abs(it->first) ) {
                                bestCost = abs(it->first);
                                nextPoint = it->second;
                                side = id;
                            }
                            break;
                        }
                    }
                }
                else {
                    double sumCost = 0;
                    vector<pair<double, int> > roulette[2]; 
                    for (int id = 0; id <= 1; ++id) {
                        int currentPoint = lsEndPoint[id];
                        for (auto it = adj[currentPoint].begin(); it != adj[currentPoint].end(); ) {
                            if (isDeleted[it->second]) {
                                it = adj[currentPoint].erase(it);
                                continue;
                            }
                            if ( tour_capacity + VRP_INSTANCE.demands[it->second] > VRP_INSTANCE.vehicleCapacity ) { ++it; continue; } 

                            roulette[id].push_back(*it);
                            sumCost += abs( it->first );
                            ++it; 
                        }
                    }
                    
                    Common::randomShuffle(roulette[0]);
                    Common::randomShuffle(roulette[1]);
                    double take_part = sumCost * Common::getRandomInt(0, 100000) / 100000;

                    for (int id = 0; id <= 1; ++id) 
                    for (auto foo : roulette[id]) {
                        take_part -= abs( foo.first );
                        if ( take_part < 1e-9 ) { nextPoint = foo.second; side = id; break; }
                    }
                }

                if (nextPoint == -1) break;
                isDeleted[nextPoint] = true;
                tour_capacity += VRP_INSTANCE.demands[nextPoint];

                if (side == 0) dq.push_front(nextPoint);
                else dq.push_back(nextPoint);
            }

            Tour tour;
            tour.append(0);
            while (dq.size()) { tour.append(dq.back()); dq.pop_back(); }
            tour.append(0);
            vrp_tour.tours.push_back(tour);
        }

        vrp_tour.balance();
        vrp_tour.validate();

            vrp_tour.debug();
            exit(0);
    }
}

#endif // INSERTED_STRATEGY
