#include <iostream>
#include "map.hpp"
#include <string>
#include "crc32.hpp"

using std::cout;
using std::endl;
using std::string;

int main()
{
    Map<std::string, int> map;
    map["One"] = 1;
    map["Two"] = 2;
    map["Three"] = 3;
    cout << "Our numbers ";
    cout << map["One"] << " " << map["Two"] << " " << map["Three"] << endl;
    map["One"] += 2;

    cout << "First number plus 2 is " << map["One"] << endl;
    return 0;
}