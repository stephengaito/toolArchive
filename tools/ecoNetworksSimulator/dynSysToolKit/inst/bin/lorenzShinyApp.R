#!/usr/bin/env Rscript

# This Rscript loads and runs a shiny app to display the Lorenz equations

library(shiny)
library(dynSysToolKit)

server <- function(input, output) {
  shortResults <- integrateLorenz(stepsPerSample = 1, numSamples = 100)
  results <- integrateLorenz(stepsPerSample = 1, numSamples = 10000)
  x <- results[,1]
  y <- results[,2]
  z <- results[,3]
  output$distPlot <- renderPlot({
    #plot3d(x, y, z, type='l', col="blue")
    lines(x,y, col="blue")
    lines(shortResults[,1], shortResults[,2], col="red")
  })
}

ui <- fluidPage(
  sidebarLayout(
    sidebarPanel(
      inputPanel(
        a(href="http://example.com/link1", "Link One"),
        a(href="http://example.com/link2", "Link Two"),
        a(href="http://example.com/link3", "Link Three")
      ),
      inputPanel(
        sliderInput("obs", "Number of observations:", min = 10, max = 500, value = 100)
      )
    ),
    mainPanel(plotOutput("distPlot"))
  )
)

shinyApp(ui = ui, server = server)
