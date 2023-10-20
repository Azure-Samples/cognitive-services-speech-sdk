# Overview
This README file documents the makefile for a V2TIC project that builds a Docker image, deploys it to Minikube, and exposes it as a service.

# Prerequisites
a) _Server-side_

○ Docker

○ Minikube

○ kubectl

○ Make/Shell/envsubst (is available by default on most Ubuntu systems). For windows, refer additional resources section at the end.

b) _Client-side_

○ Python 3.9 (To run sample_server_listeners)

  • Open Command Prompt/terminal

  • Navigate to folder **ent-v2t-azure**

  • Run command **pip install -r requirements.txt**

# Usage

Clone and checkout the repository
```
git clone https://github.com/nuance-internal/ent-v2t-azure.git

cd ent-v2t-azure

git checkout V2TIC
```
• Open the project in visual studio code or any other editor.

• [Required]: In Makefile present under root folder, Populate the SPEECH_KEY value with your speech key

• [Required]: In Makefile present under root folder, Populate the END_POINT value with your end point url.

  **Example**: END_POINT := wss://$eastus.stt.speech.microsoft.com/speech/universal/v2

• [Optional]: If there is a need, modify configmap-file.yaml file to override configuration values under etc/deployments


# Instructions

Run the make command to build, load and deploy the docker image.
```
cd <path_to_project_parent_folder>
```
Run cd <path_to_project_parent_folder> where code is checked out.
```
make build_image image_name=<image-name>
```
Run ***make build_image*** to build the Docker image. Provide a value of **image_name**, else it will default to **'default_image_datetimestamp'**.
Make a note of IMAGE_NAME displayed on screen once image is created.
```
make start_minikube
```
Run ***make start_minikube*** to start Minikube. If it is already running, it will display the status else will start the cluster.
```
make load_image image_name=<customized-image-name>
```
Run ***make load_image*** to load the Docker image into Minikube.
```
make deploy_pod deployment=<name-of-deployment> image_name=<customized-image-name>
```
Run ***make deploy_pod*** to deploy and expose service on Kubernetes. Sample deployment values: https, smtp, verizon.
Ensure a folder same as deployment name and _configmap-file_ and _deployment-file_ yamls are present under that folder.
```
make start_dashboard
```
Run ***make start_dashboard*** to open the Kubernetes dashboard.
```
make destroy_pod deployment=<name-of-deployment>
```
In case one wants to delete pod, run ***make destroy*** to destroy the deployment, service and configMap for the deployment provided. Sample deployment values: https, smtp, verizon.
```
make clean deployment=<name-of-deployment>
```
In case one wants to clean up artifacts created by make deploy_pod, run ***make clean*** to delete the pod-file.yaml file and configmap-file-instance.yaml file.
```
make enable_autoscale deployment=<name-of-deployment>
```
In case one wants to enable autoscaling, run ***make enable_autoscale*** to enable metrics server and autoscaling on the deployment. Sample deployment values: https, smtp, verizon.
```
make destroy_autoscale deployment=<name-of-deployment>
```
In case one wants to delete autoscaling, run ***make destroy_autoscale*** to destroy autoscaling on the deployment. Sample deployment values: https, smtp, verizon.
```
make display_pod_logs pod_name=<name-of-pod>
```
In case one wants to check and display logs, run ***make display_pod_logs*** to display logs from the latest deployed pod. pod_name is displayed on console logs once deployment is successful.
```
make quick_start
```
Run ***make quick_start*** to run the end-to-end https deployment starting from building the image from current codebase, starting the minikube if not already running, loading the image into the minikube cluster and deploying the https image.

# License
This project is licensed under the MIT License.

# Contact
If you have any questions, please contact us at talnaseri@microsoft.com, yusufabidi@microsoft.com. ajaymurtekar@microsoft.com

# Additional Resources
• Docker documentation: https://docs.docker.com/

• Minikube documentation: https://minikube.sigs.k8s.io/docs/

• kubectl documentation: https://kubernetes.io/docs/reference/generated/kubectl/kubectl-commands , https://kubernetes.io/docs/tasks/tools/

• Make: https://gnuwin32.sourceforge.net/packages/make.htm or https://www.gnu.org/software/make/

• WSL: https://docs.microsoft.com/en-us/windows/wsl/install

• Cygwin: https://www.cygwin.com/

# Performance Benchmark

• Benchmark tests were executed against Azure VM running HTTPS and SMTP server at a time, using a in-house load test injection tool.

| Protocol        | VM Size | \# Pod | Test Config                    | LID_MODE            | MPM | Metrics             | RTF         | Converted | Unconverted |
| --------------- | ------- | ------ | ------------------------------ | ------------------- | --- | ------------------- | ----------- | --------- | ----------- |
| HTTPS           | D4asV5  | 1      | 1 replica , single speech key  | AtStartHighAccuracy | 240 | CPU ~32%, Mem ~19%  | 0.63        | 99.99%    | 0.01%       |
| SMTP (starttls) | D4asV5  | 1      | 1 replica , single speech key  | AtStartHighAccuracy | 240 | CPU ~42%, Mem ~15%  | 0.63        | 99.99%    | 0.01%       |
| HTTPS           | D4asV5  | 2      | 2 replicas , single speech key | AtStartHighAccuracy | 360 | CPU ~50% , Mem ~19% | 0.61 , 0.64 | 99.96%    | 0.03%       |
| SMTP (starttls) | D4asV5  | 2      | 2 replicas , single speech key | AtStartHighAccuracy | 360 | CPU ~55% , Mem ~20% | 0.42 , 0.42 | 99.99%    | 0.01%       |
| HTTPS           | D4asV5  | 2      | 1 replica , two speech keys    | AtStartHighAccuracy | 360 | CPU ~57% , Mem ~20% | 0.65 , 0.65 | 99.98%    | 0.01%       |
| SMTP (starttls) | D4asV5  | 2      | 1 replica , two speech keys    | AtStartHighAccuracy | 360 | CPU ~61% , Mem ~20% | 0.64 , 0.64 | 99.98%    | 0.02%       |

# Running V2TIC Regression tests on local setup
1. Pre-requisite

a. Python 3.9

b. Git Client

c. Setup ffmpeg on windows machine(Note – This setup is only needed when local server needed to run)

•	Download ffmpeg from https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-win64-gpl.zip

•	Unzip this file to C: drive

•	Rename the extracted folder to ffmpeg

•	Set the environment path variable for ffmpeg by running the following command: setx /m PATH "C:\ffmpeg\bin;%PATH%"

•	Restart terminal/command prompt

•	Verify the installation by running: ffmpeg –version

2. Checkout the branch and install required packages.
```
git clone https://github.com/nuance-internal/ent-v2t-azure.git
cd ent-v2t-azure
git checkout voicemail2text
pip install -r requirements.txt
```
3. Run test cases
```
cd api_tests
pytest -k regression -s
```
4. Check HTML Report under reports folder.

Note: Currently default speech key and endpoint is set as a F0 pricing tier, which will be removed once v2tic goes public.
In order to change it's value, follow below steps:
• Navigate to ent-v2t-azure -> etc -> config.properties
• Modity value of speech_key and speech_region
