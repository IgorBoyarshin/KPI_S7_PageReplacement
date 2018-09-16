#include "MMU.h"


TakenPage::TakenPage(unsigned int physicalIndex, VirtualPage virtualPage) :
    physicalIndex(physicalIndex), virtualPage(virtualPage) {}


MMU::MMU() {
    for (unsigned int i = 1; i <= physicalPagesCount; i++) {
        freePhysicalPages.insert(i);
    }
}


void MMU::initProcess(const Process& process) {
    std::vector<PageEntry> pages(process.pagesCount, PageEntry());
    table[process.id] = pages;
}


void MMU::cleanProcess(const Process& process) {
    std::vector<PageEntry>& pages = table[process.id];
    auto index = 0;
    for (auto& page : pages) {
        if (page.present) {
            unloadPage(VirtualPage(process.id, index));
        }
        index++;
    }
    table.erase(process.id);
}


void MMU::queryPage(const Query& query) {
    assert(table.find(query.virtualPage.id) != table.end());

    static auto missCount = 0;
    static auto totalCount = 0;

    std::cout << "  " << "Quering page " << query.virtualPage << " as " << query.type << std::endl;
    PageEntry& pageEntry = table[query.virtualPage.id][query.virtualPage.index];
    totalCount++;
    if (!pageEntry.present) {
        missCount++;
        std::cout << "    " << "Page fault. Loading into RAM from disk" << std::endl;
        if (freePhysicalPages.empty()) {
            std::cout << "    " << "No free physical pages, finding replacement..." << std::endl;
            const TakenPage replacement = findReplacementPage();
            std::cout << "    " << "Replace with: " << replacement.virtualPage << std::endl;
            unloadPage(replacement.virtualPage);
            loadPage(query.virtualPage, replacement.physicalIndex);
        } else {
            std::cout << "    " << "Exists a free physical page: "
                                << loadPage(query.virtualPage) << std::endl;
            std::cout << "    " << "Assigning it" << std::endl;
        }
        std::cout << "    " << "Repeating query..." << std::endl;
    } else {
        std::cout << "    " << "Page present in RAM" << std::endl;
    }

    pageEntry.accessed = true;
    pageEntry.modified = (query.type == QueryType::Modification);

    std::cout << ":> Miss rate: " << 1.0 * missCount / totalCount << std::endl;
}


TakenPage MMU::findReplacementPage() {
    assert(takenPhysicalPages.size() > 0);
    return takenPhysicalPages[randInt(0, takenPhysicalPages.size() - 1)];
}


void MMU::unloadPage(const VirtualPage& virtualPage) {
    PageEntry& pageEntry = table[virtualPage.id][virtualPage.index];
    assert(pageEntry.present);

    pageEntry.present = false;
    pageEntry.accessed = false;
    pageEntry.reference = 0;
    if (pageEntry.modified) {
        pageEntry.modified = false;
        std::cout << "      " << "Writing changes to disk for " << virtualPage << std::endl;
    }

    for (auto it = takenPhysicalPages.begin(); it != takenPhysicalPages.end(); ++it) {
        if (it->virtualPage == virtualPage) {
            freePhysicalPages.insert(it->physicalIndex);
            takenPhysicalPages.erase(it);
            break;
        }
    }
}

unsigned int MMU::loadPage(const VirtualPage& virtualPage) {
    const unsigned int reference = *freePhysicalPages.begin(); // any will do
    loadPage(virtualPage, reference);
    return reference;
}

void MMU::loadPage(const VirtualPage& virtualPage, unsigned int reference) {
    PageEntry& pageEntry = table[virtualPage.id][virtualPage.index];
    assert(!pageEntry.present);

    auto it = freePhysicalPages.find(reference);
    assert(it != freePhysicalPages.end());
    freePhysicalPages.extract(it);
    takenPhysicalPages.emplace_back(reference, virtualPage);

    pageEntry.present = true;
    pageEntry.accessed = false;
    pageEntry.modified = false;
    pageEntry.reference = reference;
}
