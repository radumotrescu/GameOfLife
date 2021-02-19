#include <ImplGameOfLife.h>

#include <array>
#include <iostream>

namespace
{
    const auto Offsets = std::array<std::pair<int, int>, 8>
    {
        {
            {-1, -1},
            { -1, 0 },
            { -1, 1 },
            { 0, -1 },
            { 0, 1 },
            { 1, -1 },
            { 1, 0 },
            { 1, 1 },
        }
    };

    auto CoordsInBoardSize(int boardSize, int x, int y)
    {
        return !(x < 0 || y < 0 || x >= boardSize || y >= boardSize);
    }
}

GameOfLife::GameOfLife(int boardSize) :m_boardSize(boardSize)
{
    m_board.resize(m_boardSize); // set num rows

    for (auto& row : m_board)
    {
        row.resize(m_boardSize); // set num col
    }
}

auto GameOfLife::at(int x, int y)
{
    return m_board.at(x).at(y);
}

void GameOfLife::SetInitialState(const std::vector<std::pair<int, int>>& aliveCellsAtStart)
{
    for (const auto& [x, y] : aliveCellsAtStart)
    {
        at(x, y) = true;
    }
}

void GameOfLife::SetInitialState(const std::vector<std::vector<bool>>& aliveCellsAtStart)
{
    if (m_board.size() == aliveCellsAtStart.size() && m_board.front().size() == aliveCellsAtStart.front().size())
        m_board = aliveCellsAtStart;
}

void GameOfLife::SetInitialState(std::vector<std::vector<bool>>&& aliveCellsAtStart)
{
    if (m_board.size() == aliveCellsAtStart.size() && m_board.front().size() == aliveCellsAtStart.front().size())
        m_board = aliveCellsAtStart;
}

State GameOfLife::GetState()
{
    return m_board;
}

StateChanges GameOfLife::GenNextStateChanges()
{
    auto cellChanges = StateChanges();

    for (int i = 0; i < m_boardSize; i++)
    {
        for (int j = 0; j < m_boardSize; j++)
        {
            AnalyzeStateChanges(cellChanges, i, j);
        }
    }

    return cellChanges;
}

namespace
{
    bool isPowerOfTwo(int n)
    {
        return n > 0 && !(n & (n - 1));
    }
}

StateChanges GameOfLife::GenNextStateChangesForRow(int row)
{
    auto cellChanges = StateChanges();

    for (int j = 0; j < m_boardSize; j++)
    {
        AnalyzeStateChanges(cellChanges, row, j);
    }

    return cellChanges;
}


void GameOfLife::DoStateChanges(const std::vector<std::pair<int, int>>& cellChanges)
{
    for (const auto& [x, y] : cellChanges)
    {
        at(x, y) = !at(x, y);
    }
}

void GameOfLife::PrintBoardState()
{
    for (int i = 0; i < m_boardSize; i++)
    {
        for (int j = 0; j < m_boardSize; j++)
        {
            std::cout << at(i, j);
        }
        std::cout << "\n";
    }
    std::cout << "------------------------------------------\n";
}

void GameOfLife::ToggleCellState(const std::pair<int, int>& cell)
{
    at(cell.first, cell.second) = !at(cell.first, cell.second);
}

void GameOfLife::AnalyzeStateChanges(StateChanges& cellChanges, int i, int j)
{

    auto nrAliveNeighbors = 0;
    auto nrDeadNeighbors = 0;
    for (const auto& [offX, offY] : Offsets)
    {
        if (!CoordsInBoardSize(m_boardSize, i + offX, j + offY))
            continue;

        if (at(i + offX, j + offY))
            nrAliveNeighbors++;
        else
            nrDeadNeighbors++;
    }
    if (at(i, j)) // cell is alive
    {
        if (nrAliveNeighbors <= 1) // if 0 or 1 alive neighbors, the cell dies by solitude
            cellChanges.emplace_back(i, j);
        else if (nrAliveNeighbors >= 4) // if 4 or more alive neighbors, the cell dies by overpopulation
            cellChanges.emplace_back(i, j);
    }
    else                // cell is dead
    {
        if (nrAliveNeighbors == 3)
            cellChanges.emplace_back(i, j);
    }
}

template<>
StateChanges GameOfLife::GenNextStateChanges<2>(int compIdx)
{
    auto comps = 2;
    auto compSize = m_boardSize / comps;

    auto cellChanges = StateChanges();
    cellChanges.reserve(compSize);
    for (int i = compIdx * compSize; i < (compIdx + 1) * compSize; i++)
    {
        for (int j = 0; j < m_boardSize; j++)
        {
            AnalyzeStateChanges(cellChanges, i, j);
        }
    }

    return cellChanges;
}

template<>
StateChanges GameOfLife::GenNextStateChanges<4>(int compIdx)
{
    auto comps = 4;
    auto valsPerComp = comps / 2;

    auto compSize = m_boardSize / valsPerComp;

    auto rowIdx = compIdx / valsPerComp;
    auto colIdx = compIdx % valsPerComp;

    auto startRowIdx = rowIdx * compSize;
    auto endRowIdx = rowIdx == 0 ? (rowIdx + 1) * compSize : m_boardSize;

    auto startColIdx = colIdx * compSize;
    auto endColIdx = colIdx == 0 ? (colIdx + 1) * compSize : m_boardSize;

    auto cellChanges = StateChanges();
    cellChanges.reserve(compSize);
    for (auto i = startRowIdx; i < endRowIdx; i++)
    {
        for (auto j = startColIdx; j < endColIdx; j++)
        {
            AnalyzeStateChanges(cellChanges, i, j);
        }
    }

    return cellChanges;
}

template<>
StateChanges GameOfLife::GenNextStateChanges<16>(int compIdx)
{
    auto comps = 16;
    auto valsPerComp = comps / 4;

    auto compSize = m_boardSize / valsPerComp;

    auto rowIdx = compIdx / valsPerComp;
    auto colIdx = compIdx % valsPerComp;

    auto startRowIdx = rowIdx * compSize;
    auto endRowIdx = rowIdx != (valsPerComp - 1) ? (rowIdx + 1) * compSize : m_boardSize;

    auto startColIdx = colIdx * compSize;
    auto endColIdx = colIdx != (valsPerComp - 1) ? (colIdx + 1) * compSize : m_boardSize;

    auto cellChanges = StateChanges();
    cellChanges.reserve(compSize);
    for (auto i = startRowIdx; i < endRowIdx; i++)
    {
        for (auto j = startColIdx; j < endColIdx; j++)
        {
            AnalyzeStateChanges(cellChanges, i, j);
        }
    }

    return cellChanges;
}
