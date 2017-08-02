// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#pragma once

#include "../api/PartData.h"
#include "../api/StaffData.h"
#include "../core/elements/PartwiseMeasure.h"

namespace mx
{
    namespace impl
    {

        
        void createStavesFromMx( int inGlobalTicksPerQuarter, const core::PartwiseMeasureSet& inMxMeasures, std::vector<api::StaffData>& outStaves );
    }
}
