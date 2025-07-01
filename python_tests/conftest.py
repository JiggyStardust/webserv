import pytest
import subprocess
import time
import os
import signal

@pytest.fixture(scope="session", autouse=True)
def start_webserv():
    # Start the server
    process = subprocess.Popen(["./webserv"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    # Wait a moment to ensure the server is ready
    time.sleep(1)

    # Run the tests
    yield

    # Teardown: terminate the server
    process.terminate()
    try:
        process.wait(timeout=3)
    except subprocess.TimeoutExpired:
        process.kill()
