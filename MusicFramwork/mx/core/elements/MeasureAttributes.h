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

        MX_FORWARD_DECLARE_ATTRIBUTES( MeasureAttributes )

        struct MeasureAttributes : public AttributesInterface
        {
        public:
            MeasureAttributes();
            virtual bool hasValues() const;
            virtual std::ostream& toStream( std::ostream& os ) const;
            XsToken number;
            YesNo implicit;
            YesNo nonControlling;
            TenthsValue width;
            const 	bool hasNumber;
            bool hasImplicit;
            bool hasNonControlling;
            bool hasWidth;

            bool fromXElement( std::ostream& message, xml::XElement& xelement );
        };
    }
}
