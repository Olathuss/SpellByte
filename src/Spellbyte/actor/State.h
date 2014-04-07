#ifndef STATE_H
#define STATE_H

namespace SpellByte {
    template <class actor_type>
    class State {
    public:
        virtual void Enter(actor_type*) = 0;
        virtual void Execute(actor_type*, const Ogre::FrameEvent &evt) = 0;
        virtual void Exit(actor_type*) = 0;
        virtual ~State(){};
    };
}
#endif // STATE_H
