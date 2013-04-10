#include <iostream>
#include <vector>
#include "TM_Client.h"

#define MEM_SIZE 4

using namespace std;

//cli argument container
struct inputArgs{
    string ipAddress;
    string coreName;
    string transaction;
    int memSize;
    int sleepTime;
};

//transaction function
void * test_transaction(void *args);
void * incrementer(void *args);
void * stall(void *args);

//parse command line arguments
bool HandleInput(int argc, char *argv[], inputArgs &input);

int main(int argc, char *argv[])
{
//{{{
    int t_id = 0;
    unsigned int index = 0;
    string user_in;

    void * args = NULL;
    vector<TM_Share> shared_memory;

    inputArgs cmdInput;
        cmdInput.ipAddress = "127.0.0.1";       //default to loopback
        cmdInput.coreName = "Default Name";     //default core name (could use time or rand)
        cmdInput.transaction = "test";          //use simple test by default
        cmdInput.memSize = MEM_SIZE;            //set default memory size
        cmdInput.sleepTime = -1;                //default no sleep time used

    
    if(HandleInput(argc, argv, cmdInput))
        return 0;

    cout<<"name set:"<<cmdInput.transaction<<endl;

    //ARGS: auto sync on, address of server, port 1337
    TM_Client tm_client(true, cmdInput.ipAddress, 1337,cmdInput.coreName );

    
    if(cmdInput.transaction == "test")
    {
        //store transaction for later execution, get id in return
        t_id = tm_client.Register_Transaction(test_transaction, "test");
    }
    else if(cmdInput.transaction == "increment")
    {
        t_id = tm_client.Register_Transaction(incrementer, "increment");
    }
    else if (cmdInput.transaction == "stall")
    {
       t_id = tm_client.Register_Transaction(stall, "stall"); 
    }

    //init memory
    for(int i = 0; i < cmdInput.memSize; i++)
        shared_memory.push_back(TM_Share(i,0));

    args = &index;

    //register shared memory for use in transaction
    tm_client.Add_Shared_Memory(t_id, shared_memory);

    cout<<"Proceed?"<<endl;
    cin>>user_in;

    if(user_in == "y" && cmdInput.sleepTime == -1)
    {
        for(index = 0; index < cmdInput.memSize; index++)
            tm_client.Execute_Transaction(t_id, args);
    }
    else if(user_in == "y" && cmdInput.sleepTime >= 0 )
    {
        for(index = 0; index < cmdInput.memSize; index++)
        {
            srand(index * cmdInput.sleepTime);
            tm_client.Execute_Transaction(t_id, args);
            usleep(rand()%cmdInput.sleepTime);
        }
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
        unsigned int *index = (unsigned int*)args;
        int value = TM.shared_memory[*index].toInt();
        cout<<"Memory["<<*index<<"] = "<<value <<endl;
    END_T
//}}}
}

void * incrementer(void *args)
{
//{{{
    BEGIN_T("increment")
        unsigned int *index = (unsigned int*)args;
        //cout<<"Incrementing address "<<*index<<endl;
        int value = TM.shared_memory[*index].toInt();
        //value = TM.shared_memory[*index].toInt();
        TM.shared_memory[*index] = value + 1;
    END_T
//}}}
}

void * stall(void *args)
{
//{{{
    BEGIN_T("stall")
    char user_in;
    unsigned int *index = (unsigned int*)args;
    int value = TM.shared_memory[*index].toInt();
    cout<<"Stall on index = "<< *index<<" , value = "<<value<<endl;
    cout<<"...enter anything to continue..."<<endl;
    cin>>user_in;
    END_T
//}}}
}

bool HandleInput(int argc, char *argv[], inputArgs &input){
//{{{
    for(int i = 0; i < argc; i++){
        if((strcmp(argv[i],"-h") == 0))
            {
                cout<<"Usage:"<<endl;
                cout<<"-ip\t\t Set ip address of TM server"<<endl;
                cout<<"-n\t\t Set the name of this client"<<endl;
                cout<<"-tn\t\t Transaction name to run"<<endl;
                cout<<"\t\t[\"test\",\"increment\",\"stall\"]"<<endl;
                cout<<"-m\t\tNumber of memory locations available"<<endl;
                cout<<"-s\t\t Sleep between transactions for a maximum of N u-seconds"<<endl;
                cout<<"-h\t\t Print help (this message)"<<endl;

                return true;
            }

        if((strcmp(argv[i],"-tn") == 0))
            input.transaction = argv[i+1];
        if((strcmp(argv[i],"-ip") == 0))
            input.ipAddress = argv[i+1];
        if((strcmp(argv[i],"-n") == 0))
            input.coreName = argv[i+1];
        if((strcmp(argv[i],"-m") == 0))
            input.memSize = atoi(argv[i+1]);
        if((strcmp(argv[i],"-s") == 0))
            input.sleepTime = atoi(argv[i+1]);
    }
//}}}
}
