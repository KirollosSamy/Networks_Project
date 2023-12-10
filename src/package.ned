package networks_project;

@license(LGPL);
//
// TODO documentation
//
network Network
{
    @display("bgb=510,263");
    submodules:
        coordinator: Coordinator {
            @display("p=230,195");
        }
        Node0: Node {
            @display("p=72,83");
        }
        Node1: Node {
            @display("p=404,83");
        }
    connections:
        Node0.out --> Node1.in;
        Node1.out --> Node0.in;
        coordinator.out++ --> Node0.coordin;
        coordinator.out++ --> Node1.coordin;
}

