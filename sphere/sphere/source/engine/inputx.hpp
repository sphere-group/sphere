#ifndef INPUTX_HPP
#define INPUTX_HPP

#include "input.hpp"
inline bool AnyKeyPressed()
{

    bool key_states[MAX_KEY];
    GetKeyStates(key_states);
    for (int i = MAX_KEY-1; i>=0; --i)
    {
        if (key_states[i])
        {
            return true;
        }
    }
    return false;
}

inline void ClearKeyQueue()
{
    while (AreKeysLeft())
    {
        GetKey();
    }
}

inline const char* GetKeyString(int key, bool shift)
{
    switch (key)
    {
    case KEY_TILDE:
        return (shift ? "~" : "`");
        //case KEY_TILDE:      return (shift ? "@" : "`");
    case KEY_0:
        return (shift ? ")" : "0");
    case KEY_1:
        return (shift ? "!" : "1");
    case KEY_2:
        return (shift ? "@" : "2");
    case KEY_3:
        return (shift ? "#" : "3");
        //case KEY_2:          return (shift ? "\"" : "2");
        //case KEY_3:          return (shift ? "£" : "3");
    case KEY_4:
        return (shift ? "$" : "4");
    case KEY_5:
        return (shift ? "%" : "5");
    case KEY_6:
        return (shift ? "^" : "6");
    case KEY_7:
        return (shift ? "&" : "7");
    case KEY_8:
        return (shift ? "*" : "8");
    case KEY_9:
        return (shift ? "(" : "9");
    case KEY_MINUS:
        return (shift ? "_" : "-");
    case KEY_EQUALS:
        return (shift ? "+" : "=");
    case KEY_A:
        return (shift ? "A" : "a");
    case KEY_B:
        return (shift ? "B" : "b");
    case KEY_C:
        return (shift ? "C" : "c");
    case KEY_D:
        return (shift ? "D" : "d");
    case KEY_E:
        return (shift ? "E" : "e");
    case KEY_F:
        return (shift ? "F" : "f");
    case KEY_G:
        return (shift ? "G" : "g");
    case KEY_H:
        return (shift ? "H" : "h");
    case KEY_I:
        return (shift ? "I" : "i");
    case KEY_J:
        return (shift ? "J" : "j");
    case KEY_K:
        return (shift ? "K" : "k");
    case KEY_L:
        return (shift ? "L" : "l");
    case KEY_M:
        return (shift ? "M" : "m");
    case KEY_N:
        return (shift ? "N" : "n");
    case KEY_O:
        return (shift ? "O" : "o");
    case KEY_P:
        return (shift ? "P" : "p");
    case KEY_Q:
        return (shift ? "Q" : "q");
    case KEY_R:
        return (shift ? "R" : "r");
    case KEY_S:
        return (shift ? "S" : "s");
    case KEY_T:
        return (shift ? "T" : "t");
    case KEY_U:
        return (shift ? "U" : "u");
    case KEY_V:
        return (shift ? "V" : "v");
    case KEY_W:
        return (shift ? "W" : "w");
    case KEY_X:
        return (shift ? "X" : "x");
    case KEY_Y:
        return (shift ? "Y" : "y");
    case KEY_Z:
        return (shift ? "Z" : "z");
    case KEY_SPACE:
        return (shift ? " " : " ");
    case KEY_OPENBRACE:
        return (shift ? "{" : "[");
    case KEY_CLOSEBRACE:
        return (shift ? "}" : "]");
    case KEY_SEMICOLON:
        return (shift ? ":" : ";");
    case KEY_APOSTROPHE:
        return (shift ? "\"" : "'");
        //case KEY_APOSTROPHE: return (shift ? "~" : "#");
    case KEY_COMMA:
        return (shift ? "<" : ",");
    case KEY_PERIOD:
        return (shift ? ">" : ".");
    case KEY_SLASH:
        return (shift ? "?" : "/");
    case KEY_BACKSLASH:
        return (shift ? "|" : "\\");

    case KEY_NUM_0:
        return "0";

    case KEY_NUM_1:
        return "1";

    case KEY_NUM_2:
        return "2";

    case KEY_NUM_3:
        return "3";

    case KEY_NUM_4:
        return "4";

    case KEY_NUM_5:
        return "5";

    case KEY_NUM_6:
        return "6";

    case KEY_NUM_7:
        return "7";

    case KEY_NUM_8:
        return "8";

    case KEY_NUM_9:
        return "9";

    default:
        return "";
    }
}

#endif
