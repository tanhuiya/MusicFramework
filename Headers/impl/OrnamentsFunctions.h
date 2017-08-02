// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#pragma once

#include "../api/MarkData.h"
#include "../core/Enums.h"
#include "../impl/Converter.h"
#include "../impl/Cursor.h"

#pragma once

namespace mx
{
    namespace core
    {
        class Ornaments;
        class OrnamentsChoice;
        class AccidentalMark;
    }
    
    namespace impl
    {
        class OrnamentsFunctions
        {
        public:
            OrnamentsFunctions( const core::Ornaments& inOrnaments, impl::Cursor inCursor );
            ~OrnamentsFunctions() = default;
            OrnamentsFunctions( const OrnamentsFunctions& ) = default;
            OrnamentsFunctions( OrnamentsFunctions&& ) = default;
            OrnamentsFunctions& operator=( const OrnamentsFunctions& ) = default;
            OrnamentsFunctions& operator=( OrnamentsFunctions&& ) = default;
            
            void parseOrnaments( std::vector<api::MarkData>& outMarks ) const;
            
        private:
            const core::Ornaments& myOrnaments;
            const impl::Cursor myCursor;
            
        private:
            void parseOrnamentsSet( std::vector<api::MarkData>& outMarks ) const;
            void parseAccidentalMarkSet( std::vector<api::MarkData>& outMarks ) const;
            void parseOrnament( const core::OrnamentsChoice& choiceObj, api::MarkData& outMark ) const;
        };
    }
}
