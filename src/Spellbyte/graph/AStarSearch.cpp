#include "AStarSearch.h"

namespace SpellByte {
    namespace Graph {
        bool AStarSearch::search() {
            // Use greater so that edges with the lowest cost will have priority
            Path.clear();
            std::priority_queue<SearchNode, std::vector<SearchNode>, std::greater<SearchNode> > considerNodes;

            // Use vectors to increase CPU performance since determining if
            // a node is open or closed will be O(1), this will however increase memory usage
            std::vector<bool> openNodes(SearchGraph->nodeCount(), false);
            std::vector<bool> closedNodes(SearchGraph->nodeCount(), false);

            std::vector<SearchNode> nodeRecords = std::vector<SearchNode>(SearchGraph->nodeCount());

            nodeRecords[Origin].NodeIndex = Origin;
            nodeRecords[Origin].CostSoFar = 0.0;
            nodeRecords[Origin].EstimatedTotal = heuristic(Origin, Dest);
            nodeRecords[Origin].NodeParent = -1;

            openNodes[Origin] = true;
            SearchNode start = nodeRecords[Origin];
            considerNodes.push(start);
            Path.push_back(nodeRecords[Origin]);

            while (!considerNodes.empty()) {
                SearchNode current = considerNodes.top();
                considerNodes.pop();

                if (current.NodeIndex == Dest)
                    return true;

                //closedNode[scurrent.NodeIndex] = true;
                SearchGraph->startIterator(current.NodeIndex);
                while(!SearchGraph->atEnd(current.NodeIndex)) {
                    Edge currEdge = SearchGraph->currentEdge();
                    int neighbor = currEdge.getDest();

                    Ogre::Real toNeighborCost = current.CostSoFar + currEdge.getCost();

                    SearchGraph->nextEdge();

                    Ogre::Real heuristicCost = 0.0;

                    // If neighbor is in closed node list
                    if (closedNodes[neighbor] == true) {
                        // If  not a shorter route, skip
                        if (nodeRecords[neighbor].CostSoFar <= toNeighborCost)
                            continue;
                        // Otherwise remove it from clsoed list
                        closedNodes[neighbor] = false;

                        // Cheat by using the node's old cost value to calculate
                        // The heuristic without calling the expensive heuristic
                        heuristicCost = nodeRecords[neighbor].EstimatedTotal - toNeighborCost;
                    } else if (openNodes[neighbor] == true) {
                        // If route is better, continue
                        if (nodeRecords[neighbor].CostSoFar <= toNeighborCost)
                            continue;

                        heuristicCost = heuristic(neighbor, Dest);
                    } else {
                        heuristicCost = heuristic(neighbor, Dest);
                    }

                    nodeRecords[neighbor].NodeIndex = neighbor;
                    nodeRecords[neighbor].CostSoFar = toNeighborCost;
                    nodeRecords[neighbor].EstimatedTotal = toNeighborCost + heuristicCost;
                    nodeRecords[neighbor].NodeParent = current.NodeIndex;
                    Path.push_back(nodeRecords[neighbor]);

                    if (openNodes[neighbor] == false) {
                        openNodes[neighbor] = true;
                        considerNodes.push(nodeRecords[neighbor]);
                    }

                    openNodes[current.NodeIndex] = false;
                    closedNodes[current.NodeIndex] = true;
                }
            }

            return false;
        }
    }
}
