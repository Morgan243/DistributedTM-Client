#include "TM_Share.h"

using namespace std;

    //------------------------
    //Static Var Define
    //------------------------
    NC_Client *TM_Share::network;
    //mutex TM_Share:: queue_lock;
    //------------------------

TM_Share::TM_Share(unsigned int mem_address, unsigned int mem_value)
{
//{{{
    this->auto_sync = true;

    this->mem_address = mem_address;
    this->mem_value = mem_value;

    cout<<"Memory at address "<<this->mem_address<<" registered with value "<< this->mem_value<<endl;
//}}}
}

TM_Share::TM_Share(unsigned int mem_address, unsigned int mem_value, queue<TM_Message> *messages_ref)
{
//{{{
    this->auto_sync = true;

    this->mem_address = mem_address;
    this->mem_value = mem_value;

    cout<<"Memory at address "<<this->mem_address<<" registered with value "<< this->mem_value<<endl;

    //store pointer to the message queue
    Register_MessageQueue(messages_ref);
//}}}
}

void TM_Share::SendMessage(TM_Message message)
{
//{{{
    //clear data buffer
    bzero(this->out_buffer, sizeof(this->out_buffer));

    //get string version of data
    int size = sprintf(this->out_buffer, "%c:%u:%u", message.code, message.address, message.value);

    cout<<"Message to send: "<<this->out_buffer<<endl;

    TM_Share::network->Send(this->out_buffer, size);
//}}}
}

TM_Message TM_Share::ReceiveMessage()
{
//{{{
    unsigned pos1, pos2;
    in_buffer.clear();

    //get a message from the server
    TM_Share::network->Receive(&in_buffer,1024);

    //find first colon
    pos1 = in_buffer.find_first_of(":"); 

    //get character before first colon as code
    in_message.code = in_buffer[pos1 - 1];

    //replace colon so we can find the next one
    in_buffer[pos1] = '-';

    //find second colon
    pos2 = in_buffer.find_first_of(":");

    //get the address string and convert it to an integer
    in_message.address = (unsigned int)atoi(in_buffer.substr(pos1+1, pos2-1).c_str());

    //get the value string and convert it to an integer
    in_message.value = (unsigned int)atoi(in_buffer.substr(pos2+1, in_buffer.length() - 1).c_str());

    //display values
    cout<<hex<<"\tcode: "<<(unsigned int)in_message.code<<endl;
    cout<<hex<<"\taddr: "<<in_message.address<<endl;
    cout<<hex<<"\tvalue: "<<in_message.value<<endl;
//}}}
}

void TM_Share::Set_Auto_Sync(bool autoSync)
{
    this->auto_sync = autoSync;
}

void TM_Share::Register_MessageQueue(queue<TM_Message> *messages_ref)
{
//{{{
    this->messages = messages_ref;
    cout<<"Message queue registered in shared memory at "<<mem_address<<"..."<<endl;
//}}}
}

void TM_Share::Register_Network(NC_Client *net)
{
//{{{
   TM_Share::network = net;
   cout<<"Network registered..."<<endl;
//}}}
}

void TM_Share::TM_Read()
{
//{{{
    //set up the message to notify TM server
    out_message.code = READ;
    out_message.address = mem_address;
    out_message.value = sizeof(mem_address);

    //push it back for the network thread
    messages->push(out_message);
    cout<<"Read executed on address "<<this->mem_address<<endl;
//}}}
}

void TM_Share::TM_Write()
{
//{{{
    //setup the message for the TM server
    out_message.code = WRITE;                      //a write was made...
    out_message.address = mem_address;                //to this memory address...
    out_message.value = sizeof(mem_address);   //get size

    //push it back, oh yeah
    messages->push(out_message);
    cout<<"Write executed on address "<<this->mem_address<<endl;
    cout<<"\tAlerting TM server..."<<endl;

    TM_Share::SendMessage(out_message);

    cout<<"\tAwaiting approval from server..."<<endl;
    
    TM_Share::ReceiveMessage();
//}}}
}

//OVERLOAD: TM_Share = TM_Share
TM_Share & TM_Share::operator=(TM_Share &tm_source)
{
//{{{
    this->mem_value = tm_source.mem_value;

    //notify TM server of write
    this->TM_Write();

    //notify TM server of read
    tm_source.TM_Read();
//}}}
}

//OVERLOAD: TM_Share = int
TM_Share & TM_Share::operator=(const int source)
{
//{{{
    this->mem_value = (unsigned int) source;

    //notofy TM server of write
    this->TM_Write();
//}}}
}

//OVERLOAD: TM_Share = unsigned int
TM_Share & TM_Share::operator=(const unsigned int source)
{
//{{{
    this->mem_value = (unsigned int) source;

    //notify TM server of write
    this->TM_Write();
//}}}
}
