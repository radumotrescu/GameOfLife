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
    void SetInitialState(const std::vector<std::vector<bool>>& aliveCellsAtStart);
    void SetInitialState(std::vector<std::vector<bool>>&& aliveCellsAtStart);

    std::vector<std::pair<int, int>> GenNextStateChanges() const;
    std::vector<std::pair<int, int>> GenNextStateChanges(int compSize, int nrComp) ;
    void DoStateChanges(const std::vector<std::pair<int, int>>& cellChanges);

    int BoardSize() const
    {
        return m_boardSize;
    }
    std::vector<bool>& operator[](int index)
    {
        return m_board[index];
    }

    void ToggleCellState(const std::pair<int, int>& cell);

    void PrintBoardState();

private:
    const int m_boardSize;
    mutable std::vector<std::vector<bool>> m_board;
};