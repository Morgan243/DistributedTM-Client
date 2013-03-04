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
    return transactions[tran_id].transaction(arg);
}

void* TM_Client::Execute_Transaction(string tran_name, void *arg)
{

}
