#include "builder.h"

#include <src/util/stream/output.h>

template <>
void Out<TStringBuilder>(IOutputStream& os, const TStringBuilder& sb) {
    os << static_cast<const std::string&>(sb);
}