from abc import ABC, abstractmethod
import v2ticlib.string_utils as string_utils

class Ejector(ABC):
    @abstractmethod
    async def eject(self, request, response) -> str:
        pass

    def get_as_boolean(self, boolean_str:str):
        boolean = False
        if string_utils.is_blank(boolean_str):
            return boolean
        return boolean_str.lower() == 'true'