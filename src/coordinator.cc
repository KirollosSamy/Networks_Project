#include "coordinator.h"
#include <fstream>

Define_Module(Coordinator);

void Coordinator::initialize()
{
    std::ifstream file("../input/coordinator.txt");

    if (!file.is_open()) {
        std::cerr << "Error opening the coordinator file." << std::endl;
    }

    int starting_node;
    std::string starting_time;
    file >> starting_node >> starting_time;

    cMessage* start_msg = new cMessage(starting_time.c_str());
    send(start_msg, "out", starting_node);
}

void Coordinator::handleMessage(cMessage* msg) {}
