#include "network_layer.h"
#include <iostream>

NetworkLayer::NetworkLayer(std::string filename) {
    file.open(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening the file." << std::endl;
    }
}

bool NetworkLayer::getMsg(ErrorCode& error_code, std::string& payload) {
    std::string error;
    file >> error >> std::ws;
    error_code = ErrorCode(error);

    std::getline(file, payload);

    if(file.eof()) return false;
    return true;
}

NetworkLayer::~NetworkLayer() {
    file.close();
}