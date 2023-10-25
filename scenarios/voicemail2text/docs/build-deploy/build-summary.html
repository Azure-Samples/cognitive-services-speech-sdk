<!DOCTYPE html>

<html>

<head>

  <link rel="stylesheet" href="/docs/styles.css">

</head>


<body>

<h1>Build and deploy a sample V2TIC</h1>

<p>
In order to deploy V2TIC, you need to configure, build, and load the V2TIC image for HTTPS or SMTP services. This topic guides you through building and loading locally or on a remote host using the provided sample files. You can also use these sample files as templates for your own deployments.</p>

<p>To get started on configuring your own V2TIC deployment, go to <a href="/docs/configure/configure-basics.html">Configuration basics</a>.</p>

<p><b>Note: </b>This documentation uses minikube to set up and run a local Kubernetes environment. However, you can adapt these steps for other service.</li></p>

<p>
    Here's a quick look at what to do:
    
    <ol>
        <li>Clone V2TIC onto the server where you want to host V2TIC.</li>
        <li>Make sure you have all the <a href="../about/prerequisites.html">prerequisites</a>.</li>
        <li>Start minikube.</li>
        <li>Build the image.</li>
        <li>Load the image.</li>
        <li>Deploy the image.</li>
        <li>Verify the build.</li>
    </ol>
</p>

<p>The commands in this process use the actions defined in <a href="/docs/reference/configure-makefile.html">Makefile</a> in the project.</p>

<h2>Clone the project</h2>

<p>Clone the project from GitHub. The location should be on the server where you want to host the V2TIC API.</p>

<h2>Start minikube</h2>

<p>Before you can build your V2TIC image, you need a Kubernetes environment.</p>

<p>
    To start minikube, in your CLI, run:
    <br><br>
    <code>make start_minikube</code> or <code>minikube start --force --driver docker</code>
    <br><br>
    You can verify your Kubernetes is running with:
    <br><br>
    <code>make check_minikube_status</code> or <code>minikube status</code>
    <br><br>
    For more information on running minikube, go to the <a href="https://minikube.sigs.k8s.io/docs/">minikube documentation</a>.
</p>

<h2>Build the image</h2>

<p>
    Once you have a local Kubernetes environment, you can build the V2TIC image.</p>

    <p>Before you can build the image, you must enter the speech key (<tt>SPEECH_KEY</tt>) and end point (<tt>END_POINT</tt>) values for your Azure AI in <a href="/docs/reference/configure-makefile.html">Makefile</a>.</p>
    
    <p>Building a V2TIC image is necessary for the initial deployment, as well as whenever you make configuration changes. This is because building an image creates configuration and deployment files for that specific image (<tt>configmap-file-instance.yaml</tt> and <tt>pod-file.yaml</tt>). To terminate a build, go to <a href="/docs/destroy-autoscale/destroy.html">Destroy deployments</a>.</p>
    
    <ol>
        <li>In your CLI, navigate to the V2TIC project folder.</li>
        <li>Build the Docker image by running <code>make build_image</code>. You can add <code>image_name=</code> to name your image. For example:
        <br><br>
        <code>make build_image image_name=test_image</code>
        <br><br>
        Otherwise, the image name defaults to whatever is specified in <tt>Makefile</tt>.</li>
    </ol>

<h2>Load the image</h2>

<p>
    To load the Docker image into minikube:
    
    <ol>
        <li>Run <code>make load_image image_name=test_image</code>, where <tt>test_image</tt> is the name of your image.</li>
    </ol>
</p>

<h2>Deploy the image</h2>

<p>
    You can deploy the image for HTTPS or SMTP services. To deploy:
    
    <ol>
        <li>Check for a folder under <tt>etc/deployments</tt>. Make sure it contains <tt>HTTPS</tt> and <tt>SMTP</tt> folders, and that each folder contains a <tt>configmap-file.yaml</tt> and <tt>deployment-file.yaml.</tt> The V2TIC repo contains sample files for both HTTPS and SMTP deployments that you can use and modify. For more information on configuring these files, go to <a href="/docs/reference/deploy-configurations-templates.html">Configuration fields</a>.</li>
        <li>Run make deploy_pod to deploy and expose the services (HTTPS or SMTP) on Kubernetes:
            <ul>
            <li>For HTTPS: <code>make deploy_pod deployment=https image_name=test_image</code></li>
            <li>For SMTP: <code>make deploy_pod deployment=smtp image_name=test_image</code></li>
            </ul>
        </li>
    </ol>
</p>

<h2>Verify the build</h2>

<p>After building and deploying the image, verify the build with either kubectl commands or Kubernetes Dashboard.</p>

<h3>Kubernetes commands</h3>

<p>
    <ol>
        <li>In <tt>ent-v2t-azure</tt>, run <tt>kubectl cluster-info</tt> to display the cluster information.</li>
        <li>Run <tt>kubectl get pod</tt> to display the pod information.</li>
        <li>Run <tt>kubectl get deployment</tt> to get deployment information.</li>
        <li>Run <tt>kubectl get service</tt> to get service information.</li>
    </ol>
</p>

<h3 id="#kubernetes-dashboard">Kubernetes Dashboard</h3>

<p>
    <ol>
        <li>In <tt>ent-v2t-azure</tt>, run <code>make start_dashboard</code>.</li>
        <li>Go to the URL that appears to open the dashboard.</li>
        <li>Go to the <b>Deployments</b> and <b>Pods</b> tabs to validate and see information about your builds.
        <ul>
            <li>In <b>Pods</b>, click on the deployment <b>Name</b>, then the <b>Logs</b> icon. <br>
                <img align="left" src="/docs/images/kubernetes-dashboard-logs.png " alt="location of the Logs icon"></li><br>
        </ul></li>
        <li>Go to the <b>Services</b> tab to validate and see information about each deployed service.</li>
    </ol>
</p>

<p>Once you've built and verified your image, you can <a href="/docs/test-validate/test-validate.html">test and validate</a> the build.</p>

<p><a href="/docs/index.html">Return to table of contents</a></p>
</body></html>