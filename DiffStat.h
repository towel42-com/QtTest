#ifndef __DIFFSTAT_H
#define __DIFFSTAT_H

#include <iostream>
#include <memory>
#include <string> 
#include <list>
#include <algorithm>
#include <queue>
#include <map>


class CTextReader;
struct SFileDiff;
class CDiffParser;

class CTextReader;

class CDiffStat
{
public:
    CDiffStat();
    ~CDiffStat();

    int parse();
    std::string toString() const;

    void setStream( std::istream * stream ){ fStream = stream; }
    void setMerge( bool merge ){ fMerge = merge; }
    void setDetailed( bool detailed ){ fDetailed = detailed; }
    void setFormat( int format ){ fFormat = format; }

    int totalAdds() const;
    int totalRemoves() const;
    int totalMods() const;
    int longestNameLength() const;
    int maxTotal() const;

private:
    void AddStats( std::shared_ptr< SFileDiff > fileDiff );
    std::istream * fStream{ nullptr };
    bool fMerge{ true };
    bool fDetailed{ true };
    int fFormat{ 4 };

    std::vector< std::string > fLines;
    std::shared_ptr< CTextReader > fReader;
    std::unique_ptr< CDiffParser > fDiffParser;
    std::map< std::string, std::shared_ptr< SFileDiff > > fFileStats;
};

#endif
