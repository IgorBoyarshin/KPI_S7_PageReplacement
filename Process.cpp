#include "Process.h"


VirtualPage::VirtualPage(unsigned int id, unsigned int index) : id(id), index(index) {}


Query::Query(const VirtualPage& virtualPage, const QueryType& type) : virtualPage(virtualPage), type(type) {}


bool operator==(const VirtualPage& lhs, const VirtualPage& rhs) {
    return lhs.id == rhs.id && lhs.index == rhs.index;
}


Process::Process(unsigned int id, const unsigned int pagesCount) : id(id), pagesCount(pagesCount) {
    assert(id > 0 && "Process initialized with invalid id");
    std::cout << "Creating new Process<" << id << "> with pagesCount=" << pagesCount << std::endl;
    createWorkingSet();
};


Query Process::generateQuery() {
    if (cyclesWithCurrentWorkingSet++ >= workingSetDuration) {
        std::cout << "Changing working set for <" << id << ">" << std::endl;
        cyclesWithCurrentWorkingSet = 1;
        createWorkingSet(); // new
    }

    const QueryType queryType = (randInt(0, 1) == 0) ? QueryType::Read : QueryType::Modification;
    const bool fromWorkingSet = randInt(1, 10) <= 9; // 90%
    std::set<unsigned int>::const_iterator it = (fromWorkingSet ? workingSet : notWorkingSet).begin();
    const unsigned int pageIndex = randInt(0,   (fromWorkingSet ? workingSet : notWorkingSet).size() - 1);
    advance(it, pageIndex);

    const Query query(VirtualPage(id, *it), queryType);
    std::cout << "Generated query"
        << (fromWorkingSet ? " (working set)" : "(not working set)")
        << ": " << query << std::endl;

    return query;
}


void Process::createWorkingSet() {
    const unsigned int workingSetSize = pagesCount / 4;

    workingSet.clear();
    for (unsigned int i = 0; i < workingSetSize; i++) {
        const unsigned int index = randInt(0, pagesCount - 1);
        workingSet.insert(index);
    }
    notWorkingSet.clear();
    for (unsigned int i = 0; i < pagesCount; i++) {
        if (workingSet.find(i) == workingSet.end()) {
            notWorkingSet.insert(i);
        }
    }

    std::cout << "Now has working set: ";
    for (auto it = workingSet.begin(); it != workingSet.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << "Now has not working set: ";
    for (auto it = notWorkingSet.begin(); it != notWorkingSet.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}


std::ostream& operator<<(std::ostream& os, const VirtualPage& virtualPage) {
    std::cout << "<" << virtualPage.id << "," << virtualPage.index << ">";
    return os;
}

std::ostream& operator<<(std::ostream& os, const QueryType& type) {
    switch (type) {
        case QueryType::Modification:
            os << "Modification";
            break;
        case QueryType::Read:
            os << "Read";
            break;
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const Query& query) {
    os << "Query" << query.virtualPage << "(" << query.type << ")";
    return os;
}
