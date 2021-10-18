#include "gamemanager.h"

#include <condition_variable>
#include <mutex>
#include <thread>

#include <glibmm/dispatcher.h>

#include "chess/board/chessboardbits.h"
#include "gui/board/chessboardarea.h"
#include "gui/util/connectmenuitem.h"
#include "gui/util/connectradiomenuitem.h"


template <typename G>
inline std::shared_ptr<ChessGame> makeChessGame() {
	return std::make_shared<G>(std::make_unique<ChessboardBits>());
}

/******************************************************************************/

struct GameManager::Private {
	Private(GameManager* gm, Glib::RefPtr<Gtk::Builder> const& builder);
	~Private();
	
	GameManager* q;
	
	std::shared_ptr<ChessGame> game;
	GameType type;
	GameMode mode;
	std::shared_ptr<AI> ai;
	
	sigc::signal<void, Move> signalPlay;
	sigc::signal<void> signalUndo;
	sigc::signal<void> signalNewGame;
	sigc::signal<void> signalEndGame;
	Glib::Dispatcher dispAiMove;
	Move aiMove;
	
	std::thread loopTask;
	std::condition_variable loop;
	bool onDestruction;
	
	void gameLoop();
	void updateTurn();
	void play(Move const& move, Piece const& promotion);
	void emitAiMove();
	
	template <GameType T, typename G> void newGame();
	void handle_newGame();
	void handle_newGame960();
	void handle_newGameHill();
	void handle_newGameThreeCheck();
	void handle_newGameHorde();
	void handle_undo();
	
	template <GameMode M> void newMode();
	void handle_radioPvP();
	void handle_radioPvI();
	void handle_radioIvP();
	void handle_radioIvI();
};

/******************************************************************************/

GameManager::Private::Private(GameManager* gm, Glib::RefPtr<Gtk::Builder> const& builder)
: q(gm),
  game(makeChessGame<ChessGame>()),
  type(GameType::Classic),
  mode(GameMode::PvP),
  ai(std::make_shared<AI>(game)),
  onDestruction(false)
{
	connectMenuItem(builder, Private, newGame);
	connectMenuItem(builder, Private, newGame960);
	connectMenuItem(builder, Private, newGameHill);
	connectMenuItem(builder, Private, newGameThreeCheck);
	connectMenuItem(builder, Private, newGameHorde);
	connectMenuItem(builder, Private, undo);
	
	Gtk::RadioMenuItem* p_radioPvP;
	builder->get_widget("radioPvP", p_radioPvP);
	p_radioPvP->signal_activate().connect(sigc::mem_fun(*this, &Private::handle_radioPvP));
	connectRadioMenuItem(builder, *p_radioPvP, Private, radioPvI);
	connectRadioMenuItem(builder, *p_radioPvP, Private, radioIvP);
	connectRadioMenuItem(builder, *p_radioPvP, Private, radioIvI);
	
	dispAiMove.connect(sigc::mem_fun(*this, &Private::emitAiMove));
	
	loopTask = std::thread(&Private::gameLoop, this);
}

// TODO: je crois que c'est pas 100% fiable comme méthode
GameManager::Private::~Private() {
	onDestruction = true;
	ai->stopSearch();
	loop.notify_one();
	loopTask.join();
}

/*----------------------------------------------------------------------------*/

GameManager::GameManager(Glib::RefPtr<Gtk::Builder> const& builder)
: d(std::make_shared<Private>(this, builder))
{}

/******************************************************************************/

void GameManager::Private::gameLoop() {
	std::mutex mtx;
	std::unique_lock<std::mutex> lock(mtx);
	while (not onDestruction) {
		loop.wait(lock);
		if (onDestruction) return;
		aiMove = ai->search();
		if (not ai->wasStopped()) dispAiMove.emit();
	}
}

void GameManager::Private::updateTurn() {
	if (not q->isHumanTurn() and not game->isOver()) {
		loop.notify_one();
	}
}

void GameManager::Private::play(Move const& move, Piece const& promotion) {
	signalPlay.emit(move);
	game->play(move, &promotion);
	if (game->isOver()) {
		signalEndGame.emit();
	}
	updateTurn();
}

void GameManager::Private::emitAiMove() {
	play(aiMove, {PieceName::Queen, game->getTurn()});
}

/******************************************************************************/

void GameManager::play(Move const& move, Piece const& promotion) {
	d->play(move, promotion);
}

bool GameManager::isHumanTurn() const {
	switch (d->mode) {
		case GameMode::PvP:
			return true;
		case GameMode::PvI:
			return d->game->getTurn() == Color::White;
		case GameMode::IvP:
			return d->game->getTurn() == Color::Black;
		case GameMode::IvI:
			return false;
	}
	return false;
}

/******************************************************************************/

std::shared_ptr<AI> GameManager::getAi() const {
	return d->ai;
}

std::shared_ptr<ChessGame const> GameManager::getGame() const {
	return d->game;
}

GameMode GameManager::getMode() const {
	return d->mode;
}

GameType GameManager::getType() const {
	return d->type;
}

sigc::signal<void, Move> GameManager::signal_play() const {
	return d->signalPlay;
}

sigc::signal<void> GameManager::signal_undo() const {
	return d->signalUndo;
}

sigc::signal<void> GameManager::signal_newGame() const {
	return d->signalNewGame;
}

sigc::signal<void> GameManager::signal_endGame() const {
	return d->signalEndGame;
}

/******************************************************************************/

template <GameType T, typename G>
inline void GameManager::Private::newGame() {
	ai->stopSearch();
	if (type != T) {
		type = T;
		game = makeChessGame<G>();
		ai->setGame(game);
	} else {
		game->start();
	}
	signalNewGame.emit();
	updateTurn();
}

void GameManager::Private::handle_newGame() {
	newGame<GameType::Classic, ChessGame>();
}

void GameManager::Private::handle_newGame960() {
	newGame<GameType::Chess960, ChessGame960>();
}

void GameManager::Private::handle_newGameHill() {
	newGame<GameType::Koth, KingOfTheHill>();
}

void GameManager::Private::handle_newGameThreeCheck() {
	newGame<GameType::ThreeCheck, ThreeCheck>();
}

void GameManager::Private::handle_newGameHorde() {
	newGame<GameType::Horde, Horde>();
}

/******************************************************************************/

void GameManager::Private::handle_undo() {
	if (not game->getHistoric().empty()) {
		ai->stopSearch();
		signalUndo.emit();
		game->undo();
	}
}

/******************************************************************************/

template <GameMode M>
inline void GameManager::Private::newMode() {
	mode = M;
	ai->stopSearch();
	updateTurn();
}

void GameManager::Private::handle_radioPvP() {
	newMode<GameMode::PvP>();
}

void GameManager::Private::handle_radioPvI() {
	newMode<GameMode::PvI>();
}

void GameManager::Private::handle_radioIvP() {
	newMode<GameMode::IvP>();
}

void GameManager::Private::handle_radioIvI() {
	newMode<GameMode::IvI>();
}
