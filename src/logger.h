#ifndef LOGGER_H
#define LOGGER_H

#include "defs.h"
#include <fstream>

enum class LogType
{
    PROCESSING,
    SENDING,
    TIME_OUT,
    CONTROL,
    RECEIVING
};

struct LogData {
    Time time;
    int node;

    SeqNum seq_num;
    std::string payload;
    FrameType frame_type;
    ErrorCode error_code;
    bool lost;

    Byte trailer;
    int modified;
    int duplicate;
    int delay;
};

class Logger
{
    std::ofstream* logfile;

public:
    Logger(std::string filename);

    void log(LogType type, LogData data);

    // PROCESSING: error_code
    // SENDING: payload, seqnum, trailer, modified, duplicate, delay, lost 
    // TIME_OUT: seqnum
    // CONTROL: frame_type, seqnum, lost
    // RECEIVING: payload, seqnum

};

#endif
