#pragma once

template <typename T> inline int sgn(T val) {
    return val < T(0) ? -1 : val > T(0) ? 1 : 0;
}
