from api_tests.app.system_utils.run_command import RunCommand
from api_tests.core.reporter import reporter
from api_tests.core.assert_utils import validate

class MakeUtils(object):
    def __init__(self):
        self.rc = RunCommand()

    def build_image(self, image_name='v2tic-pytest'):
        rc_make_build_image = self.rc.run_with_output(f"make build_image image_name={image_name}")
        actual_returncode = rc_make_build_image.returncode
        validate(0, actual_returncode, 'Validate build_image response status code')
        created_image = str(rc_make_build_image.stdout.decode('utf-8')).strip().split(':')
        actual_image_name = created_image[1].strip()
        validate(image_name,actual_image_name,'Validate Image name',contains=True)
        return actual_image_name

    def start_minikube(self):
        rc_start_minikube = self.rc.run_with_output(f"make start_minikube")
        actual_returncode = rc_start_minikube.returncode
        actual_status = str(rc_start_minikube.stdout.decode('utf-8')).strip()
        validate(0, actual_returncode, 'Validate start_minikube response status code')
        validate(expected='Minikube is already running', expected2='Done! kubectl is now configured to use "minikube" cluster',
                  actual=actual_status, message='Validate Minikube status', contains=True)

    def load_image(self, created_image):
        rc_make_load_image = self.rc.run_with_output(f"make load_image image_name={created_image}")
        actual_returncode = rc_make_load_image.returncode
        actual_output = str(rc_make_load_image.stdout.decode('utf-8')).strip()
        validate(0, actual_returncode, 'Validate load_image response status code')
        validate('Image Loading...', actual_output, 'Validate Image loading')

    def check_load_image(self, created_image):
        rc_minikube_image = self.rc.run_with_output(f"minikube image list | grep {created_image}")
        actual_returncode = rc_minikube_image.returncode
        actual_output = str(rc_minikube_image.stdout.decode('utf-8')).strip()
        validate(0, actual_returncode, 'Validate check_load_image response status code')
        validate(created_image, actual_output, 'Validate Image loaded in minikube',contains=True)

    def deploy_pod(self,created_image, deployment='https'):
        std, std_err, return_code = self.rc.popen_with_output(f"make deploy_pod image_name={created_image} deployment={deployment}", '../../ent-v2t-azure', 10)
        actual_output = str(std)

        validate(0, return_code, 'Validate deploy_pod response status code')
        validate(expected='deployment.apps/https-deployment created', expected2='deployment.apps/https-deployment unchanged',
                 actual=actual_output, message='Validate deployment created', contains=True)


        validate(expected='service/https-service created', expected2='service/https-service unchanged',
                 actual=actual_output, message='Validate service created', contains=True)

        validate(expected='configmap/https-configmap-file created', expected2='configmap/https-configmap-file unchanged',
                 actual=actual_output, message='Validate configmap created', contains=True)

        validate('',str(std_err), 'Validate no error is encountered')

    def destroy_pod(self, deployment='https'):
        rc_make_destroy_pod = self.rc.run_with_output(f"make destroy_pod deployment={deployment}", '../../ent-v2t-azure')
        return_code = rc_make_destroy_pod.returncode
        actual_output = str(rc_make_destroy_pod.stdout.decode('utf-8')).strip()
        validate(0, return_code, 'Validate destroy_pod response status code')
        validate(f'deployment.apps "{deployment}-deployment" deleted', actual_output, 'Validate deployment deleted', contains=True)
        validate(f'service "{deployment}-service" deleted', actual_output, 'Validate service deleted', contains=True)
        validate(f'{deployment}-configmap-file" deleted', actual_output, 'Validate configmap deleted', contains=True)

    def clean(self, deployment='https'):
        rc_make_clean = self.rc.run_with_output(f"make clean deployment={deployment}", '../../ent-v2t-azure')
        return_code = rc_make_clean.returncode
        actual_output = str(rc_make_clean.stdout.decode('utf-8')).strip()
        validate(0, return_code, 'Validate clean response status code')
        validate('Deleted pod-file.yaml',actual_output , 'Validate pod file deleted', contains=True)
        validate('Deleted configmap-file-instance.yaml', actual_output, 'Validate configmap file deleted', contains=True)

    def quick_start(self):
        rc_make_quick_start = self.rc.popen_with_output(f"make quick_start", '../../ent-v2t-azure', 720)
        assert rc_make_quick_start.returncode == 0

    def kubectl(self):
        rc_kubectl = self.rc.run_with_output(f"kubectl get all")