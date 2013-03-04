#include "../NetComm.git/NC_Client.h"
#include "TM_Share.h"

#include <vector>
#include <queue>

struct Transaction
{
    int abort_count = 0;
    std::string name;

    //pointer to a function accepts a void pointer and returns a void pointer
    void *(*transaction)(void *) = NULL;
};


class TM_Client
{
    private:
        NC_Client network;
        std::queue<TM_Message> messages;

        std::vector<Transaction> transactions;
        Transaction temp_transaction;
        
        int FindTransaction(std::string);
    public:
         TM_Client();
         TM_Client(std::string host_address, unsigned int port);
        ~TM_Client();

        int Register_Transaction(void *(*transaction)(void *), std::string name);

        void*Execute_Transaction(int tran_id, void *arg);
        void* Execute_Transaction(std::string tran_name, void *arg);
};

