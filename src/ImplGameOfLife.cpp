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

std::vector<std::pair<int, int>> GameOfLife::GenNextStateChanges()
{
    auto cellChanges = std::vector<std::pair<int, int>>();

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
