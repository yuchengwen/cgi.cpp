#cgi.cpp

cgi.cpp is a simple, small, cross-platform CGI library for C++. It support get/post method, cookie, session, simple template and multiple file upload.

## How to build

Please use cmake to build this project. For Linux/Unix:

```bash
cd cgi.cpp
cmake .
make
```

## How to use

### The cpp source file: main.cpp

```cpp
#include "cppstr.h"
#include "cppform.h"
#include "cpppage.h"

int main()
{
    Page page;
    page.setUploadDir("/uploads/");
    page.setTmplDir("./tmpl/");
    page.setTempDir("./cache/");
    
    Page.response("title", "Cgi.cpp Demo");
    
    String name = page.form->get("name");
    page.response("name", name);
    
    page.render("index.tmpl");
}
```

### Html template file: index.tmpl

```html
<html>
    <head>
        <title><%title%></title>
    </head>
    <body>
        Hello, <%name%>
    </body>
</html>
```

## License

(The LGPL License)

Copyright (C) 2012-2013 Yu Cheng wen &lt;yu0736@live.com&gt;

Thanks very much to everyone who sends suggestions, bugs, ideas and encouragement to us.

https://github.com/ychwn/cgi.cpp
