#include "DiffStat.h"
#include <iostream>
#include <iomanip>
#include <sstream>

class CTextReader
{
public:
    CTextReader( std::istream * stream ) :
        fStream( stream )
    {
        fCurrentLineIndex = 0;
        fLinesBuffer.push_back( std::string() );
        fLinesBuffer.push_back( std::string() );
        std::getline( *fStream, fLastLineRead );
        fLinesBuffer.push_back( fLastLineRead );
    }

    int currentLineIndex() const{ return fCurrentLineIndex; }
    std::string previousLine() const{ return fLinesBuffer.front(); } 
    std::string currentLine() const{ auto pos = fLinesBuffer.begin(); pos++; return *pos; } 
    std::string nextLine() const{ return fLinesBuffer.back(); } 

    bool hasNextLine() const{ return !fStream->eof(); }
    bool moveForward()
    {
        if ( fStream->eof() )
            return false;

        fLinesBuffer.pop_front();
        std::getline( *fStream, fLastLineRead );
        fLinesBuffer.push_back( fLastLineRead );
        fCurrentLineIndex++;
        return true;
    }
private:
    std::list< std::string > fLinesBuffer;
    int fCurrentLineIndex{ 0 };
    std::string fLastLineRead;
    std::istream * fStream;
};


enum class ELinesType
{
    eAdded,
    eRemoved,
    eEscaped,
    eOthers
};

struct SChunkStat
{
    int fAdds{ 0 };
    int fRemoves {0};

    SChunkStat()
    {
        Reset();
    }

    void Reset()
    {
        fAdds = 0;
        fRemoves = 0;
    }
};

struct SStatsCounter
{
    SStatsCounter( bool mergeOpt ) :
        fMergeOpt( mergeOpt )
    {
    }

    SStatsCounter( bool mergeOpt, int adds, int removes, int mods ) :
        fMergeOpt( mergeOpt ),
        fAdds( adds ),
        fRemoves( removes ),
        fMods( mods )
    {
    }

    int total() const{ return fAdds + fRemoves + fMods; }

    void LineFound( ELinesType type )
    {
        switch( type )
        {
            case ELinesType::eAdded:
                fTempStats.first++;
                break;
            case ELinesType::eRemoved:
                fTempStats.second++;
                break;
            case ELinesType::eEscaped:
                break;
            case ELinesType::eOthers:
                ClearTempStats();
                break;
            default:
                break;
        }
    }

    void ClearTempStats()
    {
        if ( fTempStats.first == 0 && fTempStats.second == 0 )
            return;
        int mods = fMergeOpt ? std::min( fTempStats.first, fTempStats.second ) : 0;
        fAdds += fTempStats.first - mods;
        fRemoves += fTempStats.second - mods;
        fMods += mods;
        fTempStats = { 0, 0 };
    }

    void sumWith( const SStatsCounter & rhs ) 
    {
        fAdds += rhs.fAdds;
        fRemoves += rhs.fRemoves;
        fMods += rhs.fMods;
    }

    std::string outputArrayData( int numberWidth, bool detailed ) const;
    std::string outputHistogram( double scale ) const;

    std::pair< int, int > fTempStats{ 0, 0 };
    bool fMergeOpt;
    int fAdds{ 0 };
    int fRemoves{ 0 };
    int fMods{ 0 };
};

struct SFileDiff
{
public:
    SFileDiff( bool mergeOpt ) :
        fStatsCounter( mergeOpt )
    {
    }

    void writeLine( const std::string & text, bool isHeader );

    std::string fOrigFile;
    std::string fNewFile;
    std::string fOrigInfo;
    std::string fNewInfo;
    std::string fOrigChangeSetID;
    bool fBinary{ false };
    bool fDeleted{ false };
    bool fMoved{ false };

    SStatsCounter fStatsCounter;
};

class CDiffParser
{
public:
    static CDiffParser * GetDiffParser( std::shared_ptr< CTextReader > reader );

    CDiffParser( std::shared_ptr< CTextReader > reader ) :
        fReader( reader )
    {
    }

    std::list< std::shared_ptr< SFileDiff > > parse();

    void setMerge( bool merge ){ fMerge = merge; }
protected:
    virtual void parseSpecialHeader( std::map< std::string, std::string > & info, std::vector< std::string > & header );
    virtual void parseDiffHeader( std::map< std::string, std::string > & info, std::vector< std::string >  & header );
    virtual std::pair< std::string, std::string > CDiffParser::parseFileNameHeader( const std::string & str, int lineIndex );
    std::shared_ptr< SFileDiff > parseChangeHeader();
    std::shared_ptr< CTextReader > fReader;
    bool fMerge{ false };
};

class CSvnDiffParser : public CDiffParser
{
public:
    CSvnDiffParser( std::shared_ptr< CTextReader > reader ) :
        CDiffParser( reader )
    {
    }
protected:
    virtual void parseSpecialHeader( std::map< std::string, std::string > & info, std::vector< std::string >  & header );
    virtual void parseDiffHeader( std::map< std::string, std::string > & info, std::vector< std::string >  & header );
};

CDiffStat::CDiffStat()
{
}

CDiffStat::~CDiffStat()
{
}

int CDiffStat::parse()
{
    if ( !fStream )
    {
        std::cerr << "Input stream not set" << std::endl;
        return false;
    }

    fReader.reset( new CTextReader( fStream ) );

    fDiffParser.reset( CDiffParser::GetDiffParser( fReader ) );
    if ( !fDiffParser )
        std::cerr << "Could not determine format" << std::endl;
    fDiffParser->setMerge( fMerge );

    auto files = fDiffParser->parse();
    for( auto ii : files )
    {
        ii->fStatsCounter.ClearTempStats();
        AddStats( ii );
    }

    return true;
}

void CDiffStat::AddStats( std::shared_ptr< SFileDiff > fileDiff )
{
    std::string fileName = fileDiff->fNewFile.empty() ? fileDiff->fOrigFile : fileDiff->fNewFile;

    auto pos = fFileStats.find( fileName );
    if ( pos != fFileStats.end() )
        (*pos).second->fStatsCounter.sumWith( fileDiff->fStatsCounter );
    else
        fFileStats[ fileName ] = fileDiff;
}

int CDiffStat::longestNameLength() const
{
    size_t retVal = 0;
    for( auto ii : fFileStats )
    {
        retVal = std::max( retVal, ii.first.length() );
    }
    return retVal;
}

int CDiffStat::maxTotal() const
{
    int retVal = 0;
    for( auto ii : fFileStats )
    {
        retVal = std::max( retVal, ii.second->fStatsCounter.total() );
    }
    return retVal;
}


int CDiffStat::totalAdds() const
{
    int retVal = 0;
    for( auto ii : fFileStats )
    {
        retVal += ii.second->fStatsCounter.fAdds;
    }
    return retVal;
}

int CDiffStat::totalRemoves() const
{
    int retVal = 0;
    for( auto ii : fFileStats )
    {
        retVal += ii.second->fStatsCounter.fRemoves;
    }
    return retVal;
}

int CDiffStat::totalMods() const
{
    int retVal = 0;
    for( auto ii : fFileStats )
    {
        retVal += ii.second->fStatsCounter.fMods;
    }
    return retVal;
}


std::string toString( int value )
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
static int DEFAULT_MAX_WIDTH = 80;

std::string SStatsCounter::outputArrayData( int numberWidth, bool detailed ) const
{
    std::ostringstream oss;
    oss << std::setw( numberWidth ) << total() << std::setw( 0 );
    if ( detailed )
    {
        oss << std::setw( numberWidth ) << fAdds << std::setw( 0 )
            << std::setw( numberWidth ) << fRemoves << std::setw( 0 )
            ;
        if ( fMergeOpt )
            oss << std::setw( numberWidth ) << fMods << std::setw( 0 )
            ;
    }
    return oss.str();
}

std::string SStatsCounter::outputHistogram( double scale ) const
{
    std::ostringstream oss;
    double delta = 0.0;
    if ( fAdds > 0 )
    {
        double product = scale * fAdds;
        int charsCount = (int)product;
        oss << std::string( charsCount, '+' );
        delta = product - charsCount;
    }
    if ( fRemoves > 0 )
    {
        double product = scale * ( fRemoves + delta );
        int charsCount = (int)product;
        oss << std::string( charsCount, '-' );
        delta = product - charsCount;
    }
    if ( fMergeOpt && fMods > 0 )
    {
        double product = scale * ( fMods + delta );
        int charsCount = (int)product;
        oss << std::string( charsCount, '!' );
        delta = product - charsCount;
    }

    return oss.str();
}


bool startsWith( const std::string & str, const std::string & startStr )
{
    return str.substr( 0, startStr.size() ) == startStr;
}
bool endsWith( const std::string & str, const std::string & startStr )
{
    return str.substr( str.length() - startStr.size() ) == startStr;
}

CDiffParser * CDiffParser::GetDiffParser( std::shared_ptr< CTextReader > reader )
{
    if( !reader )
        return nullptr;

    auto firstLine = reader->nextLine();
    if ( startsWith( firstLine, "Index: " ) )
        return new CSvnDiffParser( reader );
    return nullptr;
}

std::list< std::shared_ptr< SFileDiff > > CDiffParser::parse()
{
    std::list< std::shared_ptr< SFileDiff > > retVal;
    std::shared_ptr< SFileDiff > currentFile;
    while( fReader->moveForward() )
    {
        auto newFile = parseChangeHeader();
        if ( !newFile )
        {
            if( currentFile )
                currentFile->writeLine( fReader->currentLine(), false );
        }
        else
        {
            if ( currentFile )
                retVal.push_back( currentFile );
            currentFile = newFile;
        }

    }
    if ( currentFile )
        retVal.push_back( currentFile );

    return retVal;
}


static std::string INDEX_SEP = std::string( 67, '=' );
static std::string BINARY_STRING = "Cannot display: file marked as a binary type.";
void CDiffParser::parseSpecialHeader( std::map< std::string, std::string > & info, std::vector< std::string > & header )
{
    if ( fReader->hasNextLine() && startsWith( fReader->currentLine(), "Index: " ) && fReader->nextLine() == INDEX_SEP )
    {
        std::vector< std::string > special_header;
        special_header.resize( 2 );

        auto pos = fReader->currentLine().find( " " );
        info[ "index" ] = fReader->currentLine().substr( pos + 1 );
        header.push_back( fReader->currentLine() );
        fReader->moveForward();
        header.push_back( fReader->currentLine() );
        fReader->moveForward();
    }
}

void CSvnDiffParser::parseSpecialHeader( std::map< std::string, std::string > & info, std::vector< std::string >  & header )
{
    CDiffParser::parseSpecialHeader( info, header );
    if ( info.find( "index" ) != info.end() && fReader->currentLine() == BINARY_STRING )
    {
        header.push_back( fReader->currentLine() );
        fReader->moveForward();
        header.push_back( fReader->currentLine() );
        fReader->moveForward();

        info[ "binary" ] = "True";
        info[ "origFile" ] = info[ "index" ];
        info[ "newFile" ] = info[ "index" ];

        info[ "origInfo" ] = "(unknown)";
        info[ "newInfo" ] = "(working copy)";
    }
}

void CDiffParser::parseDiffHeader( std::map< std::string, std::string > & info, std::vector< std::string > & header )
{
    if ( fReader->hasNextLine() && 
         ( startsWith( fReader->currentLine(), "--- " ) && startsWith( fReader->nextLine(), "+++ " ) )
         ||
         ( startsWith( fReader->currentLine(), "*** " ) && startsWith( fReader->nextLine(), "--- " ) && endsWith( fReader->currentLine(), " ****" ) )
         )
    {
        auto origFileInfos = parseFileNameHeader( fReader->currentLine().substr( 4 ), fReader->currentLineIndex() );
        info[ "origFile" ] = origFileInfos.first;
        info[ "origInfo" ] = origFileInfos.second;
        header.push_back( fReader->currentLine() );
        fReader->moveForward();

        auto newFileInfos = parseFileNameHeader( fReader->currentLine().substr( 4 ), fReader->currentLineIndex() );
        info[ "newFile" ] = newFileInfos.first;
        info[ "newInfo" ] = newFileInfos.second;
        header.push_back( fReader->currentLine() );
        fReader->moveForward();
    }
}

void CSvnDiffParser::parseDiffHeader( std::map< std::string, std::string > & info, std::vector< std::string > & header )
{
    CDiffParser::parseDiffHeader( info, header );
}

std::pair< std::string, std::string > CDiffParser::parseFileNameHeader( const std::string & str, int lineIndex )
{
    auto idx = str.find( '\t' );
    if ( idx != std::string::npos )
    {
        return std::make_pair( str.substr( 0, idx ), str.substr( idx + 1 ) );
    }
    idx = str.find( "  " );
    std::string lhs = str.substr( 0, idx );
    auto idx2 = str.find_first_not_of( " ", idx+2 );
    std::string rhs = str.substr( idx2 );
    return std::make_pair( lhs, rhs );
}


std::shared_ptr< SFileDiff > CDiffParser::parseChangeHeader()
{
    std::shared_ptr< SFileDiff > fileDiff;
    size_t start = fReader->currentLineIndex();
    std::map< std::string, std::string > info;
    std::vector< std::string > header;
    parseSpecialHeader( info, header );
    parseDiffHeader( info, header );
    auto origFilePos = info.find( "origFile" );
    auto origInfoPos = info.find( "origInfo" );
    auto newFilePos = info.find( "newFile" );
    auto newInfoPos = info.find( "newInfo" );
    if( origFilePos != info.end() && newFilePos != info.end() && origInfoPos != info.end() && newInfoPos != info.end() )
    {
        fileDiff.reset( new SFileDiff( fMerge ) );
        auto binPos = info.find( "binary" );
        if ( binPos != info.end() )
            fileDiff->fBinary = true;

        auto delPos = info.find( "deleted" );
        if ( binPos != info.end() )
            fileDiff->fDeleted = true;

        fileDiff->fOrigFile = (*origFilePos).second;
        fileDiff->fOrigInfo = (*origInfoPos).second;
        fileDiff->fNewFile = (*newFilePos).second;
        fileDiff->fNewInfo = (*newInfoPos).second;
        fileDiff->fOrigChangeSetID = info[ "origChangesetId" ];
        for( size_t ii = 0; ii < header.size(); ++ii )
        {
            const auto & currLine = header[ ii ];

            if (    startsWith( currLine, "--- " ) 
                 || startsWith( currLine, "+++ " ) 
                 || startsWith( currLine, "RCS File:" ) 
                 || startsWith( currLine, "retrieving revision "  ) 
                 || startsWith( currLine, "diff " ) 
                 || ( ( ii > start ) && currLine == INDEX_SEP && startsWith( header[ ii - 1 ], "Index: " ) )
                 || ( ( ii + 1 < header.size() ) && startsWith( currLine, "Index: " ) && header[ ii + 1 ] == INDEX_SEP ) 
                 )
            {
                fileDiff->writeLine( currLine, true );
            }
        }
    }
    return fileDiff;
}


void SFileDiff::writeLine( const std::string & text, bool isHeader )
{
    if( isHeader )
        return;

    if( ( startsWith( text, "+" ) && !startsWith( text, "+++ " ) ) )
    {
        fStatsCounter.LineFound( ELinesType::eAdded );
    }
    else if( startsWith( text, "-" ) && !startsWith( text, "--- " ) )
    {
        fStatsCounter.LineFound( ELinesType::eRemoved );
    }
    else if( startsWith( text, "\\ " ) )
    {
        fStatsCounter.LineFound( ELinesType::eEscaped );
    }
    else
    {
        fStatsCounter.LineFound( ELinesType::eOthers );
    }
}
