#pragma once

#include <vector>
#include <ImplGameOfLife.h>

using State_Contiguous= std::vector<bool>;

class GameOfLife_Contiguous
{
public:
    GameOfLife_Contiguous() = default;
    GameOfLife_Contiguous(const GameOfLife_Contiguous&) = delete;
    GameOfLife_Contiguous& operator=(const GameOfLife_Contiguous&) = delete;
    GameOfLife_Contiguous(GameOfLife_Contiguous&&) = delete;
    GameOfLife_Contiguous& operator=(GameOfLife_Contiguous&&) = delete;

    GameOfLife_Contiguous(int boardSize);

    void SetInitialState(const std::vector<std::pair<int, int>>& aliveCellsAtStart);
    void SetInitialState(const std::vector<std::vector<bool>>& aliveCellsAtStart);
    void SetInitialState(std::vector<std::vector<bool>>&& aliveCellsAtStart);

    State_Contiguous GetState();

    auto at(int x, int y);

    StateChanges GenNextStateChanges();

    template<int compSize>
    StateChanges GenNextStateChanges(int nrComp);
    StateChanges GenNextStateChangesForRow(int row);

    void DoStateChanges(const std::vector<std::pair<int, int>>& cellChanges);

    int BoardSize() const
    {
        return m_boardSize;
    }

    void ToggleCellState(const std::pair<int, int>& cell);

    void PrintBoardState();
private:

    void AnalyzeStateChanges(StateChanges& stateChanges, int i, int j);

    const int m_boardSize;
    mutable State_Contiguous m_board;
};

template<>
StateChanges GameOfLife_Contiguous::GenNextStateChanges<2>(int compIdx);

template<>
StateChanges GameOfLife_Contiguous::GenNextStateChanges<4>(int compIdx);

template<>
StateChanges GameOfLife_Contiguous::GenNextStateChanges<16>(int compIdx);