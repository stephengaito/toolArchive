# docTool

A simple goLang based documentation Markdown->html translator. 

The following is very simple Markdown processor based upon the goldmark 
Readme example. (See: [yuin/goldmark](https://github.com/yuin/goldmark)) 

It walks through all *.md files in a `docs` directory and 
renders them as HTML in the `project/html` directory. 

Since these *.md files are under our control, we explicilty allow (unsafe) 
embedded html. We can use this to provide titles for all webpages. 

## Install

```
  cd docTool
  go get
  go install
```

## Usage

To use `docTool` to convert Markdown files in a `docs` directory into Html 
files in an `tmp/html/docTool/html` directory (both of which are in the 
current directory), type: 

```
  docTool docs tmp/html/docTool/html
```
