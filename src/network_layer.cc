#include "network_layer.h"
#include <iostream>

NetworkLayer::NetworkLayer(std::string filename)
{
    file.open(filename);

    if (!file.is_open())
    {
        std::cerr << "Error opening the input file." << std::endl;
    }
}

// this function reads only one line, and if there is no more lines, it just return false.
/*
    this return the error code and the payload.
*/
bool NetworkLayer::getMsg(ErrorCode &error_code, std::string &payload)
{
    std::string error;
    file >> error >> std::ws;
    error_code = ErrorCode(error);

    std::getline(file, payload);

    if (file.eof())
        return false;
    return true;
}

// to use it -> While (getMsg) ...

NetworkLayer::~NetworkLayer()
{
    file.close();
}
