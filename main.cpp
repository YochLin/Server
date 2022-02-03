#include "server/server.h"

int main()
{
    Server s(8080, 3, 6);
    s.Start();
}