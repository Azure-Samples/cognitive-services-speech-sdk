import logging

class Config:
    api_version = 'api-version=2023-12-01-preview'
    
    def __init__(self, key: str, region: str, logger: logging.Logger = None):
        self.key = key
        self.region = region
        self.url_prefix = 'https://' + region + '.api.cognitive.microsoft.com/customvoice/'
        if logger is None:
            self.logger = logging.getLogger()
        else:
            self.logger = logger
