import pytest
from api_tests.app.system_utils.make_utils import MakeUtils

@pytest.mark.makecommands
#@pytest.mark.makeregression
def test_make_commands():
    #created_image = 'v2tic-pytest_20231018-173705'
    image_name = 'v2tic-pytest'
    make_utils = MakeUtils()
    created_image = make_utils.build_image(image_name)
    make_utils.start_minikube()
    make_utils.load_image(created_image)
    make_utils.check_load_image(created_image)
    make_utils.deploy_pod(created_image,deployment='https')
    make_utils.destroy_pod(deployment='https')
    make_utils.clean(deployment='https')

    # make_utils.quick_start()
    # make_utils.destroy_pod(deployment='https')
    # make_utils.clean(deployment='https')
