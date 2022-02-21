# The server application

The server application is a GoLang based http webserver to serve:

1. edit pages to GET markdown pages to be edited and POST changes to be 
processed. 

2. create new pages which allows the user to select a particular template 
YAML/Markdown page. 

3. rename existing pages

4. delete existing pages

5. (eventually) provide a Reference creation page (which allows the user 
to interactively select reference type as well as authors) before editing 
a "reference" page. 

The server also incorporates a walker process which:

1. indexes existing html pages for searchable content

2. runs markdown on existing `*.md` files which have not yet appeared in 
the static html page directories. 

3. maintains a concept relationships mapping used to produce static D3 
JSON mapping files in the static directories used to provide a fly-over of 
concept relationships. 

