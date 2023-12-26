#ifndef GO_BACK_H
#define GO_BACK_H

#include "defs.h"
#include "frame_m.h"
#include "logger.h"
#include "network_layer.h"
#include <deque>
#include <omnetpp.h>
#include "utils.h"

using namespace std;
using namespace omnetpp;

class GoBackN
{
    NetworkParameters par;
    int node_id;

    int MAX_SEQUENCE;
    vector<Frame_Base *> buffer;
    vector<cMessage *> timers;
    vector<FrameErrorCode> error_codes;

    NetworkLayer *network_layer;
    Logger *logger;

    // This variable represents the receiver window (of size 1)
    SeqNum frame_expected; // The sequence number expected by the receiver

    // The sender window is between [ack_expected, next_frame_to_send]
    SeqNum next_frame_to_send;  // The sequence number of the frame to be send
    SeqNum ack_expected;        // The sequence number of the first unacknowledged frame
    int num_outstanding_frames; // The number of frames currently in buffer (must be <= MAX_SEQUENCE)

    cSimpleModule *node;

public:
    GoBackN(int WS, NetworkParameters parameters, int node_id, cSimpleModule *node_ptr);
    GoBackN();
    ~GoBackN();
    // ProtocolResponse protocol(Event event, Frame_Base *frame = nullptr);
    bool protocol(Event event, Frame_Base *frame = nullptr);

    // Abdelazizs' functions
private:
    Frame_Base* framing(std::string Payload);
    std::string deFraming(Frame_Base* framedPayload);
    std::string applyByteStuffing(std::string Payload);
    std::string removeByteStuffing(std::string Payload);
    char createCheckSum(std::string Payload);
    Byte binaryAddition(std::deque<Byte> bytes);
    bool validateCheckSum(Frame_Base *frame);

    void increment(SeqNum &seq);
    void startTimer(SeqNum frame_num);
    void stopTimer(SeqNum frame_num);
    void send(SeqNum frame_num, Time frame_delay, bool error = true);
};

#endif