//Pen.cpp
#include "Pen.h"
#include <iostream>
#include <string> 
 
void Pen::write_on_paper(const std::string & words)
{
    if (InkLevelPercent <= 0)
    {
        std::cout << "I do apologize, but there is no more ink!" << std::endl;
    }
    else
    {
        std::cout << words << std::endl;
    }
    InkLevelPercent -= words.length();
}
 
void Pen::break_in_half()
{
    InkLevelPercent /= 2;
    Length /= 2;
}
 
void Pen::run_out_of_ink()
{
    InkLevelPercent = 0;
}
 
