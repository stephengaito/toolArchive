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
   "path/filepath"
   "regexp"
   "strings"
)
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

  md2htmlRegexp   := regexp.MustCompile("\\.md\\)")
  ipAddressRegexp := regexp.MustCompile("LOCAL_IP_ADDRESS")

  md := goldmark.New(
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

  os.MkdirAll(docsPath, 0755)
  os.MkdirAll(htmlPath, 0755)
  
  err := filepath.Walk(docsPath, func(filePath string, info os.FileInfo, err error) error {
    if err != nil { return err }
    
    htmlFile := strings.Replace(
      filePath,
      docsPath,
      htmlPath,
      1,
    )
    os.MkdirAll(path.Dir(htmlFile), 0755)

    if strings.HasSuffix(filePath, ".md") {
      htmlFile =  strings.Replace(htmlFile, ".md", ".html", 1)
      fmt.Printf("converting [%s] to [%s]\n", filePath, htmlFile)
      
      mdFileBytes, err := ioutil.ReadFile(filePath)
      if err != nil { return err }
      htmlFileBytes    := md2htmlRegexp.ReplaceAll(mdFileBytes, []byte(".html)"))
      localIpAddress := os.Getenv("ipAddress")
      htmlFileBytes   = ipAddressRegexp.ReplaceAll(htmlFileBytes, []byte(localIpAddress))
      
      var buf bytes.Buffer
      err = md.Convert(htmlFileBytes, &buf)
      if err != nil { return err }
      
      err = ioutil.WriteFile(htmlFile, buf.Bytes(), 0644)
      if err != nil { return err }
    } else if ! info.IsDir() {
      fmt.Printf("Copying [%s] to [%s]\n", filePath, htmlFile)
      cmd := exec.Command("cp", filePath, htmlFile)
      cmd.Stdout = os.Stdout
      cmd.Stderr = os.Stderr
      err := cmd.Run()
      if err != nil { return err }
    }
    return nil
  })
  if err != nil {
    fmt.Printf("docTool error: %v", err)
  }
  
}
