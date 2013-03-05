//#include "../NetComm.git/NC_Client.h"
#include "TM_Share.h"

#include <thread>
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
        static bool done, auto_sync;
        static std::string host_address;// = "127.0.0.1";
        static unsigned int port;// = 1337;

        static NC_Client network;
        static std::thread net_thread;
        static std::queue<TM_Message> messages;

        static std::vector<Transaction> transactions;
        static Transaction temp_transaction;
        
        static void StartNetwork();
        static int FindTransaction(std::string name);
        
    public:
         TM_Client();
         TM_Client(bool autoSync);
         TM_Client(bool autoSync, std::string host_address, unsigned int port);
        ~TM_Client();

        static int Register_Transaction(void *(*transaction)(void *), std::string name);


        static void* Execute_Transaction(int tran_id, void *arg);
        static void* Execute_Transaction(std::string tran_name, void *arg);

        static void Add_Shared_Memory(int t_id, TM_Share shared);
        static void Add_Shared_Memory(int t_id, std::vector<TM_Share> shared);
        static void Set_Shared_Memory(int t_id, std::vector<TM_Share> shared);

        static void Add_Shared_Memory(std::string name, TM_Share shared);
        static void Add_Shared_Memory(std::string name, std::vector<TM_Share> shared);
        static void Set_Shared_Memory(std::string name, std::vector<TM_Share> shared);

        static std::vector<TM_Share> Get_Shared_Memory(int t_id);
        static std::vector<TM_Share> Get_Shared_Memory(std::string name);

};

