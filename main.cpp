#include <unordered_map>
#include "utility.h"
#include "Process.h"
#include "MMU.h"


int main() {
    static const auto addProcess = [](MMU& mmu, std::vector<Process*>& pool, unsigned int id){
        const unsigned int mean = 10;
        const unsigned int deviation = 4;
        const unsigned int pagesCount = randInt(mean - deviation, mean + deviation);
        Process* newProcess = new Process(id, pagesCount);
        mmu.initProcess(*newProcess);
        pool.push_back(newProcess);
    };
    static const auto removeProcess = [](MMU& mmu, std::vector<Process*>& pool, unsigned int index) {
        Process* toDelete = pool[index];
        pool.erase(pool.begin() + index);
        mmu.cleanProcess(*toDelete);
        const unsigned int id = toDelete->id;
        delete toDelete;
        return id;
    };

    MMU mmu;
    std::vector<Process*> pool;
    const unsigned int poolSizeMean = 5;
    unsigned int id;
    for (id = 1; id <= poolSizeMean; id++) addProcess(mmu, pool, id);

    const unsigned int CYCLES_TO_SIMULATE = 555;
    for (unsigned int cycle = 0; cycle < CYCLES_TO_SIMULATE; cycle++) {
        std::cout << "-------- Cycle " << cycle << " ----------" << std::endl;

        Process* workingProcess = pool[randInt(0, pool.size() - 1)];
        mmu.queryPage(workingProcess->generateQuery());

        // Remove or spawn new process
        if (const bool changeProcessCount = randInt(0, 20) == 1; changeProcessCount) {
            const float removalProbability = 1.0f * pool.size() / (pool.size() + poolSizeMean);
            if (const bool doRemove = randInt(0, 99) < static_cast<unsigned int>(removalProbability * 100.0f);
                    doRemove) {
                if (pool.size() > 1) {
                    const unsigned int index = randInt(0, pool.size() - 1);
                    const unsigned int id = removeProcess(mmu, pool, index);
                    std::cout << ">>>>>>>>> Removing process<" << id << ">"
                        << "(now " << pool.size() << ")" << std::endl;
                }
            } else {
                addProcess(mmu, pool, id++);
                std::cout << ">>>>>>>>> Spawning a new process"
                    << "(now " << pool.size() << ")" << std::endl;
            }
        }
    }

    return 0;
}
