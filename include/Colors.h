#ifndef COLORS_H
#define COLORS_H

#include <string>

class Colors {
public:
    // ANSI color codes for terminal output
    static const std::string RESET;      // Reset all formatting
    static const std::string BOLD;       // Bold text
    
    // Foreground colors
    static const std::string RED;        // Red text
    static const std::string GREEN;      // Green text
    static const std::string YELLOW;     // Yellow text
    static const std::string BLUE;       // Blue text
    static const std::string CYAN;       // Cyan text
    static const std::string MAGENTA;    // Magenta text
    static const std::string WHITE;      // White text
    
    // Bright colors
    static const std::string BRIGHT_RED;
    static const std::string BRIGHT_GREEN;
    static const std::string BRIGHT_CYAN;
    static const std::string BRIGHT_YELLOW;
    static const std::string BRIGHT_MAGENTA;
    
    static const std::string BG_DARK_GRAY;
    static const std::string BG_BLUE;
    
    static std::string success(const std::string& text);
    static std::string error(const std::string& text);
    static std::string info(const std::string& text);
    static std::string highlight(const std::string& text);
    static std::string dim(const std::string& text);
    static std::string tableRow(const std::string& text);
    static std::string tableHeader(const std::string& text);
    static std::string prompt(const std::string& text);
    static std::string warning(const std::string& text);
};

#endif // COLORS_H
