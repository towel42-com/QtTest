bool isUnassignedNew(INDEX_TYPE arg)
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