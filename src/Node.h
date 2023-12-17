#ifndef __NETWORKS_PROJECT_NODE_H_
#define __NETWORKS_PROJECT_NODE_H_

#include <omnetpp.h>
#include "defs.h"
#include "utils.h"
#include "go_back.h"

using namespace omnetpp;


class Node : public cSimpleModule
{
  int  node_id;

protected:
  virtual void initialize() override;
  virtual void handleMessage(cMessage* msg) override;
};

#endif
