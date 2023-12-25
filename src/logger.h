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

struct LogData
{
    Time time;
    int node;

    SeqNum seq_num;
    std::string payload;
    FrameType frame_type;
    FrameErrorCode error_code;
    bool lost;

    Byte trailer;
    int modified;
    int duplicate;
    int delay;
};

class Logger
{
    //  the file in which we will log inside.
    std::ofstream logfile;
    std::string fileName;

public:
    Logger(std::string filename);

    bool log(LogType type, LogData data);

    // PROCESSING: error_code
    // SENDING: payload, seqnum, trailer, modified, duplicate, delay, lost
    // TIME_OUT: seqnum
    // CONTROL: frame_type, seqnum, lost
    // RECEIVING: payload, seqnum

    // LogData should contain the needed data for each type
    //    LogData lgData = {.time= 1, .delay}
    //    log(LogType::Sending,lgData)
};

#endif
