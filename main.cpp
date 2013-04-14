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

struct TransactionArgs
{
    int index;
    int sleepTime;
};

//transaction function
void * test_transaction(void *args);
void * incrementer(void *args);
void * stall(void *args);
void * stall_write(void *args);

//parse command line arguments
bool HandleInput(int argc, char *argv[], inputArgs &input);

int main(int argc, char *argv[])
{
//{{{
    int t_id = 0;
    unsigned int index = 0;
    string user_in;
    TransactionArgs t_args;

    void * args = &t_args;
    vector<TM_Share> shared_memory;

    inputArgs cmdInput;
        cmdInput.ipAddress = "127.0.0.1";       //default to loopback
        cmdInput.coreName = "Default Name";     //default core name (could use time or rand)
        cmdInput.transaction = "test";          //use simple test by default
        cmdInput.memSize = MEM_SIZE;            //set default memory size
        cmdInput.sleepTime = -1;                //default no sleep time used

    
    if(HandleInput(argc, argv, cmdInput))
        return 0;

    t_args.sleepTime = cmdInput.sleepTime;

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
    else if(cmdInput.transaction == "stall-write")
    {
        t_id = tm_client.Register_Transaction(stall_write, "stall-write");
    }

    //init memory
    for(int i = 0; i < cmdInput.memSize; i++)
        shared_memory.push_back(TM_Share(i,0));


    //register shared memory for use in transaction
    tm_client.Add_Shared_Memory(t_id, shared_memory);

    cout<<"Proceed?"<<endl;
    cin>>user_in;

    if(user_in == "y")
    {
        for(t_args.index = 0; t_args.index < cmdInput.memSize; t_args.index++)
            tm_client.Execute_Transaction(t_id, args);
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
        TransactionArgs t_args = *(TransactionArgs*)args;
        unsigned int index = t_args.index;
        int value = TM.shared_memory[index].toInt();
        cout<<"Memory["<<index<<"] = "<<value <<endl;

        if(t_args.sleepTime > 0 )
        {
            cout<<"Sleeping..."<<endl;
            usleep(t_args.sleepTime);
        }
    END_T
//}}}
}

void * incrementer(void *args)
{
//{{{
    BEGIN_T("increment")
        TransactionArgs t_args = *(TransactionArgs*)args;
        unsigned int index = t_args.index;
        int value = TM.shared_memory[index].toInt();
        TM.shared_memory[index] = value + 1;

        if(t_args.sleepTime > 0 )
        {
            cout<<"Sleeping..."<<endl;
            usleep(t_args.sleepTime);
        }
    END_T
//}}}
}

void * stall(void *args)
{
//{{{
    BEGIN_T("stall")
    char user_in;
    TransactionArgs t_args = *(TransactionArgs*)args;
    unsigned int index = t_args.index;
    int value = TM.shared_memory[index].toInt();
    cout<<"Stall on index = "<< index<<" , value = "<<value<<endl;
    cout<<"...enter anything to continue..."<<endl;
    cin>>user_in;
    END_T
//}}}
}

void * stall_write(void *args)
{
//{{{
    BEGIN_T("stall-write")
    char user_in;
    TransactionArgs t_args = *(TransactionArgs*)args;
    unsigned int index = t_args.index;
    int value = TM.shared_memory[index].toInt();
    cout<<"Stall WRITE on index = "<< index<<" , value = "<<value<<endl;
    cout<<"...enter anything to continue..."<<endl;
    cin>>user_in;
    TM.shared_memory[index] = value + 1;
    cout<<"Written, enter anything to attempt commit..."<<endl;
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
                cout<<"\t\t[\"test\",\"increment\",\"stall\",\"stall-write\"]"<<endl;
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
    return false;
//}}}
}
