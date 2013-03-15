#include <iostream>
#include <vector>
#include "TM_Client.h"

#define MEM_SIZE 10

using namespace std;

//cli argument container
struct inputArgs{
    string ipAddress;
    string coreName;
    string transaction;
};

//transaction function
void * test_transaction(void *args);
void * incrementer(void *args);

//parse command line arguments
bool HandleInput(int argc, char *argv[], inputArgs &input);

int main(int argc, char *argv[])
{
//{{{
    int t_id = 0;
    string user_in;

    vector<TM_Share> shared_memory;

    //create a shared memory element
    TM_Share test_mem(1, 99);

    inputArgs cmdInput;
        cmdInput.ipAddress = "127.0.0.1";       //default to loopback
        cmdInput.coreName = "Default Name";     //default core name (could use time or rand)
        cmdInput.transaction = "test";          //use simple test by default

    
    if(HandleInput(argc, argv, cmdInput))
        return 0;

    //ARGS: auto sync on, address, port 1337
    TM_Client tm_client(true, cmdInput.ipAddress, 1337,cmdInput.coreName );


    if(cmdInput.transaction == "test")
    {
        //store transaction for later execution, get id in return
        t_id = tm_client.Register_Transaction(test_transaction, "test");

        //init memory
        shared_memory.push_back(TM_Share(0,99));
    }
    else if(cmdInput.transaction == "increment")
    {
        t_id = tm_client.Register_Transaction(incrementer, "increment");
        
        //init memory
        for(int i = 0; i < MEM_SIZE; i++)
            shared_memory.push_back(TM_Share(0,99));
    }

    //register shared memory for use in transaction
    tm_client.Add_Shared_Memory(t_id, test_mem);

    cout<<"Proceed?"<<endl;
    cin>>user_in;

    if(user_in == "y")
    {
        //Execute the transaction, will repeat until no conflicts
        tm_client.Execute_Transaction(t_id, NULL);
    }

    cout<<"Enter anything to terminate..."<<endl;
    cin>>user_in;

    return 0;
//}}}
}

void * test_transaction(void *args)
{
//{{{
    //use the transaction's name to set things up
    BEGIN_T("test")
        cout<<">>TRANSACTION: Attempting write on [0], [0] = "<<TM.shared_memory[0]<<endl;
        TM.shared_memory[0] = 2;
        cout<<"\t>>TRANSACTION: Write completed on [0], [0] = "<<TM.shared_memory[0]<<endl;
    END_T
//}}}
}

void * incrementer(void *args)
{

}

bool HandleInput(int argc, char *argv[], inputArgs &input){
//{{{
    for(int i = 0; i < argc; i++){
        if((strcmp(argv[i],"-h") == 0))
            {
                cout<<"Usage:"<<endl;
                cout<<"-ip\t\t Set ip address of TM server"<<endl;
                cout<<"-n\t\t Set the name of this client"<<endl;
                cout<<"-tn\t\t Transaction name to wrong"<<endl;
                cout<<"\t\t[\"test\",\"increment\"]"<<endl;
                cout<<"-h\t\t Print help (this message)"<<endl;

                return true;
            }

        if((strcmp(argv[i],"-tn") == 0))
            input.transaction = argv[i+1];
        if((strcmp(argv[i],"-ip") == 0))
            input.ipAddress = argv[i+1];
        if((strcmp(argv[i],"-n") == 0))
            input.coreName = argv[i+1];
    }
//}}}
}
