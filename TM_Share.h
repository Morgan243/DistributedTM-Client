#include "../NetComm.git/NC_Client.h"
#include <mutex>
#include <iostream>
#include <iomanip>
#include <queue>
#include <stdio.h>
#include <stdlib.h>

//#include <osstream>

#ifndef TM_SHARE_H
#define TM_SHARE_H

//define TM encoding
#define READ 0x01
#define WRITE 0x02
#define COMMIT 0x04
#define ABORT 0x08
#define SYNC 0x10
#define MUTEX 0x20
#define INIT 0x40
#define CONTROL 0x80

enum StoreType {integer, float_real, double_real};

//Wrap the inside of a transaction function with this (see main for example)
#define BEGIN_T(name) try{ \
    Transaction TM = TM_Client::Get_Transaction(name);\
    TM.Init();

#define END_T TM.Commit(); \
    }catch(int error){throw error;}


//variables that need to be sent to the TM_Server
struct TM_Message
{
    unsigned char code;         //encoded request: read, write, commit attempt, or mutually exclusive access (standard lock)
    unsigned int address;       
    unsigned int value;
    float value_fl;
};


class TM_Share
{
    private:
        static StoreType store_type;
        char out_buffer[1024];                          //out bout messages sprintf() into this buffer
        std::string in_buffer;                          //inbound messages come into this string
        bool auto_sync;                                 //Synchronize on every mem access transparently?
        static NC_Client *network;                      //reference to the clients networking backend

        bool isWrite, isRead;                           //keep track of actions (is isRead needed?)

        unsigned int mem_address;                       //what TM address is represented
        unsigned int mem_value, new_value;              //whats the actual value/data of this address (should probably buffer)
        float mem_value_fl, new_value_fl;

        std::queue<TM_Message> *messages;               //reference to the clients queue of outgoing messages to the server
        TM_Message out_message, in_message;             //temporary message for building the vector


        static void SendMessage(char code, unsigned int address, unsigned int value);
        static void ReceiveMessage(char *code, unsigned int *address, unsigned int *value);

        static void SendMessage(char code, unsigned int address, float value);
        static void ReceiveMessage(char *code, unsigned int *address, float *value);

        void SendMessage(TM_Message message);           //parse and send a single message using the NC_Client 
        TM_Message ReceiveMessage();                    //receive data from server; continue, abort, commit success, etc.

    public:
        //lock for the vector reference of the vector queue (needed?)
        //static std::mutex queue_lock;

        TM_Share(unsigned int mem_address, unsigned int mem_value);
        TM_Share(unsigned int mem_address, unsigned int mem_value, std::queue<TM_Message> *messages_ref);

        TM_Share(unsigned int mem_address, float mem_value, bool float_type);
        TM_Share(unsigned int mem_address, float mem_value, std::queue<TM_Message> *messages_ref, bool float_type);

        void Set_Auto_Sync(bool autoSync);

        void Register_MessageQueue(std::queue<TM_Message> *messages_ref);
        static void Register_Network(NC_Client *net);

        unsigned int Get_Address();

        //attempt a commit
        static void Declare_Commit();

        //tell server that there is no more data coming, end commit phase
        static void End_Commit();

        void TM_Init();     //call when first entering a transaction (get fresh value for mem_value member)
        void TM_Read();     //Read occured, don't really need to do anything (maybe notify analysis server)
        void TM_Write();    //Write occured on the memory, notify server and abort if necessary
        void TM_Sync();     //Let the server know about the current state (read or write), abort if needed
        void TM_Commit();   //Try to commit the actual data values

        //---------------
        //overloads
        //---------------
        int toInt();
        float toFloat();
    
        //assigning one share to another (note right side is NOT constant)
        TM_Share & operator=(TM_Share &tm_source);
        TM_Share & operator+(TM_Share &tm_source);
        TM_Share & operator=(const int source);
        TM_Share & operator=(const unsigned int source);
        TM_Share & operator+(const int source);
        TM_Share & operator+(const unsigned int source);
        TM_Share & operator-(const int source);
        TM_Share & operator-(const unsigned int source);
        TM_Share & operator*(const int source);
        TM_Share & operator*(const unsigned int source);
        TM_Share & operator/(const int source);
        TM_Share & operator/(const unsigned int source);
        
        void setFloat(float val);
        TM_Share & operator=(const float source);
        TM_Share & operator+(const float source);
        TM_Share & operator-(const float source);
        TM_Share & operator*(const float source);
        TM_Share & operator/(const float source);

        friend std::ostream & operator<<(std::ostream &out, TM_Share &tm_share);
};
#endif
