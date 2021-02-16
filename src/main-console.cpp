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

static auto boardSize = 1000;

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

std::vector<std::vector<bool>> InitialBoard()
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
    return initialBoard;
}

State GenericImplementation(GameOfLife& gol)
{
    gol.SetInitialState(InitialBoard());
    TestUtils::Timer timer;
    for (int generation = 0; generation < 2; generation++)
        gol.DoStateChanges(gol.GenNextStateChanges());
    auto elapsed = timer.Elapsed();
    std::cout << "main thread time, generic implementation: " << elapsed << " milliseconds\n";
    return gol.GetState();
}

State MainThreadOneRow(GameOfLife& gol)
{
    gol.SetInitialState(InitialBoard());

    TestUtils::Timer timer;
    timer.Reset();
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
    auto elapsed = timer.Elapsed();
    std::cout << "main thread time, one row at a time: " << elapsed << " milliseconds\n";
    return gol.GetState();
}

State TwoThreads(GameOfLife& gol)
{
    gol.SetInitialState(InitialBoard());
    TestUtils::Timer timer;
    auto vecThread = std::vector<std::thread>();
    for (int i = 0; i < 2; i++)
    {
        vecThread.emplace_back(barierHalf, std::ref(gol), 2, i);
    }
    for (int i = 0; i < 2; i++)
    {
        vecThread[i].join();
    }
    auto elapsed = timer.Elapsed();
    std::cout << "two thread time: " << elapsed << " milliseconds\n";

    return gol.GetState();
}

State OneThreadOneRow(GameOfLife& gol)
{
    gol.SetInitialState(InitialBoard());
    auto vecThread = std::vector<std::thread>();

    TestUtils::Timer timer;
    timer.Reset();
    for (int i = 0; i < boardSize; i++)
    {
        vecThread.emplace_back(barierRow, std::ref(gol), i);
    }
    for (auto i = 0; i < vecThread.size(); i++)
    {
        vecThread[i].join();
    }
    auto elapsed = timer.Elapsed();
    std::cout << "one thread per row time: " << elapsed << " milliseconds\n";
    return gol.GetState();
}

int main()
{
    auto gol = GameOfLife(boardSize);

    auto genericImplementationState = GenericImplementation(gol);
    auto oneRowState = MainThreadOneRow(gol);

    auto twoThreadState = TwoThreads(gol);

    if (twoThreadState == genericImplementationState)
        std::cout << "states are equal\n";
    else
        std::cout << "states are not equal\n";

    auto rowThreadState = OneThreadOneRow(gol);

    if (rowThreadState == genericImplementationState)
        std::cout << "states are equal\n";
    else
        std::cout << "states are not equal\n";

    return 0;
}
