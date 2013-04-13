#include "Transaction.h"

#define DEBUG 0

Transaction::Transaction()
{}

Transaction::~Transaction()
{}

int Transaction::Get_Abort_Count()
{
    return this->abort_count;
}

void Transaction::Increment_Abort_Count()
{
    this->abort_count += 1;
}


void Transaction::Reset_Abort_Count()
{
    this->abort_count = 0;
}

void Transaction::SetTransaction(void *(*transaction)(void *), std::string name)
{
//{{{
    this->transaction = transaction;
    this->name = name;
//}}}
}

void Transaction::Sync()
{
//{{{
    try
    {
        for(int i = 0; i < this->shared_memory.size(); i++)
        {
            shared_memory[i].TM_Sync();
        }
    }
    catch(int error)
    {
        throw error;
    }
//}}}
}

void Transaction::Init()
{
//{{{
//    try
//    {
//        for(int i = 0; i < shared_memory.size(); i++)
//        {
//           shared_memory[i].TM_Init(); 
//        }
//    }
//    catch(int error)
//    {
//        throw error;
//    }
//}}}
}

void Transaction::Commit()
{
//{{{
    try
    {
        //first, announce commit
        //make sure sommit will go through before sending data
        TM_Share::Declare_Commit();

        //send the data
        for(int i = 0; i < shared_memory.size(); i++)
        {
           shared_memory[i].TM_Commit(); 
        }

        //tel the server that there is no more data coming, flush the sets
        TM_Share::End_Commit();
    }
    catch(int error)
    {
        throw error;
    }
//}}}
}
