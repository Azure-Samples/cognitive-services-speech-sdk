import xml.etree.ElementTree as ET


def get_values_from_xml_response(xml_response,elem_keys):
    root = ET.fromstring(xml_response)

    xml_response_dict = {}

    for elem in elem_keys:
        element = root.find(elem)
        if element.text is not None:
            xml_response_dict[elem] = element.text.strip()
        else:
            xml_response_dict[elem] = None

    return xml_response_dict