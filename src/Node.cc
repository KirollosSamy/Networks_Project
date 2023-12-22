#include "Node.h"

Define_Module(Node);

void Node::initialize()
{
    // Initialize parameters
    int WS = getParentModule()->par("WS");

    NetworkParameters parameters;
    parameters.TO = getParentModule()->par("TO");
    parameters.PT = getParentModule()->par("PT");
    parameters.TD = getParentModule()->par("TD");
    parameters.ED = getParentModule()->par("ED");
    parameters.DD = getParentModule()->par("DD");
    parameters.LP = getParentModule()->par("LP");

    node_id = getNodeId(getName());

    GoBackN go_back_N(WS, parameters, node_id);
}

void Node::handleMessage(cMessage* msg)
{   
    Frame_Base* frame = static_cast<Frame_Base*>(msg);
    if(frame != nullptr){
        //frame arrival event or checksum event
    }
    else if(msg->getKind() == (short)MsgType::SEND_FRAME){
        // Network layer ready event

        Time time = 0; //replace with the actual time to send, I guess simtime() + PT
        cMessage* msg = new cMessage();
        msg->setKind((short)MsgType::SEND_FRAME);
        scheduleAt(time, msg);
    }
    else if(msg->getKind() == (short)MsgType::TIMEOUT){
        //time out event
    }
}
