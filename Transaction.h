#include "TM_Share.h"

#ifndef TRANSACTION_H
#define TRANSACTION_H

class Transaction
{
    private:
        int abort_count = 0;
        int commit_count = 0;

    public:
        int backoff_increment = 0, backoff_time = 0;

        std::string name = "NAME_UNSET";

        //pointer to a function accepts a void pointer and returns a void pointer
        void *(*transaction)(void *) = NULL;

        //to lazy to make all the getters/setters for this
        std::vector<TM_Share> shared_memory;

        Transaction();
        ~Transaction();
        Transaction(void *(*transaction)(void *), std::string name);

        int Get_Abort_Count();
        int Get_Commit_Count();
        void Increment_Abort_Count();
        void Increment_Commit_Count();
        int getTotalAborts();
        int getTotalCommits();
        void Reset_Abort_Count();

        void SetTransaction(void *(*transaction)(void *), std::string name);

        //check all memory usage for conflicts
        void Sync();
        void Init();
        void Commit();
};
#endif
