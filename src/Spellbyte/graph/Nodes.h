#ifndef NODE_H
#define NODE_H

#include <OgreVector3.h>

/**
    Contains Node Classes
*/

namespace SpellByte {
    namespace Graph {
        //! Base Node Class
        class Node {
        protected:
            int NodeIndex;

        public:
            //! Enum to specify an invalid node
            enum {
                INVALID_NODE_INDEX = -1
            };
            //! Default constructor, sets index to invalid
            Node():NodeIndex(INVALID_NODE_INDEX){}
            //! NodeIndex should correspond to graph index
            Node(int id):NodeIndex(id) {
                // Empty Constructor
            }

            //! Destructor
            virtual ~Node(){}

            //! Set index of node
            void setIndex(int idx) {
                NodeIndex = idx;
            }

            //! Get index of node
            int getIndex() {
                return NodeIndex;
            }

            //! Comparison operator ==
            /*!
                This assumes child classes
                to also be equal
            */
            bool operator==(const Node &rhs) const {
                return this->NodeIndex == rhs.NodeIndex;
            }

            //! Not equal operator
            /*!
                This assumes child classes
                to also be equal
            */
            bool operator!=(const Node &rhs) const {
                return !(*this == rhs);
            }
        };

        //! Navigation code, contains Vector3 position
        class NavNode : public Node {
        protected:
            //! Node's vector position
            Ogre::Vector3 Position;

        public:
            NavNode():Node(){}

            NavNode(int idx, Ogre::Vector3 pos):
                    Node(idx), Position(pos)
            {
                // Empty
            }

            virtual ~NavNode(){}

            //! Get position of navigation node
            Ogre::Vector3 getPos() const {
                 return Position;
            }

            //! Set position of navigation node
            void setPos(Ogre::Vector3 newPos) {
                Position = newPos;
            }
        };
    }
}

#endif // NODE_H
