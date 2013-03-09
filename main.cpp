#include <iostream>
#include <vector>
#include "TM_Client.h"

using namespace std;

void * test_transaction(void *args);

int main(int argc, char *argv[])
{
    string user_in;
    int t_id = 0;

    //ARGS: auto sync on, use loop back, port 1337
    TM_Client tm_client(true, "127.0.0.1", 1337);

    //store transaction for later execution, get id in return
    t_id = tm_client.Register_Transaction(test_transaction, "test");

    //create a shared memory element
    TM_Share test_mem(1, 99);

    //register shared memory for use in transaction
    tm_client.Add_Shared_Memory(t_id, test_mem);

    cout<<"Proceed?"<<endl;
    cin>>user_in;

    if(user_in == "y")

    //Execute the transaction, will repeat until no conflicts
    tm_client.Execute_Transaction(t_id, NULL);

    cout<<"Close out?"<<endl;
    cin>>user_in;


    return 0;
}


void * test_transaction(void *args)
{
    //use the transaction name to set things up
    BEGIN_T("test")
    
    TM.shared_memory[0] = 2;

    //TM.shared_memory[0]
    cout<<"hellow world"<<endl;


    END_T
}
