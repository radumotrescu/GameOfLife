#pragma once

#include <vector>

class GameOfLife
{
public:
    GameOfLife() = default;
    GameOfLife(const GameOfLife&) = delete;
    GameOfLife& operator=(const GameOfLife&) = delete;
    GameOfLife(GameOfLife&&) = delete;
    GameOfLife& operator=(GameOfLife&&) = delete;

    GameOfLife(int boardSize);
    void SetInitialState(const std::vector<std::pair<int, int>>& aliveCellsAtStart);
    std::vector<std::pair<int, int>> GenNextStateChanges();
    void DoStateChanges(const std::vector<std::pair<int, int>>& cellChanges);

    int BoardSize() const
    {
        return m_boardSize;
    }
    std::vector<bool>& operator[](int index)
    {
        return m_board[index];
    }

    void PrintBoardState();

private:
    const int m_boardSize;
    std::vector<std::vector<bool>> m_board;
};