// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#pragma once

#include "../../core/ForwardDeclare.h"
#include "../../core/AttributesInterface.h"
#include "../../core/Decimals.h"
#include "../../core/Enums.h"
#include "../../core/Integers.h"
#include "../../core/Strings.h"
#include "../../core/FontSize.h"

#include <iosfwd>
#include <memory>
#include <vector>

namespace mx
{
    namespace core
    {

        MX_FORWARD_DECLARE_ATTRIBUTES( OctaveShiftAttributes )

        struct OctaveShiftAttributes : public AttributesInterface
        {
        public:
            OctaveShiftAttributes();
            virtual bool hasValues() const;
            virtual std::ostream& toStream( std::ostream& os ) const;
            UpDownStopContinue type;
            NumberLevel number;
            PositiveInteger size;
            TenthsValue dashLength;
            TenthsValue spaceLength;
            TenthsValue defaultX;
            TenthsValue defaultY;
            TenthsValue relativeX;
            TenthsValue relativeY;
            CommaSeparatedText fontFamily;
            FontStyle fontStyle;
            FontSize fontSize;
            FontWeight fontWeight;
            const bool hasType;
            bool hasNumber;
            bool hasSize;
            bool hasDashLength;
            bool hasSpaceLength;
            bool hasDefaultX;
            bool hasDefaultY;
            bool hasRelativeX;
            bool hasRelativeY;
            bool hasFontFamily;
            bool hasFontStyle;
            bool hasFontSize;
            bool hasFontWeight;

            bool fromXElement( std::ostream& message, xml::XElement& xelement );
        };
    }
}
