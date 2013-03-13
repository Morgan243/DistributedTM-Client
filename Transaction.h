#include "TM_Share.h"

#ifndef TRANSACTION_H
#define TRANSACTION_H

class Transaction
{
    private:
        int abort_count = 0;

    public:
        std::string name = "NAME_UNSET";

        //pointer to a function accepts a void pointer and returns a void pointer
        void *(*transaction)(void *) = NULL;

        //to lazy to make all the getters/setters for this
        std::vector<TM_Share> shared_memory;

        Transaction();
        ~Transaction();
        Transaction(void *(*transaction)(void *), std::string name);

        int Get_Abort_Count();
        void Increment_Abort_Count();
        void Reset_Abort_Count();

        void SetTransaction(void *(*transaction)(void *), std::string name);

        //check all memory usage for conflicts
        void Sync();
};
#endif
