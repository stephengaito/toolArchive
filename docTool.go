// The following is very simple Markdown processor based upon the goldmark 
// Readme example. (See: [yuin/goldmark](https://github.com/yuin/goldmark))
//
// It walks through all *.md files in the `docs/content` directory and 
// renders them as HTML in the `docs/html` directory. 
//
// Since these *.md files are under our control, we explicilty allow 
// (unsafe) embedded html. We can use this to provide titles for all 
// webpages. 
//
package main

import (
   "bytes"
   "fmt"
   "github.com/yuin/goldmark"
   "github.com/yuin/goldmark/extension"
   "github.com/yuin/goldmark/parser"
   "github.com/yuin/goldmark/renderer/html"
   "io/ioutil"
   "os"
   "os/exec"
   "path"
   "regexp"
   "strings"
)


var md2htmlRegexp   = regexp.MustCompile("\\.md\\)")
var ipAddressRegexp = regexp.MustCompile("LOCAL_IP_ADDRESS")
var dirIndexRegexp  = regexp.MustCompile("DIRECTORY_INDEX")
var md = goldmark.New(
    goldmark.WithExtensions(extension.GFM),
    goldmark.WithParserOptions(
      parser.WithAutoHeadingID(),
    ),
    goldmark.WithRendererOptions(
      //html.WithHardWraps(),
      html.WithXHTML(),
      html.WithUnsafe(),
    ),
  )
  
func walkDir(mdDir string, htmlDir string) {
  fmt.Printf("Working in: [%s] (%s)\n", mdDir, htmlDir)
  os.MkdirAll(htmlDir, 0755)

  mdFiles := make([]string, 0)
  
  files, err := ioutil.ReadDir(mdDir)
  if err != nil { fmt.Printf("docTool ReadDir error: %v", err) }
  
  for _, file := range files {
    aFile := file.Name()
    filePath := path.Join(mdDir, aFile)
    htmlFile := path.Join(htmlDir, aFile)
    
    if strings.HasSuffix(aFile, ".md") {
      mdFiles = append(mdFiles, aFile)
    } else if file.IsDir() {
      walkDir(filePath, htmlFile)
    } else {
      fmt.Printf("  Copying [%s]\n    to [%s]\n", filePath, htmlFile)
      cmd := exec.Command("cp", filePath, htmlFile)
      cmd.Stdout = os.Stdout
      cmd.Stderr = os.Stderr
      err := cmd.Run()
      if err != nil { fmt.Printf("docTool cp error: %v", err) }
    }
  }

  indexList := make([]string, 0)
  for _, aFile := range mdFiles {
    if strings.EqualFold(aFile, "index.md") { continue }
    fmt.Printf("indexing: [%s]\n", aFile)
    indexList = append(indexList,
      fmt.Sprintf("- [%s](%s)\n", strings.TrimSuffix(aFile, ".md"), aFile),
    )
  }
  indexStr := strings.Join(indexList, "\n")
  fmt.Printf("indexed: [%s]\n", indexStr)
  
  for _, aFile := range mdFiles {
    filePath := path.Join(mdDir, aFile)
    htmlFile := path.Join(htmlDir, aFile)
    htmlFile  = strings.Replace(htmlFile, ".md", ".html", 1)
    fmt.Printf("  Converting [%s]\n    to [%s]\n", filePath, htmlFile)
    mdFileBytes, err := ioutil.ReadFile(filePath)
    if err != nil {
      fmt.Printf("docTool could not read [%s]\n", filePath)
      continue
    }
    htmlFileBytes  := dirIndexRegexp.ReplaceAll(mdFileBytes, []byte(indexStr))
    htmlFileBytes   = md2htmlRegexp.ReplaceAll(htmlFileBytes, []byte(".html)"))
    localIpAddress := os.Getenv("ipAddress")
    htmlFileBytes   = ipAddressRegexp.ReplaceAll(htmlFileBytes, []byte(localIpAddress))
    var buf bytes.Buffer
    err = md.Convert(htmlFileBytes, &buf)
    if err != nil {
      fmt.Printf("docTool could not convert [%s] to Markdown error: %v", filePath, err) 
      continue
    }
      
    err = ioutil.WriteFile(htmlFile, buf.Bytes(), 0644)
    if err != nil {
      fmt.Printf("docTool could not write [%s] error: %v", htmlFile, err) 
      continue
    }
  }
  fmt.Println("---")
}

func main() {

  if len(os.Args) != 3 {
    fmt.Println("")
    fmt.Println("usage: docTool <docsPath> <htmlPath>")
    fmt.Println("")
    fmt.Println("  <docsPath> is a path to the collection of Markdown files")
    fmt.Println("  <htmlPath> is a path to a directory which will contain the Html files")
    fmt.Println("")
    os.Exit(-1)
  }

  docsPath := os.Args[1]
  htmlPath := os.Args[2]

  os.MkdirAll(docsPath, 0755)
  os.MkdirAll(htmlPath, 0755)

  walkDir(docsPath, htmlPath)
}
