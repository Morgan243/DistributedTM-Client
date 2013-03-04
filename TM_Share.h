#include <queue>

#define READ 0x00
#define WRITE 0x01


//could we wrap an transaction in this, throw an exception in TM_Client
//Programmers transaction catches it and throws it to execute transaction
//effectively stopping the function call part way through
#define BEGIN_T try{
#define END_T }catch(int error){throw error}

struct TM_Message
{
    unsigned char code;
    unsigned char *data;
    unsigned int data_size;

};

class TM_Share
{
    private:
        unsigned int mem_address;
        unsigned int mem_value;
        std::queue<TM_Message> *messages;

        TM_Message temp_message;

    public:
        TM_Share(unsigned int mem_address, unsigned int mem_value);
        TM_Share(unsigned int mem_address, unsigned int mem_value, std::queue<TM_Message> *messages_ref);

        void Register_MessageQueue(std::queue<TM_Message> *messages_ref);

        void TM_Read();
        void TM_Write();

        //---------------
        //overload writes
        //---------------
    
        //assigning one share to another (note right side is NOT constant)
        TM_Share & operator=(TM_Share &tm_source);
};

