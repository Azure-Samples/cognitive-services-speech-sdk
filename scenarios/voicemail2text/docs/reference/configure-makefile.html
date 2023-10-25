
<!DOCTYPE html>

<html>

<head>

  <link rel="stylesheet" href="/docs/styles.css">

</head>


<body>

<h1>Makefile information</h1>

<p>The Makefile contains is a text file to automate build and execution tasks such as building Docker image(s), loading image(s) to minikube, deploying image(s) and running services on pods. When you run <tt>make</tt> commands, like when <a href="/docs/build-deploy/build-summary.html">building and deploying V2TIC images</a>, you are using the Makefile.</p>

<p>In order to build and deploy V2TIC images, you must enter your Azure AI speech key and endpoint in the Makefile:</p>

<code>SPEECH_KEY :=
    END_POINT :=</code>

<h2>Makefile example</h2>

<code>
# By declaring targets as .PHONY, we ensure that the associated commands are always executed, regardless of the existence of files or directories with the same names as the targets.

.PHONY: default_make build_image check_minikube_status start_minikube load_image deploy_pod start_dashboard get_status destroy_pod enable_autoscale destroy_autoscale ssh_pod clean

# Input speech key and endpoint (public or private) to get started
SPEECH_KEY :=
END_POINT :=

# Provide a value of IMAGE_NAME= while calling target build_image, else it will default to 'default_image_<datetimestamp>'
# Eg: make build_image image_name=image_v2tic_01

DEFAULT_IMAGE_NAME ?= default_image

.DEFAULT_GOAL := default_make

default_make:
	@echo "No targets provided! Please consider re-running make with target as argument, Eg: make build_image or make deploy_pod"

build_image:
	@eval "export IMAGE_NAME=$(if $(image_name),$(image_name),$(DEFAULT_IMAGE_NAME))_$(shell date +'%Y%m%d-%H%M%S')";\
    	docker build -t $$IMAGE_NAME . ;\
    	echo "Image Name: $$IMAGE_NAME"

check_minikube_status:
	@minikube status

MINIKUBE_STATUS := $(shell minikube status)

start_minikube:
	@echo "Minikube status: $(MINIKUBE_STATUS)"
	@if echo "$(MINIKUBE_STATUS)" | grep -q "Running"; then \
    		echo "Minikube is already running"; \
	else \
    		minikube start --driver docker; \
    	fi

load_image:
	@eval export LOAD_IMAGE_NAME=$(image_name) ;\
	echo "Image Loading..."; \
	minikube image load $$LOAD_IMAGE_NAME

OS_NAME := $(shell uname)

deploy_pod:
	@eval export DEPLOYMENT=$(deployment);\
	export DEPLOY_IMAGE_NAME=$(image_name);\
	export SPEECH_KEY=$(SPEECH_KEY);\
	export END_POINT=$(END_POINT);\
	echo $$DEPLOYMENT ;\
	if [ -z "$(SPEECH_KEY)" ]; then\
		echo "SPEECH_KEY was not provided, please review Makefile and ensure it's value is not empty" && exit 1;\
	fi ;\
	if [ -z "$(END_POINT)" ]; then\
		echo "END_POINT was not provided, please review Makefile and ensure it's value is not empty" && exit 1;\
	fi ;\
	envsubst < etc/deployments/$$DEPLOYMENT/configmap-file.yaml > etc/deployments/$$DEPLOYMENT/configmap-file-instance.yaml ;\
	kubectl apply -f etc/deployments/$$DEPLOYMENT/configmap-file-instance.yaml;\
	export v2tic_port=$$(kubectl get configmap $$DEPLOYMENT-configmap-file -o jsonpath='{.data.v2tic_port}') && \
	export v2tic_nodeport=$$(kubectl get configmap $$DEPLOYMENT-configmap-file -o jsonpath='{.data.v2tic_nodeport}') && \
	envsubst < etc/deployments/$$DEPLOYMENT/deployment-file.yaml > etc/deployments/$$DEPLOYMENT/pod-file.yaml ;\
	kubectl apply -f etc/deployments/$$DEPLOYMENT/pod-file.yaml;\
	export vmcs_label=$$(kubectl get deployment $$DEPLOYMENT-deployment -o jsonpath='{.metadata.labels.app}') && \
	kubectl wait --for=condition=ready pod -l app=$$vmcs_label ;\
	if echo "$(OS_NAME)" | grep "Linux"; then\
		echo "Skipping port-forward on Linux" ;\
		export pod_ip="$$(kubectl get pod -l app=$$vmcs_label -o jsonpath='{.items[*].status.hostIP}')" && \
		echo Send traffic on pod ip: "$$pod_ip" ;\
	else \
		kubectl port-forward service/$$DEPLOYMENT-service $$vmcs_nodeport:$$vmcs_port;\
	fi

start_dashboard:
	minikube dashboard

get_status:
	kubectl get all

destroy_pod:
	@export DEPLOYMENT=$(deployment);\
	kubectl delete deployment.apps/$$DEPLOYMENT-deployment service/$$DEPLOYMENT-service configmap/$$DEPLOYMENT-configmap-file

clean:
	@export DEPLOYMENT=$(deployment); \
	if [ -f "etc/deployments/$$DEPLOYMENT/pod-file.yaml" ]; then \
    		echo "About to delete etc/deployments/$$DEPLOYMENT/pod-file.yaml"; \
    		rm -f "etc/deployments/$$DEPLOYMENT/pod-file.yaml"; \
    		echo "Deleted pod-file.yaml"; \
	else \
		echo "File etc/deployments/$$DEPLOYMENT/pod-file.yaml doesn't exist. Skipping deletion."; \
	fi;\
	if [ -f "etc/deployments/$$DEPLOYMENT/configmap-file-instance.yaml" ]; then \
    		echo "About to delete etc/deployments/$$DEPLOYMENT/configmap-file-instance.yaml"; \
    		rm -f "etc/deployments/$$DEPLOYMENT/configmap-file-instance.yaml"; \
    		echo "Deleted configmap-file-instance.yaml"; \
	else \
		echo "File etc/deployments/$$DEPLOYMENT/configmap-file-instance.yaml doesn't exist. Skipping deletion."; \
	fi

enable_autoscale:
	@export DEPLOYMENT=$(deployment);\
	minikube addons enable metrics-server ;\
	kubectl autoscale deployment $$DEPLOYMENT-deployment --cpu-percent=30 --min=1 --max=10

destroy_autoscale:
	@export DEPLOYMENT=$(deployment);\
	kubectl delete horizontalpodautoscaler.autoscaling/$$DEPLOYMENT-deployment

ssh_pod:
	kubectl exec -it <pod_name> -- /bin/bash 
</code>

    <p><a href="/docs/index.html">Return to table of contents</a></p>
</body></html>