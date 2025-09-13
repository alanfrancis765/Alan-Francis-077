import os
import io
import re
import json
import hashlib
import requests
import numpy as np
from bs4 import BeautifulSoup
from urllib.parse import urljoin, urlparse
from PIL import Image, ImageOps
import imagehash
import pytesseract
import cv2
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed
import threading
from requests.adapters import HTTPAdapter, Retry

# ==========================
# Config (tune here)
# ==========================
REPORT_FILE       = "report.json"
DOWNLOAD_DIR      = "downloads"
MAX_PAGES         = 3           # max pages to crawl (same domain)
STOP_ON_FIRST     = False       # set to True to stop after first match
USE_TOR           = False       # toggle TOR usage
TOR_SOCKS         = "socks5h://127.0.0.1:9050"
MAX_WORKERS       = 10          # parallel image fetch/scan
MIN_IMG_DIM       = 220         # skip tiny logos/icons
USER_AGENT        = "SatarkScanner/1.0 (+https://example.org)"
TIMEOUT_PAGE      = 20
TIMEOUT_IMAGE     = 15

# Optional preference: prefer Aadhaar ending with specific digits
PREFER_SUFFIX     = "11"  # e.g., prefer numbers ending with '11' or set to None

# Aadhaar regex (12 digits, optional spaces/dashes)
AADHAAR_REGEX     = r"\b(\d{4}[\s\-]?\d{4}[\s\-]?\d{4})\b"

# Blacklist obvious samples/demos (uppercased match)
BLACKLIST_WORDS = ["SAMPLE", "TEST", "DUMMY", "DEMO", "PRINT", "SPECIMEN", "PVC", "CARD", "VOID", "TEMPLATE", "COPY", "FOR USE IN DOCUMENTS"]

# Add known fake number patterns (we will normalize and check against digits-only)
FAKE_PATTERNS = [
    r"\b1234\s*5678\s*9012\b",
    r"\b9876\s*5432\s*1090\b",
    r"\b(\d)\1{3}\s*\1{4}\s*\1{4}\b",  # repeated digits like 1111 1111 1111
    r"\b\d{4}\s*\d{4}\s*0000\b",
    r"\b0000\s*\d{4}\s*\d{4}\b",
]

# Context words increase confidence (uppercased scanning text)
CONTEXT_WORDS     = ["AADHAAR", "AADHAR", "UIDAI", "GOVERNMENT OF INDIA", "भारत सरकार", "आधार"]

# Precompute fake explicit numbers (digits-only) from FAKE_PATTERNS where possible
_EXPLICIT_FAKE_NUMS = set()
for p in FAKE_PATTERNS:
    digits_only = re.sub(r"\D", "", p)
    if digits_only and len(digits_only) == 12:
        _EXPLICIT_FAKE_NUMS.add(digits_only)


# ==========================
# HTTP Sessions
# ==========================
def get_tor_session():
    s = requests.Session()
    s.proxies = {"http": TOR_SOCKS, "https": TOR_SOCKS}
    _configure_session(s)
    return s

def get_session(use_tor=False):
    s = get_tor_session() if use_tor else requests.Session()
    _configure_session(s)
    return s

def _configure_session(s: requests.Session):
    s.headers.update({"User-Agent": USER_AGENT})
    retries = Retry(
        total=3,
        backoff_factor=0.3,
        status_forcelist=(429, 500, 502, 503, 504),
        allowed_methods=frozenset(["GET", "HEAD"])
    )
    adapter = HTTPAdapter(max_retries=retries, pool_connections=64, pool_maxsize=64)
    s.mount("http://", adapter)
    s.mount("https://", adapter)


# ==========================
# Reporting (dedup-on-append)
# ==========================
def _load_report():
    if os.path.exists(REPORT_FILE):
        try:
            with open(REPORT_FILE, "r", encoding="utf-8") as f:
                return json.load(f)
        except Exception:
            return []
    return []

def _save_report(data):
    with open(REPORT_FILE, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2, ensure_ascii=False)

def update_report(image_path, details, source_url=None):
    data = _load_report()
    # simple dedupe key = aadhaar + source
    key = f"{details.get('Aadhaar Number','')}|{source_url or image_path}"
    if any((item.get("details",{}).get("Aadhaar Number","") + "|" + (item.get("url") or item.get("file",""))) == key for item in data):
        return
    entry = {
        "source": "image_scan" if source_url is None else "web_scan",
        "file": image_path,
        "url": source_url,
        "risk": "CRITICAL",
        "details": details
    }
    data.append(entry)
    _save_report(data)


# ==========================
# Image helpers
# ==========================
def open_image_safely(image_bytes, max_dim=1200):
    try:
        img = Image.open(io.BytesIO(image_bytes))
        img = ImageOps.exif_transpose(img)
        if img.mode not in ("P", "RGBA"):
            img = img.convert("RGB")
        w, h = img.size
        if max(w, h) > max_dim:
            scale = max_dim / max(w, h)
            img = img.resize((int(w * scale), int(h * scale)),
                             Image.Resampling.LANCZOS if hasattr(Image, "Resampling") else Image.ANTIALIAS)
        return img
    except Exception:
        return None

def pil_to_cv(img_pil):
    return cv2.cvtColor(np.array(img_pil), cv2.COLOR_RGB2BGR)

# pHash-based dedupe (average hash)
_seen_phashes = set()
_seen_lock    = threading.Lock()

def get_phash(img_bytes):
    try:
        img = Image.open(io.BytesIO(img_bytes)).convert("L").resize((8, 8),
                Image.Resampling.LANCZOS if hasattr(Image, "Resampling") else Image.ANTIALIAS)
        return str(imagehash.average_hash(img))
    except Exception:
        return None


# ==========================
# Fast preprocessing
# ==========================
def preprocess_fast_variant(img_cv):
    gray = cv2.cvtColor(img_cv, cv2.COLOR_BGR2GRAY)
    up2  = cv2.resize(gray, None, fx=2.0, fy=2.0, interpolation=cv2.INTER_CUBIC)
    clahe = cv2.createCLAHE(clipLimit=3.0, tileGridSize=(8,8)).apply(up2)
    return clahe


# ==========================
# OCR (digit-first, fallback)
# ==========================
def ocr_text(img_gray):
    # Pass 1: digits-only (fast)
    try:
        txt_digits = pytesseract.image_to_string(
            img_gray,
            lang="eng",
            config="--oem 3 --psm 6 -c tessedit_char_whitelist=0123456789"
        )
    except Exception:
        txt_digits = ""

    # If candidate pattern present, return immediately (fast path)
    if re.search(r"\d{4}\D?\d{4}\D?\d{4}", txt_digits):
        return txt_digits

    # Preprocess harder for fallback
    try:
        _, th = cv2.threshold(img_gray, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
    except Exception:
        th = img_gray

    # Pass 2: fallback with more languages (English + Hindi)
    try:
        txt_full = pytesseract.image_to_string(
            th,
            lang="eng+hin",
            config="--oem 3 --psm 6"
        )
    except Exception:
        txt_full = ""

    return txt_digits + "\n" + txt_full


# ==========================
# Verhoeff (Aadhaar) - CORRECT
# ==========================
_ver_d = [
    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
    [1, 2, 3, 4, 0, 6, 7, 8, 9, 5],
    [2, 3, 4, 0, 1, 7, 8, 9, 5, 6],
    [3, 4, 0, 1, 2, 8, 9, 5, 6, 7],
    [4, 0, 1, 2, 3, 9, 5, 6, 7, 8],
    [5, 9, 8, 7, 6, 0, 4, 3, 2, 1],
    [6, 5, 9, 8, 7, 1, 0, 4, 3, 2],
    [7, 6, 5, 9, 8, 2, 1, 0, 4, 3],
    [8, 7, 6, 5, 9, 3, 2, 1, 0, 4],
    [9, 8, 7, 6, 5, 4, 3, 2, 1, 0]
]

_ver_p = [
    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
    [1, 5, 7, 6, 2, 8, 3, 0, 9, 4],
    [5, 8, 0, 3, 7, 9, 6, 1, 4, 2],
    [8, 9, 1, 6, 0, 4, 3, 5, 2, 7],
    [9, 4, 5, 3, 1, 2, 6, 8, 7, 0],
    [4, 2, 8, 6, 5, 7, 3, 9, 0, 1],
    [2, 7, 9, 3, 8, 0, 5, 4, 1, 6],
    [7, 0, 4, 6, 9, 1, 3, 2, 5, 8]
]

def verhoeff_validate(num_str):
    """Validate Aadhaar number using Verhoeff algorithm."""
    num_str = re.sub(r"\D", "", num_str)  # Remove non-digits
    if not re.fullmatch(r"\d{12}", num_str):
        return False
    c = 0
    for i, d in enumerate(map(int, reversed(num_str))):
        c = _ver_d[c][_ver_p[i % 8][d]]
    return c == 0


# ==========================
# Utilities to detect fake numbers
# ==========================
def is_obvious_fake(digits12: str) -> bool:
    """Return True if digits-only 12-digit string is obviously fake."""
    if not digits12 or len(digits12) != 12 or not digits12.isdigit():
        return True
    # explicit known fake numbers
    if digits12 in _EXPLICIT_FAKE_NUMS:
        return True
    # all same digit: 111111111111 etc
    if len(set(digits12)) == 1:
        return True
    # block of zeros anywhere (e.g., last block 0000)
    if digits12.endswith("0000") or digits12.startswith("0000") or digits12[4:8] == "0000":
        return True
    # other ad-hoc checks (like 1234 1234 1234)
    if digits12 in {"123412341234", "000000000000"}:
        return True
    return False


# ==========================
# Parse Aadhaar (context-scored; returns best single match)
# ==========================
def parse_aadhaar_from_text(text):
    if not text or not text.strip():
        return None

    upper = text.upper()

    # Skip obvious demo/fake
    if any(w in upper for w in BLACKLIST_WORDS):
        return None

    candidates = []
    for m in re.finditer(AADHAAR_REGEX, upper):
        raw = m.group(1)
        digits = re.sub(r"[\s\-]+", "", raw)
        if len(digits) == 12 and verhoeff_validate(digits):
            # skip obvious fake numbers
            if is_obvious_fake(digits):
                continue

            # Score by nearby context words
            start = max(0, m.start() - 120)
            end   = min(len(upper), m.end() + 120)
            window = upper[start:end]
            ctx = sum(1 for w in CONTEXT_WORDS if w in window)
            score = ctx

            # Optional preference (e.g., ends with PREFER_SUFFIX)
            if PREFER_SUFFIX and digits.endswith(PREFER_SUFFIX):
                score += 2

            formatted = f"{digits[0:4]} {digits[4:8]} {digits[8:12]}"
            candidates.append((score, formatted))

    if not candidates:
        return None

    # Pick the highest-scoring candidate, tie-break by earliest formatted string
    candidates.sort(key=lambda x: (-x[0], x[1]))
    best = candidates[0][1]
    return {"Type": "Aadhaar", "Aadhaar Number": best}


# ==========================
# Scan image bytes (fast path)
# ==========================
def scan_aadhaar_image_bytes(image_bytes):
    # pHash dedupe (cheap)
    ph = get_phash(image_bytes)
    if ph is None:
        return None
    with _seen_lock:
        if ph in _seen_phashes:
            return None
        _seen_phashes.add(ph)

    # Open & size check
    img_pil = open_image_safely(image_bytes)
    if img_pil is None:
        return None
    if img_pil.width < MIN_IMG_DIM or img_pil.height < MIN_IMG_DIM:
        return None

    img_cv  = pil_to_cv(img_pil)
    proc    = preprocess_fast_variant(img_cv)

    # Quick regex pre-check after OCR
    text = ocr_text(proc)
    if not text or not re.search(r"\d{4}\D?\d{4}\D?\d{4}", text):
        return None

    return parse_aadhaar_from_text(text)


# ==========================
# Crawler (parallel)
# ==========================
def simple_crawler(start_url, download_dir=DOWNLOAD_DIR,
                   max_pages=MAX_PAGES, use_tor=USE_TOR,
                   stop_on_first=STOP_ON_FIRST, max_workers=MAX_WORKERS):
    session = get_session(use_tor)
    visited = set()
    to_visit = [start_url]
    os.makedirs(download_dir, exist_ok=True)
    matches_found = 0

    def fetch_and_scan(img_url, page_url):
        try:
            r = session.get(img_url, timeout=TIMEOUT_IMAGE)
            r.raise_for_status()
            img_bytes = r.content
        except Exception:
            return None

        result = scan_aadhaar_image_bytes(img_bytes)
        if result:
            # Save the matched image (best-effort)
            h = hashlib.md5(img_bytes).hexdigest()
            name = os.path.basename(img_url.split("?")[0]) or f"{h}.jpg"
            if not os.path.splitext(name)[1]:
                name += ".jpg"
            path = os.path.join(download_dir, name)
            try:
                with open(path, "wb") as f:
                    f.write(img_bytes)
            except Exception:
                pass

            # Output
            print("\n[✅ MATCH FOUND] Aadhaar details detected:")
            print(f"   → Type           : {result['Type']}")
            print(f"   → Aadhaar Number : {result['Aadhaar Number']}")
            print(f"   → Source         : {page_url}")

            update_report(path, result, source_url=page_url)
            return True
        return None

    pages_crawled = 0
    while to_visit and pages_crawled < max_pages:
        url = to_visit.pop(0)
        if url in visited:
            continue

        print(f"[+] Visiting: {url}")
        visited.add(url)
        pages_crawled += 1

        try:
            resp = session.get(url, timeout=TIMEOUT_PAGE)
            resp.raise_for_status()
        except Exception as e:
            print(f"[!] Failed to fetch: {url} ({e})")
            continue

        soup = BeautifulSoup(resp.text, "html.parser")
        domain = urlparse(start_url).netloc

        # Collect candidate images
        img_urls = []
        for tag in soup.find_all("img", src=True):
            img_url = urljoin(url, tag["src"])
            if not img_url.lower().endswith((".jpg", ".jpeg", ".png", ".webp")):
                continue
            img_urls.append((img_url, url))

        # Parallel scan images on page
        if img_urls:
            with ThreadPoolExecutor(max_workers=max_workers) as ex:
                futures = [ex.submit(fetch_and_scan, u, p) for (u, p) in img_urls]
                for fut in as_completed(futures):
                    try:
                        res = fut.result()
                        if res:
                            matches_found += 1
                            if stop_on_first:
                                print("\n[ℹ] Crawl stopped after first valid match.")
                                return matches_found
                    except Exception:
                        continue

        # Follow internal links
        for a in soup.find_all("a", href=True):
            nxt = urljoin(url, a["href"])
            if urlparse(nxt).netloc == domain and nxt not in visited:
                to_visit.append(nxt)

    print("\n[ℹ] Crawl finished. All Aadhaar matches have been reported.")
    return matches_found

# ==========================
# Search1API integration
#==========================
SEARCH1API_KEY = "76DCD8A5-A022-4DDC-B68D-3DF0F5F931C1"  
SEARCH1API_URL = "https://api.search1api.com/search"
SEARCH_QUERIES = [
    # "aadhaar card leak",
    # "aadhaar database exposed",
    # "aadhaar data breach",
    # "aadhaar details dump",
    "aadhaar card photo leak",
    "aadhar-card-photo-real"
    # "uidai data breach",
    # "aadhaar numbers leak",
    # "indian id data leak"
]

#----------------------------
# Fetch onion sites from Ahmia
#----------------------------

def get_onion_sites(query="", limit=10):
    url = f"https://ahmia.fi/search/?q={query}"
    headers = {"User-Agent": "Mozilla/5.0"}
    resp = requests.get(url, headers=headers, timeout=20)
    resp.raise_for_status()

    soup = BeautifulSoup(resp.text, "html.parser")
    onion_links = [a["href"] for a in soup.find_all("a", href=True) if ".onion" in a["href"]]
    return list(set(onion_links))[:limit]  


#----------------------------
# Fetch URLs from Search1API
#----------------------------

MAX_RESULTS = 5
def fetch_search1api_urls(query, max_results=MAX_RESULTS):
    try:
        response = requests.post(
            SEARCH1API_URL,
            headers={
                'Content-Type': 'application/json',
                'Authorization': f'Bearer {SEARCH1API_KEY}'
            },
            json={
                "query": query,
                "search_service": "google",
                "max_results": max_results,
                "crawl_results": 0,
                "image": False,
                "include_sites": [],
                "exclude_sites": [],
                "language": "auto"
            },
            timeout=20
        )
        response.raise_for_status()
        data = response.json()
        urls = [item.get("link") for item in data.get("results", []) if item.get("link")]
        return urls
    except Exception as e:
        print(f"[!] Error fetching URLs for query '{query}': {e}")
        return []


# ==========================
# Local image scan
# ==========================
def scan_local_image(image_path):
    if not os.path.exists(image_path):
        print(f"[!] File not found: {image_path}")
        return 0

    with open(image_path, "rb") as f:
        img_bytes = f.read()

    res = scan_aadhaar_image_bytes(img_bytes)
    if res:
        digits = res["Aadhaar Number"].replace(" ", "")
        # Double-check obvious fake patterns
        if is_obvious_fake(digits):
            print(f"[-] Fake Aadhaar pattern detected in: {image_path}")
            return 0

        print("\n[+] Aadhaar details detected:")
        print(f"   → Type           : {res['Type']}")
        print(f"   → Aadhaar Number : {res['Aadhaar Number']}")
        print(f"   → Source         : {image_path}")
        update_report(image_path, res, source_url=None)
        return 1
    else:
        print(f"[-] No Aadhaar found in: {image_path}")
        return 0

if __name__ == "__main__":
    print("Scanning... \n")

    total_matches = 0
    all_urls_to_scan = set()  # dedupe URLs across queries

    # Fetch URLs from multiple search queries
    for q in SEARCH_QUERIES:
        urls_list = get_onion_sites(q, 10)
        print(f"[+] Query '{q}' returned {len(urls_list)} URLs")
        all_urls_to_scan.update(urls_list)


    for q in SEARCH_QUERIES:
        urls_list = fetch_search1api_urls(q, max_results=MAX_RESULTS)
        print(f"[+] Query '{q}' returned {len(urls_list)} URLs")
        all_urls_to_scan.update(urls_list)

    print(f"\n[+] Total unique URLs to crawl in : {len(all_urls_to_scan)}\n")

    # Crawl each URL
    for target_url in all_urls_to_scan:
        print(f"[+] Starting crawl: {target_url}")
        matches = simple_crawler(
            target_url,
            max_pages=MAX_PAGES,
            use_tor=USE_TOR,
            stop_on_first=STOP_ON_FIRST,
            max_workers=MAX_WORKERS
        )
        total_matches += matches or 0

    # Scan local image
    local_image = r"C:\Users\alanf\Downloads\aadhaar11.jpg"
    print(f"\n[F] Scanning local file: {local_image}")
    total_matches += scan_local_image(local_image)

    # Only print "no match" if truly zero
    if total_matches == 0:
        print("\n[-] No valid Aadhaar found in scanned sources.")
