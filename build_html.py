with open("src/index.html") as f:
    page = f.read()

with open("src/index_html.h", "w") as f:
    page = page.replace('"', '\\"')
    page = "\n".join([f'"{line.strip()}"' for line in page.splitlines()])
    f.write(f'''#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char index_html[] = {page};

#endif
''')
