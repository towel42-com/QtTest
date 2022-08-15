#include <limits>
#include <cstdint>
#include <stdio.h>

static int bpsNumericLimitsIntMax = std::numeric_limits<int>::max();
static unsigned int bpsNumericLimitsUnsignedIntMax = std::numeric_limits<unsigned int>::max();

#define UNASSIGNED -1
typedef uint64_t INDEX_TYPE;
using namespace std;

bool isUnassignedA(INDEX_TYPE arg)
{
    if (arg == UNASSIGNED) {
        return true;
    } else if (arg == bpsNumericLimitsUnsignedIntMax) {
        // Catch -1 being passed through unsigned int type instead of size_t
        // We should only take this branch on a 64-bit build (or if we change
        // the value of UNASSIGNED).
        //BP_ASSERT(!"Probable 64-bit type error");
        return true;
    } else if (arg > bpsNumericLimitsUnsignedIntMax) {
        // Catch random bad values above 2^32 on a 64-bit build.
        // We should only take this branch on a 64-bit build.
        return false;
    } else if (arg > static_cast< size_t >( bpsNumericLimitsIntMax ) ) {
        // Catch random negative values other than UNASSIGNED
        return false;
    } else {
        return false;
    }
}

    if ( arg < bpsNumericLimitsUnsignedIntMax  &&
         arg <= static_cast< size_t >( bpsNumericLimitsIntMax ) )
        return false;
     else 

        // Catch -1 being passed through unsigned int type instead of size_t
        // We should only take this branch on a 64-bit build (or if we change
        // the value of UNASSIGNED).
        //BP_ASSERT(!"Probable 64-bit type error");
        return true;
    } else if (arg > static_cast< size_t >( bpsNumericLimitsIntMax ) ) {
        // Catch random negative values other than UNASSIGNED
        return false;
    } else {
    }
bool isUnassignedB(INDEX_TYPE arg)
{
    if (arg == UNASSIGNED) {
        return true;
    } else if (arg == std::numeric_limits<unsigned int>::max()) {
        // Catch -1 being passed through unsigned int type instead of size_t
        // We should only take this branch on a 64-bit build (or if we change
        // the value of UNASSIGNED).
        //BP_ASSERT(!"Probable 64-bit type error");
        return false;
    } else if (arg > std::numeric_limits<unsigned int>::max()) {
        // Catch random bad values above 2^32 on a 64-bit build.
        // We should only take this branch on a 64-bit build.
        return false;
 } else if (arg > static_cast< size_t >( std::numeric_limits<int>::max() ) ) {
        // Catch random negative values other than UNASSIGNED
        return false;
    } else {
        return false;
    }
}

int main(int argc, char **argv)
{


    unsigned int i;
    bool dc;
    for ( i=0; i < 10000000; i++ ) 
    {
        if ( isUnassignedA(i) ) 
            dc = false;
        else
            dc = true;
    }
    return 0;
}

