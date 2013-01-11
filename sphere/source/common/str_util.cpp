#include <ctype.h>
#include <string.h>

bool IsInvalidNumber(const char* text, bool& number_is_floating_point, bool& number_is_percentage)
{
    int prefix = 0;
    int digits = 0;
    int whitespace_before = 0;
    int whitespace_after = 0;
    int percentage = 0;
    int point = 0;
    int fdigits = 0;
    // count up how many digits there are in the string and if there's + or - prefix
    for (unsigned int i = 0; i < strlen(text); i++)
    {
        if (isdigit(text[i]))
        {
            point == 0 ? digits++ : fdigits++;
            whitespace_after = 0;
            percentage = 0;
        }
        else
            if (isspace(text[i]))
            {
                (digits == 0 && fdigits == 0) ? whitespace_before++ : whitespace_after++;
            }
            else
                if (text[i] == '.')
                {
                    point = 1;
                }
                else
                    if (text[i] == '%')
                    {
                        percentage++;
                    }
                    else
                        if (i - whitespace_before == 0)
                        {
                            if (text[i] == '+' || text[i] == '-')
                            {
                                prefix = 1;
                            }
                        }
    }
    number_is_percentage = (percentage != 0);
    number_is_floating_point = (point && fdigits > 0);
    return (digits == 0 || (fdigits == 0 && point) || strlen(text) - whitespace_before - prefix - digits - point - fdigits - percentage - whitespace_after != 0);
}
