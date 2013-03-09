#include "TM_Share.h"

using namespace std;

    //------------------------
    //Static Var Define
    //------------------------
    NC_Client *TM_Share::network;
    mutex TM_Share:: queue_lock;
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
    char temp_data[1024];

    //clear data buffer
    bzero(temp_data, sizeof(temp_data));

    //get string version of data
    int size = sprintf(temp_data,"%c:%u:%u", message.code, message.data, message.data_size);

    cout<<"Message to send: "<<temp_data<<endl;

    TM_Share::network->Send(temp_data, size);
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
    temp_message.code = READ;
    temp_message.data = mem_address;
    temp_message.data_size = sizeof(mem_address);

    //push it back for the network thread
    messages->push(temp_message);
    cout<<"Read executed on address "<<this->mem_address<<endl;
//}}}
}

void TM_Share::TM_Write()
{
//{{{
    //setup the message for the TM server
    temp_message.code = WRITE;
    temp_message.data = mem_address;
    temp_message.data_size = sizeof(mem_address);

    //push it back, oh yeah
    messages->push(temp_message);
    cout<<"Write executed on address "<<this->mem_address<<endl;
    cout<<"\tAlerting TM server..."<<endl;

    TM_Share::SendMessage(temp_message);
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

