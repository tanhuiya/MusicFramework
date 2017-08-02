// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#include "../../core/elements/ScoreTimewiseAttributes.h"
#include "../../core/FromXElement.h"
#include "../../xml/XElement.h"
#include "../../xml/XAttributeIterator.h"
#include <iostream>

namespace mx
{
    namespace core
    {
        ScoreTimewiseAttributes::ScoreTimewiseAttributes()
        :version( "3.0" )
        ,hasVersion( false )
        {}


        bool ScoreTimewiseAttributes::hasValues() const
        {
            return hasVersion;
        }


        std::ostream& ScoreTimewiseAttributes::toStream( std::ostream& os ) const
        {
            if ( hasValues() )
            {
                streamAttribute( os, version, "version", hasVersion );
            }
            return os;
        }


        bool ScoreTimewiseAttributes::fromXElement( std::ostream& message, xml::XElement& xelement )
        {
            const char* const className = "ScoreTimewiseAttributes";
            bool isSuccess = true;
        
            auto it = xelement.attributesBegin();
            auto endIter = xelement.attributesEnd();
        
            for( ; it != endIter; ++it )
            {
                if( parseAttribute( message, it, className, isSuccess, version, hasVersion, "version" ) ) { continue; }
            }
        
            MX_RETURN_IS_SUCCESS;
        }

    }
}
