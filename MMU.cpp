#include "MMU.h"


TakenPage::TakenPage(unsigned int physicalIndex, const VirtualPage& virtualPage) :
    physicalIndex(physicalIndex), virtualPage(virtualPage) {}


MMU::MMU() {
    for (unsigned int i = 1; i <= physicalPagesCount; i++) freePhysicalPages.insert(i);
}


void MMU::initProcess(const Process& process) {
    table[process.id] = std::vector<PageEntry>(process.pagesCount, PageEntry());
}


void MMU::cleanProcess(const Process& process) {
    const std::vector<PageEntry>& pages = table[process.id];
    unsigned int index = 0;
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

    for (unsigned int i = 0; i < takenPhysicalPages.size(); i++) {
        const TakenPage& takenPage = takenPhysicalPages[i];
        std::cout << ":> Age of " << takenPage.virtualPage <<  " = " << takenPage.age << " = " << std::bitset<16>(takenPage.age) << std::endl;
    }

    // System timer
    static unsigned int ticks = 0;
    if (ticks++ % 4 == 1) {
        for (TakenPage& takenPage : takenPhysicalPages) {
            PageEntry& pageEntry = table[takenPage.virtualPage.id][takenPage.virtualPage.index];
            assert(pageEntry.present);
            takenPage.age = (takenPage.age >> 1) | (pageEntry.accessed ? (1 << (TakenPage::MEMORY - 1)) : 0);
            pageEntry.accessed = false;
        }
    }

    static unsigned int total = 0;
    static unsigned int misses = 0;

    total++;
    std::cout << "  " << "Quering page " << query.virtualPage << " as " << query.type << std::endl;
    PageEntry& pageEntry = table[query.virtualPage.id][query.virtualPage.index];
    if (!pageEntry.present) {
        misses++;

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
        // Don't mark as accessed because we've given it age already
    } else {
        std::cout << "    " << "Page present in RAM" << std::endl;
        pageEntry.accessed = true;
    }

    pageEntry.modified = (query.type == QueryType::Modification);

    std::cout << ":> Miss rate: " << 1.0 * misses / total << std::endl;
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
    const unsigned int physicalPage = *freePhysicalPages.begin(); // any will do
    loadPage(virtualPage, physicalPage);
    return physicalPage;
}

void MMU::loadPage(const VirtualPage& virtualPage, unsigned int physicalPage) {
    PageEntry& pageEntry = table[virtualPage.id][virtualPage.index];
    assert(!pageEntry.present);

    auto it = freePhysicalPages.find(physicalPage);
    assert(it != freePhysicalPages.end());
    freePhysicalPages.extract(it);
    takenPhysicalPages.emplace_back(physicalPage, virtualPage);

    pageEntry.present = true;
    pageEntry.accessed = false;
    pageEntry.modified = false;
    pageEntry.reference = physicalPage;
}


TakenPage MMU::findReplacementPage() {
    assert(takenPhysicalPages.size() > 0);
    unsigned int minAge = (1 << TakenPage::MEMORY) - 1; // init with max possible
    unsigned int minIndex = 0;
    for (unsigned int i = 0; i < takenPhysicalPages.size(); i++) {
        const TakenPage& takenPage = takenPhysicalPages[i];
        if (takenPage.age < minAge) {
            minAge = takenPage.age;
            minIndex = i;
        }
    }

    return takenPhysicalPages[minIndex];
}
