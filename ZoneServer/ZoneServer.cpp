#include <iostream>
#include <json.h>

int main( int argc, char **argv )
{
    json j;
    j["aa"] = 1;

    std::cout << j;
}