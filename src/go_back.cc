#include "go_back.h"

GoBackN::GoBackN(int WS, NetworkParameters parameters, int node_id)
{
    MAX_SEQUENCE = WS; // as stated in the document, the maximum seq number is the window size.
    par = parameters;
    buffer = std::vector<Frame_Base>(WS + 1);

    network_layer = new NetworkLayer("../input/input" + std::to_string(node_id) + ".txt");
    logger = new Logger("../log/out.log");
    EV << "Go back N is initialized successfully from node number " << node_id << endl;
    if (DEBUG)
    {
        // deFraming("7amada");
        string payLoad = "Z$i/zo";
        Frame_Base *frame = new Frame_Base();
        string framed = framing(payLoad, frame);
        Byte trailer = (Byte)frame->getTrailer();
        EV << trailer.to_string() << endl
           << framed << endl;
        int len = framed.length();
        std::string unFramedPayload = "";
        for (int i = 1; i < len - 1; i++)
            unFramedPayload += framed[i];
        EV << validateCheckSum(unFramedPayload, frame) << endl;
    }
}

GoBackN::~GoBackN()
{
}


/*
    This function is responsible for applying binary addition as a utility for the checkSum
    After each byte wise addition, if there was any carry, we create a new byte of value 1, and apply the
    binary addition once again. (the algorithm work in this way).
    TESTED
*/
Byte GoBackN ::binaryAddition(std::deque<Byte> bytes)
{
    if (bytes.empty())
        return Byte(0);
    Byte result = bytes[0];
    EV << result.to_string() << endl;
    bytes.pop_front();
    if (bytes.empty())
        return result;

    for (auto byte : bytes)
    {
        EV << byte.to_string() << endl;
        int carry = 0;
        for (int i = 0; i < 8; i++)
        {
            // this is the full adder logic.
            int sum = result[i] + byte[i] + carry;
            carry = sum / 2;
            result[i] = sum % 2;
        }
        // this is the logic in the check sum, when there is a carry at the last bit.
        // we create new byte of weight = 1, and add its value to the result.
        EV << result.to_string() << "  is the result \n";
        if (carry)
        {
            Byte overFlowByte(1);
            result = binaryAddition({result, overFlowByte});
            EV << result.to_string() << "  is the result after adding carry\n";
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

    TESTED
*/
void GoBackN::createCheckSum(std::string Payload, Frame_Base *frame)
{
    // 1. convert all the characters into bytes.
    std::deque<Byte> bytes;
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
    TESTED
*/
bool GoBackN::validateCheckSum(std::string Payload, Frame_Base *frame)
{
    EV << "recieved string is : " << Payload << endl;
    // 1. get the checksum in form of byte
    Byte checkSum (frame->getTrailer()); 

    // 2. convert the payLoad into bytes, and append the checkSum with them
    std::deque<Byte> bytes;
    // bytes.push_back(checkSum);
    for (auto c : Payload)
        bytes.push_back(Byte(c));

    // 3. apply binary addition
    Byte summation = binaryAddition(bytes);

    // 4. the result should be all ones, so simply we can get the ones complement, and it must be 0
    Byte onesComplement = ~summation;

    // 5. if 0 return true, else return false.
    EV << checkSum.to_string() << endl << onesComplement.to_string() << endl; 
    return checkSum == onesComplement;
}

std::string deFraming(string framedPayload)
{
    int len = framedPayload.length();
    std::string unFramedPayload = "";
    for (int i = 1; i < len - 1; i++)
        unFramedPayload += framedPayload[i];
    return unFramedPayload;
}

/*
    This function applies the byte stuffing algorithm, by iterating over each character in the original payload
    then if the char was flag or esc, it append the esc before it.
    TESTED.
*/
std::string GoBackN ::applyByteStuffing(std::string Payload)
{
    std::string newPayload;
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
    TESTED
*/
std::string GoBackN ::framing(std::string Payload, Frame_Base *frame)
{
    std::string newPayload;

    // first we append the starting flag
    newPayload += FLAG;

    // then apply byte stuffing
    Payload = applyByteStuffing(Payload);
    newPayload += Payload;

    // append the checksum
    createCheckSum(Payload, frame);

    // finally we append the ending flag
    newPayload += FLAG;

    return newPayload;
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
        // Frame_Base *sentFrame = new Frame_Base();

        // 2. read the current msg to be sent.

        break;
    case Event::FRAME_ARRIVAL:
        // TODO: check on type of frame if Data, do receiver logic. If ACK or NAck do sender logic (DONE)
        if (frame)
        {                                   // this should always be true.
            if (frame->getFrameType() == (int)FrameType::DATA) // FrameType::DATA
            {
                // TODO: do the reciever logic.
                // 1. get the payload
                const char *payload = frame->getPayload();

                // 2. check that this is the seq num I am waiting for.
                //! hena el mfrod a3rf ezay baa?, bl header y3ny wla a?
                //! hena el mfrod a3ml variable, by-carry el sequence elly el mfrod ygele delw2ty, wlama as2tbl 7aga gdeda, el mfrod eny a-check hya
                //! el 7aga elly ana mestneha wla laa.

                // 3. check if there is an error or not, using the check sum
                bool valid = validateCheckSum(payload, frame);
                if (valid)
                {
                    // 4.1 send +ve Ack
                }
                else
                {
                    // 4.2 send -ve Ack
                }
            }
            else if (frame->getFrameType() == 1) // FrameType::ACK
            {
                // TODO: do the ack logic
                // we need to move the window one step forward
            }
            else
            {
                // TODO: do the NACK logic.
                // we need to stop any processing.
                // then retransmit from the errored packet.
            }
        }
        else
        {
            // EV << "How come the frame is empty? \n";
        }

        break;

    default:
        // case Event::TIMEOUT:
        // TODO: we need to start retransmitting all from the begining of the sliding window, and re-set all the timers.
        break;
        // case Event::CHECKSUM_ERR: // ezay bn3rf da mn bara, msh el mfrod el validation by7sl gowa el data?
        //     // TODO: we need to send NACK on this packet.
        //     break;
    }

    return response;
}
