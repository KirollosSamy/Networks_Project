#ifndef GO_BACK_H
#define GO_BACK_H

#include "defs.h"
#include "frame_m.h"
#include "logger.h"
#include "network_layer.h"
#include "vector"
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;
class GoBackN
{
    NetworkParameters par;

    int MAX_SEQUENCE;
    vector<Frame_Base> buffer;

    NetworkLayer *network_layer;
    Logger *logger;
    bool DEBUG = true; 
public:
    GoBackN(int WS, NetworkParameters parameters, int node_id);
    ~GoBackN();
    ProtocolResponse protocol(Event event, Frame_Base *frame = nullptr);

    // Abdelazizs' functions
private:
    bool thereIsAnError(std::vector<std::bitset<8>> &wordBytes, std::bitset<8> parityByte);
    std::string framing(std::string Payload, Frame_Base *frame);
    std::string applyByteStuffing(std::string Payload);
    void createCheckSum(std::string Payload, Frame_Base *frame);
    Byte binaryAddition(std::vector<Byte> bytes);
    bool validateCheckSum(std::string Payload, Frame_Base *frame);
};

#endif