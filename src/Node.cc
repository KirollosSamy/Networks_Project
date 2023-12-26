#include "Node.h"

Define_Module(Node);

void Node::initialize()
{
    // Initialize parameters
    int WS = getParentModule()->par("WS");

    parameters.TO = getParentModule()->par("TO");
    parameters.PT = getParentModule()->par("PT");
    parameters.TD = getParentModule()->par("TD");
    parameters.ED = getParentModule()->par("ED");
    parameters.DD = getParentModule()->par("DD");
    parameters.LP = getParentModule()->par("LP");

    node_id = getNodeId(getName());

    go_back_N = new GoBackN(WS, parameters, node_id, this);
}

Node::~Node()
{
    delete this->go_back_N;
}

void Node::handleMessage(cMessage *msg)
{
    Frame_Base* frame = dynamic_cast<Frame_Base*>(msg);

    // in case of recieving a frame
    if (frame != nullptr)
        go_back_N->protocol(Event::FRAME_ARRIVAL, frame);

    // when we want to send a frame.
    else if (msg->getKind() == (short)MsgType::SEND_FRAME)
    {
        bool more_frames = go_back_N->protocol(Event::NETWORK_LAYER_READY);

        if (more_frames)
        {
            // this is a reminder to send the next frame after the processing time of t
            Time time = simTime().dbl() + parameters.PT; // replace with the actual time to send, I guess simtime() + PT
            cMessage *msg = new cMessage();
            msg->setKind((short)MsgType::SEND_FRAME);
            scheduleAt(time, msg);
        }
    }
    else if (msg->getKind() == (short)MsgType::TIMEOUT)
        go_back_N->protocol(Event::TIMEOUT);
}
