#pragma once

#include <memory>

#include "chess/game/chessgame.h"

class AI {
    public:
        AI(std::shared_ptr<ChessGame> game);

        int getDepth();
        Move search();
        void setDepth(int depth);
        void setGame(std::shared_ptr<ChessGame> game);
        void stopSearch();
        bool wasStopped() const;

    private:
        struct Private;
        std::shared_ptr<Private> d;

        std::shared_ptr<ChessGame> game;
};
