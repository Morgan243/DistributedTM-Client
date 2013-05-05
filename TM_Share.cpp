#include "TM_Share.h"

#define DEBUG 0

using namespace std;

    //------------------------
    //Static Var Define
    //------------------------
    NC_Client *TM_Share::network;
    StoreType TM_Share::store_type;
    //mutex TM_Share:: queue_lock;
    //------------------------

TM_Share::TM_Share(unsigned int mem_address, unsigned int mem_value)
{
//{{{
    this->auto_sync = true;

    this->isWrite = false;
    this->isRead = false;

    this->mem_address = mem_address;
    this->mem_value = mem_value;

    TM_Share::store_type = integer;

    #if DEBUG
    cout<<"Memory at address "<<this->mem_address<<" registered with value "<< this->mem_value<<endl;
    #endif
//}}}
}

TM_Share::TM_Share(unsigned int mem_address, unsigned int mem_value, queue<TM_Message> *messages_ref)
{
//{{{
    this->auto_sync = true;

    this->isWrite = false;
    this->isRead = false;

    this->mem_address = mem_address;
    this->mem_value = mem_value;

    TM_Share::store_type = integer;

    #if DEBUG
    cout<<"Memory at address "<<this->mem_address<<" registered with value "<< this->mem_value<<endl;
    #endif
    //store pointer to the message queue
    Register_MessageQueue(messages_ref);
//}}}
}

TM_Share::TM_Share(unsigned int mem_address, float mem_value, bool float_type)
{
//{{{
    this->auto_sync = true;

    this->isWrite = false;
    this->isRead = false;

    this->mem_address = mem_address;
    this->mem_value_fl = mem_value;
    
    TM_Share::store_type = float_real;

    #if DEBUG
    cout<<"Memory at address "<<this->mem_address<<" registered with value "<< this->mem_value<<endl;
    #endif
//}}}
}

TM_Share::TM_Share(unsigned int mem_address, float mem_value, queue<TM_Message> *messages_ref, bool float_type)
{
//{{{
    this->auto_sync = true;

    this->isWrite = false;
    this->isRead = false;

    this->mem_address = mem_address;
    this->mem_value_fl = mem_value;

    TM_Share::store_type = float_real;

    #if DEBUG
    cout<<"Memory at address "<<this->mem_address<<" registered with value "<< this->mem_value<<endl;
    #endif

    //store pointer to the message queue
    Register_MessageQueue(messages_ref);
//}}}
}

void TM_Share::SendMessage(char code, unsigned int address, unsigned int value)
{
 //{{{
    char temp_buffer[1024];
   
    //clear data buffer
    bzero(temp_buffer, sizeof(temp_buffer));

    //get string version of data
    int size = sprintf(temp_buffer, "%c:%u:%u", code, address, value);

    TM_Share::network->Send(temp_buffer, size + 1);
    #if DEBUG
        cout<<"Sent :"<< temp_buffer<<" (done)"<<endl;
    #endif
//}}}
}

void TM_Share::ReceiveMessage(char *code, unsigned int *address, unsigned int *value)
{
//{{{
    unsigned pos1, pos2;
    string buffer;

    //get a message from the server
    TM_Share::network->Receive(&buffer,1024);

    //find first colon
    pos1 = buffer.find_first_of(":"); 

    //get character before first colon as code
    *code = buffer[pos1 - 1];

    //replace colon so we can find the next one
    buffer[pos1] = '-';

    //find second colon
    pos2 = buffer.find_first_of(":");

    //get the address string and convert it to an integer
    *address = (unsigned int)atoi(buffer.substr(pos1+1, pos2-1).c_str());

    //get the value string and convert it to an integer
    *value = (unsigned int)atoi(buffer.substr(pos2+1, buffer.length() - 1).c_str());

//}}}
}

void TM_Share::SendMessage(char code, unsigned int address, float value)
{
 //{{{
    char temp_buffer[1024];
   
    //clear data buffer
    bzero(temp_buffer, sizeof(temp_buffer));

    //get string version of data
    int size = sprintf(temp_buffer, "%c:%u:%f", code, address, value);

    TM_Share::network->Send(temp_buffer, size + 1);
    #if DEBUG
        cout<<"Sent :"<< temp_buffer<<" (done)"<<endl;
    #endif
//}}}
}

void TM_Share::ReceiveMessage(char *code, unsigned int *address, float *value)
{
//{{{
    unsigned pos1, pos2;
    string buffer;

    //get a message from the server
    TM_Share::network->Receive(&buffer,1024);

    //find first colon
    pos1 = buffer.find_first_of(":"); 

    //get character before first colon as code
    *code = buffer[pos1 - 1];

    //replace colon so we can find the next one
    buffer[pos1] = '-';

    //find second colon
    pos2 = buffer.find_first_of(":");

    //get the address string and convert it to an integer
    *address = (unsigned int)atoi(buffer.substr(pos1+1, pos2-1).c_str());

    //get the value string and convert it to an integer
    *value = (float)atof(buffer.substr(pos2+1, buffer.length() - 1).c_str());

//}}}
}

void TM_Share::SendMessage(TM_Message message)
{
//{{{
    //clear data buffer
    bzero(this->out_buffer, sizeof(this->out_buffer));

    #if DEBUG
        cout<<"Sending..."<<endl;
        cout<<"\tCode: "<<hex<<(unsigned int)message.code<<endl;
        cout<<"\tAddress: "<<hex<<message.address<<endl;
        cout<<"\tValue: "<<hex<<message.value<<endl;
        cout<<endl;
    #endif

    //get string version of data
    int size;

    if(TM_Share::store_type == integer)
       size = sprintf(this->out_buffer, "%c:%u:%u", message.code, message.address, message.value);
    else if(TM_Share::store_type == float_real)
       size = sprintf(this->out_buffer, "%c:%u:%f", message.code, message.address, message.value_fl);

    #if DEBUG
        cout<<"Messasge compiled as : "<<this->out_buffer<<"(DONE)"<<endl;
    #endif

    TM_Share::network->Send(this->out_buffer, size+1);
    //cout<<"Sending: "<<this->out_buffer<<endl;
//}}}
}

TM_Message TM_Share::ReceiveMessage()
{
//{{{
    unsigned pos1, pos2;
    in_buffer.clear();

    //get a message from the server
    TM_Share::network->Receive(&in_buffer,1024);

    //find first colon
    pos1 = in_buffer.find_first_of(":"); 

    //get character before first colon as code
    in_message.code = in_buffer[pos1 - 1];

    //replace colon so we can find the next one
    in_buffer[pos1] = '-';

    //find second colon
    pos2 = in_buffer.find_first_of(":");

    //get the address string and convert it to an integer
    in_message.address = (unsigned int)atoi(in_buffer.substr(pos1+1, pos2-1).c_str());

    if(TM_Share::store_type == integer)
    {
        //get the value string and convert it to an integer
        in_message.value = (unsigned int)atoi(in_buffer.substr(pos2+1, in_buffer.length() - 1).c_str());

        #if DEBUG
        //display values
            cout<<hex<<"\tcode: "<<(unsigned int)in_message.code<<endl;
            cout<<hex<<"\taddr: "<<in_message.address<<endl;
            cout<<hex<<"\tvalue: "<<in_message.value<<endl;
            cout<<endl;
        #endif
    }
    else if(TM_Share::store_type == float_real)
    {
        //get the value string and convert it to an integer
        in_message.value_fl = (float)atof(in_buffer.substr(pos2+1, in_buffer.length() - 1).c_str());

        #if DEBUG
        //display values
            cout<<hex<<"\tcode: "<<(unsigned int)in_message.code<<endl;
            cout<<hex<<"\taddr: "<<in_message.address<<endl;
            cout<<hex<<"\tvalue: "<<in_message.value_fl<<endl;
            cout<<endl;
        #endif
    }

    return in_message;
//}}}
}

void TM_Share::Set_Auto_Sync(bool autoSync)
{
    this->auto_sync = autoSync;
}

void TM_Share::Register_MessageQueue(queue<TM_Message> *messages_ref)
{
//{{{
    this->messages = messages_ref;
    #if DEBUG
        cout<<"Message queue registered in shared memory at "<<mem_address<<"..."<<endl;
    #endif
//}}}
}

void TM_Share::Register_Network(NC_Client *net)
{
//{{{
   TM_Share::network = net;
   #if DEBUG
       cout<<"Network registered..."<<endl;
   #endif
//}}}
}

unsigned int TM_Share::Get_Address()
{
    return mem_address;
}

void TM_Share::Declare_Commit()
{
//{{{
    char code = COMMIT;
    unsigned int address = 0;

    if(TM_Share::store_type == integer)
    {
        unsigned int value = 0; //could make this a count or something else useful

        TM_Share::SendMessage(code, address, value);

        TM_Share::ReceiveMessage(&code, &address, &value);

        if(code & ABORT)
        {
            throw ABORT;
        }
    }
    else if(TM_Share::store_type == float_real)
    {
        float value = 0.0;

        TM_Share::SendMessage(code, address, value);

        TM_Share::ReceiveMessage(&code, &address, &value);

        if(code & ABORT)
        {
            throw ABORT;
        }
    }
//}}}
}

void TM_Share::End_Commit()
{
//{{{
    char code = COMMIT | CONTROL;
    unsigned int address = 0;
    
    if(TM_Share::store_type == integer)
    {
        unsigned int value = 0; //could make this a count or something else useful

        TM_Share::SendMessage(code, address, value);

        TM_Share::ReceiveMessage(&code, &address, &value);

        if(code & ABORT)
        {
            throw ABORT;
        }
    }
    else if(TM_Share::store_type == float_real)
    {
        float value = 0.0; //could make this a count or something else useful

        TM_Share::SendMessage(code, address, value);

        TM_Share::ReceiveMessage(&code, &address, &value);

        if(code & ABORT)
        {
            throw ABORT;
        }
    }
//}}}
}

void TM_Share::TM_Init()
{
//{{{
    #if DEBUG 
        cout<<"initializing address: "<<mem_address<<endl;
    #endif
    out_message.code = INIT;
    out_message.address = mem_address;

    if(TM_Share::store_type == integer)
    {
        out_message.value = 0;

        TM_Share::SendMessage(out_message);

        TM_Share::ReceiveMessage();

        if(in_message.code & ABORT)
        {
            this->isRead = false;
            throw ABORT;
        }
        else
        {
            this->isRead = true;
            mem_value = in_message.value;
        }
    }
    else if(TM_Share::store_type == float_real)
    {
        out_message.value_fl = 0.0;

        TM_Share::SendMessage(out_message);

        TM_Share::ReceiveMessage();

        if(in_message.code & ABORT)
        {
            this->isRead = false;
            throw ABORT;
        }
        else
        {
            this->isRead = true;
            mem_value_fl = in_message.value_fl;
        }
    }
//}}}
}

void TM_Share::TM_Read()
{
//{{{
    //set up the message to notify TM server
    out_message.code = READ;
    out_message.address = mem_address;
    out_message.value = 0;//mem_value;
    out_message.value_fl = 0.0;

    //push it back for the network thread
   
    //if not read yet
    if(!this->isRead)
    {
        this->isRead = true;
        TM_Init();
    }
    #if DEBUG
        cout<<"Read executed on address "<<this->mem_address<<endl;
    #endif
//}}}
}

void TM_Share::TM_Write()
{
//{{{
    //setup the message for the TM server
    out_message.code = WRITE;                   //a write was made...
    out_message.address = mem_address;          //to this memory address...
    out_message.value = new_value;              //get speculative value
    out_message.value_fl = new_value_fl;

    //push it back, oh yeah
    #if DEBUG
        cout<<"Write executed on address "<<this->mem_address<<endl;
        cout<<"\tAlerting TM server..."<<endl;
    #endif
    
    TM_Share::SendMessage(out_message);
    
    #if DEBUG
        cout<<"\tAwaiting approval from server..."<<endl;
    #endif
    
    //in message set and returned here
    TM_Share::ReceiveMessage();

    //see if the server rejected the transaction
    if(in_message.code & ABORT)
    {
        this->isWrite = false;
        throw ABORT;
    }
//}}}
}

void TM_Share::TM_Sync()
{
//{{{
    //encode message
    if(isWrite)
        out_message.code = SYNC | WRITE;
    else 
        out_message.code = SYNC | READ;

    //indicate the address
    out_message.address = mem_address;

    //value shouldn't matter
    out_message.value = 0;                  
    out_message.value_fl = 0.0;

    #if DEBUG
        cout<<"Syncing address "<<out_message.address<<" with server..."<<endl;
        cout<<"\tContacting server..."<<endl;
    #endif

    TM_Share::SendMessage(out_message);

    #if DEBUG
        cout<<"Waiting for server sync response..."<<endl;
    #endif

    TM_Share::ReceiveMessage();
//}}}
}

void TM_Share::TM_Commit()
{
//{{{
    //Only send a message if need to update a value
    if(this->isWrite)
    {
        out_message.code = WRITE | COMMIT;                   //a write was made...
        out_message.value = new_value;              //get speculative value
        out_message.value_fl = new_value_fl;
        
        #if DEBUG   
            cout<<"Commiting the changes to address "<<this->mem_address<<endl;
        #endif
    }
    else if(this->isRead)
    {
        out_message.code = READ | COMMIT;                   
        out_message.value = mem_value;
        out_message.value_fl = mem_value_fl;
        
        #if DEBUG
            cout<<"Commiting the read finish to address "<<this->mem_address<<endl;
        #endif
    
    }
    else
    {
        #if DEBUG
            cout<<"No access was made on address "<<this->mem_address<<", nothing to commit..."<<endl;
        #endif
        return;
    }

    out_message.address = this->mem_address;          //to this memory address...

    #if DEBUG
        cout<<"\tPushing TM server..."<<endl;
    #endif

    TM_Share::SendMessage(out_message);

    TM_Share::ReceiveMessage();
//}}}
}

//OVERLOAD: TM_Share = TM_Share
TM_Share & TM_Share::operator=(TM_Share &tm_source)
{
//{{{
    this->isWrite = true;

    this->new_value = tm_source.mem_value;

    //notify TM server of write
    this->TM_Write();

    //notify TM server of read
    tm_source.TM_Read();

    return *(this);
//}}}
}

int  TM_Share::toInt()
{
//{{{
    TM_Read();

    if(isWrite)
       return new_value;
    else
       return mem_value;
//}}}
}

float TM_Share::toFloat()
{
//{{{
    TM_Read();

    if(isWrite)
       return new_value_fl;
    else
       return mem_value_fl;
//}}}
}

TM_Share & TM_Share::operator+(TM_Share &tm_source)
{
//{{{
    this->isWrite = true;

    this->new_value = tm_source.mem_value;

    //notify TM server of write
    this->TM_Write();

    //notify TM server of read
    tm_source.TM_Read();

    return *this;
//}}}
}

//OVERLOAD: TM_Share = int
TM_Share & TM_Share::operator=(const int source)
{
//{{{
    this->isWrite = true;

    this->new_value = (unsigned int) source;

    //notofy TM server of write
    this->TM_Write();

    return *this;
//}}}
}

//OVERLOAD: TM_Share = unsigned int
TM_Share & TM_Share::operator=(const unsigned int source)
{
//{{{
    this->isWrite = true;

    this->new_value = (unsigned int) source;

    //notify TM server of write
    this->TM_Write();

    return *this;
//}}}
}

TM_Share & TM_Share::operator+(const int source)
{
//{{{
    this->isWrite = true;

    this->new_value += (unsigned int) source;

    //notofy TM server of write
    this->TM_Write();

    return *this;
//}}}
}

void TM_Share::setFloat(float val)
{
    this->isWrite = true;

    this->new_value_fl += val;

    this->TM_Write();
}

//TM_Share & TM_Share::operator=(const float source)
//{
////{{{
//    this->isWrite = true;
//
//    this->new_value_fl = source;
//
//    //notofy TM server of write
//    this->TM_Write();
//
//    return *this;
////}}}
//}
//
//TM_Share & TM_Share::operator+(const float source)
//{
////{{{
//    this->isWrite = true;
//
//    this->new_value_fl += source;
//
//    //notofy TM server of write
//    this->TM_Write();
//
//    return *this;
////}}}
//}

//OVERLOAD: <<TM_Share<< (used with cout primarily)
ostream& operator<<(ostream &out, TM_Share &tm_share)
{
//{{{
    tm_share.TM_Read();

    if(tm_share.isWrite)
        out<<tm_share.new_value;
    else
        out<<tm_share.mem_value;

    return out;
//}}}
}
