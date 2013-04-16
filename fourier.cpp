#include <math.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{

    int M = 50;
    int num_elements = (int)(6.0/.01);
    double result[num_elements];
    double time = -3.0;
    double d_t = .01;

    for(int i = 0; i < num_elements; i++)
    {
        result[i] = 0.5;
        for(int j = 1; j < M; j++)
        {
            result[i] += (2.0/3.141)*(1.0/j)*sin((j*3.141)/2.0)*cos(j*3.141*time);
        }
        cout<<i<<", "<<result[i]<<endl;
        time += d_t;
    }

    return 0;
}

