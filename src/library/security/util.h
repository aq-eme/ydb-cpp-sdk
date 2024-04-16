#pragma once

#include <src/util/generic/fwd.h>
#include <src/util/datetime/base.h>

namespace NKikimr {
    // copy-pasted from <robot/library/utils/time_convert.h>
    template<typename Rep, typename Period>
    constexpr ui64 ToMicroseconds(std::chrono::duration<Rep, Period> value) {
        return std::chrono::duration_cast<std::chrono::microseconds>(value).count();
    }
    
    template<typename Clock, typename Duration>
    constexpr TInstant ToInstant(std::chrono::time_point<Clock, Duration> value) {
        return TInstant::MicroSeconds(ToMicroseconds(value.time_since_epoch()));
    }
}