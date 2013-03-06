#include "../NetComm.git/NC_Client.h"
#include <mutex>
#include <iostream>
#include <queue>

#define READ 0x00
#define WRITE 0x01


//could we wrap an transaction in this, throw an exception in TM_Client
//Programmers transaction catches it and throws it to execute transaction
//effectively stopping the function call part way through
//#define BEGIN_T try{
//#define BEGIN_T(name) try{ vector<TM_Share> TM = TM_Client::Get_Shared_Memory(name); 

#define BEGIN_T(name) try{ Transaction TM = TM_Client::Get_Transaction(name); 
#define END_T }catch(int error){throw error;}

#ifndef TM_SHARE_H
#define TM_SHARE_H

struct TM_Message
{
    unsigned char code;
    unsigned char *data;
    unsigned int data_size;

};

class TM_Share
{
    private:
        static NC_Client *network;

        unsigned int mem_address;
        unsigned int mem_value;
        std::queue<TM_Message> *messages;

        TM_Message temp_message;

    public:
        static std::mutex queue_lock;

        TM_Share(unsigned int mem_address, unsigned int mem_value);
        TM_Share(unsigned int mem_address, unsigned int mem_value, std::queue<TM_Message> *messages_ref);

        void Register_MessageQueue(std::queue<TM_Message> *messages_ref);
        static void Register_Network(NC_Client *net);

        void TM_Read();
        void TM_Write();

        //---------------
        //overload writes
        //---------------
    
        //assigning one share to another (note right side is NOT constant)
        TM_Share & operator=(TM_Share &tm_source);
        TM_Share & operator=(const int source);
        TM_Share & operator=(const unsigned int source);
};

#endif
