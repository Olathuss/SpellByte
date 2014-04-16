#ifndef EDGES_H
#define EDGES_H

#include "Nodes.h"

namespace SpellByte {
    namespace Graph {
        //! Defines an edge between two nodes
        class Edge {
        protected:
            //! Origin node, refers to node's index
            int OriginNode;
            //! Destination node's index
            int DestNode;

            //! Cost of traversing edge
            Ogre::Real Cost;

        public:
            Edge(int origin, int dest, Ogre::Real cost):
                    OriginNode(origin), DestNode(dest),
                    Cost(cost)
            {
                // Empty constructor
            }

            Edge(int origin, int dest):
                    OriginNode(origin), DestNode(dest),
                    Cost(1.0)
            {
                // Empty constructor
            }

            Edge():OriginNode(Node::INVALID_NODE_INDEX),
                    DestNode(Node::INVALID_NODE_INDEX),
                    Cost(1.0)
            {
                // Empty constructor
            }

            virtual ~Edge(){}

            //! Get origin node's index
            int getOrigin() { return OriginNode; }

            //! Set origin node's index
            void setOrigin(int origin) { OriginNode = origin; }

            //! Get destination node index
            int getDest() const { return DestNode; }

            //! Set destination node index
            void setDest(int dest) { DestNode = dest; }

            //! Get cost
            Ogre::Real getCost() const { return Cost; }

            //! Set cost
            void setCost(Ogre::Real newCost) { Cost = newCost; }

            // Comparison operators required for graph search
            bool operator==(const Edge &rhs) {
                return this->OriginNode == rhs.OriginNode &&
                        this->DestNode == rhs.DestNode &&
                        this->Cost == rhs.Cost;
            }

            bool operator!=(const Edge &rhs) {
                return !(*this == rhs);
            }

            bool operator<(const Edge &rhs) {
                return this->Cost < rhs.Cost;
            }

            bool operator>(const Edge &rhs) {
                return this->Cost > rhs.Cost;
            }
        };
    }
}

#endif // EDGES_H
