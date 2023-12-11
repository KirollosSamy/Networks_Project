#include "logger.h"
#include <iomanip>
#include <iostream>

Logger::Logger(std::string filename) {
    logfile.open(filename);

    if (!logfile.is_open()) {
        std::cerr << "Error opening the log file." << std::endl;
    }
}

void Logger::log(LogType type, LogData data) {
    switch (type) {

    case LogType::PROCESSING:
        logfile << "At time [" << data.time << "], Node[" << data.node << "] , "
            << "Introducing channel error with code =[" << data.error_code << " ] ." << std::endl;
        break;

    case LogType::SENDING:
        logfile << "At time [" << data.time << "], Node[" << data.node << "] , "
            << "[sent] frame with seq_num=[" << data.seq_num << "] and payload=[" << data.payload << "] and trailer=["
            << data.trailer << "] , " << "Modified [" << data.modified << "] , Lost [" << std::boolalpha << data.lost
            << "], Duplicate [" << data.duplicate << "], Delay [" << data.delay << "]." << std::endl;
        break;

    case LogType::TIME_OUT:
        logfile << "Time out event at time [" << data.time << "], Node[" << data.node << "] , "
            << "for frame with seq_num=[" << data.seq_num << " ] ." << std::endl;
        break;

    case LogType::CONTROL:
        logfile << "At time [" << data.time << "], Node[" << data.node << "] , " << "Sending["
            << data.frame_type << "] with number [" << data.seq_num << "] , loss ["
            << std::boolalpha << data.lost << "]." << std::endl;
        break;

    case LogType::RECEIVING:
        logfile << "At time [" << data.time << "], Node[" << data.node << "] , " << "Uploading payload=["
            << data.payload << "] and seq_num=[" << data.seq_num << "] to the network layer." << std::endl;
        break;
    }
}
