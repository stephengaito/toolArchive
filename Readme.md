# go-concept-mapper-wiki

A simple goLang based Concept Mapper based upon a Markdown Wiki

## Objective

Provide

1. A simple "postit-note" Markdown Wiki

    - editing a page will render the markdown as static HTML which can be 
    served directly by a webserver (such as NGinx).

    - each page has a YAML/JSON header which can contain information for 
    rendering the page, as well as "maps" and "links" fields used for 
    producing a D3 based fly over of concept relationships. 

    - the static HTML is ephemeral... it is recreated by a walker process 
    which monitors changes. 

    - the "maps" and "links" fields in each page drives a master concept 
    mapping table in an SQLite3 database, which is used to produce static 
    JSON pages on disk which can be served by the webserver.

    - the SQLite3 data base also provides an index of all static HTML 
    pages, which is also recreated by a walker process (that is we 
    incorporate the go-searcher functionality). 

    - the SQLite3 data base is ephemeral and is recreated by a walker 
    process. 

    