#ifndef __ANALYSE_H_
#define __ANALYSE_H_

#include <list>
#include <fstream>
#include <assert.h>
#include <algorithm>
#include "matrix.h"


using namespace std;

/***************************************************/
/*  Численный результат расчета для одной функции  */
/***************************************************/
class TResult
{
friend class TResultList;
private:
    string name;
    vector<double> results;
    double minResults;
    double maxResults;
public:
    TResult(void)
    {
        minResults = maxResults = 0;
    }
    TResult(const TResult& r)
    {
        results = r.results;
        minResults = r.minResults;
        maxResults = r.maxResults;
        name = r.name;
    }
    TResult(vector<double>& res,string fname)
    {
        results = res;
        minResults = *min_element(results.begin(),results.end());
        maxResults = *max_element(results.begin(),results.end());
        name = fname;
    }
    TResult(double* res,unsigned sz,string fname)
    {
        results.assign(res,res+sz);
        minResults = *min_element(results.begin(),results.end());
        maxResults = *max_element(results.begin(),results.end());
        name = fname;
    }
   ~TResult(void)
    {
        destroy();
    }
    void destroy(void)
    {
        results.clear();
        name.clear();
    }
    bool write(ofstream&);
    bool read(ifstream&);
    vector<double>& getResults(void)
    {
        return results;
    }
    double& getResults(unsigned i)
    {
        return results[i];
    }
    TResult& operator = (const TResult& r)
    {
        minResults = r.minResults;
        maxResults = r.maxResults;
        results = r.results;
        return *this;
    }
    double min(void) const
    {
        return minResults;
    }
    double max(void) const
    {
        return maxResults;
    }
    string getName(void)
    {
        return name;
    }
};
/***************************************************/
/*           Список результатов расчета            */
/***************************************************/
class TResultList : public list<TResult>
{
private:
    tm solTime;
public:
    TResultList(void)// : list()
    {
    }
   ~TResultList(void)
    {
    }
    void addSolutionTime(tm t)
    {
        solTime = t;
    }
    tm getSolutionTime(void)
    {
        return solTime;
    }
    void addResult(vector<double>& res, string n)
    {
        TResultList::iterator it = begin();
        TResult c(res,n);

        for (it = begin(); it != end(); it++)
            if (!n.compare(it->name))
            {
                *it = c;
                return;
            }
        push_back(c);
    }
    void addResult(double* res, unsigned sz, string n)
    {
        TResultList::iterator it = begin();
        TResult c(res,sz,n);

        for (it = begin(); it != end(); it++)
            if (!n.compare(it->name))
            {
                *it = c;
                return;
            }
        push_back(c);
    }
    TResult& operator [] (unsigned);
    bool write(ofstream& out)
    {
        TResultList::iterator it;
        int num = (int)size();

        out.write((char*)&num,sizeof(int));
        if (out.fail())
            return false;
        for (it = begin(); it != end(); it++)
            if (!it->write(out))
                return false;
        out.write((char*)&solTime,sizeof(tm));
        return !out.fail();
    }
    bool read(ifstream& in)
    {
        int num;
        TResult c;

        clear();
        in.read((char*)&num,sizeof(int));
        if (in.fail())
            return false;
        for (int i = 0; i < num; i++)
        {
            if (!c.read(in))
                return false;
            push_back(c);
        }
        in.read((char*)&solTime,sizeof(tm));
        return !in.fail();
    }
    unsigned getFuncNum(string n)
    {
        TResultList::iterator it = begin();
        unsigned num = 0;

        for (it = begin(); it != end(); it++,num++)
            if (!n.compare(it->name))
                break;
        return num;
    }
};



#endif //__ANALYSE_H_
