import requests
import pytest

headers = {
    "Accept": "application/json",
    "User-Agent": "my-scraper/1.0",
    "Connection": "keep-alive",
    "Referer": "https://httpbin.dev"
}

webpages = [
    ('http://127.0.0.1:8080/',  200),
    ('http://127.0.0.1:8080/images',  200)
]

@pytest.mark.parametrize("url, expected_status", webpages)

def test_webpages(url, expected_status):
    try:
        r = requests.get(url, timeout=5, headers=headers)
        status = r.status_code
    except requests.exceptions.RequestException:
        status = 420

    assert status == expected_status
