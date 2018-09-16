#ifndef PROCESS_H
#define PROCESS_H

#include "utility.h"
#include <set>
#include <cassert>
#include <algorithm>
#include <ostream>
#include <iostream>




struct VirtualPage {
    unsigned int id;
    unsigned int index;

    VirtualPage(unsigned int id, unsigned int index);
};

bool operator==(const VirtualPage& lhs, const VirtualPage& rhs);


enum class QueryType {
    Modification, Read
};

struct Query {
    const VirtualPage virtualPage;
    const QueryType type;

    Query(VirtualPage virtualPage, QueryType type);
};

std::ostream& operator<<(std::ostream& os, const VirtualPage& virtualPage);
std::ostream& operator<<(std::ostream& os, const QueryType& type);
std::ostream& operator<<(std::ostream& os, const Query& query);


class Process {
public:
    const unsigned int id;
    const unsigned int pagesCount;
private:
    std::set<unsigned int> workingSet;
    std::set<unsigned int> notWorkingSet;
    const unsigned int workingSetDuration = 8;
    unsigned int cyclesWithCurrentWorkingSet = 0;

public:
    Process(unsigned int id, const unsigned int pagesCount);
    Query generateQuery();

private:
    void createWorkingSet();
};


#endif
