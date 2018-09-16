#include <unordered_map>
#include "utility.h"
#include "Process.h"
#include "MMU.h"
 

int main() {
    MMU mmu;
    std::vector<Process*> pool;
    const unsigned int startingPoolSize = 5;
    unsigned int id;
    for (id = 1; id <= startingPoolSize; id++) {
        const unsigned int pagesCount = randInt(4, 16);
        Process* newProcess = new Process(id, pagesCount);
        pool.push_back(newProcess);
        mmu.initProcess(*newProcess);
    }

    const unsigned int CYCLES_TO_SIMULATE = 555;
    for (unsigned int cycle = 0; cycle < CYCLES_TO_SIMULATE; cycle++) {
        std::cout << "-------- Cycle " << cycle << " ----------" << std::endl;

        if (const bool removeProcess = randInt(0,10) == 1; removeProcess) {
            if (pool.size() > 1) {
                const unsigned int index = randInt(0, pool.size() - 1);
                Process* toDelete = pool[index];
                std::cout << ">>>>>>>>> Removing process<" << toDelete->id << ">"
                    << "(now " << (pool.size() - 1) << ")" << std::endl;
                pool.erase(pool.begin() + index);
                delete toDelete;
            }
        }
        if (const bool addProcess = randInt(0,10) == 1; addProcess) {
            std::cout << ">>>>>>>>> Spawning a new process" << "(now " << (pool.size()+1) << ")" << std::endl;
            const unsigned int pagesCount = randInt(4, 16);
            Process* newProcess = new Process(id++, pagesCount);
            pool.push_back(newProcess);
            mmu.initProcess(*newProcess);
        }

        Process* workingProcess = pool[randInt(0, pool.size() - 1)];
        mmu.queryPage(workingProcess->generateQuery());
    }


    return 0;
}
