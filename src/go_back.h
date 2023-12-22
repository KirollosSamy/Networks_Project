#ifndef GO_BACK_H
#define GO_BACK_H

#include "defs.h"
#include "frame_m.h"
#include "logger.h"
#include "network_layer.h"

class GoBackN {
    NetworkParameters par;

    int MAX_SEQUENCE;
    Frame_Base* buffer;

    NetworkLayer* network_layer;
    Logger* logger;

public:
    GoBackN(int WS, NetworkParameters parameters, int node_id);
    ~GoBackN();
    ProtocolResponse protocol(Event event, Frame_Base* frame = nullptr);
};

#endif