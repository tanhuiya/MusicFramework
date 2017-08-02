// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#pragma once

#include "../../core/ForwardDeclare.h"
#include "../../core/AttributesInterface.h"
#include "../../core/Enums.h"
#include "../../core/Integers.h"

#include <iosfwd>
#include <memory>
#include <vector>

namespace mx
{
    namespace core
    {

        MX_FORWARD_DECLARE_ATTRIBUTES( BeamAttributes )

        struct BeamAttributes : public AttributesInterface
        {
        public:
            BeamAttributes();
            virtual bool hasValues() const;
            virtual std::ostream& toStream( std::ostream& os ) const;
            BeamLevel number;
            YesNo repeater;
            Fan fan;
            bool hasNumber;
            bool hasRepeater;
            bool hasFan;

            bool fromXElement( std::ostream& message, xml::XElement& xelement );
        };
    }
}
