/*  Copyright (C) 2012 National Institute For Space Research (INPE) - Brazil.

This file is part of the GeoDMA - a Toolbox that integrates Data Mining Techniques with object-based
and multi-temporal analysis of satellite remotely sensed imagery.

GeoDMA is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License,
or (at your option) any later version.

GeoDMA is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with GeoDMA. See COPYING. If not, write to
GeoDMA Team at <thales@dpi.inpe.br, raian@dpi.inpe.br, castejon@dpi.inpe.br>.
*/

/*!
  \file gdma/lib/featureextraction/attribute.cpp

  \brief A structure to hold the informations about features.

  \author Raian Vargas Maretto
  \author Thales Sehn Korting
  \author Emiliano Ferreira Castejon
*/

// TerraLib Includes
#include <terralib/common/Exception.h>
#include <terralib/core/translator/Translator.h>

// STL Includes
#include <string>

// GeoDMA Includes
#include "geodmalib/common/messageMediator.hpp"
#include "geodmalib/featureextraction/attribute.hpp"

// Default Constructor
gdma::fte::Attribute::Attribute() :
    m_name(),
    m_shortName(),
    m_valueType(0),
    m_stringValue(),
    m_int16Value(0),
    m_int32Value(0),
    m_int64Value(0),
    m_floatValue(0),
    m_doubleValue(0) {
}

// Default Destructor
gdma::fte::Attribute::~Attribute() { }

// String Attribute Constructor
gdma::fte::Attribute::Attribute(std::string name, std::string shortName, std::string stringValue) :
    m_int16Value(0),
    m_int32Value(0),
    m_int64Value(0),
    m_floatValue(0),
    m_doubleValue(0) {
    this->m_name = name;

    if (shortName.length() > 10) {
        shortName = sendShortNameWarning(shortName);
    }
    this->m_shortName = shortName;

    this->m_valueType = te::dt::STRING_TYPE;
    this->m_stringValue = stringValue;
}

// int16 Attribute Constructor
gdma::fte::Attribute::Attribute(std::string name, std::string shortName, int16_t int16Value) :
    m_stringValue(""),
    m_int32Value(0),
    m_int64Value(0),
    m_floatValue(0),
    m_doubleValue(0) {
    this->m_name = name;

    if (shortName.length() > 10) {
        shortName = sendShortNameWarning(shortName);
    }
    this->m_shortName = shortName;

    this->m_valueType = te::dt::INT16_TYPE;
    this->m_int16Value = int16Value;
}

// int32 Attribute Constructor
gdma::fte::Attribute::Attribute(std::string name, std::string shortName, int32_t int32Value) :
    m_stringValue(""),
    m_int16Value(0),
    m_int64Value(0),
    m_floatValue(0),
    m_doubleValue(0) {
    this->m_name = name;

    if (shortName.length() > 10) {
        shortName = sendShortNameWarning(shortName);
    }
    this->m_shortName = shortName;

    this->m_valueType = te::dt::INT32_TYPE;
    this->m_int32Value = int32Value;
}

// int64 Attribute Constructor
gdma::fte::Attribute::Attribute(std::string name, std::string shortName, int64_t int64Value) :
    m_stringValue(""),
    m_int16Value(0),
    m_int32Value(0),
    m_floatValue(0),
    m_doubleValue(0) {
    this->m_name = name;

    if (shortName.length() > 10) {
        shortName = sendShortNameWarning(shortName);
    }
    this->m_shortName = shortName;

    this->m_valueType = te::dt::INT64_TYPE;
    this->m_int64Value = int64Value;
}

// double Attribute Constructor
gdma::fte::Attribute::Attribute(std::string name, std::string shortName, float floatValue) :
    m_stringValue(""),
    m_int16Value(0),
    m_int32Value(0),
    m_int64Value(0),
    m_doubleValue(0) {
    this->m_name = name;
    if (shortName.length() > 10) {
        shortName = sendShortNameWarning(shortName);
    }
    this->m_shortName = shortName;
    this->m_valueType = te::dt::FLOAT_TYPE;
    this->m_floatValue = floatValue;
}

// double Attribute Constructor
gdma::fte::Attribute::Attribute(std::string name, std::string shortName, double doubleValue) :
    m_stringValue(),
    m_int16Value(0),
    m_int32Value(0),
    m_int64Value(0),
    m_floatValue(0) {
    this->m_name = name;

    if (shortName.length() > 10) {
        shortName = sendShortNameWarning(shortName);
    }
    this->m_shortName = shortName;

    this->m_valueType = te::dt::DOUBLE_TYPE;
    this->m_doubleValue = doubleValue;
}

// Copy Constructor
gdma::fte::Attribute::Attribute(const Attribute &rhs) {
    this->m_name = rhs.m_name;
    this->m_shortName = rhs.m_shortName;
    this->m_valueType = rhs.m_valueType;
    this->m_stringValue = rhs.m_stringValue;
    this->m_int16Value = rhs.m_int16Value;
    this->m_int32Value = rhs.m_int32Value;
    this->m_int64Value = rhs.m_int64Value;
    this->m_floatValue = rhs.m_floatValue;
    this->m_doubleValue = rhs.m_doubleValue;
}

// Retrieve the attribute name
std::string gdma::fte::Attribute::getName() const {
    return this->m_name;
}

// Sets the attribute name
void gdma::fte::Attribute::setName(std::string name) {
    this->m_name = name;
}

// Retrieve the short name of the attribute
std::string gdma::fte::Attribute::getShortName() const {
    return this->m_shortName;
}

// Sets the short name of the attribute
void gdma::fte::Attribute::setShortName(std::string name) {
    this->m_shortName = name;
}

// Retrieve the type of the value stored.
int gdma::fte::Attribute::getValueType() const {
    return this->m_valueType;
}

// Method that retrieve the attribute value.
std::string gdma::fte::Attribute::getString() const {
    if (this->m_valueType == te::dt::STRING_TYPE) {
        return this->m_stringValue;
    } else {
        throw te::common::Exception(TE_TR("Invalid Type. The attribute type is not string."));
        return "";
    }
}

// Method that set the attribute value.
bool gdma::fte::Attribute::setString(std::string value) {
    if (this->m_valueType == te::dt::STRING_TYPE) {
        this->m_stringValue = value;
        return true;
    } else {
        throw te::common::Exception(TE_TR("Invalid Type. The attribute type is not string."));
        return false;
    }
}

// Method that retrieve the attribute value.
int16_t gdma::fte::Attribute::getInt16() const {
    if (this->m_valueType == te::dt::INT16_TYPE) {
        return this->m_int16Value;
    } else {
        throw te::common::Exception(TE_TR("Invalid Type. The attribute type is not int16."));
        return 0;
    }
}

// Method that set the attribute value.
bool gdma::fte::Attribute::setInt16(int16_t value) {
    if (this->m_valueType == te::dt::INT16_TYPE) {
        this->m_int16Value = value;
        return true;
    } else {
        throw te::common::Exception(TE_TR("Invalid Type. The attribute type is not int16."));
        return false;
    }
}

// Method that retrieve the attribute value.
int32_t gdma::fte::Attribute::getInt32() const {
    if (this->m_valueType == te::dt::INT32_TYPE) {
        return this->m_int32Value;
    } else {
        throw te::common::Exception(TE_TR("Invalid Type. The attribute type is not int32."));
        return 0;
    }
}

// Method that set the attribute value.
bool gdma::fte::Attribute::setInt32(int32_t value) {
    if (this->m_valueType == te::dt::INT32_TYPE) {
        this->m_int32Value = value;
        return true;
    } else {
        throw te::common::Exception(TE_TR("Invalid Type. The attribute type is not int32."));
        return false;
    }
}

// Method that retrieve the attribute value.
int64_t gdma::fte::Attribute::getInt64() const {
    if (this->m_valueType == te::dt::INT64_TYPE) {
        return this->m_int64Value;
    } else {
        throw te::common::Exception(TE_TR("Invalid Type. The attribute type is not int64."));
        return 0;
    }
}

// Method that set the attribute value.
bool gdma::fte::Attribute::setInt64(int64_t value) {
    if (this->m_valueType == te::dt::INT64_TYPE) {
        this->m_int64Value = value;
        return true;
    } else {
        throw te::common::Exception(TE_TR("Invalid Type. The attribute type is not int64."));
        return false;
    }
}

// Method that retrieve the attribute value.
float gdma::fte::Attribute::getFloat() const {
    if (this->m_valueType == te::dt::FLOAT_TYPE) {
        return this->m_floatValue;
    } else {
        throw te::common::Exception(TE_TR("Invalid Type. The attribute type is not float."));
        return 0;
    }
}

// Method that set the attribute value.
bool gdma::fte::Attribute::setFloat(float value) {
    if (this->m_valueType == te::dt::FLOAT_TYPE) {
        this->m_floatValue = value;
        return true;
    } else {
        throw te::common::Exception(TE_TR("Invalid Type. The attribute type is not float."));
        return false;
    }
}

// Method that retrieve the attribute value.
double gdma::fte::Attribute::getDouble() const {
    if (this->m_valueType == te::dt::DOUBLE_TYPE) {
        return this->m_doubleValue;
    } else {
        throw te::common::Exception(TE_TR("Invalid Type. The attribute type is not double."));
        return 0;
    }
}

// Method that set the attribute value.
bool gdma::fte::Attribute::setDouble(double value) {
    if (this->m_valueType == te::dt::DOUBLE_TYPE) {
        this->m_doubleValue = value;
        return true;
    } else {
        throw te::common::Exception(TE_TR("Invalid Type. The attribute type is not double."));
        return false;
    }
}

double gdma::fte::Attribute::getAsDouble() const
{
  switch (m_valueType) {
    case te::dt::INT16_TYPE:
        return static_cast< double >(m_int16Value);
        break;
    case te::dt::INT32_TYPE:
        return static_cast< double >(m_int32Value);
        break;
    case te::dt::INT64_TYPE:
        return static_cast< double >(m_int64Value);
        break;
    case te::dt::FLOAT_TYPE:
        return static_cast< double >(m_floatValue);
        break;
    case te::dt::DOUBLE_TYPE:
        return m_doubleValue;
        break;
    default:
        throw te::common::Exception(TE_TR("Invalid Type. The attribute type is not numeric."));
        break;
  }
}

// -----------------------------------------
// Private Members

std::string gdma::fte::Attribute::sendShortNameWarning(std::string shortName) {
    std::stringstream msg;
    msg << "Feature short name '" << shortName << "' is too long! It will be truncated to '";
    shortName.resize(10);
    msg << shortName << "'!";

    gdma::common::MessageMediator::getInstance().sendWarningMsg(TE_TR(msg.str()));

    return shortName;
}
