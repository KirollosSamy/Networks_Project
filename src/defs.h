// definition of each variables, asamy el bytes w keda.

#ifndef DEFS_H
#define DEFS_H

#include <bitset>
#include <vector>

typedef float Time;
typedef unsigned int SeqNum;
typedef std::bitset<4> ErrorCode;
typedef std::bitset<8> Byte;
typedef std::vector<Byte> ByteStream;

enum class FrameType {
    NACK,
    ACK,
    DATA,
};

// Overloading the << operator of FrameType for logging purposes
std::ostream& operator<<(std::ostream& os, FrameType frame_type);

#endif
