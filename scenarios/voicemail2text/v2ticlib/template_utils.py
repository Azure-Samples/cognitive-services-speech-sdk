import json
from typing import Final
import os
import enum
import typing
import quopri
from v2ticlib import config_utils
import v2ticlib.string_utils as string_utils
from jinja2 import Environment, FileSystemLoader, Template
from Common.abstract_profile import AbstractProfile
import v2ticlib.constants.fields as Fields
import v2ticlib.constants.constants as Constants
import v2ticlib.converter_utils as converter_utils
import v2ticlib.request_utils as request_utils

class TemplateTypeEnums(enum.Enum):
    request = 'request.j2'
    response = 'response.j2'
    after_deposit_ack = 'after_deposit_ack.j2'
    smtp_return_code = 'smtp_return_code.j2'

    def is_mandatory(self):
        return self == TemplateTypeEnums.request or self == TemplateTypeEnums.response

    def is_optional(self):
        return not self.is_mandatory()

class TemplateUtils():

    def __init__(self):
        self._config_base = 'profiles'
        self._profile_folder = self.get_profile_folder()
        self._templates_folder = os.path.join(self._profile_folder, 'templates')
        self._environment = Environment(loader=FileSystemLoader(self._templates_folder))
        self._templates: dict = {}
        for template in self._environment.list_templates():
            self._templates[template] = self._environment.get_template(template)

        self._validate_mandatory_templates()

        self._profile: AbstractProfile = self.load_profile()

    def _validate_mandatory_templates(self):
        missing_templates = []
        missing_templates = [mandatory_template for mandatory_template in [TemplateTypeEnums.request.value, TemplateTypeEnums.response.value]
                             if self._templates.get(mandatory_template) is None]
        if len(missing_templates) > 0:
            raise Exception(f'mandatory template {missing_templates} missing!')

    def load_profile(self):
        return converter_utils.str_to_instance(self.get_profile())

    def get_property(self, property):
        return config_utils.get_property(self._config_base, property)

    def get_profile_folder(self):
        return self.get_property("folder")

    def get_profile(self):
        return self.get_property("profile")

    def render_request(self, initial_request_content:dict, headers: typing.Mapping[str, str]):
        context = self._profile.create_request_context(initial_request_content, headers)
        request = self._do_render_as_json(TemplateTypeEnums.request, context, context_key=Constants.CONTEXT)
        request_utils.set_headers(request, headers)
        request_utils.set_request_fields_if_not_exist(request, initial_request_content)
        return request

    def render_response(self, request):
        request = self._profile.get_response_context(request)
        response = self._do_render_response(TemplateTypeEnums.response, request)
        return self._profile.update_response_hook(request, response)

    def has_after_deposit_ack_template(self):
        return self._has_template(TemplateTypeEnums.after_deposit_ack)

    def _has_template(self, template_type: TemplateTypeEnums):
        return template_type.value in self._templates.keys()

    def render_after_deposit_ack(self, request):
        request = self._profile.get_after_deposit_ack_context(request)
        after_deposit_ack = self._do_render_response(TemplateTypeEnums.after_deposit_ack, request)
        return self._profile.update_after_deposit_ack_hook(request, after_deposit_ack)

    def has_smtp_return_code_template(self):
        return self._has_template(TemplateTypeEnums.smtp_return_code)

    def render_smtp_return_code(self, request) -> str:
        context = self._profile.get_smtp_return_code_context(request)
        return self._do_render(TemplateTypeEnums.smtp_return_code.value, context)

    def _do_render_response(self, template_type: TemplateTypeEnums, request):
        response:dict = self._do_render_as_json(template_type, request)
        headers = response.get(Fields.HEADERS)
        if headers is None:
            response[Fields.HEADERS] = {}

        response[Fields.HEADERS] = self._render_headers(headers, request)

        response[Fields.BODY] = self._render_body(response.get(Fields.BODY), request)

        return response

    def _render_headers(self, headers:dict, context):
        for header in headers.keys():
            headers[header] = self._render_header(headers[header], context)
        return headers

    def _render_header(self, header_value, context):
        if type(header_value) == str:
            return header_value

        # assume header is a dict and needs rendering
        header_str:str = self._render_import(header_value, context)

        encoding = header_value[Fields.ENCODING]
        encoded_header:bytes = header_str.encode(encoding)
        quopri_header = quopri.encodestring(encoded_header)
        content_type =  Constants.US_ASCII
        return quopri_header.decode(content_type)

    def _render_body(self, body, context):
        if body is None or type(body) == str:
            return body

        # assume body is a dict and needs rendering
        body_str:str = self._render_import(body, context)
        encoding = string_utils.default_if_empty(body.get(Fields.ENCODING), Constants.UTF_8)
        return body_str.encode(encoding)

    def _render_import(self, field:dict, context):
        template = field[Fields.IMPORT]
        return self._do_render(template, context)

    def _do_render_as_json(self, template_type: TemplateTypeEnums, context, context_key=None):
        string = self._do_render(template_type.value, context, context_key)
        return json.loads(string)

    def _do_render(self, template_name: str, context, context_key=None):
        key = string_utils.default_if_empty(context_key, TemplateTypeEnums.request.name)
        template: Template = self._templates[template_name]
        return template.render({key: context})

template_utils: Final[TemplateUtils] = TemplateUtils()