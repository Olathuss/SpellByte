#ifndef ASTARSEARCH_H
#define ASTARSEARC_H

#include <algorithm>
#include "Graph.h"

namespace SpellByte {
    namespace Graph {
        class AStarSearch {
        private:
            int Origin;
            int Dest;

            Graph *SearchGraph;

            //void search();

            struct SearchNode {
            public:
                int NodeIndex;
                Ogre::Real CostSoFar;
                Ogre::Real EstimatedTotal;
                int NodeParent;

                bool operator<(const SearchNode &rhs) {
                    return this->EstimatedTotal < rhs.EstimatedTotal;
                }

                bool operator>(const SearchNode &rhs) {
                    return this->EstimatedTotal > rhs.EstimatedTotal;
                }
            };

            std::vector<SearchNode> Path;

            Ogre::Real getCost(int source, int dest) {
                return SearchGraph->getEdge(source, dest).getCost();
            }

            Ogre::Real heuristic(int source, int dest) {
                Ogre::Vector3 oPosition = SearchGraph->getNode(source).getPos();
                Ogre::Vector3 dPosition = SearchGraph->getNode(dest).getPos();
                return oPosition.distance(dPosition);
            }

        public:
            AStarSearch(int origin, int dest, Graph *graph):
                Origin(origin), Dest(dest), SearchGraph(graph) {
            }

            ~AStarSearch(){};

            bool search();

            std::vector<int> getPath() {
                int currentNode = Dest;
                std::vector<int> FoundPath;
                for (unsigned int i = Path.size(); i >= 0; --i) {
                    if (Path[i].NodeIndex == Origin) {
                        FoundPath.push_back(Origin);
                        break;
                    }
                    if (Path[i].NodeIndex == currentNode) {
                        FoundPath.push_back(currentNode);
                        currentNode = Path[i].NodeParent;
                    }
                }
                std::reverse(FoundPath.begin(), FoundPath.end());
                return FoundPath;
            }
        };
    }
}

#endif // ASTARSEARCH_H
