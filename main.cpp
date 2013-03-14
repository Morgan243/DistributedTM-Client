#include <iostream>
#include <vector>
#include "TM_Client.h"

using namespace std;

struct inputArgs{
    string ipAddress;
    string coreName;
};

void * test_transaction(void *args);
void HandleInput(int argc, char *argv[], inputArgs &input);

int main(int argc, char *argv[])
{
    string user_in;
    int t_id = 0;
    
    inputArgs cmdInput;
    HandleInput(argc, argv, cmdInput);
    cout << cmdInput.coreName << endl;
    cout << cmdInput.ipAddress << endl;
    //ARGS: auto sync on, use loop back, port 1337
    TM_Client tm_client(true, "127.0.0.1", 1337, "IM_A_PI");

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

void HandleInput(int argc, char *argv[], inputArgs &input){
    for(int i = 0; i < argc; i++){
        if((strcmp(argv[i],"-ip") == 0))
            input.ipAddress = argv[i+1];
        if((strcmp(argv[i],"-n") == 0))
            input.coreName = argv[i+1];
    }
}

void * test_transaction(void *args)
{
    //use the transaction name to set things up
    BEGIN_T("test")
        cout<<">>MAIN: Attempting write on [0], [0] = "<<TM.shared_memory[0]<<endl;
        TM.shared_memory[0] = 2;
        cout<<"\t>>MAIN: Write on 0 completed, [0] = "<<TM.shared_memory[0]<<endl;


        cout<<"hellow world"<<endl;

    END_T
}
