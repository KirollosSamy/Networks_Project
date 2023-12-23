#include "go_back.h"

GoBackN::GoBackN(int WS, NetworkParameters parameters, int node_id)
{
    MAX_SEQUENCE = WS; // as stated in the document, the maximum seq number is the window size.
    par = parameters;
    buffer = new Frame_Base[MAX_SEQUENCE + 1];

    network_layer = new NetworkLayer("../input/input" + std::to_string(node_id) + ".txt");
    logger = new Logger("../log/out.log");
}

GoBackN::~GoBackN()
{
    delete[] buffer;
}

/*
    This function applies the byte stuffing algorithm, by iterating over each character in the original payload
    then if the char was flag or esc, it append the esc before it.
*/
omnetpp::opp_string GoBackN ::applyByteStuffing(omnetpp::opp_string Payload)
{
    omnetpp::opp_string newPayload;
    for (auto c : Payload)
    {
        // append the ESC before any Flag or ESC
        if (c == FLAG || c == ESC)
            newPayload += ESC;
        // always append the character to the newPayload.
        newPayload += c;
    }
    return newPayload;
}

/*
    This function is responsible for applying the framing on the payload only
    this is done by appending the flag at the begining, then append the payload after applying the byte stuffing
    Then at the end append the trailing flag.
*/
omnetpp::opp_string GoBackN ::framing(omnetpp::opp_string Payload)
{
    omnetpp::opp_string newPayload;

    // first we append the starting flag
    newPayload += FLAG;
    // then apply byte stuffing
    newPayload += applyByteStuffing(Payload);
    // finally we append the ending flag
    newPayload += FLAG;

    return newPayload;
}

/*
    This function takes the payload, and apply on it the checksum logic, and
*/
bool GoBackN::thereIsAnError(std::vector<std::bitset<8>> &wordBytes, std::bitset<8> parityByte)
{
    for (auto byte : wordBytes)
        parityByte = parityByte ^ byte;

    return parityByte.to_ulong() != 0;
}

/*
    This function is responsible for applying binary addition as a utility for the checkSum
*/
Byte GoBackN ::binaryAddition(std::vector<Byte> bytes)
{
    if (bytes.empty())
        return Byte(0);
    Byte result = bytes[bytes.size() - 1];
    bytes.pop_back();
    if (bytes.empty)
        return result;
    for (auto byte : bytes)
    {
        int carry = 0;
        for (int i = 7; i >= 0; i--)
        {
            // this is the full adder logic.
            int sum = result[i] ^ byte[i] ^ carry;
            carry = (result[i] && byte[i]) || (sum && carry);
            result[i] = sum;
        }
        // this is the logic in the check sum, when there is a carry at the last bit.
        // we create new byte of weight = 1, and add its value to the result.
        if (carry)
        {
            Byte overFlowByte(1);
            for (int i = 7; i >= 0; i--)
            {
                // this is the full adder logic.
                int sum = result[i] ^ overFlowByte[i] ^ carry;
                carry = (result[i] && overFlowByte[i]) || (sum && carry);
                result[i] = sum;
            }
        }
    }
    return result;
}

/*
    This function is responsible for creating the check sum on the byte stuffed payload.
    this is done by dividing the message into k blocks each of size one char.
    then applying binary addition.
    if there were any carry, we should add it to the result.
    then after the binary addition, we should apply 1's complement.
    then append the checksum at the trailer of the frame.

    ? NOTE: i am sending the frame pointer by reference to save the appended check sum.
*/
void GoBackN::createCheckSum(omnetpp::opp_string Payload, Frame_Base &*frame)
{
    // 1. convert all the characters into bytes.
    std::vector<Byte> bytes;
    for (auto c : Payload)
        bytes.push_back(Byte(c));

    // 2. apply binary addition on them
    //! we need to be careful for the carry.
    Byte summation = binaryAddition(bytes);

    // 3. apply the one's complement
    Byte onesComplement = ~summation;

    // 4. convert the checkSum into char.
    char checkSum = (char)onesComplement.to_ulong();

    // 5. append the checkSum in the trailer.
    frame->setTrailer(checkSum);
}

/*
    This function is responsible for validating the message using the checksum
*/
bool GoBackN::validateCheckSum(omnetpp::opp_string Payload, Frame_Base *frame)
{
    // 1. get the checksum in form of byte
    Byte checkSum(frame->getTrailer());

    // 2. convert the payLoad into bytes, and append the checkSum with them
    std::vector<Byte> bytes;
    bytes.push_back(checkSum);
    for (auto c : Payload)
        bytes.push_back(Byte(c));

    // 3. apply binary addition
    Byte summation = binaryAddition(bytes);

    // 4. the result should be all ones, so simply we can get the ones complement, and it must be 0
    Byte onesComplement = ~summation;

    // 5. if 0 return true, else return false.
    return !onesComplement.to_ullong();
}

ProtocolResponse GoBackN::protocol(Event event, Frame_Base *frame)
{
    ProtocolResponse response;

    switch (event)
    {
    case Event::NETWORK_LAYER_READY:
        // ready to send
        // TODO: just send the next packet.
        // 1. create a new frame, then send it.
        break;
    case Event::FRAME_ARRIVAL:
        // TODO: check on type of frame if Data, do receiver logic. If ACK or NAck do sender logic (DONE)
        if (frame)
        { // this should always be true.
            if (frame->getFrameType() == FrameType::DATA)
            {
                // TODO: do the reciever logic.
                const char *payload = frame->getPayload();
            }
            else if (frame->getFrameType() == FrameType::ACK)
            {
                // TODO: do the ack logic
            }
            else
            {
                // TODO: do the NACK logic.
            }
        }
        else
        {
            EV << "How come the frame is empty? \n";
        }

        break;

    case Event::TIMEOUT:
        // TODO: we need to start retransmitting all from the begining of the sliding window, and re-set all the timers.
        break;
    case Event::CHECKSUM_ERR:
        // TODO: we need to send NACK on this packet.
        break;

    default:
        break;
    }

    return response;
}