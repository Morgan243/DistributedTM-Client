#include <iostream>
#include <vector>
#include <math.h>
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
    int backoff_delta;
    int rerun;
    int *fourier_iters;
    bool auto_proceed;
    StoreType data_type;
};

struct TransactionArgs
{
    int index;
    int sleepTime;
    int iterations;
    int iterations_index;
    int maxIndex;
    float time;
    float dt;
    int current_index;
};

//transaction function
void * reader(void *args);
void * reader_fl(void *args);
void * incrementer(void *args);
void * incrementer_fl(void *args);
void * stall(void *args);
void * stall_write(void *args);

void * fourier_index(void * args);
void * fourier(void * args);

//parse command line arguments
bool HandleInput(int argc, char *argv[], inputArgs &input);

int main(int argc, char *argv[])
{
//{{{
    int t_id = 0, t_id2 = 0;
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
        cmdInput.backoff_delta = 300;           //default to a 3 u-second incremental backoff
        cmdInput.auto_proceed = false;          //default requires user interaction
        cmdInput.rerun = 1;                     //default run the selected transaction once
        cmdInput.data_type = integer;           //default to integer runtime data types
        cmdInput.fourier_iters = new int[5] {2,6,20,100,1000};

    if(HandleInput(argc, argv, cmdInput))
        return 0;

    t_args.sleepTime = cmdInput.sleepTime;
    t_args.iterations = 100;
    t_args.time = 0;
    t_args.dt = .001;
    t_args.maxIndex = cmdInput.memSize - 1;
    t_args.iterations_index = 0;

    cout<<"name set:"<<cmdInput.transaction<<endl;

    //ARGS: auto sync on, address of server, port 1337
    TM_Client::Init(true, cmdInput.ipAddress, 1337,cmdInput.coreName );
    //TM_Client tm_client(true, cmdInput.ipAddress, 1337, cmdInput.coreName );
    
    if(cmdInput.transaction == "test")
    {
        //store transaction for later execution, get id in return
        t_id = TM_Client::Register_Transaction(reader, "reader", cmdInput.backoff_delta);
    }
    else if(cmdInput.transaction == "increment")
    {
        t_id = tm_client.Register_Transaction(incrementer, "increment", cmdInput.backoff_delta);
    }
    else if(cmdInput.transaction == "increment_fl")
    {
        t_id = tm_client.Register_Transaction(incrementer_fl, "increment_fl", cmdInput.backoff_delta);
    }
    else if(cmdInput.transaction == "reader_fl")
    {
        t_id = tm_client.Register_Transaction(reader_fl, "reader_fl", cmdInput.backoff_delta);
    }
    else if (cmdInput.transaction == "stall")
    {
       t_id = tm_client.Register_Transaction(stall, "stall", cmdInput.backoff_delta); 
    }
    else if(cmdInput.transaction == "stall-write")
    {
        t_id = tm_client.Register_Transaction(stall_write, "stall-write", cmdInput.backoff_delta);
    }
    else if(cmdInput.transaction == "fourier")
    {
        t_id = tm_client.Register_Transaction(fourier, "fourier", cmdInput.backoff_delta);
        t_id2 = tm_client.Register_Transaction(fourier_index, "fourier_index", cmdInput.backoff_delta);
        TM_Client::setAbortReturn(true);
    }

    //init memory
    if(cmdInput.data_type == integer)
    {
        for(int i = 0; i < cmdInput.memSize; i++)
            shared_memory.push_back(TM_Share(i, 0));
    }
    else if(cmdInput.data_type == float_real)
    {
        for(int i = 0; i < cmdInput.memSize; i++)
            shared_memory.push_back(TM_Share(i, 0.0, true));
    }


    //register shared memory for use in transaction
    tm_client.Add_Shared_Memory(t_id, shared_memory);


    for(int i = 0; i < cmdInput.rerun; i++)
    {
        if(!cmdInput.auto_proceed)
        {
            cout<<"Proceed?"<<endl;
            cin>>user_in;
        }

        if((user_in == "y" || cmdInput.auto_proceed) && cmdInput.transaction == "fourier")
        {
            tm_client.Add_Shared_Memory(t_id2, shared_memory);

            while(t_args.iterations_index < 5)
            {
                //cout<<"index: "<<t_args.current_index<<", iteration index: "<<t_args.iterations_index<<", time: "<<t_args.time<<endl;
                //get an index and iteration index
                tm_client.Execute_Transaction(t_id2, args);

                //set iterations
                t_args.iterations = cmdInput.fourier_iters[t_args.iterations_index];

                tm_client.Execute_Transaction(t_id, args);
            }
        }
        else if(user_in == "y" || cmdInput.auto_proceed)
        {
            for(t_args.index = 0; t_args.index < cmdInput.memSize; t_args.index++)
                tm_client.Execute_Transaction(t_id, args);
        }
    }

    if(!cmdInput.auto_proceed)
    {
        cout<<"Enter anything to terminate..."<<endl;
        cin>>user_in;
    }

    cout<<"Total Abort: "<<TM_Client::GetTotalAborts()<<endl;
    cout<<"Total Commit: "<<TM_Client::GetTotalCommits()<<endl;

    return 0;
//}}}
}

void * reader(void *args)
{
//{{{
    //use the transaction's name to set things up
    BEGIN_T("reader")
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

void * reader_fl(void *args)
{
//{{{
    //use the transaction's name to set things up
    BEGIN_T("reader_fl")
        TransactionArgs t_args = *(TransactionArgs*)args;
        unsigned int index = t_args.index;
        float value = TM.shared_memory[index].toFloat();
        cout<<"Memory Float["<<index<<"] = "<<value <<endl;

        if(t_args.sleepTime > 0 )
        {
            cout<<"Sleeping..."<<endl;
            usleep(t_args.sleepTime);
        }
    END_T
//}}}
}

void * incrementer_fl(void *args)
{
//{{{
    BEGIN_T("increment_fl")
        TransactionArgs t_args = *(TransactionArgs*)args;
        unsigned int index = t_args.index;
        float value = TM.shared_memory[index].toFloat();
        TM.shared_memory[index].setFloat(value + 1.5);

        cout<<"Value now: "<<TM.shared_memory[index].toFloat();

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

void * fourier_index(void * args)
{
BEGIN_T("fourier_index")
    TransactionArgs *t_args = (TransactionArgs*)args;

     float currentIndex =  TM.shared_memory[t_args->maxIndex].toFloat();
     t_args->iterations_index = (int)TM.shared_memory[t_args->maxIndex - 1].toFloat();

    //if still more to go, increment index for the next node
    if(currentIndex < t_args->maxIndex - 3)
    {
        TM.shared_memory[t_args->maxIndex].setFloat(currentIndex + 1);
        cout<<"Inceremtented current index..."<<endl;
    }
    else
    {
        TM.shared_memory[t_args->maxIndex].setFloat(0);
        TM.shared_memory[t_args->maxIndex - 1].setFloat(t_args->iterations_index + 1);
        cout<<"Set current index to zero!!!"<<endl;
    }

    t_args->current_index = (int)currentIndex;
END_T
}

void * fourier(void * args)
{
//{{{
    BEGIN_T("fourier")
    TransactionArgs t_args = *(TransactionArgs*)args;
    TM.shared_memory[t_args.current_index].setFloat(.5);

    float value = TM.shared_memory[t_args.current_index].toFloat();

    t_args.time = (t_args.dt * (float) t_args.current_index);

        for(int i = 1; i < t_args.iterations; i++)
        {
            value += (2.0/3.141)*(1.0/i)*sin((i*3.141)/2.0)*cos(i*3.141*t_args.time);
        }

    TM.shared_memory[t_args.current_index].setFloat(value);
    cout<<"Fourier: "<< value<<endl; 

    if(t_args.sleepTime > 0 )
        {
            cout<<"Sleeping..."<<endl;
            usleep(t_args.sleepTime);
        }
    END_T
//}}}
}

bool HandleInput(int argc, char *argv[], inputArgs &input)
{
//{{{
    for(int i = 0; i < argc; i++){
        if((strcmp(argv[i],"-h") == 0))
            {
                cout<<"Usage:"<<endl;
                cout<<"-ip\t\t Set ip address of TM server"<<endl;
                cout<<"-n\t\t Set the name of this client"<<endl;
                cout<<"-tn\t\t Transaction name to run"<<endl;
                cout<<"\t\t[\"test\",\"increment\",\"stall\",\"stall-write\", \"fourier\"]"<<endl;
                cout<<"-m\t\tNumber of memory locations available"<<endl;
                cout<<"-s\t\t Sleep between transactions for a maximum of N u-seconds"<<endl;
                cout<<"-b\t\tLinear backoff of aborted transactions (in microseconds)"<<endl;
                cout<<"-y\t\t Auto proceed, run without user input"<<endl;
                cout<<"-r\t\tRerun the selected transaction N times"<<endl;
                cout<<"-type\t\tSelect data type of shared memory"<<endl;
                cout<<"\t\t[\"int\",\"float\"]"<<endl;
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
        if((strcmp(argv[i],"-b") == 0))
            input.backoff_delta = atoi(argv[i+1]);
        if((strcmp(argv[i],"-y") == 0))
            input.auto_proceed = true;
        if((strcmp(argv[i],"-r") == 0))
            input.rerun = atoi(argv[i+1]);
        if((strcmp(argv[i],"-type") == 0))
        {
            cout<<"Type detected..."<<endl;
            if((strcmp(argv[i+1],"int") == 0))
                input.data_type = integer;
            if((strcmp(argv[i+1],"float") == 0))
                input.data_type = float_real;
        }

    }
    return false;
//}}}
}
