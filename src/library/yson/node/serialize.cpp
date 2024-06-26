#include "serialize.h"

#include "node_visitor.h"

#include <ydb-cpp-sdk/library/yson/consumer.h>

namespace NYT {

////////////////////////////////////////////////////////////////////////////////

void Serialize(const std::string& value, NYson::IYsonConsumer* consumer)
{
    consumer->OnStringScalar(value);
}

void Serialize(const std::string_view& value, NYson::IYsonConsumer* consumer)
{
    consumer->OnStringScalar(value);
}

void Serialize(const char* value, NYson::IYsonConsumer* consumer)
{
    consumer->OnStringScalar(value);
}

void Deserialize(std::string& value, const TNode& node)
{
    value = node.AsString();
}

#define SERIALIZE_SIGNED(type) \
void Serialize(type value, NYson::IYsonConsumer* consumer) \
{ \
    consumer->OnInt64Scalar(static_cast<i64>(value)); \
}

#define SERIALIZE_UNSIGNED(type) \
void Serialize(type value, NYson::IYsonConsumer* consumer) \
{ \
    consumer->OnUint64Scalar(static_cast<ui64>(value)); \
}

SERIALIZE_SIGNED(signed char)
SERIALIZE_SIGNED(short)
SERIALIZE_SIGNED(int)
SERIALIZE_SIGNED(long)
SERIALIZE_SIGNED(long long)

SERIALIZE_UNSIGNED(unsigned char)
SERIALIZE_UNSIGNED(unsigned short)
SERIALIZE_UNSIGNED(unsigned int)
SERIALIZE_UNSIGNED(unsigned long)
SERIALIZE_UNSIGNED(unsigned long long)

#undef SERIALIZE_SIGNED
#undef SERIALIZE_UNSIGNED

void Deserialize(i64& value, const TNode& node)
{
    value = node.AsInt64();
}

void Deserialize(ui64& value, const TNode& node)
{
    value = node.AsUint64();
}

void Serialize(double value, NYson::IYsonConsumer* consumer)
{
    consumer->OnDoubleScalar(value);
}

void Deserialize(double& value, const TNode& node)
{
    value = node.AsDouble();
}

void Serialize(bool value, NYson::IYsonConsumer* consumer)
{
    consumer->OnBooleanScalar(value);
}

void Deserialize(bool& value, const TNode& node)
{
    value = node.AsBool();
}

void Serialize(const TNode& node, NYson::IYsonConsumer* consumer)
{
    TNodeVisitor visitor(consumer);
    visitor.Visit(node);
}

void Deserialize(TNode& value, const TNode& node)
{
    value = node;
}

////////////////////////////////////////////////////////////////////////////////

} // namespace NYT
