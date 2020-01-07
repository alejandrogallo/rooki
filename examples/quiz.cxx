#!/usr/bin/env rooki
// Author: Alejandro Gallo.
// License: 2020, GPLv3.
// Email: aamsgallo@gmail.com
// rooki:flags -pedantic -std=c++11 -Wall -Wextra
// rooki:flags -I $(dirname $src)/include
// rooki:flags $(pkg-config --cflags ncursesw)
// rooki:flags $(pkg-config --libs ncursesw)
// rooki:spell g++ -x c++ $f $flags -o $bin

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <locale.h>
#include <utility>
#include <stdarg.h>
#include <random>

#include <CLI11.hpp>
#include <json.hpp>
#include <ncurses.h>

using json = nlohmann::json;

struct Window {
  int height, width, starty, startx;
  WINDOW *win;
  Window(int h, int w, int sy, int sx):
    height(h), width(w), starty(sy), startx(sx), win(newwin(h, w, sy, sx)){}
  inline Window& clear() { wclear(win); return *this; }
  inline ~Window() { delwin(win); }
  inline Window& refresh() { wrefresh(win); return *this; }
  inline std::pair<int,int> size() {
    std::pair<int,int> result;
    getmaxyx(win, result.first, result.second);
    return result;
  }
};

struct InfoWindow: public Window {
  InfoWindow(int sy, int sx, int w=COLS): Window(1, w, sy, sx) {};
  void print(const std::string format, ...) {
    va_list args;
    va_start(args, format);
    clear();
    mvwprintw(win, 0, 0, format.data(), args);
    refresh();
    va_end(args);
  }
};

struct UI {
  UI() {
    std::setlocale(LC_ALL, ""); // for utf8
    initscr(); // Start curses mode
    cbreak(); // Pass on everything
    raw(); 
    noecho(); //echo swtiched off
    keypad(stdscr,TRUE); //function keys reading enabled
    curs_set(0);
    clear();
    refresh();
  }
  ~UI() { endwin(); }
};

void saveWords(const json &jsonDoc, const std::string filePath) {
  std::ofstream o(filePath);
  std::cout << "Saving " << jsonDoc.size()
            << " words in file " << filePath << std::endl;
  o << std::setw(2) << jsonDoc;
  o.close();
}

int main(int argc, char *argv[])
{

  std::string inputFile, outputFile;
  CLI::App app{"Small word trainer"};
  app.add_option("input file", inputFile, "Input json file")->required();
  app.add_option("input file", outputFile, "Input json file");
  CLI11_PARSE(app, argc, argv);

  if (!outputFile.size()) outputFile = inputFile;

  json jsonDoc; { std::ifstream fd{inputFile}; fd >> jsonDoc; }
  assert(jsonDoc.is_array());
  std::cout << jsonDoc.size() << std::endl;

  std::random_device rd;
  std::mt19937 rng(rd());

  UI ui;
  Window questionWindow(LINES - 2, COLS, 0, 0);
  InfoWindow info(LINES - 1, 0);
  InfoWindow menu(LINES - 2, 0);

  menu.print("[1] bad | [2] repeat | [3] good | [4] very good");

  bool continueLoop{true};

  while (continueLoop) {

    std::uniform_int_distribution<int> uniDist(0, jsonDoc.size()-1);
    size_t index(uniDist(rng));
    auto& w(jsonDoc.at(index));

    //{
      //std::stringstream msg;
      //msg << index << "/" << jsonDoc.size();
      //info.print(msg.str());
    //}

    std::string word;
    std::string value;
    try {
      word = w.at("k");
      value = w.at("v");
    } catch (json::out_of_range &e) {
      std::stringstream msg;
      msg << e.what() << " (" << w << ")";
      info.clear();
      info.print(msg.str());
      getch();
      info.clear();
      continue;
    }

    // clearing
    questionWindow.clear();

    box(questionWindow.win, '|', '-');
    auto s{questionWindow.size()};
    std::string punctuation;
    int wordW(0.5 * s.second - 0.2 * word.size());
    int wordH(0.5 * s.first);
    mvwprintw(questionWindow.win, wordH, wordW, "%s", word.data());

    // refresh
    questionWindow.refresh();

    if (w.count("p") && w.at("p") == "4") {
      info.print("Ignoring " + word);
      continue;
    }


    bool decisionIsMade{false};
    while (!decisionIsMade) {
      auto keyPress(getch());
      switch (keyPress) {
        case 'q':
          info.print("Exiting...");
          saveWords(jsonDoc, outputFile);
          getch();
          return 0;
          break;
        case 'Q':
          info.print("Exiting without saving");
          return 0;
          break;
        case 'd':
          info.print("Deleting " + word);
          jsonDoc.erase(index);
          decisionIsMade = true;
          break;
        case '1': case '2': case '3': case '4':
          punctuation = keyPress;
          w["p"] = punctuation;
          info.clear().refresh();
          mvwprintw(questionWindow.win, wordH + 1, wordW, "---");
          mvwprintw(questionWindow.win, wordH + 2, wordW, value.data());
          questionWindow.refresh();
          decisionIsMade = true;
          getch();
          break;
        default:
          info.print(
            std::string{"No binding for: "} + std::string{(char)keyPress});
          break;
      }
    }

  }

  return 0;
}
