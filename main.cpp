#include "server/server.h"

int main()
{
    Server s(2524, 6);
    s.Start();
}