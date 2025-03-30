#pragma once

#include <vector>
#include <map>
#include <string>
#include <set>
#include <chrono>
#include "InputEvent.h"

// 패턴 정의: [(DOWN, ALT), (UP, ALT), ...]
using MicroPattern = std::vector<std::pair<EventType, unsigned int>>;

// 패턴 빈도수 맵: Pattern -> Count
using PatternFrequencyMap = std::map<MicroPattern, int>;

// 정렬된 빈도수 쌍: {Pattern, Count}
using PatternCountPair = std::pair<MicroPattern, int>;

class PatternAnalyzer {
public:
    // 파일 파싱
    static std::vector<InputEvent> parseLogFile(const std::string& filename);
    
    // 패턴 분석
    static PatternFrequencyMap calculateMicroPatternFrequencies(const std::vector<InputEvent>& events);
    static bool isWithinTimeWindow(const InputEvent& event1, const InputEvent& event2, long long threshold_ms);
    static std::string getVirtualKeyName(unsigned int keyCode);
    static void printFrequencies(const PatternFrequencyMap& frequencies, const std::string& label);
    
    // 봇 탐지 기능
    static double calculateTopNConcentration(const std::vector<PatternCountPair>& sortedFrequencies,
        long long totalInstances, int N);
    static int calculateCoveragePatternCount(const std::vector<PatternCountPair>& sortedFrequencies,
        long long totalInstances, double coveragePercentage);
    static double calculateSuspiciousPatternScore(const PatternFrequencyMap& frequencies,
        long long totalInstances, const std::set<MicroPattern>& suspiciousPatterns);
    static double calculateBotSuspicionScore(double top2Concentration,
        double top5Concentration, int patternsFor50Coverage, double suspiciousScore);
    static bool isBotSuspected(double finalScore);

    // 분석 결과를 JSON 파일로 저장
    static void saveAnalysisResults(
        const std::string& filename,
        const std::vector<PatternCountPair>& sortedFrequencies,
        double top2Concentration,
        double top5Concentration,
        int coveragePatternCount,
        double suspiciousScore,
        double finalScore);
}; 