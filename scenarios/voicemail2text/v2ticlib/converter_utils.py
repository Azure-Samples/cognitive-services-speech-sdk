import importlib

def has_module(class_str:str) -> bool:
    return '.' in class_str

def str_to_instance(class_str:str, *args, **kwargs) -> any:
    return str_to_class(class_str)(*args, **kwargs)

def str_to_class(class_str:str) -> any:

    if has_module(class_str) == False:
        return eval(class_str)

    module_name, class_name = class_str.rsplit(".", 1)
    try:
        module = importlib.import_module(module_name)
        try:
            return getattr(module, class_name)
        except AttributeError:
            print(f'Class does not exist {class_name}')
    except ImportError:
        print(f'Module does not exist {module_name}')

    raise Exception(f'Cant load class: {class_str}!')