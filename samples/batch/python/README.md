# How to use the Speech Services Batch Transcription API from Python

## Download and install the API client library

To execute the sample you need to generate the Python library for the REST API which is generated through [swagger](swagger.io).

Follow these steps for the installation:

1. Go to https://editor.swagger.io
2. Click **File**, then click **Import URL**
3. Enter the Swagger URL including the region for your Speech Services subscription: `https://<your-region>.cris.ai/docs/v2.0/swagger`
4. Click **Generate Client** and select **Python**
5. Save the client library
6. Extract the downloaded python-client-generated.zip somewhere in your file system.
7. Install the extracted python-client module in your Python environment using pip: `pip install path/to/package/python-client`.
8. The installed package has the name `swagger_client`. You can check that the installation worked using the command `python -c "import swagger_client"`.

NOTE:

Due to a [known bug in the swagger autogeneration](https://github.com/swagger-api/swagger-codegen/issues/7541), you might encounter errors on importing the `swagger_client` package.
These can be fixed by deleting the line with the content
```py
    from swagger_client.models.model import Model  # noqa: F401,E501
```
from the file `swagger_client\models\model.py` and the line with the content
```py
from swagger_client.models.inner_error import InnerError  # noqa: F401,E501
```
from the file `swagger_client\models\inner_error.py` inside the installed package. The error message will tell you where these files are located for your installation.

## Install other dependencies

The sample uses the `requests` library. You can install it with the command

```bash
pip install requests
```

## Run the sample code

The sample code itself is [main.py](python-client/main.py).
You will need to adapt the following information to run the sample:

1. Your subscription key and the associated [region](https://aka.ms/csspeech/region). This must be the same region you [downloaded the client](#download-and-install-the-api-client-library) for; it won't work for any other region.
1. The uri of an audio recording in blob storage
1. (Optional:) The model ids of both an adapted acoustic and language model, if you want to use a custom model.

You can use a development environment like PyCharm to edit, debug, and execute the sample.

