#include "../NetComm.git/NC_Client.h"
#include "TM_Share.h"

#include <vector>
#include <queue>

//maybe this should be a class?
struct Transaction
{
    int abort_count = 0;
    std::string name;

    std::vector<TM_Share> shared_memory;

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
        
        int FindTransaction(std::string name);
        
    public:
         TM_Client();
         TM_Client(std::string host_address, unsigned int port);
        ~TM_Client();

        int Register_Transaction(void *(*transaction)(void *), std::string name);


        void*Execute_Transaction(int tran_id, void *arg);
        void* Execute_Transaction(std::string tran_name, void *arg);

        void Add_Shared_Memory(int t_id, TM_Share shared);
        void Add_Shared_Memory(int t_id, std::vector<TM_Share> shared);
        void Set_Shared_Memory(int t_id, std::vector<TM_Share> shared);

        void Add_Shared_Memory(std::string name, TM_Share shared);
        void Add_Shared_Memory(std::string name, std::vector<TM_Share> shared);
        void Set_Shared_Memory(std::string name, std::vector<TM_Share> shared);

};
