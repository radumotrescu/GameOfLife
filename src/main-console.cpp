#include <stdio.h>
#include <thread>
#include <vector>
#include <chrono>
#include <random>

#include <ImplGameOfLife.h>
#include <ImplGameOfLife_Contiguous.h>
#include <ThreadUtils.h>

#include <TestUtils.h>

static Semaphore mutex(1);
static Semaphore t1(0);
static Semaphore t2(2);
static int count = 0;
static int n = 2;

static auto boardSize = 30000;
static auto numGenerations = 1;

static Barrier barrier(boardSize);

void barrierRow(GameOfLife& gol, int row)
{
    for (auto generation = 0; generation < numGenerations; generation++)
    {
        auto stateChange = gol.GenNextStateChangesForRow(row);
        barrier.phase1();
        gol.DoStateChanges(stateChange);
        barrier.phase2();
    }
}

std::vector<std::vector<bool>>& InitialBoard()
{
    unsigned seed = 5;
    std::default_random_engine generator(seed);

    std::uniform_int_distribution<int> distribution(0, 1);

    static auto initialBoard = std::vector<std::vector<bool>>(boardSize);
    static auto isInit = false;
    if (!isInit)
    {
        for (auto& row : initialBoard)
            row.resize(boardSize);

        for (int i = 0; i < boardSize; ++i)
        {
            for (int j = 0; j < boardSize; ++j)
                initialBoard[i][j] = static_cast<bool>(distribution(generator));
        }
        isInit = true;
    }
    return initialBoard;
}

State GenericImplementation(GameOfLife& gol)
{
    gol.SetInitialState(InitialBoard());
    TestUtils::Timer timer;
    for (int generation = 0; generation < numGenerations; generation++)
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
    for (int generation = 0; generation < numGenerations; generation++)
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

void barrierTwo(GameOfLife& gol, int compIdx)
{
    static Barrier BarrierHalf(2);
    for (auto generation = 0; generation < numGenerations; generation++)
    {
        auto stateChange = gol.GenNextStateChanges<2>(compIdx);
        BarrierHalf.phase1();
        mutex.wait();
        gol.DoStateChanges(stateChange);
        mutex.notify();
        BarrierHalf.phase2();
    }
}
State TwoThreads(GameOfLife& gol)
{
    gol.SetInitialState(InitialBoard());
    TestUtils::Timer timer;
    auto vecThread = std::vector<std::thread>();
    for (int i = 0; i < 2; i++)
    {
        vecThread.emplace_back(barrierTwo, std::ref(gol), i);
    }
    for (auto& vec: vecThread)
    {
        vec.join();
    }
    auto elapsed = timer.Elapsed();
    std::cout << "two threads time: " << elapsed << " milliseconds\n";

    return gol.GetState();
}

State MainThreadFourComps(GameOfLife& gol)
{
    gol.SetInitialState(InitialBoard());

    TestUtils::Timer timer;
    auto stateChanges = std::vector<StateChanges>();
    for (int generation = 0; generation < numGenerations; generation++)
    {
        for (int compIdx = 0; compIdx < 4; compIdx++)
        {
            stateChanges.push_back(gol.GenNextStateChanges<4>(compIdx));
        }

        for (const auto& stateChange : stateChanges)
            gol.DoStateChanges(stateChange);

        stateChanges.clear();
    }
    auto elapsed = timer.Elapsed();
    std::cout << "main thread time, four comps: " << elapsed << " milliseconds\n";
    //gol.PrintBoardState();
    return gol.GetState();
}

void barrierFour(GameOfLife& gol, int compIdx)
{
    static Barrier bQuart(4);
    for (auto generation = 0; generation < numGenerations; generation++)
    {
        auto stateChange = gol.GenNextStateChanges<4>(compIdx);
        bQuart.phase1();
        mutex.wait();
        gol.DoStateChanges(stateChange);
        mutex.notify();
        bQuart.phase2();
    }
}

State FourThreads(GameOfLife& gol)
{
    gol.SetInitialState(InitialBoard());
    TestUtils::Timer timer;
    auto vecThread = std::vector<std::thread>();
    for (int i = 0; i < 4; i++)
    {
        vecThread.emplace_back(barrierFour, std::ref(gol), i);
    }
    for (auto& vec: vecThread)
    {
        vec.join();
    }
    auto elapsed = timer.Elapsed();
    std::cout << "four threads time: " << elapsed << " milliseconds\n";
    //gol.PrintBoardState();

    return gol.GetState();
}

void barrierSixteen(GameOfLife& gol, int compIdx)
{
    static Barrier bSixteen(16);
    for (auto generation = 0; generation < numGenerations; generation++)
    {
        auto stateChange = gol.GenNextStateChanges<16>(compIdx);
        bSixteen.phase1();
        mutex.wait();
        gol.DoStateChanges(stateChange);
        mutex.notify();
        bSixteen.phase2();
    }
}

State SixteenThreads(GameOfLife& gol)
{
    gol.SetInitialState(InitialBoard());
    TestUtils::Timer timer;
    auto vecThread = std::vector<std::thread>();
    for (int i = 0; i < 16; i++)
    {
        vecThread.emplace_back(barrierSixteen, std::ref(gol), i);
    }
    for (auto& vec: vecThread)
    {
        vec.join();
    }
    auto elapsed = timer.Elapsed();
    std::cout << "sixteen threads time: " << elapsed << " milliseconds\n";
    //gol.PrintBoardState();

    return gol.GetState();
}

void barrierSixteen_contigous(GameOfLife_Contiguous& gol, int compIdx)
{
    static Barrier bSixteen(16);
    for (auto generation = 0; generation < numGenerations; generation++)
    {
        auto stateChange = gol.GenNextStateChanges<16>(compIdx);
        bSixteen.phase1();
        mutex.wait();
        gol.DoStateChanges(stateChange);
        mutex.notify();
        bSixteen.phase2();
    }
}

State_Contiguous SixteenThreads(GameOfLife_Contiguous& gol)
{
    gol.SetInitialState(InitialBoard());
    TestUtils::Timer timer;
    auto vecThread = std::vector<std::thread>();
    for (int i = 0; i < 16; i++)
    {
        vecThread.emplace_back(barrierSixteen_contigous, std::ref(gol), i);
    }
    for (auto& vec: vecThread)
    {
        vec.join();
    }
    auto elapsed = timer.Elapsed();
    std::cout << "sixteen threads time: " << elapsed << " milliseconds\n";
    //gol.PrintBoardState();

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
        vecThread.emplace_back(barrierRow, std::ref(gol), i);
    }
    for (auto& vec: vecThread)
    {
        vec.join();
    }
    auto elapsed = timer.Elapsed();
    std::cout << "one thread per row time: " << elapsed << " milliseconds\n";
    return gol.GetState();
}

int main()
{
    auto gol = GameOfLife(boardSize);
    //gol.SetInitialState(InitialBoard());
    //gol.PrintBoardState();
    std::cout << boardSize << " x " << boardSize << " grid\n";

    auto genericImplementationState = GenericImplementation(gol);
    //auto oneRowState = MainThreadOneRow(gol);

    auto twoThreadState = TwoThreads(gol);
    if (twoThreadState == genericImplementationState)
        std::cout << "states are equal\n";
    else
        std::cout << "states are not equal\n";

    //auto fourComps = MainThreadFourComps(gol);
    //if (fourComps == genericImplementationState)
    //    std::cout << "states are equal\n";
    //else
    //    std::cout << "states are not equal\n";

    auto fourThreadState = FourThreads(gol);
    if (fourThreadState == genericImplementationState)
        std::cout << "states are equal\n";
    else
        std::cout << "states are not equal\n";

    auto sixteenThreadState = SixteenThreads(gol);
    if (sixteenThreadState == genericImplementationState)
        std::cout << "states are equal\n";
    else
        std::cout << "states are not equal\n";

    auto gol_contigous = GameOfLife_Contiguous(boardSize);

    auto sixteenThreadState_contigous = SixteenThreads(gol_contigous);
    //if (sixteenThreadState_contigous == genericImplementationState)
    //    std::cout << "states are equal\n";
    //else
    //    std::cout << "states are not equal\n";

    //auto rowThreadState = OneThreadOneRow(gol);
    //if (rowThreadState == genericImplementationState)
    //    std::cout << "states are equal\n";
    //else
    //    std::cout << "states are not equal\n";

    return 0;
}
