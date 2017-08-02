// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#pragma once

#include "../../core/ForwardDeclare.h"
#include "../../core/ElementInterface.h"
#include "../../core/Enums.h"
#include "../../core/elements/TupletTypeAttributes.h"

#include <iosfwd>
#include <memory>
#include <vector>

namespace mx
{
    namespace core
    {

        MX_FORWARD_DECLARE_ATTRIBUTES( TupletTypeAttributes )
        MX_FORWARD_DECLARE_ELEMENT( TupletType )

        inline TupletTypePtr makeTupletType() { return std::make_shared<TupletType>(); }
		inline TupletTypePtr makeTupletType( const NoteTypeValue& value ) { return std::make_shared<TupletType>( value ); }
		inline TupletTypePtr makeTupletType( NoteTypeValue&& value ) { return std::make_shared<TupletType>( std::move( value ) ); }

        class TupletType : public ElementInterface
        {
        public:
            TupletType();
            TupletType( const NoteTypeValue& value );

            virtual bool hasAttributes() const;
            virtual bool hasContents() const;
            virtual std::ostream& streamAttributes( std::ostream& os ) const;
            virtual std::ostream& streamName( std::ostream& os ) const;
            virtual std::ostream& streamContents( std::ostream& os, const int indentLevel, bool& isOneLineOnly ) const;
            TupletTypeAttributesPtr getAttributes() const;
            void setAttributes( const TupletTypeAttributesPtr& attributes );
            NoteTypeValue getValue() const;
            void setValue( const NoteTypeValue& value );

            bool fromXElement( std::ostream& message, xml::XElement& xelement );

        private:
            NoteTypeValue myValue;
            TupletTypeAttributesPtr myAttributes;
        };
    }
}
