#pragma once

// One bar's vertical slot, shared by every track column so bar lines land
// at the same pixel height everywhere. rowCount is the tallest number of
// notes any single track has within this bar; tracks with fewer notes in
// this bar just leave the rest of the band blank (no filler rows).
struct BarBand
{
    int barIndex = 0;
    int yPixel = 0;
    int rowCount = 1;
};
