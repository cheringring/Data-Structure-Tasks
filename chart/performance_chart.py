import matplotlib.pyplot as plt
import matplotlib
matplotlib.rc('font', family='Arial Unicode MS')  # macOS 기본 유니코드 폰트
plt.rcParams['axes.unicode_minus'] = False

url_counts = [10, 20, 50, 100]
exec_times = [1.2, 2.5, 5.8, 10.3]
memories = [5, 8, 15, 25]
urls_per_sec = [8.3, 8.0, 8.6, 9.7]

plt.figure(figsize=(8,5))

# 실행 시간 그래프
plt.plot(url_counts, exec_times, marker='o', label='실행 시간(초)')
# 메모리 사용량 그래프
plt.plot(url_counts, memories, marker='s', label='메모리 사용량(MB)')
# 처리된 URL/초 그래프
plt.plot(url_counts, urls_per_sec, marker='^', label='처리된 URL/초')

plt.title('웹 크롤러 성능 분석')
plt.xlabel('URL 수')
plt.ylabel('측정값')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig('performance.png')
plt.show()