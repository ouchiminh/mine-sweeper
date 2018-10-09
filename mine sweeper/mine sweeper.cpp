// mine sweeper.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//
#define _SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING

#include <ostream>
#include <istream>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <random>
#include <sstream>
#include <exception>
#include "boost/spirit/include/qi.hpp"

namespace mnsw{
	namespace {
		class bomb_exception : public std::exception {
		public:
			bomb_exception() : std::exception{""} {}
		};
		constexpr char BOMB_CHAR[] = " B";
		constexpr char FLAG_CHAR[] = "！";
		constexpr char NONE_CHAR[] = "  ";
		constexpr char NOT_OPENED[] = "■";
	}

	class cell {
		
		bool bomb_;
		bool flag_;
		bool opened_;
		int bomb_around_;
	public:
		cell(bool bomb) noexcept : bomb_{bomb}, flag_{false}, opened_{false} {}
		cell() noexcept : bomb_{false}, flag_{false}, opened_{false} {}

		void print(std::ostream & out) const {
			if (flag_) {
				out << FLAG_CHAR;
			}
			else if (!opened_) {
				out << NOT_OPENED;
			} else if (bomb_around_) {
				out << ' ' << bomb_around_;
			} else {
				out << NONE_CHAR;
			}
		}

		/// <summary>
		/// mark as bomb or disable flag
		/// </summary>
		/// <returns>current flag state</returns>
		bool flag() noexcept {
			flag_ ^= true;
			return flag_;
		}
		/// <summary>
		/// open and THROW EXCEPTION IF BOMB IS HERE
		/// </summary>
		/// <returns>
		/// bomb count around this cell
		/// </returns>
		int open() noexcept(false) {
			if (flag_) return false;
			opened_ = true;
			if (bomb_) throw bomb_exception();
			return bomb_around_;
		}
		void set_bomb_around(int bombcnt) noexcept { bomb_around_ = bombcnt; }
		void set_as_bomb() noexcept { bomb_ = true; }
		bool is_bomb() const noexcept { return bomb_; }
		bool is_opened() const noexcept { return opened_; }
		int get_around() const noexcept { return bomb_around_; }
	};

	class map {
		std::vector<cell> map_;
		size_t open_cnt_;

		size_t width_, height_;
		size_t flag_, bomb_;

		size_t coord_to_idx(const std::pair<size_t, size_t> & coord) {
			return width_ * coord.second + coord.first;
		}
		size_t coord_to_idx(std::pair<size_t, size_t> && coord) { return coord_to_idx(coord); }
		auto idx_to_coord(size_t idx)->std::pair<size_t, size_t> {
			return std::make_pair(idx % width_, idx / width_);
		}

		auto cells_around_cell(std::pair<size_t, size_t> const & coord)->std::array<cell *, 8>{
			std::array<cell *, 8> result{ nullptr };
			/**
			 * 0 1 2
			 * 3   4
			 * 5 6 7
			 **/
			// flag is true, no elm in the direction
			bool top = coord.second == 0, bottom = coord.second == height_ - 1;
			bool left = coord.first == 0, right = coord.first == width_ - 1;

			// top lane
			if (!top) {
				for (auto i = (left ? 1ULL : 0ULL); i < (right ? 2ULL : 3ULL); i++) {
					result[i] = &map_.at(coord_to_idx({ coord.first + i - 1, coord.second - 1 }));
				}
			}
			// mid lane
			{
				if (!left) result[3] = &map_.at(coord_to_idx({ coord.first-1, coord.second }));
				if (!right) result[4] = &map_.at(coord_to_idx({ coord.first + 1, coord.second }));
			}
			// bottom lane
			if (!bottom) {
				for (auto i = (left ? 1ULL : 0ULL); i < (right ? 2ULL : 3ULL); i++) {
					result[i + 5] = &map_.at(coord_to_idx({ coord.first + i - 1, coord.second + 1 }));
				}
			}
			return result;
		}
		
		void init(std::pair<size_t, size_t> first_coord) {
			const auto idx = coord_to_idx(first_coord);
			std::random_device rd;
			for (auto i = 0ULL; i < bomb_; i++) {
				map_.at(i).set_as_bomb();
			}
			do {
				std::shuffle(std::begin(map_), std::end(map_), std::mt19937{rd()});
			} while (map_.at(idx).is_bomb());

			// count bomb around each cell
			auto is_bomb = [](cell const * c) {return c == nullptr ? 0 : static_cast<unsigned>(c->is_bomb()); };
			for (auto i = 0ULL; i < width_ * height_; i++) {
				auto cur_coord = idx_to_coord(i);
				auto cells = cells_around_cell(cur_coord);
				int cnt(0);
				for (auto const & j : cells) cnt += is_bomb(j);
				map_.at(i).set_bomb_around(cnt);
			}
		}

	public:
		map(size_t width, size_t height, size_t bomb) : open_cnt_{ 0 } {
			set(width, height, bomb);
		}
		map() : open_cnt_{ 0 } {}
		void set(size_t width, size_t height, size_t bomb){
			const std::pair<size_t, size_t> size(std::min(9ull, width), std::min(9ull, height));
			width_ = size.first;
			height_ = size.second;
			bomb_ = std::min(!bomb?1ULL:bomb, size.first * size.second / 2);
			map_.reserve(size.first * size.second + 20);
			for (auto i = 0ULL; i < size.first * size.second; i++) {
				map_.emplace_back();
			}
		}

		/// <summary>
		/// open cell, invalid cell coordinate will be ignored
		/// </summary>
		/// <param name="x">cell coord</param>
		/// <param name="y">cell coord</param>
		/// <returns>@true : still alive; @false : game over</returns>
		size_t open(size_t x, size_t y) {
			if(open_cnt_ == 0)init({ x, y });
			if (map_.at(coord_to_idx({ x,y })).is_opened()) return width_ * height_ - open_cnt_;
			open_cnt_++;
			try {
				if (!map_.at(coord_to_idx({ x, y })).open()) {
					for (auto & i : cells_around_cell({ x, y })) {
						if (!i) continue;
						auto coord = idx_to_coord(std::distance(&map_.front(), i));
						open(coord.first, coord.second);
					}
				}
			} catch (bomb_exception&) { return 0; }
			return width_ * height_ - open_cnt_;
		}
		void flag(size_t x, size_t y) {
			map_.at(coord_to_idx({ x, y })).flag();
		}
		void finish() {
			map_.clear();
		}
		void print(std::ostream & out) {
			for (auto i = 0ULL; i < width_; i++) out << ' ' << i;
			for (auto i = 0ULL; i < width_ * height_; i++) {
				if (i % width_ == 0) out << std::endl << (char)('a' + i/width_);
				map_.at(i).print(out);
			}
		}
	};

	class game {
		map map_;

	public:
		game() = default;

		/// <summary>
		/// make user's dreams come true.
		/// </summary>
		/// <param name="cmd">
		/// o [a-i]-[0-8] : open designated cell.
		/// f [a-i]-[0-8] : mark designated cell as bomb.
		/// s [0-9]-[0-9] [0-n] : (re)start at designated map size and bomb count.
		/// e : quit game
		/// </param>
		/// <returns>
		/// @true : game is still continuing.
		/// @false : game is over.
		/// </returns>
		bool command(std::string_view cmd) {
			namespace qi = boost::spirit::qi;
			char comtype;
			char y;
			int x;
			int bomb_cnt;
			auto beg = std::begin(cmd);

			auto expr = qi::char_ >> qi::char_ >> '-' >> qi::int_ >> qi::int_;
			bool success = qi::phrase_parse(beg, std::end(cmd), expr, qi::space, comtype, y, x, bomb_cnt);
			std::stringstream ss;
			switch (comtype) {
			case 'o':
				if(!map_.open(x, y - 'a')) return false;
				break;
			case 'f':
				map_.flag(x, y - 'a');
				break;
			case 's':
				map_.finish();
				map_.set(x, y - 'a', bomb_cnt);
				break;
			case 'e':
				map_.finish();
				return false;
			}
			map_.print(ss);
			std::cout << ss.str() << std::endl;
			return true;
		}
	};
}

int main() {
	std::string combuf;
	mnsw::game g;

	do {
		std::cout << std::endl << '>' << std::flush;
		std::getline(std::cin, combuf);
	} while (g.command(combuf));
	return 0;
}
