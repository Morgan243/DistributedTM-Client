#include "TM_Client.h"

using namespace std;

    //------------------------------
    //Static Variables need defining
    //------------------------------
    //{{{
    bool TM_Client::done;
    bool TM_Client::auto_sync;

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
    TM_Client::done = false;
    TM_Client::auto_sync = autoSync;

    //use default host settings
    TM_Client::network.Init(false, TM_Client::host_address, TM_Client::port);

    //give TM_Share access to the network
    TM_Share::Register_Network(&network);
//}}}
}

TM_Client::TM_Client(bool autoSync, string hostAddress, unsigned int prt) 
{
//{{{
    TM_Client::done = false;
    TM_Client::auto_sync = autoSync;

    TM_Client::host_address = hostAddress;
    TM_Client::port = prt;

    //intialize network using user specified host settings
    TM_Client::network.Init(false, TM_Client::host_address, TM_Client::port);

    TM_Client::network.Connect();

    //give TM_Share access to the network
    TM_Share::Register_Network(&TM_Client::network);
//}}}
}

TM_Client::~TM_Client()
{
//{{{
    //make sure everything stops (thread)
    TM_Client::done = true;

    cout<<"Sending SHUTDOWN to server..."<<endl;
    TM_Client::network.Send("SHUTDOWN", 8);
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

        cout<<"Transaction "<<name<<" with id "<< transactions.size() - 1<<" registered..."<<endl;

        //return the transaction id
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
    //local variable to keep loop going (should be move to transaction class)
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

    //make sure it will work
    if(t_id >= 0 && t_id < transactions.size())
        return Execute_Transaction(t_id, arg);
    else
    {
        cout<<"Invalide transaction selected: "<<tran_name<<endl;
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

    //not found
    cout<<"Unable to find transaction named "<<name<<endl;
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
    else
        cout<<"Transaction with id "<<t_id<<" does not exist..."<<endl;
//}}}
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
            cout<<"Adress: "<<temp_message.address<<endl;
            cout<<"Value: "<<temp_message.value<<endl;

            //send data to host(not implemented)
        }
    }
//}}}
}
