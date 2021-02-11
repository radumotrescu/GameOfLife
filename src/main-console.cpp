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
    gol.DoStateChanges(gol.GenNextStateChanges());
    auto elapsed = timer.Elapsed();

    std::cout << elapsed << " milliseconds";

    return 0;
}
