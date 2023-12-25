#include "go_back.h"

GoBackN::GoBackN()
{
    // empty default constructor.
}

GoBackN::GoBackN(int WS, NetworkParameters parameters, int node_id, cSimpleModule *node_ptr)
{
    MAX_SEQUENCE = WS; // as stated in the document, the maximum seq number is the window size.
    par = parameters;

    buffer = std::vector<Frame_Base *>(WS + 1);
    Timers = std::vector<cMessage *>(WS + 1);

    network_layer = new NetworkLayer("../input/input" + std::to_string(node_id) + ".txt");
    logger = new Logger("../log/out.log");

    // initially all of them are 0.
    frame_expected = 0;
    next_frame_to_send = 0;
    ack_expected = 0;
    num_outstanding_frames = 0;
    node = node_ptr;
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
Byte GoBackN::binaryAddition(std::deque<Byte> bytes)
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
char GoBackN::createCheckSum(std::string Payload)
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
    return (char)onesComplement.to_ulong();
}

/*
    This function is responsible for validating the message using the checksum
    TESTED
*/
bool GoBackN::validateCheckSum(Frame_Base *frame)
{
    std::string Payload = frame->getPayload();

    EV << "recieved string is : " << Payload << endl;
    // 1. get the checksum in form of byte
    Byte checkSum(frame->getTrailer());

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
    EV << checkSum.to_string() << endl
       << onesComplement.to_string() << endl;
    return checkSum == onesComplement;
}

/*
    Utility function used to remove the start and end flags.
    and remove the stuffed bytes.
*/
std::string GoBackN::deFraming(Frame_Base *recievedFrame)
{
    string recievedPayload = recievedFrame->getPayload();

    int len = recievedPayload.length();
    std::string unFramedPayload = "";

    // just to remove the start and the end flags.
    for (int i = 1; i < len - 1; i++)
        unFramedPayload += recievedPayload[i];

    string unStuffedPayload = removeByteStuffing(unFramedPayload);
    return unStuffedPayload;
}

/*
    This function applies the byte stuffing algorithm, by iterating over each character in the original payload
    then if the char was flag or esc, it append the esc before it.
    TESTED.
*/
std::string GoBackN::applyByteStuffing(std::string Payload)
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
    This is a utility function used to apply deframing at the reciever side.
    TESTED
*/
std::string GoBackN::removeByteStuffing(std::string payload)
{
    bool findEsc = false;
    std::string newPayload = "";

    for (char c : payload)
    {
        if (findEsc)
        {
            findEsc = false;
            newPayload += c;
            continue;
        }
        if (c == ESC)
        {
            findEsc = true;
            continue;
        }
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

Frame_Base *GoBackN::framing(std::string Payload)
{
    Frame_Base *frame = new Frame_Base();
    // we always send a data. 
    frame->setFrameType(FrameType::DATA);

    std::string newPayload;

    // first we append the starting flag
    newPayload += FLAG;

    // then apply byte stuffing
    Payload = applyByteStuffing(Payload);
    newPayload += Payload;

    // finally we append the ending flag
    newPayload += FLAG;

    // append the checksum
    char checkSum = createCheckSum(newPayload);

    // append the sequence number in the header.
    frame->setHeader(next_frame_to_send);

    // Append the checkSum in the trailer.
    frame->setTrailer(checkSum);

    return frame;
}

void GoBackN::startTimer(SeqNum frame_num)
{
    cMessage *timer_msg = new cMessage();
    timer_msg->setKind((short)MsgType::TIMEOUT);

    Timers[frame_num] = timer_msg;

    node->scheduleAt(simTime().dbl() + par.TO, timer_msg);
}

/*
    This function is us
*/
void GoBackN::stopTimer(SeqNum frame_num)
{
    // TODO: how the hell are we going to stop the time?
    cMessage *timer_msg = Timers[frame_num];

    // This is used to cancel the timer msg previously sent, thus canceling the timer
    node->cancelEvent(timer_msg);
    delete timer_msg;
}

void GoBackN::send(SeqNum frame_num, Time delay)
{
    Frame_Base *frame = buffer[frame_num];
    node->sendDelayed(frame, delay, "out");
    startTimer(frame_num);
}

/*
    utility function used to increment the value of the sequnce number,
    and get it back to 0 if it exceeded the maximum sequnce value.
*/
void GoBackN::increment(SeqNum &seq)
{
    seq = (seq + 1) % MAX_SEQUENCE;
}

bool GoBackN::protocol(Event event, Frame_Base *frame)
{
    bool more_frames = true;
    FrameErrorCode error_code;
    std::string payLoad = "";

    switch (event)
    {
    case Event::NETWORK_LAYER_READY:
        more_frames = network_layer->getMsg(error_code, payLoad);

        // TODO: Handle error code

        if (more_frames)
        {
            // 1. apply framing
            Frame_Base *newFrame = framing(payLoad);

            // we just need to send it
            Time delay = 0;
            //! we still need to apply the delay logic, depending on the error code.
            send(next_frame_to_send, delay);
            num_outstanding_frames++;
            increment(next_frame_to_send);
        }

        break;
    case Event::FRAME_ARRIVAL:

        /* Ack n implies n − 1, n − 2, etc. Check for this. */
        // while (between(ack expected, r.ack, next frame to send)) {
        //     /* Handle piggybacked ack. */
        //     nbuffered = nbuffered − 1; /* one frame fewer buffered */
        //     stop timer(ack expected); /* frame arrived intact; stop timer */
        //     inc(ack expected); /* contract sender’s window */
        // }

        if (frame->getFrameType() == (int)FrameType::DATA) // FrameType::DATA
        {
            // TODO: do the reciever logic.

            // 1. check that this is the seq num I am waiting for.
            //! hena el mfrod a3rf ezay baa?, bl header y3ny wla a?
            //! hena el mfrod a3ml variable, by-carry el sequence elly el mfrod ygele delw2ty, wlama as2tbl 7aga gdeda, el mfrod eny a-check hya
            //! el 7aga elly ana mestneha wla laa.
            int recieved_seq_num = frame->getHeader();
            if (recieved_seq_num == frame_expected)
            {
                // 3. check if there is an error or not, using the check sum
                bool valid = validateCheckSum(frame);
                if (valid)
                {
                    // 4.1 send +ve Ack
                    string received_payload = deFraming(frame);

                    // create ack frame
                    Frame_Base *ack_frame = new Frame_Base();
                    ack_frame->setAckNum(frame_expected + 1); // we ack the next correct frame, so it is the expected + 1
                    increment(frame_expected);
                    ack_frame->setFrameType(FrameType::ACK);
                    node->send(ack_frame, "out");

                }
                else
                {
                    // 4.2 send -ve Ack
                    Frame_Base* nack_frame = new Frame_Base();
                    nack_frame->setAckNum(frame_expected); // we ack the next correct frame, so it is the expected + 1
                    nack_frame->setFrameType(FrameType::NACK);
                    node->send(nack_frame, "out");
                }
            }
            else
            {
                // just keep silent.
                // NACK are not sent on out of order messages. 
            }
        }

        // Sender
        else if (frame->getFrameType() == (int)FrameType::ACK) // FrameType::ACK
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

        break;

    case Event::TIMEOUT:
        next_frame_to_send = ack_expected;
        for (int i = 1; i <= num_outstanding_frames; i++)
        {
            // TODO: calculate the delay required to send the given frame
            Time delay = 0;
            send(next_frame_to_send, delay);
            increment(next_frame_to_send);
        }
    }

    return true;
}
