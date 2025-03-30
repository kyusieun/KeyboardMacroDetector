import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns


# 데이터 로드
def load_analysis(filename):
    # 전체 파일을 읽어서 'metrics' 행을 찾음
    with open(filename, "r") as f:
        lines = f.readlines()

    # 'metrics' 행의 인덱스 찾기
    metrics_idx = next(i for i, line in enumerate(lines) if "metrics" in line)

    # 패턴 데이터와 메트릭 데이터 분리해서 읽기
    patterns = pd.read_csv(filename, nrows=metrics_idx - 1)
    metrics = pd.read_csv(filename, skiprows=metrics_idx + 1)

    return patterns, metrics


# 봇과 사람 데이터 로드
bot_patterns, bot_metrics = load_analysis("./build/bot_analysis.csv")
human_patterns, human_metrics = load_analysis("./build/human_analysis.csv")

# 1. Top N 패턴만 시각화 (예: 상위 10개)
plt.figure(figsize=(15, 6))
plt.subplot(1, 2, 1)
sns.barplot(data=bot_patterns.head(10), x="pattern_id", y="frequency")
plt.title("Top 10 Bot Patterns")
plt.xticks(rotation=45)

plt.subplot(1, 2, 2)
sns.barplot(data=human_patterns.head(10), x="pattern_id", y="frequency")
plt.title("Top 10 Human Patterns")
plt.xticks(rotation=45)
plt.tight_layout()
plt.savefig("top10_patterns_comparison.png", dpi=300, bbox_inches="tight")
plt.close()

# 2. 누적 분포 그래프
plt.figure(figsize=(10, 6))
bot_cumsum = bot_patterns["frequency"].cumsum() / bot_patterns["frequency"].sum() * 100
human_cumsum = (
    human_patterns["frequency"].cumsum() / human_patterns["frequency"].sum() * 100
)

plt.plot(range(1, len(bot_cumsum) + 1), bot_cumsum, label="Bot")
plt.plot(range(1, len(human_cumsum) + 1), human_cumsum, label="Human")
plt.xlabel("Number of Patterns")
plt.ylabel("Cumulative Percentage (%)")
plt.title("Cumulative Pattern Distribution")
plt.legend()
plt.grid(True)
plt.savefig("cumulative_distribution.png", dpi=300, bbox_inches="tight")
plt.close()

# 3. 빈도 구간별 히스토그램
plt.figure(figsize=(12, 5))
plt.subplot(1, 2, 1)
plt.hist(bot_patterns["frequency"], bins=20, edgecolor="black")
plt.title("Bot Pattern Frequency Distribution")
plt.xlabel("Frequency")
plt.ylabel("Count")

plt.subplot(1, 2, 2)
plt.hist(human_patterns["frequency"], bins=20, edgecolor="black")
plt.title("Human Pattern Frequency Distribution")
plt.xlabel("Frequency")
plt.ylabel("Count")
plt.tight_layout()
plt.savefig("boxplot_comparison.png", dpi=300, bbox_inches="tight")
plt.close()
