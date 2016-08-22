#!/usr/bin/env Rscript

# This Rscript loads and runs a shiny app to display the a simple 4-d cube

library(shiny)
library(ndViewer)

server <- function(input, output) {
  
  cube <- matrix(c(
    0.1,0.1,0,0,
    0.9,0.1,0,0,
    0.9,0.9,0,0,
    0.1,0.9,0,0,
    0.1,0.1,1,0
  ), nrow = 5, ncol = 4, byrow = TRUE)
    
  output$echo <- renderText(
    paste(input$vp.test1, input$vp.test2, input$va.test1, input$va.test2, sep = " ")
  )
  
  output$distPlot <- renderPlot({
    .R_graphicsParameters()
    lines(cube[,1], cube[,2], col="blue")
  }, bg = "grey")
}

coordLabels <- c("test1", "test2")

ui <- fluidPage(
  sidebarLayout(
    sidebarPanel(
      viewPointUI(coordLabels),
      viewAngleUI(coordLabels)
    ),
    mainPanel(
      textOutput("echo"),
      plotOutput("distPlot"))
  )
)

shinyApp(ui = ui, server = server)
