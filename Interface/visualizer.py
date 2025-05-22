from flask import Flask, render_template_string, request, send_file, jsonify
import subprocess
import os
import threading
import time
import traceback

app = Flask(__name__)

# 크롤링 상태 저장
crawl_status = {
    "running": False,
    "log": [],
    "completed": False
}

# HTML 템플릿
HTML = """
<!DOCTYPE html>
<html>
<head>
    <title>웹 크롤러 시각화</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f5f5f5; }
        .container { max-width: 1000px; margin: 0 auto; background: white; padding: 20px; border-radius: 5px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
        .recommendation-box { background-color: #e8f4fc; border: 1px solid #cce5ff; border-radius: 5px; padding: 10px; margin-bottom: 15px; }
        .recommendation-box h3 { margin-top: 0; color: #0066cc; }
        .recommendation-item { margin-bottom: 8px; }
        .recommendation-url { font-family: monospace; background-color: #f0f0f0; padding: 2px 5px; border-radius: 3px; }
        h1 { color: #333; }
        .form-group { margin-bottom: 15px; }
        label { display: block; margin-bottom: 5px; font-weight: bold; }
        input[type="text"], input[type="number"] { width: 100%; padding: 8px; border: 1px solid #ddd; border-radius: 4px; }
        input[type="number"] { width: 100px; }
        button { padding: 10px 15px; background: #4CAF50; color: white; border: none; border-radius: 4px; cursor: pointer; }
        button:hover { background: #45a049; }
        button:disabled { background: #cccccc; cursor: not-allowed; }
        #log { height: 200px; overflow-y: auto; border: 1px solid #ddd; padding: 10px; margin-top: 20px; background: #f9f9f9; }
        .status { margin-top: 15px; font-weight: bold; }
        .graph-container { margin-top: 20px; text-align: center; }
        .graph-image { max-width: 100%; border: 1px solid #ddd; }
    </style>
</head>
<body>
    <div class="container">
        <h1>웹 크롤러 시각화 도구</h1>
        
        <div class="recommendation-box">
            <h3>추천 설정</h3>
            <div class="recommendation-item"><strong>해커 뉴스:</strong> <span class="recommendation-url">https://news.ycombinator.com</span> - URL 20개, 스레드 4개</div>
            <div class="recommendation-item"><strong>위키피디아:</strong> <span class="recommendation-url">https://en.wikipedia.org/wiki/Main_Page</span> - URL 25개, 스레드 4개</div>
            <div class="recommendation-item"><strong>GitHub:</strong> <span class="recommendation-url">https://github.com/trending</span> - URL 30개, 스레드 4개</div>
            <p><small>지나치게 많은 URL을 설정하면 그래프가 복잡해질 수 있습니다.</small></p>
        </div>
        
        <div class="form-group">
            <label for="url">크롤링할 URL:</label>
            <input type="text" id="url" class="form-control" placeholder="https://example.com">
        </div>
        
        <div class="form-group">
            <label for="maxUrls">최대 URL 수:</label>
            <input type="number" id="maxUrls" class="form-control" value="20" min="1" max="100">
        </div>
        
        <div class="form-group">
            <label for="threads">스레드 수:</label>
            <input type="number" id="threads" class="form-control" value="4" min="1" max="8">
        </div>
        
        <button id="startBtn" class="btn btn-success" onclick="startCrawl()">크롤링 시작</button>
        <button id="visualizeBtn" class="btn btn-primary" onclick="visualizeGraph()" style="margin-left: 10px;">결과 시각화</button>
        <div id="currentUrl" style="margin-top: 10px; font-style: italic;"></div>
        
        <div id="log"></div>
        
        <div class="graph-container" id="graphContainer" style="display: none; margin-top: 20px; text-align: center;">
            <h2 id="graphTitle">웹 그래프 시각화</h2>
            <button id="refreshBtn" class="btn btn-secondary" onclick="forceClearCache()" style="margin-bottom: 10px;">캐시 초기화 및 새로고침</button>
            <div id="imageContainer"></div>
        </div>
    </div>
    
    <script>
        let logUpdateInterval;
        
        function startCrawl() {
            const url = document.getElementById('url').value;
            const maxUrls = document.getElementById('maxUrls').value;
            const threads = document.getElementById('threads').value;
            
            // URL이 비어있으면 기본값 설정
            if (!url) {
                alert('URL을 입력해주세요.');
                return;
            }
            
            // 현재 URL 표시
            document.getElementById('currentUrl').textContent = `크롤링 URL: ${url}`;
            
            document.getElementById('startBtn').disabled = true;
            document.getElementById('visualizeBtn').disabled = true;
            document.getElementById('startBtn').textContent = '크롤링 중...';
            
            fetch('/crawl', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    url: url,
                    max_urls: maxUrls,
                    threads: threads
                }),
            })
            .then(response => response.json())
            .then(data => {
                if (data.status === 'started') {
                    // 크롤링 시작 후 상태 확인 시작
                    checkStatus();
                } else {
                    alert('크롤링 시작 중 오류가 발생했습니다: ' + data.error);
                    document.getElementById('startBtn').disabled = false;
                    document.getElementById('startBtn').textContent = '크롤링 시작';
                }
            })
            .catch(error => {
                console.error('Error:', error);
                alert('크롤링 시작 중 오류가 발생했습니다.');
                document.getElementById('startBtn').disabled = false;
                document.getElementById('startBtn').textContent = '크롤링 시작';
            });
        }
        
        function checkStatus() {
            fetch('/status')
                .then(response => response.json())
                .then(data => {
                    // 로그 표시
                    const logDiv = document.getElementById('log');
                    logDiv.innerHTML = '';
                    
                    if (data.log && data.log.length > 0) {
                        const logContainer = document.createElement('div');
                        logContainer.className = 'log-container';
                        
                        data.log.forEach(line => {
                            const logLine = document.createElement('div');
                            logLine.className = 'log-line';
                            logLine.textContent = line;
                            logContainer.appendChild(logLine);
                        });
                        
                        logDiv.appendChild(logContainer);
                    }
                    
                    // 크롤링 완료 확인
                    if (data.completed) {
                        document.getElementById('startBtn').disabled = false;
                        document.getElementById('visualizeBtn').disabled = false;
                        document.getElementById('startBtn').textContent = '크롤링 시작';
                    } else if (data.running) {
                        // 지속 확인
                        setTimeout(checkStatus, 1000);
                    }
                });
        }
        
        function forceClearCache() {
            // 브라우저 캐시 강제 초기화
            if (window.caches) {
                caches.keys().then(function(names) {
                    for (let name of names) caches.delete(name);
                });
            }
            
            // 서버에 캐시 초기화 요청
            fetch('/graph?clearcache=' + new Date().getTime(), {
                headers: {
                    'Pragma': 'no-cache',
                    'Cache-Control': 'no-cache, no-store, must-revalidate',
                    'Expires': '0'
                },
                cache: 'no-store'
            }).then(() => {
                // 페이지 새로고침 (강제 새로고침)
                window.location.reload(true);
            });
        }
        
        function visualizeGraph() {
            // 시각화 중 표시
            document.getElementById('visualizeBtn').disabled = true;
            document.getElementById('visualizeBtn').textContent = '시각화 중...';
            
            fetch('/visualize', {
                headers: {
                    'Cache-Control': 'no-cache, no-store, must-revalidate',
                    'Pragma': 'no-cache',
                    'Expires': '0'
                },
                cache: 'no-store'
            })
                .then(response => response.json())
                .then(data => {
                    document.getElementById('visualizeBtn').disabled = false;
                    document.getElementById('visualizeBtn').textContent = '결과 시각화';
                    
                    if (data.status === 'success') {
                        document.getElementById('graphContainer').style.display = 'block';
                        
                        // 크롤링 URL 정보 표시
                        let graphTitle = document.getElementById('graphTitle');
                        if (data.url) {
                            graphTitle.textContent = `웹 그래프 시각화: ${data.url}`;
                        } else {
                            graphTitle.textContent = '웹 그래프 시각화';
                        }
                        
                        // 이미지 요소 삭제 후 새로 생성
                        const container = document.getElementById('imageContainer');
                        container.innerHTML = '';
                        
                        // 새 이미지 요소 생성
                        const newImg = document.createElement('img');
                        newImg.id = 'graphImage';
                        newImg.className = 'graph-image';
                        newImg.style.maxWidth = '100%';
                        newImg.style.border = '1px solid #ddd';
                        newImg.style.marginTop = '10px';
                        
                        // 캐시 방지를 위한 고유 쿼리 파라미터 추가
                        const uniqueParam = data.timestamp || new Date().getTime();
                        newImg.src = '/graph?nocache=' + uniqueParam;
                        
                        // 이미지 로드 완료 후 표시
                        newImg.onload = function() {
                            console.log('이미지 로드 완료:', data.filesize, 'bytes');
                        };
                        
                        // 이미지 로드 오류 처리
                        newImg.onerror = function() {
                            console.error('이미지 로드 오류');
                            alert('그래프 이미지 로드 중 오류가 발생했습니다.');
                        };
                        
                        container.appendChild(newImg);
                    } else {
                        alert('그래프 시각화 중 오류가 발생했습니다: ' + (data.error || '알 수 없는 오류'));
                    }
                })
                .catch(error => {
                    document.getElementById('visualizeBtn').disabled = false;
                    document.getElementById('visualizeBtn').textContent = '결과 시각화';
                    console.error('Error:', error);
                    alert('그래프 시각화 중 오류가 발생했습니다.');
                });
        }
    </script>
</body>
</html>
"""

@app.route('/')
def index():
    return render_template_string(HTML)

@app.route('/crawl', methods=['POST'])
def crawl():
    global crawl_status
    
    if crawl_status["running"]:
        return {"error": "Already running"}, 400
    
    data = request.json
    url = data.get('url', 'https://news.ycombinator.com')
    max_urls = data.get('max_urls', 20)
    threads = data.get('threads', 4)
    
    # 크롤링 상태 초기화
    crawl_status = {
        "running": True,
        "log": [],
        "completed": False
    }
    
    # 별도 스레드에서 크롤링 실행
    threading.Thread(target=run_crawler, args=(url, max_urls, threads)).start()
    
    return {"status": "started"}

def run_crawler(url, max_urls, threads):
    global crawl_status
    
    try:
        # WebCrawler 디렉토리 경로
        webcrawler_dir = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "WebCrawler")
        
        # 웹크롤러 실행 경로
        crawler_path = os.path.join(webcrawler_dir, "webcrawler")
        
        # DOT 파일 경로 - WebCrawler 디렉토리에 생성됨
        dot_file_path = os.path.join(webcrawler_dir, "web_graph.dot")
        
        # 현재 작업 디렉토리 확인
        crawl_status["log"].append(f"[시스템] 현재 디렉토리: {os.getcwd()}")
        crawl_status["log"].append(f"[시스템] WebCrawler 디렉토리: {webcrawler_dir}")
        crawl_status["log"].append(f"[시스템] DOT 파일 경로: {dot_file_path}")
        
        # 기존 DOT 파일 삭제
        if os.path.exists(dot_file_path):
            try:
                os.remove(dot_file_path)
                crawl_status["log"].append(f"[시스템] 기존 DOT 파일 삭제: {dot_file_path}")
            except Exception as e:
                crawl_status["log"].append(f"[시스템] DOT 파일 삭제 실패: {str(e)}")
        
        # 웹크롤러 실행 명령
        cmd = f"{crawler_path} {url} {max_urls} {threads}"
        crawl_status["log"].append(f"[시스템] 크롤링 시작: URL={url}, 최대 URL 수={max_urls}, 스레드 수={threads}")
        
        # 프로세스 실행 및 출력 캡처 - 인코딩 오류 처리
        process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        
        # 실시간으로 출력 캡처 - 안전한 인코딩 처리
        for line_bytes in process.stdout:
            try:
                # UTF-8로 디코딩 시도
                line = line_bytes.decode('utf-8').strip()
            except UnicodeDecodeError:
                try:
                    # UTF-8 실패 시 latin-1로 시도 (모든 바이트 처리 가능)
                    line = line_bytes.decode('latin-1').strip()
                except Exception:
                    # 디코딩 실패 시 안전한 문자로 대체
                    line = "[인코딩 오류: 읽을 수 없는 텍스트]"
            
            crawl_status["log"].append(line)
            if len(crawl_status["log"]) > 100:  # 로그 크기 제한
                crawl_status["log"] = crawl_status["log"][-100:]
        
        process.wait()
        crawl_status["log"].append(f"[시스템] 크롤링 완료: 종료 코드={process.returncode}")
        
        # DOT 파일 생성 확인
        # 웹크롤러 디렉토리에서 파일 찾기
        dot_files = [f for f in os.listdir(webcrawler_dir) if f.endswith(".dot")]
        crawl_status["log"].append(f"[시스템] WebCrawler 디렉토리의 DOT 파일: {dot_files}")
        
        # 전체 파일 시스템에서 DOT 파일 찾기 (절대 경로 사용으로 인해 다른 위치에 생성될 수 있음)
        import glob
        all_dot_files = glob.glob(os.path.join(webcrawler_dir, "*.dot")) + \
                       glob.glob(os.path.join(os.getcwd(), "*.dot"))
        crawl_status["log"].append(f"[시스템] 찾은 모든 DOT 파일: {all_dot_files}")
        
        # 현재 작업 디렉토리에서 DOT 파일 찾기
        cwd_dot_files = [f for f in os.listdir(os.getcwd()) if f.endswith(".dot")]
        crawl_status["log"].append(f"[시스템] 현재 디렉토리의 DOT 파일: {cwd_dot_files}")
        
        # 찾은 DOT 파일 중 가장 최근 파일 사용
        found_dot_file = None
        
        # 1. 웹크롤러 디렉토리에서 찾기
        if dot_files:
            # 최신 파일 선택 (수정 시간 기준)
            latest_dot_file = max([os.path.join(webcrawler_dir, f) for f in dot_files], 
                                 key=lambda x: os.path.getmtime(x))
            found_dot_file = latest_dot_file
            crawl_status["log"].append(f"[시스템] WebCrawler 디렉토리에서 최신 DOT 파일 찾음: {found_dot_file}")
        
        # 2. 현재 디렉토리에서 찾기
        elif cwd_dot_files:
            latest_dot_file = max([os.path.join(os.getcwd(), f) for f in cwd_dot_files], 
                                 key=lambda x: os.path.getmtime(x))
            found_dot_file = latest_dot_file
            crawl_status["log"].append(f"[시스템] 현재 디렉토리에서 최신 DOT 파일 찾음: {found_dot_file}")
        
        # 3. 전체 경로에서 찾기
        elif all_dot_files:
            latest_dot_file = max(all_dot_files, key=lambda x: os.path.getmtime(x))
            found_dot_file = latest_dot_file
            crawl_status["log"].append(f"[시스템] 전체 경로에서 최신 DOT 파일 찾음: {found_dot_file}")
        
        # 찾은 DOT 파일 처리
        if found_dot_file and os.path.exists(found_dot_file):
            file_size = os.path.getsize(found_dot_file)
            crawl_status["log"].append(f"[시스템] DOT 파일 생성 확인: {found_dot_file} ({"{:,}".format(file_size)} 바이트)")
            
            # DOT 파일 내용 확인
            try:
                with open(found_dot_file, 'r', encoding='utf-8') as f:
                    dot_content = f.read(100)
                    crawl_status["log"].append(f"[시스템] DOT 파일 내용 미리보기: {dot_content}...")
                
                # DOT 파일을 인터페이스 디렉토리로 복사 (필요한 경우에만)
                interface_dot_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "web_graph.dot")
                
                # 같은 파일이 아닌 경우에만 복사
                if os.path.abspath(found_dot_file) != os.path.abspath(interface_dot_path):
                    import shutil
                    shutil.copy2(found_dot_file, interface_dot_path)
                    crawl_status["log"].append(f"[정보] 그래프 파일 준비 완료: {os.path.basename(interface_dot_path)}")
                else:
                    crawl_status["log"].append(f"[정보] 그래프 파일 사용 준비 완료")
            except Exception as e:
                # 사용자 친화적인 오류 메시지
                crawl_status["log"].append(f"[정보] 그래프 파일 처리 중 문제가 발생했지만, 시각화는 계속 진행됩니다.")
                # 디버깅용 로그는 콘솔에만 출력
                print(f"DEBUG - DOT 파일 처리 오류: {str(e)}")
                print(f"DEBUG - 오류 세부 정보:\n{traceback.format_exc()}")
        else:
            crawl_status["log"].append(f"[정보] 그래프 파일이 없어 새로 생성합니다.")
            
            # 수동으로 DOT 파일 생성 시도
            try:
                # 빈 그래프 생성
                interface_dot_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "web_graph.dot")
                with open(interface_dot_path, 'w') as f:
                    f.write("digraph WebGraph {\n")
                    f.write("  rankdir=LR;\n")
                    f.write("  node [shape=box, style=filled, fillcolor=lightblue];\n")
                    f.write(f"  node0 [label=\"{url}\"];\n")
                    f.write("  node0 -> node0 [label=\"No graph data\"];\n")
                    f.write("}\n")
                crawl_status["log"].append(f"[정보] 기본 그래프 생성 완료")
            except Exception as e:
                crawl_status["log"].append(f"[정보] 그래프 생성 중 문제가 발생했지만, 시각화는 계속 진행됩니다.")
                # 디버깅용 로그는 콘솔에만 출력
                print(f"DEBUG - 그래프 생성 오류: {str(e)}")
                print(f"DEBUG - 오류 세부 정보:\n{traceback.format_exc()}")
        
    except Exception as e:
        crawl_status["log"].append(f"[정보] 크롤링 중 문제가 발생했지만, 계속 진행합니다.")
        # 디버깅용 로그는 콘솔에만 출력
        print(f"DEBUG - 크롤링 오류: {str(e)}")
        print(f"DEBUG - 오류 세부 정보:\n{traceback.format_exc()}")
    finally:
        crawl_status["running"] = False
        crawl_status["completed"] = True
        crawl_status["url"] = url  # 크롤링한 URL 저장

@app.route('/status')
def status():
    return crawl_status

@app.route('/visualize')
def visualize():
    try:
        # 인터페이스 디렉토리의 DOT 파일 경로 (웹크롤러에서 복사한 파일)
        interface_dot_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), "web_graph.dot")
        
        # WebCrawler 디렉토리의 DOT 파일 경로 (원본 파일)
        webcrawler_dot_file = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "WebCrawler", "web_graph.dot")
        
        # 두 경로 모두 확인
        print(f"Interface DOT file: {interface_dot_file}, exists: {os.path.exists(interface_dot_file)}")
        print(f"WebCrawler DOT file: {webcrawler_dot_file}, exists: {os.path.exists(webcrawler_dot_file)}")
        
        # 인터페이스 디렉토리에 파일이 없는 경우, WebCrawler 디렉토리에서 복사 시도
        if not os.path.exists(interface_dot_file) and os.path.exists(webcrawler_dot_file):
            import shutil
            shutil.copy2(webcrawler_dot_file, interface_dot_file)
            print(f"Copied DOT file from WebCrawler to Interface directory")
        
        # 사용할 DOT 파일 결정
        if os.path.exists(interface_dot_file):
            dot_file = interface_dot_file
        elif os.path.exists(webcrawler_dot_file):
            dot_file = webcrawler_dot_file
        else:
            return jsonify({"error": "DOT 파일이 존재하지 않습니다. 먼저 크롤링을 실행해주세요."}), 404
            
        # DOT 파일 내용 확인
        with open(dot_file, 'r', encoding='utf-8') as f:
            dot_content = f.read(100)  # 처음 100자만 확인
            print(f"DOT file content preview: {dot_content}")
            if not dot_content.strip().startswith("digraph"):
                return jsonify({"error": "DOT 파일 형식이 잘못되었습니다."}), 400
        
        # 타임스탬프와 랜덤 값을 추가하여 캐시 문제 해결
        timestamp = int(time.time())
        import random
        random_suffix = ''.join([str(random.randint(0, 9)) for _ in range(6)])
        unique_id = f"{timestamp}_{random_suffix}"
        png_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), f"web_graph_{unique_id}.png")
        
        # 기존 PNG 파일 삭제 - 최대 10개만 유지
        png_files = [f for f in os.listdir(os.path.dirname(os.path.abspath(__file__))) 
                    if f.startswith("web_graph_") and f.endswith(".png")]
        
        # 파일 생성 시간순으로 정렬
        png_files.sort(key=lambda f: os.path.getmtime(
            os.path.join(os.path.dirname(os.path.abspath(__file__)), f)), reverse=True)
        
        # 최신 10개를 제외한 나머지 삭제
        for old_file in png_files[10:]:
            try:
                os.remove(os.path.join(os.path.dirname(os.path.abspath(__file__)), old_file))
                print(f"Removed old file: {old_file}")
            except Exception as e:
                print(f"Failed to remove old file {old_file}: {e}")
        
        # DOT 파일을 PNG로 변환 - 디버그 정보 추가
        result = subprocess.run(["dot", "-Tpng", dot_file, "-o", png_file], capture_output=True, text=True)
        
        if result.returncode != 0:
            error_msg = result.stderr or "Unknown error"
            return jsonify({"error": f"Graphviz 오류: {error_msg}"}), 500
            
        # 파일 생성 확인
        if not os.path.exists(png_file):
            return jsonify({"error": "PNG 파일 생성 실패"}), 500
            
        # 파일 크기 확인
        file_size = os.path.getsize(png_file)
        if file_size == 0:
            return jsonify({"error": "PNG 파일이 비어 있습니다."}), 500
            
        # 현재 파일 이름 저장
        app.config['CURRENT_PNG'] = os.path.basename(png_file)
        
        # 크롤링한 URL 정보 추가
        url_info = ""
        if "url" in crawl_status:
            url_info = crawl_status["url"]
            
        return jsonify({
            "status": "success", 
            "image": os.path.basename(png_file), 
            "timestamp": timestamp,
            "filesize": file_size,
            "url": url_info
        })
    except Exception as e:
        import traceback
        traceback_str = traceback.format_exc()
        return jsonify({"error": str(e), "traceback": traceback_str}), 500

@app.route('/graph')
def get_graph():
    # 타임스탬프가 포함된 현재 PNG 파일 사용
    if 'CURRENT_PNG' in app.config:
        png_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), app.config['CURRENT_PNG'])
    else:
        # 폴백: 가장 최근 PNG 파일 찾기
        png_files = [f for f in os.listdir(os.path.dirname(os.path.abspath(__file__))) 
                    if f.startswith("web_graph_") and f.endswith(".png")]
        if png_files:
            # 가장 최근 파일 사용
            png_files.sort(reverse=True)
            png_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), png_files[0])
        else:
            # 기본 파일명 사용
            png_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), f"web_graph_{int(time.time())}.png")
            # 빈 이미지 생성
            subprocess.run(["dot", "-Tpng", "-Gsize=1,1", "-Grankdir=LR", "-o", png_file], input="digraph { a [label=\"No graph available\"] }", text=True)
    
    # 캐시 방지 헤더 추가
    response = send_file(png_file, mimetype='image/png')
    response.headers["Cache-Control"] = "no-store, no-cache, must-revalidate, max-age=0"
    response.headers["Pragma"] = "no-cache"
    response.headers["Expires"] = "0"
    return response

if __name__ == '__main__':
    app.run(debug=True)