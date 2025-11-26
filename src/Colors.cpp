#include "Colors.h"
using namespace std;
const string Colors::RESET = "\033[0m";
const string Colors::BOLD = "\033[1m";

const string Colors::RED = "\033[31m";
const string Colors::GREEN = "\033[32m";
const string Colors::YELLOW = "\033[33m";
const string Colors::BLUE = "\033[34m";
const string Colors::MAGENTA = "\033[35m";
const string Colors::CYAN = "\033[36m";
const string Colors::WHITE = "\033[37m";

const string Colors::BRIGHT_RED = "\033[91m";
const string Colors::BRIGHT_GREEN = "\033[92m";
const string Colors::BRIGHT_CYAN = "\033[96m";
const string Colors::BRIGHT_YELLOW = "\033[93m";
const string Colors::BRIGHT_MAGENTA = "\033[95m";

const string Colors::BG_DARK_GRAY = "\033[100m";
const string Colors::BG_BLUE = "\033[44m";

string Colors::success(const string& text) {
    return BRIGHT_GREEN + text + RESET;
}

string Colors::error(const string& text) {
    return BRIGHT_RED + text + RESET;
}

string Colors::info(const string& text) {
    return BRIGHT_CYAN + text + RESET;
}

string Colors::highlight(const string& text) {
    return BOLD + YELLOW + text + RESET;
}

string Colors::dim(const string& text) {
    return "\033[2m" + text + RESET;
}

string Colors::tableRow(const string& text) {
    return CYAN + text + RESET;
}

string Colors::tableHeader(const string& text) {
    return BG_DARK_GRAY + BOLD + BRIGHT_YELLOW + text + RESET;
}

string Colors::prompt(const string& text) {
    return BRIGHT_MAGENTA + text + RESET;
}

string Colors::warning(const string& text) {
    return BRIGHT_YELLOW + text + RESET;
}
