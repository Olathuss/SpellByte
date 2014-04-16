#ifndef SPELLBYTE_GRAPH_H
#define SPELLBYTE_GRAPH_H

#include <vector>
#include <forward_list>
#include "Nodes.h"
#include "Edges.h"
#include <utilities/tinyxml2.h>
#include <define.h>

//! Graph class
/*
    TODO: Implement as SparseGraph, for now
    this graph does not support removing nodes
    and edges

    This of type Digraph by default
*/

namespace SpellByte {
    namespace Graph {
        //! Graph Class, graph is of type digraph
        class Graph {
        public:
            typedef std::vector<NavNode> NodeVector;
            // Use forward list for efficiency
            typedef std::forward_list<Edge> EdgeList;
            typedef std::vector<EdgeList> EdgeListVector;

            //! Node Iterator
            typedef typename NodeVector::iterator NodeIterator;
            typedef typename EdgeList::iterator EdgeIterator;
            //typedef typename NodeVector::const_iterator ConstNodeIterator;

        private:
            // Nodes for graph
            NodeVector      Nodes;
            // Edges for graph
            EdgeListVector  Edges;

            EdgeIterator edgeIterator;

            // Index of next node
            int NextNodeIndex;

            // Check if edge is unique
            bool uniqueEdge(int origin, int dest) const;

        public:
            NodeIterator begin() {
                return Nodes.begin();
            }

            NodeIterator end() {
                return Nodes.end();
            }

            //! Constructor
            Graph();

            ~Graph();

            //! Returns node at given index
            const NavNode &getNode(int idx) const;

            //! Returns non-const node
            NavNode &getNode(int idx);

            //! Returns const edge
            const Edge &getEdge(int origin, int dest) const;

            //! Returns non-const edge
            Edge &getEdge(int origin, int dest);

            //! Get next node index
            int getNextNodeIndex() const { return NextNodeIndex; }

            //! Adds node to the graph and returns index
            int addNode(NavNode &node);

            // Removes a node, not implemented - placement method
            //void removeNode(int idx){}

            //! Add ege to graph
            /*!
                This method ensures the node's are valid
            */
            void addEdge(Edge edge);

            // Removes edge, not implemented
            //void removeEdge(int origin, int dest) {}

            //! Returns number of nodes
            int nodeCount() const { return Nodes.size(); }

            int edgeCount() const {
                int total = 0;
                for (unsigned int i = 0; i < Edges.size(); ++i) {
                    for (std::forward_list<Edge>::const_iterator it = Edges[i].begin();
                         it != Edges[i].end(); ++it)
                         total++;
                }
                return total;
            }

            //! Clears graph, removes all nodes and edges
            void clearGraph() {
                Nodes.clear();
                Edges.clear();
                NextNodeIndex = 0;
                LOG("NextNodeIndex = " + Ogre::StringConverter::toString(NextNodeIndex));
            }

            //! Returns true if graph is empty
            bool empty() const { return Nodes.empty(); }

            /*//! Nonconst iterator for edges
            class EdgeIterator {
                private:
                    typename EdgeList::iterator     CurrentEdge;
                    Graph<NodeType, EdgeType>       *EdgeGraph;
                    const int                       NodeIndex;

                public:
                    EdgeIterator(Graph<NodeType, EdgeType> *graph,
                                 int whichNode): EdgeGraph(graph),
                                 NodeIndex(whichNode)
                    {
                        CurrentEdge = EdgeGraph->Edges[NodeIndex].begin();
                    }

                    EdgeIterator &operator=(const CurrentEdge& rhs) {
                        this->CurrentEdge = rhs;
                    }

                    typename CurrentEdge begin() {
                        return EdgeGraph->Edges[NodeIndex].begin();
                    }

                    void operator++() {
                        ++CurrentEdge;
                    }

                    typename CurrentEdge end() {
                        return EdgeGraph->Edges[NodeIndex].end();
                    }
            };*/

            void startIterator(int whichNode) {
                edgeIterator = Edges[whichNode].begin();
            }

            bool atEnd(int whichNode) {
                return edgeIterator == Edges[whichNode].end();
            }

            void nextEdge() {
                ++edgeIterator;
            }

            Edge &currentEdge() {
                return *edgeIterator;
            }
        };
    }
}

#endif // SPELLBYTE_GRAPH_H
