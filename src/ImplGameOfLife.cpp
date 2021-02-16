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

void GameOfLife::SetInitialState(const std::vector<std::pair<int, int>>& aliveCellsAtStart)
{
    for (const auto& [x, y] : aliveCellsAtStart)
    {
        m_board[x][y] = true;
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

StateChanges GameOfLife::GenNextStateChanges() const
{
    auto cellChanges = StateChanges();

    for (int i = 0; i < m_boardSize; i++)
    {
        for (int j = 0; j < m_boardSize; j++)
        {
            auto nrAliveNeighbors = 0;
            auto nrDeadNeighbors = 0;
            for (const auto& [offX, offY] : Offsets)
            {
                if (!CoordsInBoardSize(m_boardSize, i + offX, j + offY))
                    continue;

                if (m_board[i + offX][j + offY])
                    nrAliveNeighbors++;
                else
                    nrDeadNeighbors++;
            }
            if (m_board[i][j]) // cell is alive
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
    }

    return cellChanges;
}

namespace
{
    bool isPowerOfTwo(int n)
    {
        return n > 0 && !(n & (n-1));
    }
}

StateChanges GameOfLife::GenNextStateChanges(int comps, int compIdx)
{
    if (!isPowerOfTwo(comps))
        return {};

    auto cellChanges = StateChanges();

    auto compSize = m_boardSize / comps;

    auto rowSize = comps / 2;
    auto colSize = rowSize / 2;

    for (int i = compIdx * compSize; i < (compIdx + 1) * compSize; i++)
    {
        for (int j = 0; j < m_boardSize; j++)
        {
            auto nrAliveNeighbors = 0;
            auto nrDeadNeighbors = 0;
            for (const auto& [offX, offY] : Offsets)
            {
                if (!CoordsInBoardSize(m_boardSize, i + offX, j + offY))
                    continue;

                if (m_board[i + offX][j + offY])
                    nrAliveNeighbors++;
                else
                    nrDeadNeighbors++;
            }
            if (m_board[i][j]) // cell is alive
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
    }

    return cellChanges;
}

StateChanges GameOfLife::GenNextStateChangesForRow(int row)
{
    auto cellChanges = StateChanges();

    for (int j = 0; j < m_boardSize; j++)
    {
        auto nrAliveNeighbors = 0;
        auto nrDeadNeighbors = 0;
        for (const auto& [offX, offY] : Offsets)
        {
            if (!CoordsInBoardSize(m_boardSize, row + offX, j + offY))
                continue;

            if (m_board[row + offX][j + offY])
                nrAliveNeighbors++;
            else
                nrDeadNeighbors++;
        }
        if (m_board[row][j]) // cell is alive
        {
            if (nrAliveNeighbors <= 1) // if 0 or 1 alive neighbors, the cell dies by solitude
                cellChanges.emplace_back(row, j);
            else if (nrAliveNeighbors >= 4) // if 4 or more alive neighbors, the cell dies by overpopulation
                cellChanges.emplace_back(row, j);
        }
        else                // cell is dead
        {
            if (nrAliveNeighbors == 3)
                cellChanges.emplace_back(row, j);
        }
    }

    return cellChanges;
}


void GameOfLife::DoStateChanges(const std::vector<std::pair<int, int>>& cellChanges)
{
    for (const auto& [x, y] : cellChanges)
    {
        m_board[x][y] = !m_board[x][y];
    }
}

void GameOfLife::PrintBoardState()
{
    for (int i = 0; i < m_boardSize; i++)
    {
        for (int j = 0; j < m_boardSize; j++)
        {
            std::cout << m_board[i][j];
        }
        std::cout << "\n";
    }
    std::cout << "------------------------------------------\n";
}

void GameOfLife::ToggleCellState(const std::pair<int, int>& cell)
{
    m_board[cell.first][cell.second] = !m_board[cell.first][cell.second];
}
