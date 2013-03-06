#include "TM_Client.h"

using namespace std;

    bool TM_Client::done;
    bool TM_Client::auto_sync;

    string TM_Client::host_address = "127.0.0.1";
    unsigned int TM_Client::port = 1337;

    NC_Client TM_Client::network;

    queue<TM_Message> TM_Client::messages;
    vector<Transaction> TM_Client::transactions;

    Transaction TM_Client::temp_transaction;


TM_Client::TM_Client()// : network()
{
//{{{
    this->done = false;
    this->auto_sync = true;


    //use default host settings
    TM_Client::network.Init(false, this->host_address, this->port);


    TM_Share::Register_Network(&network);

    //launch network thread
    //net_thread = std::thread(&TM_Client::StartNetwork, this);
//}}}
}

TM_Client::TM_Client(bool autoSync) // : network()
{
//{{{
    this->done = false;
    this->auto_sync = autoSync;

    //use default host settings
    TM_Client::network.Init(false, this->host_address, this->port);

    TM_Share::Register_Network(&network);
    

    //launch network thread
    //net_thread = std::thread(&TM_Client::StartNetwork, this);
//}}}
}

TM_Client::TM_Client(bool autoSync, string hostAddress, unsigned int prt) 
              // : network(hostAddress, prt)
{
//{{{
    this->done = false;
    this->auto_sync = autoSync;

    this->host_address = hostAddress;
    this->port = prt;

    TM_Client::network.Init(false, this->host_address, this->port);

    TM_Share::Register_Network(&network);

    //launch network thread
    //net_thread = std::thread(&TM_Client::StartNetwork, this);
//}}}
}

TM_Client::~TM_Client()
{
    this->done = true;
    //net_thread.join();
}

int TM_Client::Register_Transaction(void *(*transaction)(void *), string name)
{
//{{{
    temp_transaction.name = name;

    if(transaction != NULL)
    {
        temp_transaction.transaction = transaction;

        transactions.push_back(temp_transaction);


        cout<<"Transaction "<<name<<" with id "<< transactions.size() - 1<<" registered..."<<endl;

        return transactions.size() - 1;
    }
    else
    {
        cout<<"Error registering transaction..."<<endl;
        return -1;
    }
//}}}
}

void* TM_Client::Execute_Transaction(int tran_id, void *arg)
{
//{{{
    bool wasAborted = false;

    do
    {
        //Need to (re)load shared memory values here
        try
        {
            //attempt the transaction
            return transactions[tran_id].transaction(arg);

            wasAborted = false;
        }
        catch(int error) //catch a conflict exception
        {
            //need to try this again
            wasAborted = true;

            //count the aborts
            transactions[tran_id].Increment_Abort_Count();
        }
    }
    while(wasAborted); //keep trying until it isn't aborted
//}}}
}

void* TM_Client::Execute_Transaction(string tran_name, void *arg)
{
//{{{
    int t_id = FindTransaction(tran_name);

    if(t_id >= 0)
        return Execute_Transaction(t_id, arg);
    else
        return NULL;
//}}}
}

int TM_Client::FindTransaction(string name)
{
//{{{
    for( int i = 0; i < transactions.size(); i++)
    {
        if(transactions[i].name == name)
            return i;
    }

    cout<<"Unable to find transaction named "<<name<<endl;

    //not found
    return -1;
//}}}
}

void TM_Client::Add_Shared_Memory(int t_id, TM_Share shared)
{
    if(t_id >= 0 && t_id < transactions.size())
    {
        shared.Register_MessageQueue(&messages);
        transactions[t_id].shared_memory.push_back(shared);
    }
    else
        cout<<"Transaction with id "<<t_id<<" does not exist..."<<endl;
}

void TM_Client::Add_Shared_Memory(int t_id, vector<TM_Share> shared)
{

}

void TM_Client::Set_Shared_Memory(int t_id, vector<TM_Share> shared)
{

}

void TM_Client::Add_Shared_Memory(string name, TM_Share shared)
{

}

void TM_Client::Add_Shared_Memory(string name, vector<TM_Share> shared)
{

}

void TM_Client::Set_Shared_Memory(string name, vector<TM_Share> shared)
{

}

vector<TM_Share> TM_Client::Get_Shared_Memory(int t_id)
{
    return transactions[t_id].shared_memory;
}

vector<TM_Share> TM_Client::Get_Shared_Memory(string name)
{
    int t_id = FindTransaction(name);

    return transactions[t_id].shared_memory;
}

Transaction TM_Client::Get_Transaction(int t_id)
{
    return transactions[t_id];
}

Transaction TM_Client::Get_Transaction(string name)
{
    int t_id = FindTransaction(name);

    return transactions[t_id];
}

void TM_Client::StartNetwork()
{
//{{{
    TM_Message temp_message;
    int temp_size = 0;
    while(!done)
    {

        TM_Share::queue_lock.lock();
            temp_size = messages.size();
        TM_Share::queue_lock.unlock();

        if(temp_size)
        {
            TM_Share::queue_lock.lock();
                temp_message = messages.front();
                messages.pop();
            TM_Share::queue_lock.unlock();

            //debug statements
            cout<<"-New TM Message-"<<endl;
            cout<<"Code: "<<temp_message.code<<endl;
            cout<<"Data: "<<temp_message.data<<endl;
            cout<<"Data size: "<<temp_message.data_size<<endl;

            //send data to host
        }
    }
//}}}
}
