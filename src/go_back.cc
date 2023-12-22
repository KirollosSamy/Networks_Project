#include "go_back.h"

GoBackN::GoBackN(int WS, NetworkParameters parameters, int node_id) {
    MAX_SEQUENCE = WS;
    par = parameters;
    buffer = new Frame_Base[MAX_SEQUENCE + 1];

    network_layer = new NetworkLayer("../input/input" + std::to_string(node_id) + ".txt");
    logger = new Logger("../log/out.log");
}


GoBackN::~GoBackN() {
    delete[] buffer;
}

ProtocolResponse GoBackN::protocol(Event event, Frame_Base* frame) {
    ProtocolResponse response;

    switch (event)
    {
    case Event::NETWORK_LAYER_READY:
        // ready to send
        break;
    case Event::FRAME_ARRIVAL:
        //TODO: check on type of frame if Data, do receiver logic. If ACK or NAck do sender logic
        break;

    case Event::TIMEOUT:
        
        break;
    case Event::CHECKSUM_ERR:
        
        break;
        
    default:
        break;
    }

    return response;
}