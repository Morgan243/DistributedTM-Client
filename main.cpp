#include <iostream>
#include "TM_Client.h"

using namespace std;

void * test_transaction(void *args);

int main(int argc, char *argv[])
{
    int t_id = 0;

    //auto sync, use loop back
    TM_Client tm_client(true, "127.0.0.1", 1337);

    t_id = tm_client.Register_Transaction(test_transaction, "test");

    tm_client.Execute_Transaction(t_id, NULL);

    return 0;
}


int count = -1;
void * test_transaction(void *args)
{
    BEGIN_T
    
    count++;

    cout<<"hellow world"<<endl;

    if(!count)
        throw 1;

   

    END_T
}
