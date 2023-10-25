<!DOCTYPE html>

<html>

<head>

  <link rel="stylesheet" href="/docs/styles.css">

</head>


<body>

<h1>Test and validate</h1>

<p>After building and deploying the image, you can test and validate it.</p>

<h2>Set SSH Tunneling Port Forwarding</h2>

<p>Port forwarding is required for facilitating the connections from localhost to the pod of the minikube in which the services are running during your tests.</p>

<p>
    To set up the port number of your service deployment:
    
    <ol>
        <li>Open the configmap-file.yaml for your service deployment, either <tt>../etc/deployments/https/configmap-file.yaml</tt> or <tt>../etc/deployments/smtp/configmap-file.yaml</tt>.</li>
        <li>Look for the port number on the <code>v2tic_nodeport</code> line. For example:
            <br><br>
            <code>v2tic_nodeport= "31000"</code>
        </li>
        <li>Create the port forwarding connections by establishing SSH to minikube, and ensure the SSH tunneling sessions are always intact and live.
            <br><br>
            In a new screen, run this command, where <tt>31000</tt> and <tt>32000</tt> are the port numbers for your service deployments:
            <br><br>
            <code>ssh -i ~/.minikube/machines/minikube/id_rsa docker@$(minikube ip) -L \*:31000:0.0.0.0:31000 -L \*:32000:0.0.0.0:32000</code>
            <br><br>You can close the screen after. 
        </li>
    </ol>
</p>

<h2>Start listeners for capturing responses</h2>

<p>V2TIC has listeners for capturing responses in order to see and test those responses. Listener files are found in <tt>sample-server-listeners</tt>.</p>

<p>In order to send responses to listeners, you need to change the destination in your V2TIC deployment configuration to the listener port. Once you've finished testing, you must change them back to the real destination values.</p>

(what and how to change those values)

<p>To start the listener script for each deployment:</p>

<h3>HTTPS</h3>

<p>
    In a new terminal, run this command in the V2TIC project folder:
    <code>ent-v2t-azure$ /usr/bin/python3.9 sample_server_listeners/sample_http_server.py
    </code>
    You should see in response, where the IP address is the address for your V2TIC server:
    <code>HTTP Server listening for responses on http://10.33.135.37:8443/response
    </code>
    Responses will appear in this terminal.
</p>

<h3>SMTP</h3>

<p>
    In a new terminal. run this command in the V2TIC project folder:
    <code>/ent-v2t-azure$ /usr/bin/python3.9 sample_server_listeners/sample_smtp_server.py
    </code>
    You should see in response, where the IP address is the address for your V2TIC server:
    <code>SMTP Server Listening On 10.33.135.37:9025
    </code>
    Responses will appear in this terminal.
</p>


<h2>Test your services</h2>

<p>You can run local and remote tests to verify your builds and port forwarding are working. You'll need the sample audio file found at <tt>../etc/audio/sample-audio.txt</tt> to run these tests.</p>

<h3>Test your HTTPS service</h3>

<h4>Local</h4>

<p>
    <ol>
        <li>In your project, either run the test script located at <tt>sample_clients/sample_https_client.py</tt> to request a transcription, or use the curl command below. In this command, <tt>31000</tt> is your service deployment's port number and <tt>10.33.135.37:8443</tt> is the IP address and port for the listener:
            <br><br>
            <code>curl -X PUT -vN https://127.0.0.1:31000/transcribe  -k -H "X-Reference: testing-http-push" -H "Connection: close" -H "Content-Type: audio/wav" -H "X-Language: en-US" -H "X-Return-URL:  http://10.33.135.37:8443/response" -H "Content-Transfer-Encoding: base64" -H "X-Caller: 1234567890" -d @etc/audio/sample-audio.txt</code>
        </li>
        <li>In the listener terminal, check the response for a <tt>conversionResponse</tt> message. The <tt>Location</tt> value matches the request ID (<tt>scrid</tt>).</li>
        <li>You can also check for transcription response on the service container logs with a kubectl command or the Kubernetes Dashboard:
            <ul>
                <li>kubectl:
                    <ol>
                        <li>In your project, run <tt>kubectl get pod</tt> to display the pod's information. The pod's name, redy status, status, restarts, and age appears.</li>
                        <li>Run <code>kubectl logs</code> with the pod's name to display the logs. For example:
                        <br><br>
                        <code>kubectl logs test_pod</code></li>
                    </ol></li>
                <li>Kubernetes Dashboard:
                    <ul>
                        <li>Follow the Kubernetes Dashboard instructions <a href="/docs/build-deploy/build-summary.html#kubernetes-dashboard">verify build instructions</a> to view the transcription response logs.</li>
                    </ul>
                </li>
            </ul>
        </li>
    </ol>
</p>

<h4>Remote</h4>

<p>
    <ol>
        <li>Copy <tt>sample-audio.txt</tt> to another host and make sure it's accessible.</li>
        <li>In your project, run this command, where <tt>31000</tt> is your service deployment's port number and <tt>http://10.33.135.37:8443</tt> is the secure V2TIC address, which is the same address as the listener, and the port configured for the listener:
            <br><br>
            <code>curl -X PUT -vN https://<Docker_IP>:31000/transcribe  -k -H "X-Reference: testing-http-push" -H "Connection: close" -H "Content-Type: audio/wav" -H "X-Language: en-US" -H "X-Return-URL: http://10.33.135.37:8443/response" -H "Content-Transfer-Encoding: base64" -H "X-Caller: 1234567890" -d @audio/sample-audio.txt </code>
        </li>
        <li>In the listener terminal, check the response for a <tt>conversionResponse</tt> message. The <tt>Location</tt> value matches the request ID (<tt>scrid</tt>).</li>
        <li>Check for transcription response on the service container logs with a kubectl command or the Kubernetes Dashboard:
            <ul>
                <li>kubectl:
                    <ol>
                        <li>In your project, run <tt>kubectl get pod</tt> to display the pod's information. The pod's name, redy status, status, restarts, and age appears.</li>
                        <li>Run <code>kubectl logs</code> with the pod's name to display the logs. For example:
                        <br><br>
                        <code>kubectl logs test_pod</code></li>
                    </ol></li>
                <li>Kubernetes Dashboard:
                    <ul>
                        <li>Follow the Kubernetes Dashboard instructions <a href="/docs/build-deploy/build-summary.html#kubernetes-dashboard">verify build instructions</a> to view the transcription response logs.</li>
                    </ul>
                </li>
            </ul>
        </li>
    </ol>
</p>

<h3>Test your SMTP service</h3>

<h4>Local</h4>

<p>
    <ol>
        <li>From your localhost, run <tt>sample_smtp_client.py</tt> found at <tt>/sample_smtp_clients/sample_smtp_client.py</tt>.</li>
        <li>In the listener terminal, check the response for a <tt>conversionResponse</tt> message. The <tt>Location</tt> value matches the request ID (<tt>scrid</tt>).</li>
        <li>Check for transcription response on the service container logs with a kubectl command or the Kubernetes Dashboard:
            <ul>
                <li>kubectl:
                    <ol>
                        <li>In your project, run <tt>kubectl get pod</tt> to display the pod's information. The pod's name, redy status, status, restarts, and age appears.</li>
                        <li>Run <code>kubectl logs</code> with the pod's name to display the logs. For example:
                        <br><br>
                        <code>kubectl logs test_pod</code></li>
                    </ol></li>
                <li>Kubernetes Dashboard:
                    <ul>
                        <li>Follow the Kubernetes Dashboard instructions <a href="/docs/build-deploy/build-summary.html#kubernetes-dashboard">verify build instructions</a> to view the transcription response logs.</li>
                    </ul>
                </li>
            </ul>
        </li>
    </ol>
</p>

<h4>Remote</h4>

<p><b>Note:</b> Requires the aiosmptlib library in addition to Python3.</p>

<p>
    <ol>
        <li>Copy <tt>sample_smtp_client.py</tt> found at <tt>/sample_smtp_clients/sample_smtp_client.py</tt> to another host.</li>
        <li>In <tt>sample_smtp_client.py</tt>, edit the <tt>smtp_server</tt> to the address of <tt>smtp_server</tt> in your V2TIC deployment.</li>
        <li>Run <tt>sample_smtp_client.py</tt> on your other host using this command:
        <br><br>
        <code>/usr/bin/python3 sample_smtp_clients/sample_smtp_client.py</code></li>
        <li>Check for transcription response on the service container logs with a kubectl command or the Kubernetes Dashboard:
            <ul>
                <li>kubectl:
                    <ol>
                        <li>In your project, run <tt>kubectl get pod</tt> to display the pod's information. The pod's name, redy status, status, restarts, and age appears.</li>
                        <li>Run <code>kubectl logs</code> with the pod's name to display the logs. For example:
                        <br><br>
                        <code>kubectl logs test_pod</code></li>
                    </ol></li>
                <li>Kubernetes Dashboard:
                    <ul>
                        <li>Follow the Kubernetes Dashboard instructions <a href="/docs/build-deploy/build-summary.html#kubernetes-dashboard">verify build instructions</a> to view the transcription response logs.</li>
                    </ul>
                </li>
            </ul>
        </li>
    </ol>
</p>

<h2>Check and debug deployments</h2>

<p>To check the status of a deployment, run <tt>kubectly get all</tt> in the V2TIC project to list the status of the cluster. The first section lists the pod names and their status:</p>

<code>
    /ent-v2t-azure# kubectl get all
    NAME                                    READY   STATUS             RESTARTS      AGE
    pod/https-deployment-7468cf8b6b-zvstl   1/1     Running            1 (20h ago)   20h
    pod/smtp-deployment-8c74fb5c6-9lgd8     0/1     CrashLoopBackOff   2 (19s ago)   35s
</code>

<p>To see the logs for a specific pod, run <tt>kubectl logs &lt;pod/service-deployment&gt;</pod></tt>:</p>

<code>
    ent-v2t-azure# kubectl logs -f pod/smtp-deployment-8c74fb5c6-9lgd8
Traceback (most recent call last):
  File "/app/servers/smtp_server.py", line 9, in &lt;module&gt;
    import Common.request_handler as request_handler
  File "/app/Common/request_handler.py", line 3, in &lt;module&gt;
    from Common.response_creator import ResponseCreator
  File "/app/Common/response_creator.py", line 1, in &lt;module&gt;
    from v2ticlib.template_utils import template_utils
  File "/app/v2ticlib/template_utils.py", line 144, in &lt;module&gt;
    template_utils: Final[TemplateUtils] = TemplateUtils()
  File "/app/v2ticlib/template_utils.py", line 36, in __init__
    self._templates[template] = self._environment.get_template(template)
  File "/usr/local/lib/python3.9/dist-packages/jinja2/environment.py", line 1010, in get_template
    return self._load_template(name, globals)
  File "/usr/local/lib/python3.9/dist-packages/jinja2/environment.py", line 969, in _load_template
    template = self.loader.load(self, name, self.make_globals(globals))
  File "/usr/local/lib/python3.9/dist-packages/jinja2/loaders.py", line 138, in load
    code = environment.compile(source, name, filename)
  File "/usr/local/lib/python3.9/dist-packages/jinja2/environment.py", line 768, in compile
    self.handle_exception(source=source_hint)
  File "/usr/local/lib/python3.9/dist-packages/jinja2/environment.py", line 936, in handle_exception
    raise rewrite_traceback_stack(source=source)
  File "/app/etc/profiles/sample_smtp/templates/response_body.j2", line 12, in template
    converted_text: {{This person called and left you a message. Please call voicemail - Nuance}}
jinja2.exceptions.TemplateSyntaxError: expected token 'end of print statement', got 'person '
</code>


<p><a href="/docs/index.html">Return to table of contents</a></p>
</body></html>