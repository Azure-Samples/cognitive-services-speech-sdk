import subprocess
import time
import threading


class RunCommand(object):
    def __init__(self):
        pass

    def run_with_output(self, command, directory = ".."):
        command = command
        output = subprocess.run(command, capture_output=True, shell=True, cwd=directory, timeout=300)
        return output

    def capture_output(event, pipe, output):
        for line in iter(pipe.readline, b''):
            output.append(line)
            if event.is_set():
                break

    def popen_with_output(self, command, directory, timeout_seconds=10):
        try:
            process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, shell=True, cwd=directory)

            # Lists to store the captured output from threads
            stdout_output = []
            stderr_output = []

            # Create threads to capture stdout and stderr
            event = threading.Event()
            stdout_thread = threading.Thread(target=RunCommand.capture_output, args=(event, process.stdout, stdout_output))
            stderr_thread = threading.Thread(target=RunCommand.capture_output, args=(event, process.stderr, stderr_output))

            # Start the threads
            stdout_thread.start()
            stderr_thread.start()

            time.sleep(timeout_seconds)
            std_out = "".join(stdout_output)
            std_err = "".join(stderr_output)
            subprocess.Popen("TASKKILL /F /PID {pid} /T".format(pid=process.pid))

            event.set()   # Tell the thread to stop
            stdout_thread.join(timeout=2)
            stderr_thread.join(timeout=2)

            return std_out, std_err, 0

        except Exception as e:
            print(f"An error occurred: {str(e)}")
            return "", "", -1