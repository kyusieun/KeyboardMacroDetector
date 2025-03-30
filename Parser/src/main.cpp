#include "../include/PatternAnalyzer.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <set>
#include <iomanip>

int main() {
    // --- 로그 파일 파싱 및 패턴 분석 ---
    std::string botLogFilename = "MacroPattern.csv";
    std::string humanLogFilename = "UserPattern.csv";

    std::vector<InputEvent> botEvents = PatternAnalyzer::parseLogFile(botLogFilename);
    std::cout << "Parsed " << botEvents.size() << " events from bot log." << std::endl;

    std::vector<InputEvent> humanEvents = PatternAnalyzer::parseLogFile(humanLogFilename);
    std::cout << "Parsed " << humanEvents.size() << " events from human log." << std::endl;

    std::cout << "\n=== Bot Pattern Analysis ===" << std::endl;
    PatternFrequencyMap botFrequencies = PatternAnalyzer::calculateMicroPatternFrequencies(botEvents);
    PatternAnalyzer::printFrequencies(botFrequencies, "Bot");

    std::cout << "\n=== Human Pattern Analysis ===" << std::endl;
    PatternFrequencyMap humanFrequencies = PatternAnalyzer::calculateMicroPatternFrequencies(humanEvents);
    PatternAnalyzer::printFrequencies(humanFrequencies, "Human");

    // --- 빈도수 정렬 및 총 인스턴스 계산 ---
    std::vector<PatternCountPair> sortedBotFreqs(botFrequencies.begin(), botFrequencies.end());
    std::vector<PatternCountPair> sortedHumanFreqs(humanFrequencies.begin(), humanFrequencies.end());
    
    std::sort(sortedBotFreqs.begin(), sortedBotFreqs.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    std::sort(sortedHumanFreqs.begin(), sortedHumanFreqs.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    long long totalBotInstances = std::accumulate(sortedBotFreqs.begin(), sortedBotFreqs.end(), 0LL,
        [](long long sum, const auto& pair) { return sum + pair.second; });
    long long totalHumanInstances = std::accumulate(sortedHumanFreqs.begin(), sortedHumanFreqs.end(), 0LL,
        [](long long sum, const auto& pair) { return sum + pair.second; });

    // --- 의심 패턴 목록 정의 ---
    std::set<MicroPattern> suspiciousPatternSet;
    
    // humanFrequencies에서 빈도수가 2 이하인 패턴들을 의심 패턴으로 추가
    for (const auto& pair : humanFrequencies) {
        if (pair.second <= 2) {
            suspiciousPatternSet.insert(pair.first);
        }
    }

    // --- 봇 데이터 분석 및 판정 ---
    std::cout << "\n=== Analyzing Bot Data ===" << std::endl;
    double botTop2 = PatternAnalyzer::calculateTopNConcentration(sortedBotFreqs, totalBotInstances, 2);
    double botTop5 = PatternAnalyzer::calculateTopNConcentration(sortedBotFreqs, totalBotInstances, 5);
    int botCoverageCount = PatternAnalyzer::calculateCoveragePatternCount(sortedBotFreqs, totalBotInstances, 50.0);
    double botSuspiciousScore = PatternAnalyzer::calculateSuspiciousPatternScore(botFrequencies, totalBotInstances, suspiciousPatternSet);

    std::cout << "Bot Top 2 Conc.: " << std::fixed << std::setprecision(2) << botTop2 << "%" << std::endl;
    std::cout << "Bot Top 5 Conc.: " << std::fixed << std::setprecision(2) << botTop5 << "%" << std::endl;
    std::cout << "Bot 50% Cover #: " << botCoverageCount << std::endl;
    std::cout << "Bot Suspicious %: " << std::fixed << std::setprecision(2) << botSuspiciousScore << "%" << std::endl;

    double botFinalScore = PatternAnalyzer::calculateBotSuspicionScore(botTop2, botTop5, botCoverageCount, botSuspiciousScore);
    std::cout << "Bot Final Score: " << std::fixed << std::setprecision(4) << botFinalScore << std::endl;
    std::cout << "Bot Suspected: " << (PatternAnalyzer::isBotSuspected(botFinalScore) ? "Yes" : "No") << std::endl;

    // --- 사람 데이터 분석 및 판정 ---
    std::cout << "\n=== Analyzing Human Data ===" << std::endl;
    double humanTop2 = PatternAnalyzer::calculateTopNConcentration(sortedHumanFreqs, totalHumanInstances, 2);
    double humanTop5 = PatternAnalyzer::calculateTopNConcentration(sortedHumanFreqs, totalHumanInstances, 5);
    int humanCoverageCount = PatternAnalyzer::calculateCoveragePatternCount(sortedHumanFreqs, totalHumanInstances, 50.0);
    double humanSuspiciousScore = PatternAnalyzer::calculateSuspiciousPatternScore(humanFrequencies, totalHumanInstances, suspiciousPatternSet);

    std::cout << "Human Top 2 Conc.: " << std::fixed << std::setprecision(2) << humanTop2 << "%" << std::endl;
    std::cout << "Human Top 5 Conc.: " << std::fixed << std::setprecision(2) << humanTop5 << "%" << std::endl;
    std::cout << "Human 50% Cover #: " << humanCoverageCount << std::endl;
    std::cout << "Human Suspicious %: " << std::fixed << std::setprecision(2) << humanSuspiciousScore << "%" << std::endl;

    double humanFinalScore = PatternAnalyzer::calculateBotSuspicionScore(humanTop2, humanTop5, humanCoverageCount, humanSuspiciousScore);
    std::cout << "Human Final Score: " << std::fixed << std::setprecision(4) << humanFinalScore << std::endl;
    std::cout << "Human Suspected: " << (PatternAnalyzer::isBotSuspected(humanFinalScore) ? "Yes" : "No") << std::endl;

    // 봇 데이터 분석 결과 저장
    PatternAnalyzer::saveAnalysisResults(
        "bot_analysis.csv",
        sortedBotFreqs,
        botTop2,
        botTop5,
        botCoverageCount,
        botSuspiciousScore,
        botFinalScore
    );

    // 사람 데이터 분석 결과 저장
    PatternAnalyzer::saveAnalysisResults(
        "human_analysis.csv",
        sortedHumanFreqs,
        humanTop2,
        humanTop5,
        humanCoverageCount,
        humanSuspiciousScore,
        humanFinalScore
    );

    std::cout << "\nPress Enter to exit..." << std::endl;
    getchar();
    return 0;
} 