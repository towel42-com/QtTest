//Pen.h
#ifndef PEN_H_INCLUDED
#define PEN_H_INCLUDED

#include <string>

enum MyColor
{
    red,
    orange,
    yellow,
    green,
    blue,
    purple,
    transparent,
    brown,
    black
};
 
enum PenStyle
{
    ballpoint,
    rollerball,
    fountain,
    felt_tip,
};
 
class Pen
{
public:
    int Length;
    int InkLevelPercent;
    std::string Brand;
    MyColor InkColor;
    MyColor ShellColor;
    PenStyle Style;
 
    void write_on_paper(const std::string & words);
    void break_in_half();
    void run_out_of_ink();
};
 
#endif // PEN_H_INCLUDED

