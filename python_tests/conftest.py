import pytest
import subprocess
import time
import os
import signal
import sys  # Needed if using sys.stdout/sys.stderr

@pytest.fixture(scope="session", autouse=True)
def start_webserv():
    # Start the server with visible output
    process = subprocess.Popen(
        ["./webserv"],
        stdout=sys.stdout,
        stderr=sys.stderr
    )

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
