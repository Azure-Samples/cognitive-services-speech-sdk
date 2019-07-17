from synth_test_analysis import get_time

FILE = r"C:\workspace\CallCenterAgents\unimrcp-1.6.0\x64\Release\log\unimrcpclient_2019.07.16_21.28.11.069910.log"
AUDIO_TIME = 952  # in ms

if __name__ == "__main__":
    with open(FILE, "r") as reader:
        lines = reader.readlines()

    request_time = {}
    complete_time = {}
    for _idx in range(0, len(lines)):
        line = lines[_idx]
        if line.startswith("2019") and "Send MRCPv2 Data" in line and "RECOGNIZE" in lines[_idx+1]:
            time = get_time(line.split()[0:2])
            id = lines[_idx+2].split()[1]
            request_time[id] = time
        elif "Receive MRCPv2 Data" in line and "RECOGNITION-COMPLETE" in lines[_idx+1]:
            time = get_time(line.split()[0:2])
            id = lines[_idx+2].split()[1]
            complete_time[id] = time
            if not lines[_idx+3].strip().endswith("success"):
                print("error")

    durations = []
    for x in request_time:
        duration = complete_time[x] - request_time[x]
        durations.append(duration.total_seconds()*1000 - AUDIO_TIME)
    print(f"max latency:\t{max(durations):.0f}ms")
    print(f"min latency:\t{min(durations):.0f}ms")
    print(f"average latency:\t{sum(durations) / len(durations):.0f}ms")
