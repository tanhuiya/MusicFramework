// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#pragma once

#include "../../core/ForwardDeclare.h"
#include "../../core/ElementInterface.h"
#include "../../core/Integers.h"

#include <iosfwd>
#include <memory>
#include <vector>

namespace mx
{
    namespace core
    {

        MX_FORWARD_DECLARE_ELEMENT( FrameFrets )

        inline FrameFretsPtr makeFrameFrets() { return std::make_shared<FrameFrets>(); }
		inline FrameFretsPtr makeFrameFrets( const PositiveInteger& value ) { return std::make_shared<FrameFrets>( value ); }
		inline FrameFretsPtr makeFrameFrets( PositiveInteger&& value ) { return std::make_shared<FrameFrets>( std::move( value ) ); }

        class FrameFrets : public ElementInterface
        {
        public:
            FrameFrets();
            FrameFrets( const PositiveInteger& value );

            virtual bool hasAttributes() const;
            virtual bool hasContents() const;
            virtual std::ostream& streamAttributes( std::ostream& os ) const;
            virtual std::ostream& streamName( std::ostream& os ) const;
            virtual std::ostream& streamContents( std::ostream& os, const int indentLevel, bool& isOneLineOnly ) const;
            PositiveInteger getValue() const;
            void setValue( const PositiveInteger& value );

            bool fromXElement( std::ostream& message, xml::XElement& xelement );

        private:
            PositiveInteger myValue;
        };
    }
}
