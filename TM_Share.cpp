#include "TM_Share.h"

using namespace std;

TM_Share::TM_Share(unsigned int mem_address, unsigned int mem_value)
{
    this->mem_address = mem_address;
    this->mem_value = mem_value;
    cout<<"Memory at address "<<this->mem_address<<" registered with value"<< this->mem_value<<endl;
}

TM_Share::TM_Share(unsigned int mem_address, unsigned int mem_value, queue<TM_Message> *messages_ref)
{
    this->mem_address = mem_address;
    this->mem_value = mem_value;
    cout<<"Memory at address "<<this->mem_address<<" registered with value"<< this->mem_value<<endl;

    Register_MessageQueue(messages_ref);
}

void TM_Share::Register_MessageQueue(queue<TM_Message> *messages_ref)
{
    this->messages = messages_ref;
    cout<<"Message Queue registered..."<<endl;
}


void TM_Share::TM_Read()
{
    //set up the message to notify HOST
    temp_message.code = READ;
    temp_message.data = (unsigned char *) &mem_address;
    temp_message.data_size = sizeof(mem_address);

    //push it back for the network thread
    messages->push(temp_message);
    cout<<"Read executed on address "<<this->mem_address<<endl;
}

void TM_Share::TM_Write()
{
    //setup the message for the host
    temp_message.code = WRITE;
    temp_message.data = (unsigned char *) &mem_address;
    temp_message.data_size = sizeof(mem_address);

    //push it back, oh yeah
    messages->push(temp_message);
    cout<<"Write executed on address "<<this->mem_address<<endl;
}

TM_Share & TM_Share::operator=(TM_Share &tm_source)
{
    this->mem_value = tm_source.mem_value;
    this->TM_Write();
    tm_source.TM_Read();
}
