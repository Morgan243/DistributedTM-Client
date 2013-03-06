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

    Register_MessageQueue(messages_ref);
//}}}
}


void TM_Share::Set_Auto_Sync(bool autoSync)
{
    this->auto_sync = autoSync;
}

void TM_Share::Register_MessageQueue(queue<TM_Message> *messages_ref)
{
    this->messages = messages_ref;
    cout<<"Message queue registered in shared memory at "<<mem_address<<"..."<<endl;
}

void TM_Share::Register_Network(NC_Client *net)
{
    TM_Share::network = net;
   cout<<"Network registered..."<<endl;
}


void TM_Share::TM_Read()
{
//{{{
    //set up the message to notify HOST
    temp_message.code = READ;
    temp_message.data = (unsigned char *) &mem_address;
    temp_message.data_size = sizeof(mem_address);

    //push it back for the network thread
    messages->push(temp_message);
    cout<<"Read executed on address "<<this->mem_address<<endl;
//}}}
}

void TM_Share::TM_Write()
{
//{{{
    //setup the message for the host
    temp_message.code = WRITE;
    temp_message.data = (unsigned char *) &mem_address;
    temp_message.data_size = sizeof(mem_address);

    //push it back, oh yeah
    messages->push(temp_message);
    cout<<"Write executed on address "<<this->mem_address<<endl;
//}}}
}

TM_Share & TM_Share::operator=(TM_Share &tm_source)
{
//{{{
    this->mem_value = tm_source.mem_value;
    this->TM_Write();
    tm_source.TM_Read();
//}}}
}

TM_Share & TM_Share::operator=(const int source)
{
    this->mem_value = (unsigned int) source;
    this->TM_Write();
}

TM_Share & TM_Share::operator=(const unsigned int source)
{
    this->mem_value = (unsigned int) source;
    this->TM_Write();
}

