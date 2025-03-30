#define NOMINMAX
#include "../include/PatternAnalyzer.h"
#include "../include/Constants.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include <numeric>
#include <cmath>

std::vector<InputEvent> PatternAnalyzer::parseLogFile(const std::string& filename) {
    std::vector<InputEvent> events;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file " + filename);
    }

    if (!std::getline(file, line)) {
        std::cerr << "Warning: File is empty or could not read header line from " << filename << std::endl;
        return events;
    }

    if (line != "Timestamp,EventType,KeyCode") {
        std::cerr << "Warning: Unexpected header format in file " << filename << ": " << line << std::endl;
    }

    int lineNumber = 1;
    while (std::getline(file, line)) {
        lineNumber++;
        std::stringstream ss(line);
        std::string segment;
        std::vector<std::string> parts;

        while (std::getline(ss, segment, ',')) {
            parts.push_back(segment);
        }

        if (parts.size() == 3) {
            try {
                InputEvent event;
                long long timestamp_ms = std::stoll(parts[0]);
                event.timestamp = std::chrono::time_point<std::chrono::high_resolution_clock>(
                    std::chrono::milliseconds(timestamp_ms)
                );

                std::string eventTypeString = parts[1];
                if (eventTypeString == "KEY_DOWN") {
                    event.type = EventType::KEY_DOWN;
                }
                else if (eventTypeString == "KEY_UP") {
                    event.type = EventType::KEY_UP;
                }
                else {
                    std::cerr << "Warning: Unknown event type '" << eventTypeString
                        << "' at line " << lineNumber << " in file " << filename << std::endl;
                    continue;
                }

                event.keyCode = std::stoul(parts[2]);
                events.push_back(event);
            }
            catch (const std::invalid_argument& e) {
                std::cerr << "Warning: Invalid data format at line " << lineNumber
                    << " in file " << filename << " - " << e.what() << " (Line: " << line << ")" << std::endl;
            }
            catch (const std::out_of_range& e) {
                std::cerr << "Warning: Data out of range at line " << lineNumber
                    << " in file " << filename << " - " << e.what() << " (Line: " << line << ")" << std::endl;
            }
        }
        else {
            std::cerr << "Warning: Incorrect number of fields (" << parts.size() << ") at line " << lineNumber
                << " in file " << filename << " (Line: " << line << ")" << std::endl;
        }
    }

    file.close();
    return events;
}

bool PatternAnalyzer::isWithinTimeWindow(const InputEvent& event1, const InputEvent& event2, long long threshold_ms) {
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(event2.timestamp - event1.timestamp);
    return duration.count() <= threshold_ms;
}

PatternFrequencyMap PatternAnalyzer::calculateMicroPatternFrequencies(const std::vector<InputEvent>& events) {
    PatternFrequencyMap frequencies;
    const int patternMinLength = 6;
    const int patternMaxLength = 8;
    const long long timeThresholdMs = 300;

    for (size_t i = 0; i < events.size(); ++i) {
        if (events[i].type == EventType::KEY_DOWN && events[i].keyCode == VK_LMENU) {
            for (int len = patternMinLength; len <= patternMaxLength && i + len <= events.size(); ++len) {
                MicroPattern currentPattern;
                bool patternValid = true;
                bool containsCoreKey = false;

                for (int j = 0; j < len; ++j) {
                    if (j > 0 && !isWithinTimeWindow(events[i + j - 1], events[i + j], timeThresholdMs)) {
                        patternValid = false;
                        break;
                    }
                    currentPattern.push_back({events[i + j].type, events[i + j].keyCode});
                    if (Constants::patternStartKeys.count(events[i+j].keyCode)) {
                        containsCoreKey = true;
                    }
                }

                if (patternValid && containsCoreKey) {
                    frequencies[currentPattern]++;
                }
            }
        }
    }
    return frequencies;
}

std::string PatternAnalyzer::getVirtualKeyName(unsigned int keyCode) {
    switch (keyCode) {
        case VK_F1: return "F1";
        case VK_F2: return "F2";
        case VK_F3: return "F3";
        case VK_F4: return "F4";
        case VK_F5: return "F5";
        case VK_F6: return "F6";
        case VK_F7: return "F7";
        case VK_F8: return "F8";
        case VK_F9: return "F9";
        case VK_F10: return "F10";
        case VK_F11: return "F11";
        case VK_F12: return "F12";
        case VK_RETURN: return "ENTER";
        case VK_ESCAPE: return "ESC";
        case VK_TAB: return "TAB";
        case VK_SPACE: return "SPACE";
        case VK_BACK: return "BACKSPACE";
        case VK_DELETE: return "DELETE";
        case VK_CONTROL: return "CTRL";
        case VK_MENU: return "ALT";
        case VK_SHIFT: return "SHIFT";
        case VK_CAPITAL: return "CAPS";
        case VK_LMENU: return "LALT";
        case VK_UP: return "UP";
        case VK_DOWN: return "DOWN";
        case VK_LEFT: return "LEFT";
        case VK_RIGHT: return "RIGHT";
        case 0x30: return "0";
        case 0x31: return "1";
        case 0x32: return "2";
        case 0x33: return "3";
        case 0x34: return "4";
        case 0x35: return "5";
        case 0x36: return "6";
        case 0x37: return "7";
        case 0x38: return "8";
        case 0x39: return "9";
        case 0x41: return "A";
        case 0x42: return "B";
        case 0x43: return "C";
        case 0x44: return "D";
        case 0x45: return "E";
        case 0x46: return "F";
        case 0x47: return "G";
        case 0x48: return "H";
        case 0x49: return "I";
        case 0x4A: return "J";
        case 0x4B: return "K";
        case 0x4C: return "L";
        case 0x4D: return "M";
        case 0x4E: return "N";
        case 0x4F: return "O";
        case 0x50: return "P";
        case 0x51: return "Q";
        case 0x52: return "R";
        case 0x53: return "S";
        case 0x54: return "T";
        case 0x55: return "U";
        case 0x56: return "V";
        case 0x57: return "W";
        case 0x58: return "X";
        case 0x59: return "Y";
        case 0x5A: return "Z";
        default: return "VK_" + std::to_string(keyCode);
    }
}

void PatternAnalyzer::printFrequencies(const PatternFrequencyMap& frequencies, const std::string& label) {
    std::vector<std::pair<MicroPattern, int>> sortedFrequencies(frequencies.begin(), frequencies.end());
    std::sort(sortedFrequencies.begin(), sortedFrequencies.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });

    std::cout << "--- " << label << " Micro-Pattern Frequencies ---" << std::endl;
    int totalPatterns = 0;
    for(const auto& pair : sortedFrequencies) {
        totalPatterns += pair.second;
    }

    for (const auto& pair : sortedFrequencies) {
        const MicroPattern& pattern = pair.first;
        int count = pair.second;
        double percentage = (totalPatterns > 0) ? (static_cast<double>(count) / totalPatterns * 100.0) : 0.0;

        std::cout << "Count: " << count << " (" << std::fixed << std::setprecision(2) << percentage << "%) - Pattern: ";
        for (const auto& eventPair : pattern) {
            std::cout << "[" << (eventPair.first == EventType::KEY_DOWN ? "DOWN" : "UP") << "," 
                     << getVirtualKeyName(eventPair.second) << "] ";
        }
        std::cout << std::endl;
    }
    std::cout << "Total unique patterns: " << frequencies.size() << std::endl;
    std::cout << "Total pattern instances: " << totalPatterns << std::endl;
    std::cout << "---------------------------------" << std::endl;
}

double PatternAnalyzer::calculateTopNConcentration(const std::vector<PatternCountPair>& sortedFrequencies,
    long long totalInstances, int N) {
    if (totalInstances == 0 || sortedFrequencies.empty() || N <= 0) {
        return 0.0;
    }

    long long topNCount = 0;
    int count = 0;
    for (const auto& pair : sortedFrequencies) {
        if (count >= N) {
            break;
        }
        topNCount += pair.second;
        count++;
    }

    return (static_cast<double>(topNCount) / totalInstances) * 100.0;
}

int PatternAnalyzer::calculateCoveragePatternCount(const std::vector<PatternCountPair>& sortedFrequencies,
    long long totalInstances, double coveragePercentage) {
    if (totalInstances == 0 || sortedFrequencies.empty() || coveragePercentage <= 0.0) {
        return 0;
    }
    coveragePercentage = std::min(100.0, coveragePercentage);

    long long targetCount = static_cast<long long>(totalInstances * (coveragePercentage / 100.0));
    if (targetCount <= 0) {
        return (totalInstances > 0 && coveragePercentage > 0.0) ? 1 : 0;
    }

    long long currentCount = 0;
    int patternsNeeded = 0;

    for (const auto& pair : sortedFrequencies) {
        currentCount += pair.second;
        patternsNeeded++;
        if (currentCount >= targetCount) {
            break;
        }
    }
    if (currentCount < targetCount) {
        return static_cast<int>(sortedFrequencies.size());
    }

    return patternsNeeded;
}

double PatternAnalyzer::calculateSuspiciousPatternScore(const PatternFrequencyMap& frequencies,
    long long totalInstances, const std::set<MicroPattern>& suspiciousPatterns) {
    if (totalInstances == 0 || frequencies.empty() || suspiciousPatterns.empty()) {
        return 0.0;
    }

    long long suspiciousCount = 0;
    for (const MicroPattern& suspiciousPat : suspiciousPatterns) {
        auto it = frequencies.find(suspiciousPat);
        if (it != frequencies.end()) {
            suspiciousCount += it->second;
        }
    }

    return (static_cast<double>(suspiciousCount) / totalInstances) * 100.0;
}

double PatternAnalyzer::calculateBotSuspicionScore(double top2Concentration,
    double top5Concentration, int patternsFor50Coverage, double suspiciousScore) {
    const double THRESH_CONC_TOP2_LOW = 30.0;
    const double THRESH_CONC_TOP5_LOW = 45.0;
    const double THRESH_COVERAGE_HIGH = 12.0;
    const double THRESH_SUSPICIOUS_HIGH = 8.0;

    double scoreConcentration = 0.0;
    if (top5Concentration < THRESH_CONC_TOP5_LOW) {
        scoreConcentration = 1.0 - (top5Concentration / THRESH_CONC_TOP5_LOW);
    }
    scoreConcentration = std::max(0.0, std::min(1.0, scoreConcentration));

    double scoreFlatness = 0.0;
    if (patternsFor50Coverage > 0) {
        scoreFlatness = static_cast<double>(patternsFor50Coverage) / THRESH_COVERAGE_HIGH;
    }
    scoreFlatness = std::max(0.0, std::min(1.0, scoreFlatness));

    double scoreSuspicious = 0.0;
    if (THRESH_SUSPICIOUS_HIGH > 0) {
        scoreSuspicious = suspiciousScore / THRESH_SUSPICIOUS_HIGH;
    }
    scoreSuspicious = std::max(0.0, std::min(1.0, scoreSuspicious));

    const double WEIGHT_CONCENTRATION = 0.5;
    const double WEIGHT_FLATNESS = 0.4;
    const double WEIGHT_SUSPICIOUS = 0.1;

    double finalScore = (WEIGHT_CONCENTRATION * scoreConcentration) +
        (WEIGHT_FLATNESS * scoreFlatness) +
        (WEIGHT_SUSPICIOUS * scoreSuspicious);

    finalScore = std::max(0.0, std::min(1.0, finalScore));

    return finalScore;
}

bool PatternAnalyzer::isBotSuspected(double finalScore) {
    const double FINAL_DECISION_THRESHOLD = 0.6;
    return finalScore > FINAL_DECISION_THRESHOLD;
}

void PatternAnalyzer::saveAnalysisResults(
    const std::string& filename,
    const std::vector<PatternCountPair>& sortedFrequencies,
    double top2Concentration,
    double top5Concentration,
    int coveragePatternCount,
    double suspiciousScore,
    double finalScore) {
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    // CSV 형식으로 저장
    file << "pattern_id,pattern,frequency,percentage\n";
    
    int totalPatterns = 0;
    for (const auto& pair : sortedFrequencies) {
        totalPatterns += pair.second;
    }

    int patternId = 1;
    for (const auto& pair : sortedFrequencies) {
        const auto& pattern = pair.first;
        int count = pair.second;
        double percentage = (totalPatterns > 0) ? 
            (static_cast<double>(count) / totalPatterns * 100.0) : 0.0;

        // 패턴을 문자열로 변환
        std::stringstream patternStr;
        for (const auto& event : pattern) {
            patternStr << (event.first == EventType::KEY_DOWN ? "DOWN" : "UP") 
                      << "_" << getVirtualKeyName(event.second) << "|";
        }

        file << patternId << "," 
             << patternStr.str() << "," 
             << count << "," 
             << std::fixed << std::setprecision(2) << percentage << "\n";
        patternId++;
    }

    // 메트릭 정보 저장
    file << "\nmetrics\n";
    file << "metric,value\n";
    file << "top2_concentration," << top2Concentration << "\n";
    file << "top5_concentration," << top5Concentration << "\n";
    file << "coverage_pattern_count," << coveragePatternCount << "\n";
    file << "suspicious_score," << suspiciousScore << "\n";
    file << "final_score," << finalScore << "\n";

    file.close();
} 