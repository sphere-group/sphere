
#include "benchmark.hpp"
#include "video.hpp"
#include "input.hpp"
#include "time.hpp"
#include "render.hpp"

const int TIME_PER_TEST = 1000;  // milliseconds
const int RIGHT_RESULT_MARGIN = 180;

static void DrawResults(SFONT& font, int current);
static void SaveResults();

static int test_BlitImageEmpty();
static int test_BlitImageFilled();
static int test_BlitImageRandom();
static int test_BlitImageMask();
static int test_TransformBlit();
static int test_DirectBlit();
static int test_DirectTransformBlit();
static int test_Points();
static int test_Lines();
static int test_GradientLines();
static int test_Triangles();
static int test_GradientTriangles();
static int test_Rectangles();
static int test_GradientRectangles();
static int test_GrabImage();

static int test_DirectGrab();
static int test_ClearScreen();
static int test_ClearScreenMemset();
static int test_BlitImageBlackScreen();
static int test_FlipScreen();
static struct
{

    // constants
    const char* name;
    int (*function)();

    // mutables
    int result;

}
s_Tests[] = {
                { "BlitImage (empty)",   test_BlitImageEmpty      },
                { "BlitImage (filled)",  test_BlitImageFilled     },
                { "BlitImage (random)",  test_BlitImageRandom     },
                { "BlitImageMask",       test_BlitImageMask       },
                { "TransformBlit",       test_TransformBlit       },
                { "DirectBlit",          test_DirectBlit          },
                { "DirectTransformBlit", test_DirectTransformBlit },
                { "Points",              test_Points              },
                { "Lines",               test_Lines               },
                { "Gradient Lines",      test_GradientLines       },
                { "Triangles",           test_Triangles           },
                { "Gradient Triangles",  test_GradientTriangles   },
                { "Rectangles",          test_Rectangles          },
                { "Gradient Rectangles", test_GradientRectangles  },

                { "GrabImage" ,          test_GrabImage           },
                { "DirectGrab",          test_DirectGrab          },
                { "ClearScreen",         test_ClearScreen         },
                { "ClearScreenMemset",         test_ClearScreenMemset         },
                { "test_BlitImageBlackScreen", test_BlitImageBlackScreen},
                { "FlipScreen",          test_FlipScreen          },
            };
static const int s_NumTests = sizeof(s_Tests) / sizeof(*s_Tests);

////////////////////////////////////////////////////////////////////////////////
void BeginBenchmarks(const SSystemObjects& system_objects)
{
    SwitchResolution(320, 240);

    // create a drawable system font
    SFONT font;
    if (!font.CreateFromFont(system_objects.font))
    {

        //QuitMessage("Could not create system font");
        return;
    }
    for (int i = 0; i < s_NumTests; i++)
    {

        DrawResults(font, i);
        s_Tests[i].result = s_Tests[i].function();

    }
    // display the final results
    while (!AreKeysLeft())
    {
        DrawResults(font, -1); // all of them
    }
    GetKey();

    // save the results to a file
    SaveResults();
}

////////////////////////////////////////////////////////////////////////////////
void DrawResults(SFONT& font, int current)
{
    struct Local
    {
        static inline int min(int a, int b)
        {
            return (a < b ? a : b);
        }
    };

    // do some precalculations
    int font_height = font.GetMaxHeight();
//  int ellipses_width = font.GetStringWidth("... ");

    // find out how many rows to draw
    int num_results = Local::min(s_NumTests, current + 1);
    if (current == -1)
    {
        num_results = s_NumTests;
    }

    // clear the screen
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), CreateRGBA(0, 0, 0, 255));

    // render each row
    for (int i = 0; i < num_results; i++)
    {
        int x = font.GetStringWidth(s_Tests[i].name);
        font.DrawString(0, font_height * i, s_Tests[i].name, CreateRGBA(255, 255, 255, 255));
        font.DrawString(x, font_height * i, "... ", CreateRGBA(255, 255, 255, 255));

        // if test has already been processed, draw result
        if (i != current)
        {
            char s[10];
            sprintf(s, "%d", s_Tests[i].result);
            int result_width = font.GetStringWidth(s);
            font.DrawString(RIGHT_RESULT_MARGIN - result_width, font_height * i, s, CreateRGBA(255, 255, 255, 255));
        }
    }

    if (current == -1)
    {
        font.DrawString(0, (s_NumTests + 1) * font_height, "Press any key to exit", CreateRGBA(255, 255, 255, 255));
    }

    FlipScreen();
}

////////////////////////////////////////////////////////////////////////////////
void SaveResults()
{
    FILE* file = fopen("results.txt", "w");
    if (!file)
    {
        return;
    }

    for (int i = 0; i < s_NumTests; i++)
    {
        fprintf(file, "%-20s... %10d\n", s_Tests[i].name, s_Tests[i].result);
    }

    fclose(file);
}

////////////////////////////////////////////////////////////////////////////////
int test_BlitImageEmpty()
{
    RGBA* pixels = new RGBA[16 * 16];
    memset(pixels, 0, sizeof(RGBA) * 16 * 16);
    IMAGE image = CreateImage(16, 16, pixels);
    delete[] pixels;

    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {
        BlitImage(image, 0, 0, CImage32::BLEND);
        count++;
    }

    DestroyImage(image);
    return count;
}

////////////////////////////////////////////////////////////////////////////////
int test_BlitImageFilled()
{
    RGBA* pixels = new RGBA[16 * 16];
    memset(pixels, 255, sizeof(RGBA) * 16 * 16);
    IMAGE image = CreateImage(16, 16, pixels);
    delete[] pixels;

    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {
        BlitImage(image, 0, 0, CImage32::BLEND);
        count++;
    }

    DestroyImage(image);
    return count;
}

////////////////////////////////////////////////////////////////////////////////
int test_BlitImageRandom()
{
    RGBA* pixels = new RGBA[16 * 16];
    for (int i = 0; i < 16 * 16; i++)
    {
        pixels[i].red   = rand() % 256;
        pixels[i].green = rand() % 256;
        pixels[i].blue  = rand() % 256;
        pixels[i].alpha = rand() % 256;
    }
    IMAGE image = CreateImage(16, 16, pixels);
    delete[] pixels;

    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {
        BlitImage(image, 15, 15, CImage32::BLEND);
        count++;
    }

    DestroyImage(image);
    return count;
}

////////////////////////////////////////////////////////////////////////////////
int test_BlitImageMask()
{
    RGBA* pixels = new RGBA[16 * 16];
    for (int i = 0; i < 16 * 16; i++)
    {
        pixels[i].red   = rand() % 256;
        pixels[i].green = rand() % 256;
        pixels[i].blue  = rand() % 256;
        pixels[i].alpha = rand() % 256;
    }
    IMAGE image = CreateImage(16, 16, pixels);
    delete[] pixels;

    RGBA mask = { 128, 128, 128, 128 };
    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {
        BlitImageMask(image, 0, 0, CImage32::BLEND, mask, CImage32::MULTIPLY);
        count++;
    }

    DestroyImage(image);
    return count;
}

////////////////////////////////////////////////////////////////////////////////
int test_TransformBlit()
{
    RGBA* pixels = new RGBA[16 * 16];
    for (int i = 0; i < 16 * 16; i++)
    {
        pixels[i].red   = rand() % 256;
        pixels[i].green = rand() % 256;
        pixels[i].blue  = rand() % 256;
        pixels[i].alpha = rand() % 256;
    }
    IMAGE image = CreateImage(16, 16, pixels);
    delete[] pixels;

    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {
//    int offset = count & 31;  // bottom four bits
        int x[4] = { count, count + 16, 24,  8 };
        int y[4] = { 0, 0, 16, 16 };
        TransformBlitImage(image, x, y, CImage32::BLEND);
        count++;
    }

    DestroyImage(image);
    return count;
}

////////////////////////////////////////////////////////////////////////////////
int test_DirectBlit()
{
    RGBA* pixels = new RGBA[16 * 16];
    for (int i = 0; i < 16 * 16; i++)
    {
        pixels[i].red   = rand() % 256;
        pixels[i].green = rand() % 256;
        pixels[i].blue  = rand() % 256;
        pixels[i].alpha = rand() % 256;
    }

    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {
        DirectBlit(0, 0, 16, 16, pixels);
        count++;
    }

    delete[] pixels;
    return count;
}

////////////////////////////////////////////////////////////////////////////////
int test_DirectTransformBlit()
{
    RGBA* pixels = new RGBA[16 * 16];
    for (int i = 0; i < 16 * 16; i++)
    {
        pixels[i].red   = rand() % 256;
        pixels[i].green = rand() % 256;
        pixels[i].blue  = rand() % 256;
        pixels[i].alpha = rand() % 256;
    }

    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {
//    int offset = count & 31;  // bottom four bits
        int x[4] = { count, count + 16, 24,  8 };
        int y[4] = { 0, 0, 16, 16 };
        DirectTransformBlit(x, y, 16, 16, pixels);
        count++;
    }

    delete[] pixels;
    return count;
}

////////////////////////////////////////////////////////////////////////////////
int test_Points()
{
    RGBA c = CreateRGBA(255, 255, 255, 128);

    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {
        DrawPoint(0, 0, c);
        count++;
    }

    return count;
}

////////////////////////////////////////////////////////////////////////////////
int test_Lines()
{
    RGBA c = CreateRGBA(255, 255, 255, 128);

    int x[2] = { 0, GetScreenWidth() };
    int y[2] = { 0, GetScreenHeight() };

    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {
        DrawLine(x, y, c);
        count++;
    }

    return count;
}

////////////////////////////////////////////////////////////////////////////////
int test_GradientLines()
{
    int  x[2] = { 0, GetScreenWidth() };
    int  y[2] = { 0, GetScreenHeight() };
    RGBA c[2] = { { 255, 255, 255, 0 }, { 255, 255, 255, 255 } };

    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {
        DrawGradientLine(x, y, c);
        count++;
    }

    return count;
}

////////////////////////////////////////////////////////////////////////////////
int test_Triangles()
{
    int x[3] = { 0, 32, 32 };
    int y[3] = { 16, 0, 32 };
    RGBA c = { 255, 255, 255, 128 };

    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {
        DrawTriangle(x, y, c);
        count++;
    }

    return count;
}

////////////////////////////////////////////////////////////////////////////////
int test_GradientTriangles()
{
    int x[3] = { 0, 32, 32 };
    int y[3] = { 16, 0, 32 };
    RGBA c[3] = {
                    { 255, 0, 0, 128 },
                    { 0, 255, 0, 192 },
                    { 0, 0, 255, 255 },
                };

    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {
        DrawGradientTriangle(x, y, c);
        count++;
    }

    return count;
}

////////////////////////////////////////////////////////////////////////////////
int test_Rectangles()
{
    RGBA c = { 128, 128, 128, 128 };

    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {
        DrawRectangle(0, 0, 32, 32, c);
        count++;
    }

    return count;
}

////////////////////////////////////////////////////////////////////////////////
int test_GradientRectangles()
{
    RGBA c[4] = {
                    { 128, 128, 128, 128 },
                    { 255, 128, 128, 255 },
                    { 128, 255, 128, 255 },
                    { 128, 128, 255, 255 },
                };

    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {
        DrawGradientRectangle(0, 0, 32, 32, c);
        count++;
    }

    return count;
}

////////////////////////////////////////////////////////////////////////////////
int test_GrabImage()
{
    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {

        IMAGE img = GrabImage(0, 0, 32, 32);
        DestroyImage(img);
        count++;
    }
    return count;
}
////////////////////////////////////////////////////////////////////////////////
int test_DirectGrab()
{
    RGBA* pixels = new RGBA[32 * 32];
    if (!pixels) return 0;
    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {

        DirectGrab(0, 0, 32, 32, pixels);
        count++;
    }
    delete[] pixels;
    return count;
}
////////////////////////////////////////////////////////////////////////////////
int test_ClearScreen()
{
    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {

        ClearScreen();
        count++;
    }
    return count;
}
/////////////////////
int test_BlitImageBlackScreen()
{
    RGBA* pixels = new RGBA[320 * 240];
    memset(pixels, 255, sizeof(RGBA) * 320 * 240);
    IMAGE image = CreateImage(320, 240, pixels);
    delete[] pixels;
    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {

        BlitImage(image, 0, 0, CImage32::BLEND);
        count++;
    }
    return count;
}
////////////////////////////////////////////////////////////////////////////////
int test_ClearScreenMemset()
{
    RGBA* pixels = new RGBA[320 * 240];
    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {

        //BlitImage(image, 0, 0);
        memset(pixels, 0, sizeof(RGBA) * 320 * 240);
        count++;
    }
    delete[] pixels;
    return count;
}
////////////////////////////////////////////////////////////////////////////////
int test_FlipScreen()
{
    int count = 0;
    dword start = GetTime();
    while (start + TIME_PER_TEST > GetTime())
    {
        FlipScreen();
        count++;
    }

    return count;
}

////////////////////////////////////////////////////////////////////////////////
