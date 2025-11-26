#ifndef COLORS_H
#define COLORS_H

#include <string>
using namespace std;
class Colors {
public:
    // ANSI color codes for terminal output
    static const string RESET;      // Reset all formatting
    static const string BOLD;       // Bold text
    
    // Foreground colors
    static const string RED;        // Red text
    static const string GREEN;      // Green text
    static const string YELLOW;     // Yellow text
    static const string BLUE;       // Blue text
    static const string CYAN;       // Cyan text
    static const string MAGENTA;    // Magenta text
    static const string WHITE;      // White text
    
    // Bright colors
    static const string BRIGHT_RED;
    static const string BRIGHT_GREEN;
    static const string BRIGHT_CYAN;
    static const string BRIGHT_YELLOW;
    static const string BRIGHT_MAGENTA;
    
    static const string BG_DARK_GRAY;
    static const string BG_BLUE;
    
    static string success(const string& text);
    static string error(const string& text);
    static string info(const string& text);
    static string highlight(const string& text);
    static string dim(const string& text);
    static string tableRow(const string& text);
    static string tableHeader(const string& text);
    static string prompt(const string& text);
    static string warning(const string& text);
};

#endif // COLORS_H
