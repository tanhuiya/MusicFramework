// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#pragma once

#include "../../core/ForwardDeclare.h"
#include "../../core/AttributesInterface.h"
#include "../../core/Decimals.h"
#include "../../core/Enums.h"
#include "../../core/Strings.h"

#include <iosfwd>
#include <memory>
#include <vector>

namespace mx
{
    namespace core
    {

        MX_FORWARD_DECLARE_ATTRIBUTES( BarlineAttributes )

        struct BarlineAttributes : public AttributesInterface
        {
        public:
            BarlineAttributes();
            virtual bool hasValues() const;
            virtual std::ostream& toStream( std::ostream& os ) const;
            RightLeftMiddle location;
            XsToken segno;
            XsToken coda;
            DivisionsValue divisions;
            bool hasLocation;
            bool hasSegno;
            bool hasCoda;
            bool hasDivisions;

            bool fromXElement( std::ostream& message, xml::XElement& xelement );
        };
    }
}
