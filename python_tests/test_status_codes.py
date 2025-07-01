import requests
import pytest

base_url = "http://127.0.0.1:8080"

headers = {
	#"Accept": "application/json",
	#"User-Agent": "my-scraper/1.0",
	#"Connection": "keep-alive",
	#"Referer": "https://httpbin.dev"
}

webpages = [
	('/',  200),
	('/images/',  200),
	('/submit/', 200),
	('/who.py?firstname=first&lastname=last&favcolor=color', 200),
	('/images', 301),
	('/submit', 301),
	('/puzzle/', 301)
]

@pytest.mark.parametrize("path, expected_status", webpages)
def test_webpages(path, expected_status):
	try:
		request = requests.get(base_url + path, timeout=5, headers=headers, allow_redirects=False)
		status = request.status_code
	except requests.exceptions.RequestException:
		status = None

	assert status == expected_status
