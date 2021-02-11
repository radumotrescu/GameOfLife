#include <stdio.h>

#include <ImplGameOfLife.h>
#include <Semaphore.h>
#include <thread>
#include <vector>

#include <chrono>
#include <random>

#include <TestUtils.h>

static Semaphore mutex(1);
static Semaphore t1(0);
static Semaphore t2(1);
static int count = 0;
static int n = 2;

void barrier(GameOfLife& gol, int nComps, int compIdx)
{
    for (auto generation = 0; generation < 2; generation++)
    {
        auto stateChange = gol.GenNextStateChanges(nComps, compIdx);
        mutex.wait();
        count++;

        if (count == n)
        {
            t2.wait();
            t1.notify();
        }
        mutex.notify();

        t1.wait();
        t1.notify();

        gol.DoStateChanges(stateChange);

        mutex.wait();
        count--;

        if (count == 0)
        {
            t1.wait();
            t2.notify();
        }
        mutex.notify();
        t2.wait();
        t2.notify();
    }
}

void workThread(GameOfLife& gol, int nComps, int compIdx)
{
    for (auto generation = 0; generation < 2; generation++)
    {
        auto stateChange = gol.GenNextStateChanges(nComps, compIdx);
        gol.DoStateChanges(stateChange);
    }
}

void cellSwap(GameOfLife& gol, int y, int x)
{
    mutex.wait();

    gol.ToggleCellState({ x, y });

    mutex.notify();
}

int main()
{
    bool show_demo_window = false;
    bool show_another_window = false;
    bool showGameOfLifeWindow = false;
    auto runSimulation = false;
    auto doNextIteration = false;

    unsigned seed = 5;
    std::default_random_engine generator(seed);

    std::uniform_int_distribution<int> distribution(0, 1);

    auto boardSize = 10000;

    auto initialBoard = std::vector<std::vector<bool>>(boardSize);
    for (auto& row : initialBoard)
        row.resize(boardSize);

    for (int i = 0; i < boardSize; ++i)
    {
        for (int j = 0; j < boardSize; ++j)
            initialBoard[i][j] = static_cast<bool>(distribution(generator));
    }

    auto gol = GameOfLife(boardSize);
    //gol.SetInitialState({
    //    {1, 1},
    //    {2, 1},
    //    {3, 1},
    //    {31, 30},
    //    {31, 31},
    //    {31, 32},
    //    });

    gol.SetInitialState(initialBoard);

    TestUtils::Timer timer;
    for (int generation = 0; generation < 2; generation++)
        gol.DoStateChanges(gol.GenNextStateChanges());
    auto elapsed = timer.Elapsed();
    std::cout << "main thread time: " << elapsed << " milliseconds";

    auto noThreadStage = gol.GetState();

    std::cout << "\n";
    gol.SetInitialState(initialBoard);

    timer.Reset();
    auto vecThread = std::vector<std::thread>();
    for (int i = 0; i < 2; i++)
    {
        vecThread.emplace_back(barrier, std::ref(gol), 2, i);
    }
    for (int i = 0; i < 2; i++)
    {
        vecThread[i].join();
    }
    elapsed = timer.Elapsed();
    std::cout << "two thread time: " << elapsed << " milliseconds";

    std::cout << "\n";
    auto twoThreadState = gol.GetState();
    if (twoThreadState == noThreadStage)
        std::cout << "states are equal";
    else
        std::cout << "states are not equal";

    return 0;
}
