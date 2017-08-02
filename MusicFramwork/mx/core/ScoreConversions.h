// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#pragma once

#include <iostream>
#include "../core/elements/ScorePartwise.h"
#include "../core/elements/ScoreTimewise.h"
#include "../core/elements/ScoreHeaderGroup.h"
#include <memory>

namespace mx
{
    namespace core
    {
    	// Converts a 'score-timewise' to 'score-partwise'
    	// caution - uses shallow copies, does not clone
    	ScorePartwisePtr timewisePartwise( const ScoreTimewisePtr& inScore );

    	// Converts a 'score-timewise' to 'score-partwise'
    	// caution - uses shallow copies, does not clone
    	ScoreTimewisePtr partwiseTimewise( const ScorePartwisePtr& inScore );
    }
}
