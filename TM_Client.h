//#include "../NetComm.git/NC_Client.h"
//#include "TM_Share.h"
#include "Transaction.h"

#include <thread>
#include <vector>
#include <queue>


class TM_Client
{
    private:
        static bool done,                               //stop everything
                    auto_sync,                          //sync on every mem access or manually
                    name_announced,
                    abort_return;

        static std::string client_name;                 //friendly name for the client
        static std::string host_address;                //adress of TM server
        static unsigned int port;                       //port of TM server

        static NC_Client network;                       //Networking back end
        static std::thread net_thread;                  //thread for networking (use for no suto_sync)
        static std::queue<TM_Message> messages;         //queue of messages to send to TM server (no auto_sync)

        static std::vector<Transaction> transactions;   //loaded transactionc, id is transactions index
        static Transaction temp_transaction;            //Used for creating a transaction
        
        static void StartNetwork();                     //Launch a thread into this to handle messages (no auto_sync)
        static int FindTransaction(std::string name);   //find a transaction by it friendly name

    public:
        TM_Client();
        TM_Client(bool autoSync);
        TM_Client(bool autoSync, std::string host_address, unsigned int port);
        TM_Client(bool autoSync, std::string hostAddress, unsigned int prt, std::string clientName);
        TM_Client(bool autoSync, std::string hostAddress, unsigned int prt, std::string clientName, bool abort_return);
        ~TM_Client();

        static void setAbortReturn(bool abrt_ret);

        void Set_Client_Name(std::string clientName);
        void Announce_Client_Name();

        //load a function as a transaction, should be: 'void* my_func(void *args)'
        //returns the transaction id (vector index)
        static int Register_Transaction(void *(*transaction)(void *), std::string name);
        static int Register_Transaction(void *(*transaction)(void *), std::string name, int backoff_delta);

        //Run a transaction previously loaded
        static void* Execute_Transaction(int tran_id, void *arg);
        static void* Execute_Transaction(std::string tran_name, void *arg);

        //Added memory hosted on the TM server; t_id = transaction id to correlate it too
        static void Add_Shared_Memory(int t_id, TM_Share shared);
        static void Add_Shared_Memory(int t_id, std::vector<TM_Share> shared);
        static void Set_Shared_Memory(int t_id, std::vector<TM_Share> shared);

        //Added memory hosted on the TM server; name = transaction name to correlate it too
        static void Add_Shared_Memory(std::string name, TM_Share shared);
        static void Add_Shared_Memory(std::string name, std::vector<TM_Share> shared);
        static void Set_Shared_Memory(std::string name, std::vector<TM_Share> shared);

        //Get shared memory paired with a transaction, using the transaction id or name
        static std::vector<TM_Share> Get_Shared_Memory(int t_id);
        static std::vector<TM_Share> Get_Shared_Memory(std::string name);

        //Get the transaction object using transaction id or name
        static Transaction Get_Transaction(int t_id);
        static Transaction Get_Transaction(std::string name);

        static int GetTotalAborts();
        static int GetTotalCommits();
};
