#include <stdio.h>
#include <thread>
#include <vector>
#include <chrono>
#include <random>

#include <ImplGameOfLife.h>
#include <ThreadUtils.h>

#include <TestUtils.h>

static Semaphore mutex(1);
static Semaphore t1(0);
static Semaphore t2(2);
static int count = 0;
static int n = 2;

static auto boardSize = 5000;

static Barrier BarrierHalf(2);

void barierHalf(GameOfLife& gol, int nComps, int compIdx)
{
    for (auto generation = 0; generation < 2; generation++)
    {
        auto stateChange = gol.GenNextStateChanges(nComps, compIdx);
        
        BarrierHalf.phase1();
        gol.DoStateChanges(stateChange);
        BarrierHalf.phase2();
    }
}

static Barrier barrier(boardSize);

void barierRow(GameOfLife& gol, int row)
{
    for (auto generation = 0; generation < 2; generation++)
    {
        auto stateChange = gol.GenNextStateChangesForRow(row);
        barrier.phase1();
        gol.DoStateChanges(stateChange);
        barrier.phase2();
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
    unsigned seed = 5;
    std::default_random_engine generator(seed);

    std::uniform_int_distribution<int> distribution(0, 1);

    auto initialBoard = std::vector<std::vector<bool>>(boardSize);
    for (auto& row : initialBoard)
        row.resize(boardSize);

    for (int i = 0; i < boardSize; ++i)
    {
        for (int j = 0; j < boardSize; ++j)
            initialBoard[i][j] = static_cast<bool>(distribution(generator));
    }

    auto elapsed = 0ll;
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
    //for (int generation = 0; generation < 2; generation++)
    //    gol.DoStateChanges(gol.GenNextStateChanges());
    //auto elapsed = timer.Elapsed();
    //std::cout << "main thread time: " << elapsed << " milliseconds";

    //auto noThreadStage = gol.GetState();

    //std::cout << "\n";

    //gol.PrintBoardState();
    auto stateChanges = std::vector<StateChanges>();
    for (int generation = 0; generation < 2; generation++)
    {
        for (int row = 0; row < boardSize; row++)
        {
            stateChanges.push_back(gol.GenNextStateChangesForRow(row));
        }

        for (const auto& stateChange : stateChanges)
            gol.DoStateChanges(stateChange);

        stateChanges.clear();
    }
    elapsed = timer.Elapsed();
    std::cout << "main thread time: " << elapsed << " milliseconds\n";

    auto noThreadStage = gol.GetState();
    //gol.PrintBoardState();

    std::cout << "\n";

    gol.SetInitialState(initialBoard);

    timer.Reset();
    {
        auto vecThread = std::vector<std::thread>();
        for (int i = 0; i < 2; i++)
        {
            vecThread.emplace_back(barierHalf, std::ref(gol), 2, i);
        }
        for (int i = 0; i < 2; i++)
        {
            vecThread[i].join();
        }
        elapsed = timer.Elapsed();
        std::cout << "two thread time: " << elapsed << " milliseconds";
    }

    auto twoThreadState = gol.GetState();
    std::cout << "\n";
    if (twoThreadState == noThreadStage)
        std::cout << "states are equal";
    else
        std::cout << "states are not equal";

    std::cout << "\n";
    gol.SetInitialState(initialBoard);
    auto vecThread = std::vector<std::thread>();

    timer.Reset();
    for (int i = 0; i < boardSize; i++)
    {
        vecThread.emplace_back(barierRow, std::ref(gol), i);
    }
    for (auto i = 0; i < vecThread.size(); i++)
    {
        vecThread[i].join();
    }
    elapsed = timer.Elapsed();
    std::cout << "one thread per row time: " << elapsed << " milliseconds";

    std::cout << "\n";
    auto rowThreadState = gol.GetState();
    if (rowThreadState == noThreadStage)
        std::cout << "states are equal";
    else
        std::cout << "states are not equal";

    std::cout << "\n";
    //gol.PrintBoardState();


    return 0;
}
