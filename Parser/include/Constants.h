#pragma once
#include <windows.h>
#include <unordered_set>

namespace Constants {
    const std::unordered_set<unsigned int> patternStartKeys = { VK_LMENU };
    
    // 임계값 정의
    const double THRESH_CONC_TOP2_LOW = 30.0;
    const double THRESH_CONC_TOP5_LOW = 45.0;
    const double THRESH_COVERAGE_HIGH = 12.0;
    const double THRESH_SUSPICIOUS_HIGH = 8.0;
    const double FINAL_DECISION_THRESHOLD = 0.6;

    // 가중치 정의
    const double WEIGHT_CONCENTRATION = 0.5;
    const double WEIGHT_FLATNESS = 0.4;
    const double WEIGHT_SUSPICIOUS = 0.1;
} 