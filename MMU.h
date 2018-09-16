#ifndef MMU_H
#define MMU_H

#include <set>
#include <vector>
#include <unordered_map>
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
    TakenPage(unsigned int physicalIndex, VirtualPage virtualPage);
};


class MMU {
private:
    const unsigned int physicalPagesCount = 10;
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
    unsigned int loadPage(const VirtualPage& virtualPage);
    void loadPage(const VirtualPage& virtualPage, unsigned int reference);
};


#endif
