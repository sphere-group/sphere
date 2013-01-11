#ifndef TRANSLATE_HPP
#define TRANSLATE_HPP
const char* TranslateString(const char* string);
void TranslateMenu(HMENU menu);
void TranslateDialog(HWND hWnd);
const char* GetLanguage();
void SetLanguage(const char* language);
#endif
