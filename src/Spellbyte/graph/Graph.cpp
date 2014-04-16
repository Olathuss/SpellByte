#include "Graph.h"

namespace SpellByte {
    namespace Graph {
        Graph::Graph():NextNodeIndex(0) {
            Nodes = NodeVector();
        }

        Graph::~Graph() {
            // Empty
        }

        //! Returns node at given index
        NavNode &Graph::getNode(int idx) {
            assert( idx < NextNodeIndex &&
                    "Graph::getNode: invalid node index");

            return Nodes[idx];
        }

        //! Returns const node at given index
        const NavNode &Graph::getNode(int idx) const {
            assert( idx < NextNodeIndex &&
                    "Graph::getNode: invalid node index");

            return Nodes[idx];
        }

        //! Returns const edge
        const Edge &Graph::getEdge(int origin, int dest) const {
            assert( origin >= 0 && origin < NextNodeIndex &&
                   "Graph::getEdge: invalid origin node");

            assert( dest >= 0 &&  dest < NextNodeIndex &&
                   "Graph::getEdge: invalid destination node");

            for (EdgeList::const_iterator it = Edges[origin].begin();
                 it != Edges[origin].end();
                 ++it) {
                    if (it->getDest() == dest)
                        return *it;
            }
        }

        //! Returns nonconst edge
        Edge &Graph::getEdge(int origin, int dest) {
            assert( origin >= 0 && origin < NextNodeIndex &&
                   "Graph::getEdge: invalid origin node");

            assert( dest >= 0 &&  dest < NextNodeIndex &&
                   "Graph::getEdge: invalid destination node");

            for (EdgeList::iterator it = Edges[origin].begin();
                 it != Edges[origin].end();
                 ++it) {
                    if (it->getDest() == dest)
                        return *it;
            }
        }

        //! Adds node to graph
        int Graph::addNode(NavNode &node) {
            assert( node.getIndex() == NextNodeIndex &&
                    "Graph::addNode: invalid node index");

            Nodes.push_back(node);
            Edges.push_back(EdgeList());

            return NextNodeIndex++;
        }

        //! Adds edge to graph
        void Graph::addEdge(Edge edge) {
            assert( edge.getOrigin() < NextNodeIndex &&
                    edge.getDest() < NextNodeIndex &&
                   "Graph::addEdge: invalid edge");

            if (uniqueEdge(edge.getOrigin(), edge.getDest())) {
                Edges[edge.getOrigin()].push_front(edge);
                // Since edge is digraph by default
                // then assume opposite edge is also unique
                Edge newEdge = edge;
                newEdge.setOrigin(edge.getDest());
                newEdge.setDest(edge.getOrigin());
                Edges[edge.getDest()].push_front(newEdge);
            }
        }

        //! Check if edge is unique
        bool Graph::uniqueEdge(int origin, int dest) const {
            for (typename EdgeList::const_iterator it = Edges[origin].begin();
                 it != Edges[origin].end();
                 ++it) {
                    if (it->getDest() == dest) {
                        return false;
                    }
            }

            return true;
        }
    }
}

