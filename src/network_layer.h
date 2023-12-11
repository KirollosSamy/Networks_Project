#ifndef NETWORK_LAYER_H
#define NETWORK_LAYER_H

#include "defs.h"
#include "fstream"

// This class simulates the network layer as a simple text file
class NetworkLayer {
    std::ifstream file;
public:
    NetworkLayer(std::string filename);
    bool getMsg(ErrorCode& error_code, std::string& payload);
    ~NetworkLayer();
};

#endif