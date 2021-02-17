#pragma once

#include <vector>

using StateChange = std::pair<int, int>;
using StateChanges = std::vector<StateChange>;
using State = std::vector<std::vector<bool>>;

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

    State GetState();

    StateChanges GenNextStateChanges() const;

    template<int compSize>
    StateChanges GenNextStateChanges(int nrComp);
    StateChanges GenNextStateChangesForRow(int row);

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
    mutable State m_board;
};

template<>
StateChanges GameOfLife::GenNextStateChanges<2>(int compIdx);

template<>
StateChanges GameOfLife::GenNextStateChanges<4>(int compIdx);