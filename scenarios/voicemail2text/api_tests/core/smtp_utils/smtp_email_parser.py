class EmailResponseParser:
    def __init__(self,email_from,email_to,email_content):
        self.email_from = email_from
        self.email_to = email_to
        self.email_content = email_content
        self.email_headers = None
        self.email_body = None

    def do_parse_email_content(self):
        lines = self.email_content.splitlines()
        email_body = None
        email_headers = {}
        for line in lines:
            if line.startswith('"'):
                email_body = line
            elif ":" in line:
                split_line = line.split(":")
                email_headers[split_line[0]] = split_line[1]
        return email_headers,email_body

    def get_parse_email_text(self):
        self.email_headers,self.email_body = self.do_parse_email_content()