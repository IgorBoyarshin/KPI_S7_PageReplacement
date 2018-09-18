#ifndef MMU_H
#define MMU_H

#include <set>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <bitset>
#include "utility.h"
#include "Process.h"


struct PageEntry {
    bool present = false;
    bool accessed = false;
    bool modified = false;
    unsigned int reference = 0;
};


struct TakenPage {
    unsigned int physicalIndex;
    VirtualPage virtualPage;

    static const unsigned int MEMORY = 16;
    unsigned int age = 1 << (MEMORY - 1);

    TakenPage(unsigned int physicalIndex, const VirtualPage& virtualPage);
};


class MMU {
private:
    const unsigned int physicalPagesCount = 15;
    std::set<unsigned int> freePhysicalPages;
    std::vector<TakenPage> takenPhysicalPages;
    // processId, virtPageNumber
    std::unordered_map<unsigned int, std::vector<PageEntry>> table;

public:
    MMU();
    void initProcess(const Process& process);
    void cleanProcess(const Process& process);
    void queryPage(const Query& query);

private:
    TakenPage findReplacementPage();
    void unloadPage(const VirtualPage& virtualPage);
    void loadPage(const VirtualPage& virtualPage, unsigned int reference);
    unsigned int loadPage(const VirtualPage& virtualPage);
};


#endif
