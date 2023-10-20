def is_blank(string:str):
    return string is None or string.strip() == ""

def is_not_blank(string:str):
    return not is_blank(string)

def default_if_empty(string:str, default:str):
    if is_blank(string):
        return default
    return string