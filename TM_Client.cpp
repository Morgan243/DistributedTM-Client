#include "TM_Client.h"

using namespace std;

TM_Client::TM_Client() : network()
{

}

TM_Client::TM_Client(string host_address, unsigned int port) 
               : network(host_address, port)
{

}

TM_Client::~TM_Client()
{

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
            transactions[tran_id].abort_count++;
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
    if(t_id > 0 && t_id < transactions.size())
        transactions[t_id].shared_memory.push_back(shared);
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
