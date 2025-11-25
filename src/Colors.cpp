#include "Colors.h"

const std::string Colors::RESET = "\033[0m";
const std::string Colors::BOLD = "\033[1m";

const std::string Colors::RED = "\033[31m";
const std::string Colors::GREEN = "\033[32m";
const std::string Colors::YELLOW = "\033[33m";
const std::string Colors::BLUE = "\033[34m";
const std::string Colors::MAGENTA = "\033[35m";
const std::string Colors::CYAN = "\033[36m";
const std::string Colors::WHITE = "\033[37m";

const std::string Colors::BRIGHT_RED = "\033[91m";
const std::string Colors::BRIGHT_GREEN = "\033[92m";
const std::string Colors::BRIGHT_CYAN = "\033[96m";
const std::string Colors::BRIGHT_YELLOW = "\033[93m";
const std::string Colors::BRIGHT_MAGENTA = "\033[95m";

const std::string Colors::BG_DARK_GRAY = "\033[100m";
const std::string Colors::BG_BLUE = "\033[44m";

std::string Colors::success(const std::string& text) {
    return BRIGHT_GREEN + text + RESET;
}

std::string Colors::error(const std::string& text) {
    return BRIGHT_RED + text + RESET;
}

std::string Colors::info(const std::string& text) {
    return BRIGHT_CYAN + text + RESET;
}

std::string Colors::highlight(const std::string& text) {
    return BOLD + YELLOW + text + RESET;
}

std::string Colors::dim(const std::string& text) {
    return "\033[2m" + text + RESET;
}

std::string Colors::tableRow(const std::string& text) {
    return CYAN + text + RESET;
}

std::string Colors::tableHeader(const std::string& text) {
    return BG_DARK_GRAY + BOLD + BRIGHT_YELLOW + text + RESET;
}

std::string Colors::prompt(const std::string& text) {
    return BRIGHT_MAGENTA + text + RESET;
}

std::string Colors::warning(const std::string& text) {
    return BRIGHT_YELLOW + text + RESET;
}
