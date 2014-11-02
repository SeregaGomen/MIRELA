#include <math.h>
#include <string.h>
#include "analyse.h"

using namespace std;

//------------------------------------------------------------------------------------------------
bool TResult::write(ofstream& out)
{
    int len = results.size();

    out.write((char*)&len,sizeof(int));
    if (out.fail())
        return false;
    for (int i = 0; i < len; i++)
    {
        out.write((char*)&(results[i]),sizeof(double));
        if (out.fail())
            return false;
    }
    out.write((char*)&(len = (int)name.length() + 1),sizeof(int));
    out.write((char*)name.c_str(),len);
    return !out.fail();
}
//---------------------------------------------------------------------------------------------------------------------------
bool TResult::read(ifstream& in)
{
    int len;
    char* n;

    in.read((char*)&len,sizeof(int));
    if (in.fail())
        return false;
    results.resize(len,0);
    for (int i = 0; i < len; i++)
    {
        in.read((char*)&(results[i]),sizeof(double));
        if (in.fail())
            return false;
    }
    in.read((char*)&len,sizeof(int));
    if (in.fail())
        return false;
    if (len)
    {
        n = new char[len];
        in.read(n,len);
        name = n;
        delete [] n;
        if (in.fail())
            return false;
    }
    minResults = *min_element(results.begin(),results.end());
    maxResults = *max_element(results.begin(),results.end());
    return true;
}
//---------------------------------------------------------------------------------------------------------------------------
TResult& TResultList::operator [] (unsigned I)
{
    TResultList::iterator it = begin();
    unsigned i = 0;

    while (it != end())
    {
        if (i == I)
            break;
        i++;
        it++;
    }
    return *it;
}
//---------------------------------------------------------------------------------------------------------------------------
