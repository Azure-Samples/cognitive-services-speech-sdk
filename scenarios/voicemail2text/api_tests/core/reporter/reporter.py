import allure


def report(name,message=None):
    if message:
        print(f"{name} : {message}")
        allure.attach(f"{message}", name={name}, attachment_type=allure.attachment_type.TEXT)
    else:
        print(name)