#include "TM_Client.h"

#define DEBUG 0

using namespace std;

    //------------------------------
    //Static Variables need defining
    //------------------------------
    //{{{
    bool TM_Client::done;
    bool TM_Client::auto_sync;
    bool TM_Client::name_announced;
    bool TM_Client::abort_return;

    string TM_Client::client_name;

    string TM_Client::host_address = "127.0.0.1";
    unsigned int TM_Client::port = 1337;

    NC_Client TM_Client::network;

    queue<TM_Message> TM_Client::messages;
    vector<Transaction> TM_Client::transactions;

    Transaction TM_Client::temp_transaction;
    //}}}
    //------------------------------


TM_Client::TM_Client()
{
//{{{
    TM_Client::name_announced = false;
    TM_Client::done = false;
    TM_Client::auto_sync = true;

    //use default host settings
    TM_Client::network.Init(false, this->host_address, this->port);

    //give TM_Share access to the network
    TM_Share::Register_Network(&network);
//}}}
}

TM_Client::TM_Client(bool autoSync)
{
//{{{
    TM_Client::name_announced = false;
    TM_Client::done = false;
    TM_Client::auto_sync = autoSync;
    TM_Client::abort_return = false;

    //use default host settings
    TM_Client::network.Init(false, TM_Client::host_address, TM_Client::port);

    //give TM_Share access to the network
    TM_Share::Register_Network(&network);
//}}}
}

TM_Client::TM_Client(bool autoSync, string hostAddress, unsigned int prt) 
{
//{{{
    TM_Client::name_announced = false;
    TM_Client::done = false;
    TM_Client::auto_sync = autoSync;
    TM_Client::abort_return = false;

    TM_Client::host_address = hostAddress;
    TM_Client::port = prt;

    //intialize network using user specified host settings
    TM_Client::network.Init(false, TM_Client::host_address, TM_Client::port);

    TM_Client::network.Connect();

    //give TM_Share access to the network
    TM_Share::Register_Network(&TM_Client::network);
//}}}
}

TM_Client::TM_Client(bool autoSync, string hostAddress, unsigned int prt, string clientName) 
{
//{{{
    TM_Client::name_announced = false;
    TM_Client::done = false;
    TM_Client::auto_sync = autoSync;
    TM_Client::abort_return = false;

    TM_Client::host_address = hostAddress;
    TM_Client::port = prt;

    //intialize network using user specified host settings
    TM_Client::network.Init(false, TM_Client::host_address, TM_Client::port);

    TM_Client::network.Connect();

    //give TM_Share access to the network
    TM_Share::Register_Network(&TM_Client::network);

    TM_Client::Set_Client_Name(clientName);

    TM_Client::Announce_Client_Name();
//}}}
}

TM_Client::TM_Client(bool autoSync, string hostAddress, unsigned int prt, string clientName, bool abort_return) 
{
//{{{
    TM_Client::name_announced = false;
    TM_Client::done = false;
    TM_Client::auto_sync = autoSync;
    TM_Client::abort_return = abort_return;

    TM_Client::host_address = hostAddress;
    TM_Client::port = prt;

    //intialize network using user specified host settings
    TM_Client::network.Init(false, TM_Client::host_address, TM_Client::port);

    TM_Client::network.Connect();

    //give TM_Share access to the network
    TM_Share::Register_Network(&TM_Client::network);

    TM_Client::Set_Client_Name(clientName);

    TM_Client::Announce_Client_Name();
//}}}
}

void TM_Client::Init(bool autoSync, std::string hostAddress, unsigned int prt, std::string clientName)
{
 //{{{
    TM_Client::name_announced = false;
    TM_Client::done = false;
    TM_Client::auto_sync = autoSync;

    TM_Client::host_address = hostAddress;
    TM_Client::port = prt;

    //intialize network using user specified host settings
    TM_Client::network.Init(false, TM_Client::host_address, TM_Client::port);

    TM_Client::network.Connect();

    //give TM_Share access to the network
    TM_Share::Register_Network(&TM_Client::network);

    TM_Client::Set_Client_Name(clientName);

    TM_Client::Announce_Client_Name();
//}}}  
}

TM_Client::~TM_Client()
{
//{{{
    //make sure everything stops (thread)
    TM_Client::done = true;
   
    #ifdef DEBUG
    cout<<"Sending SHUTDOWN to server..."<<endl;
    #endif
    
    TM_Client::network.Send("SHUTDOWN", 9);
//}}}
}

void TM_Client::setAbortReturn(bool abrt_ret)
{
    TM_Client::abort_return = abrt_ret;
}

void TM_Client::Set_Client_Name(string clientName)
{
    TM_Client::client_name = clientName;
}

void TM_Client::Announce_Client_Name()
{
//{{{
    if(!TM_Client::name_announced)
    {
        //send name to server
        TM_Client::network.Send(TM_Client::client_name.c_str(), TM_Client::client_name.length() + 1);
    
        TM_Client::name_announced = true;
    }
//}}}
}

int TM_Client::Register_Transaction(void *(*transaction)(void *), string name)
{
//{{{
    //use temp transaction to creatd the transaction for the vector
    temp_transaction.name = name;

    //make sure we aren't being f*cked with
    if(transaction != NULL)
    {
        //assign function pointer
        temp_transaction.transaction = transaction;

        //added to the BLOT (big list of transactions)
        transactions.push_back(temp_transaction);
        #ifdef DEBUG
        cout<<"Transaction "<<name<<" with id "<< transactions.size() - 1<<" registered..."<<endl;
        #endif
        //return the transaction id
        return transactions.size() - 1;
    }
    else
    {
        #ifdef DEBUG
        cout<<"Error registering transaction..."<<endl;
        #endif
        return -1; 
    }
//}}}
}

int TM_Client::Register_Transaction(void *(*transaction)(void *), string name, int backoff_delta)
{
//{{{
    //use temp transaction to creatd the transaction for the vector
    temp_transaction.name = name;

    //make sure we aren't being f*cked with
    if(transaction != NULL)
    {
        //assign function pointer
        temp_transaction.transaction = transaction;

        //added to the BLOT (big list of transactions)
        transactions.push_back(temp_transaction);
        #ifdef DEBUG
            cout<<"Transaction "<<name<<" with id "<< transactions.size() - 1<<" registered..."<<endl;
        #endif

        transactions.back().backoff_increment = backoff_delta;

        //return the transaction id
        return transactions.size() - 1;
    }
    else
    {
        #ifdef DEBUG
            cout<<"Error registering transaction..."<<endl;
        #endif
        return -1; 
    }
//}}}
}

void* TM_Client::Execute_Transaction(int tran_id, void *arg)
{
//{{{
    //local variable to keep loop going (should be move to transaction class)
    bool wasAborted = false;
    void * ret_val;

    do
    {
        //Need to (re)load shared memory values here
        try
        {
            //attempt the transaction
            ret_val =  transactions[tran_id].transaction(arg);

            wasAborted = false;
            transactions[tran_id].backoff_time = 0;
            transactions[tran_id].Increment_Commit_Count();

            return ret_val;
        }
        catch(int error) //catch a conflict exception
        {
            //need to try this again
            wasAborted = true;

            //count the aborts
            transactions[tran_id].Increment_Abort_Count();

            cout<<"Transaction aborted, total aborts: "<< transactions[tran_id].Get_Abort_Count()<<endl;;

            if(transactions[tran_id].backoff_increment > 0)
            {
                transactions[tran_id].backoff_time += transactions[tran_id].backoff_increment;
                usleep(transactions[tran_id].backoff_time);
            }

            if(abort_return)
                return NULL;
        }
    }
    while(wasAborted); //keep trying until it isn't aborted

//}}}
}

void* TM_Client::Execute_Transaction(string tran_name, void *arg)
{
//{{{
    int t_id = FindTransaction(tran_name);

    //make sure it will work
    if(t_id >= 0 && t_id < transactions.size())
        return Execute_Transaction(t_id, arg);
    else
    {
        #ifdef DEBUG
        cout<<"Invalide transaction selected: "<<tran_name<<endl;
        #endif
        return NULL;
    }
//}}}
}

int TM_Client::FindTransaction(string name)
{
//{{{
    //find and return the approriate index
    for( int i = 0; i < transactions.size(); i++)
    {
        if(transactions[i].name == name)
            return i;
    }

    #ifdef DEBUG
    //not found
    cout<<"Unable to find transaction named "<<name<<endl;
    #endif
    return -1;
//}}}
}

void TM_Client::Add_Shared_Memory(int t_id, TM_Share shared)
{
//{{{
    //error check index
    if(t_id >= 0 && t_id < transactions.size())
    {
        shared.Register_MessageQueue(&messages);
        shared.Register_Network(&TM_Client::network);
        transactions[t_id].shared_memory.push_back(shared);
    }
    #ifdef DEBUG
    else
    {
        cout<<"Transaction with id "<<t_id<<" does not exist..."<<endl;
    }
    #endif
//}}}
}

void TM_Client::Add_Shared_Memory(int t_id, vector<TM_Share> shared)
{
//{{{
    if(t_id >= 0 && t_id < transactions.size()){
        for(int i = 0; i < shared.size(); i++){
            //cout<<"i:"<<i<<", address:"<<shared[i].Get_Address();
            //shared[i].Register_MessageQueue(&messages);
            shared[i].Register_Network(&TM_Client::network);
            transactions[t_id].shared_memory.push_back(shared[i]);
        }
    }
//}}}
}

void TM_Client::Set_Shared_Memory(int t_id, vector<TM_Share> shared)
{}

void TM_Client::Add_Shared_Memory(string name, TM_Share shared)
{}

void TM_Client::Add_Shared_Memory(string name, vector<TM_Share> shared)
{}

void TM_Client::Set_Shared_Memory(string name, vector<TM_Share> shared)
{}

vector<TM_Share> TM_Client::Get_Shared_Memory(int t_id)
{
    return transactions[t_id].shared_memory;
}

vector<TM_Share> TM_Client::Get_Shared_Memory(string name)
{
//{{{
    int t_id = FindTransaction(name);

    return transactions[t_id].shared_memory;
//}}}
}

Transaction TM_Client::Get_Transaction(int t_id)
{
    return transactions[t_id];
}

Transaction TM_Client::Get_Transaction(string name)
{
//{{{
    int t_id = FindTransaction(name);

    return transactions[t_id];
//}}}
}

void TM_Client::StartNetwork()
{
//{{{
    TM_Message temp_message;
    int temp_size = 0;
    while(!done)
    {

        //TM_Share::queue_lock.lock();
            temp_size = messages.size();
        //TM_Share::queue_lock.unlock();

        if(temp_size)
        {
            //TM_Share::queue_lock.lock();
                temp_message = messages.front();
                messages.pop();
            //TM_Share::queue_lock.unlock();
            #ifdef DEBUG
            //debug statements
            cout<<"-New TM Message-"<<endl;
            cout<<"Code: "<<temp_message.code<<endl;
            cout<<"Adress: "<<temp_message.address<<endl;
            cout<<"Value: "<<temp_message.value<<endl;
            #endif
            //send data to host(not implemented)
        }
    }
//}}}
}

int TM_Client::GetTotalAborts()
{
    int total = 0;
    for(int i = 0; i < transactions.size(); i++)
    {
        total += transactions[i].Get_Abort_Count();
    }
    return total;
}

int TM_Client::GetTotalCommits()
{
    int total = 0;
    for(int i = 0; i < transactions.size(); i++)
    {
        total += transactions[i].Get_Commit_Count();
    }
    return total;
}
