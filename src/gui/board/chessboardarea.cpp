#include "chessboardarea.h"

#include <algorithm>
#include <complex>
#include <map>
#include <vector>

#include <gdkmm/general.h>
#include <glibmm/binding.h>

#include "gui/dialogs/endgamedialog.h"
#include "gui/util/connectcheckmenuitem.h"
#include "gui/util/connectmenuitem.h"
#include "gui/util/drawpiece.h"
#include "gui/util/drawutil.h"
#include "piecestaken.h"
#include "promotiondialog.h"
#include "util/sgn.h"
#include "comparator.h"


#define LEFT  0
#define RIGHT 1

/******************************************************************************/

struct ChessboardArea::Private {
	Private(ChessboardArea* cba,
		Glib::RefPtr<Gtk::Builder> const& builder,
		std::shared_ptr<ThemeManager const> theme,
		std::shared_ptr<GameManager> game);
	
	ChessboardArea* q;
	
	struct {
		double x0, y0;
		std::shared_ptr<double> size;
	} p;
	
	std::shared_ptr<ThemeManager const> theme;
	std::shared_ptr<GameManager> game;
	PiecesTaken* piecesTaken[2];
	
	Square selected[2];
	bool hasSelection[2];
	bool secondRelease;
	struct {
		bool down;
		double x, y;
	} mouse;
	std::map<Move, int, MoveComp> arrows;
	std::map<Square, int, SquareComp> marks;
	
	std::vector<Glib::RefPtr<Glib::Binding>> bindings;
	
	bool makePlay(Square const& square);
	
	int convertX(int x) const;
	int convertY(int y) const;
	Square convert(Square const& square);
	std::complex<double> middleOf(Square const& square);
	
	void drawArrow(Cairo::RefPtr<Cairo::Context> const& cr, Square const& from, Square const& to);
	void drawArrows(Cairo::RefPtr<Cairo::Context> const& cr);
	void drawBoard(Cairo::RefPtr<Cairo::Context> const& cr);
	void drawCoordinates(Cairo::RefPtr<Cairo::Context> const& cr);
	void drawGrid(Cairo::RefPtr<Cairo::Context> const& cr);
	void drawLastMove(Cairo::RefPtr<Cairo::Context> const& cr);
	void drawMarks(Cairo::RefPtr<Cairo::Context> const& cr);
	void drawPieces(Cairo::RefPtr<Cairo::Context> const& cr);
	void drawSelectedPiece(Cairo::RefPtr<Cairo::Context> const& cr);
	void drawSelectedSquare(Cairo::RefPtr<Cairo::Context> const& cr);
	void drawValidMoves(Cairo::RefPtr<Cairo::Context> const& cr);
	void highlightMiddle(Cairo::RefPtr<Cairo::Context> const& cr);
	void showCheckRemaining(Cairo::RefPtr<Cairo::Context> const& cr);

	void handle_endGame();
	void handle_leftClicPress(Square const& square);
	void handle_leftClicRelease(Square const& square);
	void handle_newGame();
	void handle_play(Move const& move);
	void handle_rightClic(GdkEventButton* event, Square const& square);
	void handle_undo();

	bool on_button_event(GdkEventButton* event);
};

/******************************************************************************/

ChessboardArea::Private::Private(ChessboardArea* cba,
	Glib::RefPtr<Gtk::Builder> const& builder,
	std::shared_ptr<ThemeManager const> theme,
	std::shared_ptr<GameManager> game)
: q(cba),
  theme(theme),
  game(game),
  bindings(5)
{
	p.size = std::make_shared<double>();
	hasSelection[LEFT] = hasSelection[RIGHT] = false;
	mouse.down = secondRelease = false;
	
	game->signal_play().connect(sigc::mem_fun(*this, &Private::handle_play));
	game->signal_undo().connect(sigc::mem_fun(*this, &Private::handle_undo));
	game->signal_newGame().connect(sigc::mem_fun(*this, &Private::handle_newGame));
	game->signal_endGame().connect(sigc::mem_fun(*this, &Private::handle_endGame));
	
	builder->get_widget_derived("leftPiecesTaken", piecesTaken[size_t(Color::White)],
		theme, p.size, Color::White);
	builder->get_widget_derived("rightPiecesTaken", piecesTaken[size_t(Color::Black)],
		theme, p.size, Color::Black);
}

/*----------------------------------------------------------------------------*/

ChessboardArea::ChessboardArea()
: Glib::ObjectBase("ChessboardArea"),
  do_drawGrid(*this, "drawGrid"),
  do_drawLastMove(*this, "drawLastMove"),
  do_drawValidMoves(*this, "drawValidMoves"),
  do_drawCoordinates(*this, "drawCoordinates"),
  do_flipBoard(*this, "flipBoard")
{}

ChessboardArea::ChessboardArea(
	BaseObjectType* /*cobject*/,
	Glib::RefPtr<Gtk::Builder> const& /*builder*/)
: ChessboardArea()
{}

ChessboardArea::ChessboardArea(
	BaseObjectType* cobject,
	Glib::RefPtr<Gtk::Builder> const& builder,
	std::shared_ptr<ThemeManager const> theme,
	std::shared_ptr<GameManager> game)
: Glib::ObjectBase("ChessboardArea"),
  Gtk::DrawingArea(cobject),
  do_drawGrid(*this, "drawGrid"),
  do_drawLastMove(*this, "drawLastMove"),
  do_drawValidMoves(*this, "drawValidMoves"),
  do_drawCoordinates(*this, "drawCoordinates"),
  do_flipBoard(*this, "flipBoard"),
  d(std::make_shared<Private>(this, builder, theme, game))
{
	add_events(Gdk::BUTTON_PRESS_MASK
		| Gdk::BUTTON_RELEASE_MASK
		| Gdk::POINTER_MOTION_MASK
	);
	signal_button_press_event().connect(sigc::mem_fun(*d, &Private::on_button_event));
	signal_button_release_event().connect(sigc::mem_fun(*d, &Private::on_button_event));
	
	connectCheckMenuItem(builder, ChessboardArea, drawValidMoves, queue_draw, d->bindings[0]);
	connectCheckMenuItem(builder, ChessboardArea, drawLastMove, queue_draw, d->bindings[1]);
	connectCheckMenuItem(builder, ChessboardArea, drawGrid, queue_draw, d->bindings[2]);
	connectCheckMenuItem(builder, ChessboardArea, drawCoordinates, queue_draw, d->bindings[3]);
	connectCheckMenuItem(builder, ChessboardArea, flipBoard, queue_draw, d->bindings[4]);
}

/******************************************************************************/

int ChessboardArea::Private::convertX(int x) const {
	if (q->do_flipBoard.get_value()) {
		return game->getGame()->getTurn() == Color::Black ? 7 - x : x;
	} else {
		return game->getMode() == GameMode::IvP ? 7 - x : x;
	}
}

int ChessboardArea::Private::convertY(int y) const {
	if (q->do_flipBoard.get_value()) {
		return game->getGame()->getTurn() == Color::Black ? y : 7 - y;
	} else {
		return game->getMode() == GameMode::IvP ? y : 7 - y;
	}
}

inline Square ChessboardArea::Private::convert(Square const& square) {
	return {convertX(square.x), convertY(square.y)};
}

/*----------------------------------------------------------------------------*/

inline std::complex<double> ChessboardArea::Private::middleOf(Square const& square) {
	return std::complex<double>(
		p.x0 + *p.size * (square.x + 0.5),
		p.y0 + *p.size * (square.y + 0.5)
	);
}

/******************************************************************************/

void ChessboardArea::Private::handle_newGame() {
	piecesTaken[0]->clear();
	piecesTaken[1]->clear();
    marks.clear();
    arrows.clear();
	q->queue_draw();
}

/*----------------------------------------------------------------------------*/

void ChessboardArea::Private::handle_endGame() {
	EndGameDialog dialog(*q, game->getGame()->getResultat());
	dialog.run();
}

/*----------------------------------------------------------------------------*/

void ChessboardArea::Private::handle_play(Move const& move) {
	auto chess = game->getGame();
	Piece taken = chess->getPiece(move.to);
	if (chess->isPassCapture(move, taken)) {
		piecesTaken[size_t(opposite(chess->getTurn()))]->add(PieceName::Pawn);
	}
	if (not chess->isCastle(move)) {
		piecesTaken[size_t(taken.color)]->add(taken);
	}
	hasSelection[LEFT] = false;
	secondRelease = false;
	marks.clear();
	arrows.clear();
	q->queue_draw();
}

/*----------------------------------------------------------------------------*/

void ChessboardArea::Private::handle_undo() {
	auto chess = game->getGame();
	auto historic = chess->getHistoric();
	Play last = historic.back();
	if (last.type == MoveType::PassCapture) {
		piecesTaken[size_t(chess->getTurn())]->remove(PieceName::Pawn);
	}
	if (last.type != MoveType::Castle) {
		piecesTaken[size_t(last.taken.color)]->remove(last.taken);
	}
	hasSelection[LEFT] = false;
	secondRelease = false;
	marks.clear();
	arrows.clear();
	q->queue_draw();
}

/******************************************************************************/

bool ChessboardArea::Private::makePlay(Square const& square) {
	auto chess = game->getGame();
	Move move = {selected[LEFT], square};
	if (game->isHumanTurn() and chess->isValid(move)) {
		hasSelection[LEFT] = false;
		secondRelease = false;
		Piece promotion;
		if (chess->isPromotion(move)) {
			Color turn = chess->getTurn();
			PromotionDialog promo(*q, theme, p.size, turn);
			promotion.name = promo.run();
			promotion.color = turn;
		}
		game->play(move, promotion);
		return true;
	}
	return false;
}

/******************************************************************************/

void ChessboardArea::Private::handle_leftClicPress(Square const& square) {
	mouse.down = true;
	auto chess = game->getGame();
	if (hasSelection[LEFT] and makePlay(square)) {
		return;
	}
	if (chess->getPiece(square).name == PieceName::Empty) {
		hasSelection[LEFT] = false;
		secondRelease = false;
		marks.clear();
		arrows.clear();
	} else {
		if (hasSelection[LEFT] and selected[LEFT] != square) {
			secondRelease = false;
		}
		hasSelection[LEFT] = true;
		selected[LEFT] = square;
	}
	q->queue_draw();
}

void ChessboardArea::Private::handle_leftClicRelease(Square const& square) {
	mouse.down = false;
	if (hasSelection[LEFT]) {
		if (square == selected[LEFT]) {
			if (secondRelease) {
				hasSelection[LEFT] = false;
				secondRelease = false;
			}
		} else {
			makePlay(square);
		}
		if (hasSelection[LEFT]) {
			secondRelease = true;
		}
	}
	q->queue_draw();
}

/*----------------------------------------------------------------------------*/

int maskToIndex(guint state) {
	if ((state & GDK_CONTROL_MASK) == GDK_CONTROL_MASK) {
		return 1;
	} else if ((state & GDK_SHIFT_MASK) == GDK_SHIFT_MASK) {
		return 2;
	} else if ((state & GDK_MOD1_MASK) == GDK_MOD1_MASK) {
		return 3;
	}
	return 0;
}

#define MAP_INSERT(map, key, i) \
	if (auto [iter, success] = map.insert({key, i}); not success) { \
		if (iter->second == i) { \
			map.erase(iter); \
		} else { \
			iter->second = i; \
		} \
	}
void ChessboardArea::Private::handle_rightClic(GdkEventButton* event, Square const& square) {
	if (event->type == GDK_BUTTON_PRESS) {
		hasSelection[RIGHT] = true;
		selected[RIGHT] = square;
	} else if (event->type == GDK_BUTTON_RELEASE) {
		if (hasSelection[RIGHT]) {
			int i = maskToIndex(event->state);
			if (selected[RIGHT] == square) {
				MAP_INSERT(marks, square, i);
			} else {
				Move move = {selected[RIGHT], square};
				MAP_INSERT(arrows, move, i);
			}
			hasSelection[RIGHT] = false;
			q->queue_draw();
		}
	}
}
#undef MAP_INSERT

/*----------------------------------------------------------------------------*/

bool ChessboardArea::Private::on_button_event(GdkEventButton* event) {
	int x = convertX(floor((event->x - p.x0) / *p.size));
	int y = convertY(floor((event->y - p.y0) / *p.size));
	
	if (x < 0 or x > 7 or y < 0 or y > 7) {
		return false;
	}
	
	if (event->button == 1) {
		if (event->type == GDK_BUTTON_PRESS) {
			handle_leftClicPress({x, y});
		} else if (event->type == GDK_BUTTON_RELEASE) {
			handle_leftClicRelease({x, y});
		}
	} else if (event->button == 3) {
		handle_rightClic(event, {x, y});
	}
	
	return true;
}

/******************************************************************************/

bool ChessboardArea::on_motion_notify_event(GdkEventMotion *event) {
	d->mouse.x = event->x;
	d->mouse.y = event->y;
	if (d->mouse.down) {
		queue_draw();
	}
	return true;
}

/******************************************************************************/

void ChessboardArea::Private::drawGrid(Cairo::RefPtr<Cairo::Context> const& cr) {
	cr->save();
	cr->set_line_width(1.0);
	cr->set_source_rgb(0.0, 0.0, 0.0);
	for (int i = 0; i <= 8; ++i) {
		drawLine(cr, p.x0, p.y0 + *p.size * i, *p.size * 8, 0);
		drawLine(cr, p.x0 + *p.size * i, p.y0, 0, *p.size * 8);
	}
	cr->stroke();
	cr->restore();
}

/*----------------------------------------------------------------------------*/

void ChessboardArea::Private::drawBoard(Cairo::RefPtr<Cairo::Context> const& cr) {
	cr->save();
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if ((i + j) % 2 == 1) {
				cr->set_source_rgb(0.06, 0.62, 0.91);
			} else {
				cr->set_source_rgb(1.0, 1.0, 1.0);
			}
			cr->rectangle(p.x0 + *p.size * i, p.y0 + *p.size * j, *p.size, *p.size);
			cr->fill();
		}
	}
	cr->restore();
}

/*----------------------------------------------------------------------------*/

void ChessboardArea::Private::drawPieces(Cairo::RefPtr<Cairo::Context> const& cr) {
	auto chess = game->getGame();
	Square square;
	for (square.x = 0; square.x < 8; ++square.x) {
		for (square.y = 0; square.y < 8; ++square.y) {
			if (not (mouse.down and hasSelection[LEFT] and square == selected[LEFT])) {
				drawPiece(theme, chess->getPiece(square), cr,
					p.x0, p.y0,
					convertX(square.x), convertY(square.y),
					*p.size
				);
			}
		}
	}
}

/*----------------------------------------------------------------------------*/

void ChessboardArea::Private::drawValidMoves(Cairo::RefPtr<Cairo::Context> const& cr) {
	auto moves = game->getGame()->getValidMoves(selected[LEFT]);
	cr->save();
	Gdk::Cairo::set_source_rgba(cr, theme->validMoves);
	for (auto move : moves) {
		auto middle = middleOf(convert(move.to));
		cr->arc(
			middle.real(),
			middle.imag(),
			*p.size / 8,
			0.0,
			2 * G_PI
		);
		cr->fill();
	}
	cr->restore();
}

/*----------------------------------------------------------------------------*/

void ChessboardArea::Private::drawLastMove(Cairo::RefPtr<Cairo::Context> const& cr) {
	auto historic = game->getGame()->getHistoric();
	if (not historic.empty()) {
		Move last = historic.back().move;
		cr->save();
		Gdk::Cairo::set_source_rgba(cr, theme->lastMove);
		drawSquare(cr, p.x0, p.y0, *p.size, convert(last.from));
		drawSquare(cr, p.x0, p.y0, *p.size, convert(last.to));
		cr->fill();
		cr->restore();
	}
}

/*----------------------------------------------------------------------------*/

void ChessboardArea::Private::drawSelectedSquare(Cairo::RefPtr<Cairo::Context> const& cr) {
	cr->save();
	Gdk::Cairo::set_source_rgba(cr, theme->selected);
	drawSquare(cr, p.x0, p.y0, *p.size, convert(selected[LEFT]));
	cr->fill();
	cr->restore();
}

/*----------------------------------------------------------------------------*/

void ChessboardArea::Private::drawSelectedPiece(Cairo::RefPtr<Cairo::Context> const& cr) {
	cr->save();
	int x = convertX(std::clamp(convertX(floor((mouse.x - p.x0) / *p.size)), 0, 7));
	int y = convertY(std::clamp(convertY(floor((mouse.y - p.y0) / *p.size)), 0, 7));
	
	cr->set_line_width(5);
	cr->set_source_rgba(1.0, 1.0, 1.0, 0.8);
	cr->rectangle(
		p.x0 + 3 + *p.size * x,
		p.y0 + 3 + *p.size * y,
		*p.size - 6,
		*p.size - 6
	);
	cr->stroke();
	
	drawPiece(theme, game->getGame()->getPiece(selected[LEFT]), cr,
		p.x0 + mouse.x - *p.size / 2,
		p.y0 + mouse.y - *p.size / 2,
		0, 0, *p.size
	);
	cr->restore();
}

/*----------------------------------------------------------------------------*/

void ChessboardArea::Private::drawMarks(Cairo::RefPtr<Cairo::Context> const& cr) {
	cr->save();
	for (auto& square : marks) {
		Gdk::Cairo::set_source_rgba(cr, theme->mark[square.second]);
		drawSquare(cr, p.x0, p.y0, *p.size, convert(square.first));
		cr->fill();
	}
	cr->restore();
}

/*----------------------------------------------------------------------------*/

void ChessboardArea::Private::drawCoordinates(Cairo::RefPtr<Cairo::Context> const& cr) {
	for (int i = 0; i < 8; ++i) {
		Gdk::Cairo::set_source_rgba(cr, theme->board[i % 2]);
		drawText(cr, std::to_string(convertX(i) + 1), *p.size * 0.2,
			p.x0 + *p.size * 0.12,
			p.y0 + *p.size * (7 - i + 0.15)
		);
		drawText(cr, std::string(1, convertX(i) + 'a'), *p.size * 0.2,
			p.x0 + *p.size * (i + 1 - 0.12),
			p.y0 + *p.size * (8 - 0.15)
		);
	}
}

/*----------------------------------------------------------------------------*/

inline bool isKnightMove(int dx, int dy) {
	return dx + dy == 3 and dx != 3 and dy != 3;
}

inline std::complex<double> vector(Square const& from, Square const& to) {
	return std::complex<double> (to.x - from.x, to.y - from.y);
}

void ChessboardArea::Private::drawArrow(Cairo::RefPtr<Cairo::Context> const& cr,
Square const& from, Square const& to) {
	constexpr std::complex<double> i(0, 1);
	
	std::complex<double> vec;
	{
		Square start;
		int dx = to.x - from.x;
		int dy = to.y - from.y;
		int adx = abs(dx);
		int ady = abs(dy);
		if (isKnightMove(adx, ady)) {
			auto middleFrom = middleOf(from);
			if (adx > ady) {
				start.x = from.x + dx;
				start.y = from.y;
				middleFrom += *p.size * 0.375 * sgn(dx);
			} else {
				start.x = from.x;
				start.y = from.y + dy;
				middleFrom += i * (*p.size * 0.375 * sgn(dy));
			}
			move_to(cr, middleFrom);
			line_to(cr, middleOf(start));
		} else {
			start = from;
		}
		vec = vector(start, to);
		vec /= std::abs(vec);
		vec *= *p.size * 0.375;
		if (not isKnightMove(adx, ady)) {
			move_to(cr, middleOf(start) + vec);
		}
	}
	
	auto middleTo = middleOf(to);
	std::complex<double> turtle = middleTo - vec * 0.99;
	
	line_to(cr, turtle);
	cr->stroke();
	
	turtle -= vec * 0.1;
	vec *= i;
	vec /= 0.375;
	vec *= 0.25;
	turtle += vec;
	
	drawTriangle(cr, turtle, middleTo, turtle - 2.0 * vec);
}

void ChessboardArea::Private::drawArrows(Cairo::RefPtr<Cairo::Context> const& cr) {
	cr->save();
	for (auto& arrow : arrows) {
		Gdk::Cairo::set_source_rgba(cr, theme->arrow[arrow.second]);
		cr->set_line_width(*p.size * 0.2);
		drawArrow(cr, convert(arrow.first.from), convert(arrow.first.to));
	}
	cr->restore();
}

/*----------------------------------------------------------------------------*/

void ChessboardArea::Private::highlightMiddle(Cairo::RefPtr<Cairo::Context> const& cr) {
	cr->save();
	cr->set_line_width(5);
	cr->set_source_rgba(1.0, 1.0, 1.0, 0.8);
	cr->rectangle(p.x0 + *p.size * 3, p.y0 + *p.size * 3, *p.size * 2, *p.size * 2);
	cr->stroke();
	cr->restore();
}

void ChessboardArea::Private::showCheckRemaining(Cairo::RefPtr<Cairo::Context> const& cr) {
	ThreeCheck const* chess = dynamic_cast<ThreeCheck const*>(game->getGame().get());
	cr->set_source_rgba(0.5, 0.5, 0.5, 1.0);
	for (int i = 0; i < 2; ++i) {
		Color color = static_cast<Color>(i);
		auto middle = middleOf(convert(chess->getSquareKing(color)));
		drawText(cr, std::to_string(chess->checkRemaining(color)), *p.size * 0.75,
			middle.real(), middle.imag()
		);
	}
}

/*----------------------------------------------------------------------------*/

bool ChessboardArea::on_draw(Cairo::RefPtr<Cairo::Context> const& cr) {
	Gtk::Allocation allocation = get_allocation();
	int width = allocation.get_width();
	int height = allocation.get_height();
	int size = std::min(width, height);
	int pad = size % 8;
	d->p.x0 = double(width - size + pad) / 2;
	d->p.y0 = double(height - size + pad) / 2;
	*d->p.size = size / 8;
	
	auto toDraw = d->theme->chessboard->scale_simple(size - pad, size - pad, Gdk::INTERP_HYPER);
	Gdk::Cairo::set_source_pixbuf(cr, toDraw, d->p.x0, d->p.y0);
	cr->paint();
	// drawBoard(cr);
	
	if (do_drawLastMove.get_value()) {
		d->drawLastMove(cr);
	}
	if (d->hasSelection[LEFT]) {
		d->drawSelectedSquare(cr);
	}
	if (do_drawCoordinates.get_value()) {
		d->drawCoordinates(cr);
	}
	if (not d->marks.empty()) {
		d->drawMarks(cr);
	}
	if (do_drawGrid.get_value()) {
		d->drawGrid(cr);
	}
	d->drawPieces(cr);
	if (d->game->getType() == GameType::Koth) {
		d->highlightMiddle(cr);
	}
	if (not d->arrows.empty()) {
		d->drawArrows(cr);
	}
	if (d->hasSelection[LEFT] and do_drawValidMoves.get_value() and d->game->isHumanTurn()) {
		d->drawValidMoves(cr);
	}
	if (d->game->getType() == GameType::ThreeCheck) {
		d->showCheckRemaining(cr);
	}
	if (d->mouse.down and d->hasSelection[LEFT]) {
		d->drawSelectedPiece(cr);
	}
	
	return true;
}
