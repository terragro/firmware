#include "RadioLib.h"
#include <vector>
using std::vector;

class PacketQueue
{
public:
    PacketQueue() {}

    vector<String> items;

    int size()
    {
        return this->items.size();
    }

    String shift()
    {
        if (this->items.empty())
        {
            throw std::out_of_range("Cannot shift from an empty vector.");
        }

        String item = this->items.front();
        this->items.erase(this->items.begin());
        return item;
    }

    int push(String item)
    {
        this->items.push_back(item);
        return this->items.size();
    }

    bool itemAvailable()
    {
        return (this->items.size() > 0);
    }
};