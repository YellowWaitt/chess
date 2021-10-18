#pragma once

#include <memory>

#include <gtkmm/builder.h>
#include <sigc++/sigc++.h>

#include "chess/ai/ai.h"
#include "chess/game/games.h"


enum class GameMode {
	PvP, PvI, IvP, IvI
};

class GameManager {
	public:
		GameManager(Glib::RefPtr<Gtk::Builder> const& builder);
		
		bool isHumanTurn() const;
		std::shared_ptr<AI> getAi() const;
		std::shared_ptr<ChessGame const> getGame() const;
		GameMode getMode() const;
		GameType getType() const;
		void play(Move const& move, Piece const& promotion);
		sigc::signal<void> signal_endGame() const;
		sigc::signal<void> signal_newGame() const;
		sigc::signal<void, Move> signal_play() const;
		sigc::signal<void> signal_undo() const;
	
	private:
		struct Private;
		std::shared_ptr<Private> d;
};
